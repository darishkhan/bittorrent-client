#include "download_file.h"

void download_file(const std::string &filename, const std::string &output_path)
{
    int sock = 0;
    json mi = parse_torrent_file(filename);
    json info = mi["info"];

    auto peers = get_peers(mi);
    std::cout << peers.size() << "\n";
    std::string ip_port = peers[2].ipAddr;
    // std::cout << ip_port << std::endl;

    do_handshake(mi, ip_port, sock);

    send_interested(sock);
    wait_for_unchoke(sock);

    size_t block_length = 16384;
    size_t standard_piece_length = mi["info"]["piece length"];
    size_t total_file_size = mi["info"]["length"];
    size_t num_pieces = (total_file_size + standard_piece_length - 1) / standard_piece_length;
    // std::cout << block_length << " " << standard_piece_length << " " << total_file_size << " " << num_pieces << std::endl;

    std::ofstream outfile(output_path, std::ios::binary);

    for (int piece_index = 0; piece_index < num_pieces; piece_index++)
    {

        size_t current_piece_size = (piece_index == num_pieces - 1) ? (total_file_size % standard_piece_length) : standard_piece_length;
        if (current_piece_size == 0)
            current_piece_size = standard_piece_length;

        size_t remaining = current_piece_size;
        size_t offset = 0; // Ensure correct starting offset

        // std::cout << "Downloading piece " << piece_index << " of size " << current_piece_size << std::endl;

        while (remaining > 0)
        {
            size_t current_block_length = std::min(block_length, remaining);
            send_request(sock, piece_index, offset, current_block_length);

            std::vector<char> length_buffer(4);
            int bytes_received = recv(sock, length_buffer.data(), 4, 0);
            if (bytes_received != 4)
            {
                std::cerr << "Error receiving message length or incomplete read: " << bytes_received << std::endl;
                break; // Consider moving to the next piece instead of breaking the entire loop
            }

            int total_bytes_received = 0;
            int message_length = ntohl(*reinterpret_cast<int *>(length_buffer.data()));

            std::vector<char> message(message_length);
            while (total_bytes_received < message_length)
            {
                bytes_received = recv(sock, message.data() + total_bytes_received, message_length - total_bytes_received, 0);
                if (bytes_received <= 0)
                {
                    std::cerr << "Error receiving message or connection closed" << std::endl;
                    break;
                }
                total_bytes_received += bytes_received;
            }

            if (message[0] == 7)
            {
                std::vector<char> received_block(message.begin() + 9, message.end());
                outfile.write(received_block.data(), received_block.size());
                remaining -= current_block_length;
                offset += current_block_length;
            }
        }
        std::cout << "Piece " << piece_index << " downloaded to " << output_path << std::endl;
    }
    outfile.close();
    std::cout << "File" << " downloaded to " << output_path << std::endl;
}