#include "handle_peer.h"

std::string func(char c)
{
    int k = int(c);
    k = (k+256)%256;
    return std::to_string(k);
}

std::string url_encode(const std::string & hash)
{
    std::string encoded;
    for(auto i = 0; i < hash.size(); i += 2) {
        encoded += '%' + hash.substr(i, 2);
    }
    return encoded;
}

std::vector<Peer> decode_peers(std::string encoded_peers)
{
    std::vector<Peer> peers;
    for(auto i = 0; i < encoded_peers.size(); i += 6) {
        auto peer = encoded_peers.substr(i, 6);
        // First four bytes are the ip, the final two bytes are the port in big endian.
        peers.push_back(
                Peer(
                        static_cast<unsigned char>(peer[0]),
                        static_cast<unsigned char>(peer[1]),
                        static_cast<unsigned char>(peer[2]),
                        static_cast<unsigned char>(peer[3])
                    ,
                (static_cast<unsigned short>(peer[4]) << 8) | static_cast<unsigned char>(peer[5])));
    }
    return peers;
}

std::vector<Peer> get_peers(json& mi) {
    std::string bencoded_info = encode_json(mi["info"]);
    std::string info_hash = get_sha1(bencoded_info);
    std::string length = std::to_string(int(mi["info"]["length"]));
    std::string url = mi["announce"].get<std::string>() + "?info_hash="+url_encode(info_hash)+"&peer_id=98892233955566778899&port=6881&uploaded=0&downloaded=0&left="+length+"&compact=1";
    http::Request request{url};
    const http::Response response = request.send("GET");
    std::string response_body{response.body.begin(), response.body.end()};
    std::string response_body_view(response_body.data(), response_body.size());
    json decoded_response = decode_bencoded_value(response_body_view);
    return decode_peers(decoded_response["peers"]);
}

void prepare_handshake(std::string& handshake_msg, std::string info_hash)
{
    char protocolLength = 19;
    handshake_msg.push_back(protocolLength);
    char protocol[20] = "BitTorrent protocol";
    handshake_msg+=protocol;
    for (int i = 0; i < 8 ; ++i) {
        handshake_msg.push_back(0);
    }
    handshake_msg.append(hex_to_string(info_hash));
    handshake_msg+="98892233955566778899";
}

int do_handshake(json &mi, std::string ipAddress, int& sock)
{
    std::string bencoded_info = encode_json(mi["info"]);
    std::string info_hash = get_sha1(bencoded_info);

    size_t pos = ipAddress.find_first_of(':');
    std::string ip_str = ipAddress.substr(0, pos);
    std::string port_str = ipAddress.substr(pos+1, std::string::npos);
    int port = stoi(port_str);

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        std::cerr << "Error creating a socket" << std::endl;
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip_str.c_str());
    if (server_addr.sin_addr.s_addr == INADDR_NONE) {
        std::cerr << "Invalid IP Address" << std::endl;
        return 1;
    }
    // Connect to the server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0 ) {
        std::cerr << "Connection Failed" << std::endl;
        return 1;
    }

    std::string handshake_msg;
    prepare_handshake(handshake_msg, info_hash);

    if (send(sock, handshake_msg.data(), handshake_msg.size(), 0) < 0) {
        std::cerr << "Failed to send handshake" << std::endl;
        return 1;
    }
    std::vector<char> handshake_resp(handshake_msg.size());
    if(recv(sock, handshake_resp.data(), handshake_resp.size(), 0) < 0) {
        std::cerr << "Failed to recv handshake" << std::endl;
        return 1;
    }
    if(!handshake_resp.empty()) {
        std::string resp(handshake_resp.end() - 20, handshake_resp.end());
        std::cout << "Peer ID: " << hash_to_hexstring(resp) << std::endl;
    }
    return 0;
}