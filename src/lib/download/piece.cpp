#include "piece.h"

int send_message(int sock, const std::vector<char> &message)
{
    if (send(sock, message.data(), message.size(), 0) < 0)
    {
        std::cerr << "Failed to send message" << std::endl;
        return 0;
    }
    return 1;
}

std::vector<char> recieve_message(int sock, size_t length)
{
    std::vector<char> buffer(length);
    recv(sock, buffer.data(), length, 0);
    return buffer;
}

void send_interested(int sock)
{
    Msg interested_msg = {htonl(1), 2}; // Length is 1, ID is 2 for interested
    send_message(sock, std::vector<char>(reinterpret_cast<char *>(&interested_msg), reinterpret_cast<char *>(&interested_msg) + sizeof(interested_msg)));
}

void send_request(int sock, uint32_t index, uint32_t begin, uint32_t length_block)
{
    ReqMsg req_msg = {htonl(13), 6, htonl(index), htonl(begin), htonl(length_block)};
    send_message(sock, std::vector<char>(reinterpret_cast<char *>(&req_msg), reinterpret_cast<char *>(&req_msg) + sizeof(req_msg)));
}

bool verify_piece(const std::string &piece_data, const std::string &expected_hash)
{
    std::string actual_hash = get_sha1(piece_data);
    return actual_hash == expected_hash;
}

int wait_for_unchoke(int sock)
{
    const int bufferSize = 4; // Buffer size for the length prefix of the message
    char buffer[bufferSize];

    while (true)
    {
        memset(buffer, 0, bufferSize);
        // std::cout<<"ok   ";
        if (recv(sock, buffer, bufferSize, 0) < 0)
        {
            std::cerr << "Error receiving message length" << std::endl;
            break;
        }
        std::cout<<"buffer: "<<buffer<<" ";

        uint32_t msgLength = ntohl(*reinterpret_cast<uint32_t *>(buffer));
        if (msgLength == 0)
        {
            continue;
        }
        std::cout << "message Length: " << msgLength << std::endl;

        if (msgLength < 1)
        {
            std::cerr << "Invalid message length received" << std::endl;
            break;
        }

        char msgID;
        if (recv(sock, &msgID, 1, 0) < 0)
        {
            std::cerr << "Error receiving message ID" << std::endl;
            break;
        }

        if (msgID == 1)
        {
            std::cout << "Received unchoke message ID " << std::endl;
            return 1;
        }
        else
        {
            // If not an unchoke message, skip the rest of the message
            std::vector<char> dummyBuffer(msgLength - 1);
            if (recv(sock, dummyBuffer.data(), msgLength - 1, 0) < 0)
            {
                std::cerr << "Error receiving the rest of the message" << std::endl;
                break;
            }
        }
    }
    return 0;
}

void download_piece(const std::string &filename, int piece_index, const std::string &output_path)
{
    int sock = 0;
    json mi = parse_torrent_file(filename);
    json info = mi["info"];

    auto peers = get_peers(mi);

    std::string ip_port = peers[1].ipAddr;
    // std::cout << ip_port << std::endl;
    do_handshake(mi, ip_port, sock);

    send_interested(sock);
    wait_for_unchoke(sock);

    size_t piece_length = 16384;
    size_t standard_piece_length = mi["info"]["piece length"];
    size_t total_file_size = mi["info"]["length"];
    size_t num_pieces = (total_file_size + standard_piece_length - 1) / standard_piece_length;
    // std::cout << piece_length << " " << standard_piece_length << " " << total_file_size << " " << num_pieces << std::endl;

    std::ofstream outfile(output_path, std::ios::binary);
    size_t current_piece_size = (piece_index == num_pieces - 1) ? (total_file_size % standard_piece_length) : standard_piece_length;
    if (current_piece_size == 0)
    {
        current_piece_size = standard_piece_length; // Handle case where file size is an exact multiple of piece length
    }

    size_t remaining = current_piece_size;
    size_t offset = 0;

    while (remaining > 0)
    {
        size_t block_length = std::min(piece_length, remaining);
        send_request(sock, piece_index, offset, block_length);

        std::vector<char> length_buffer(4);
        int bytes_received = recv(sock, length_buffer.data(), 4, 0);
        // std::cout << "got here "<<bytes_received;
        if (bytes_received != 4)
        {
            std::cerr << "Error receiving message length or incomplete read: " << bytes_received << std::endl;
            break;
        }

        int total_bytes_received = 0;
        int message_length = ntohl(*reinterpret_cast<int *>(length_buffer.data()));

        std::vector<char> message(message_length);
        while (total_bytes_received < message_length)
        {
            bytes_received = recv(sock, message.data() + total_bytes_received, message_length - total_bytes_received, 0);
            // std::cout << "bytes received: " << bytes_received << std::endl;
            if (bytes_received <= 0)
            {
                std::cerr << "Error receiving message or connection closed" << std::endl;
                break;
            }
            total_bytes_received += bytes_received;
        }

        if (message[0] == 7)
        {
            // Extract block data from message
            std::vector<char> received_block(message.begin() + 9, message.end()); // Skip 1 byte of ID, 4 bytes of index, 4 bytes of begin
            outfile.write(received_block.data(), received_block.size());
            // Update remaining and offset
            remaining -= block_length;
            offset += block_length;
            // Check if this was the last block
            if (remaining == 0)
            {
                // std::cout << "Last block received, exiting loop." << std::endl;
                break;
            }
        }
    }

    outfile.close();
    std::cout << "Piece " << piece_index << " downloaded to " << output_path << std::endl;
}

