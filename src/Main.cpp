#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>
#include <algorithm>
#include "lib/sha1/utils.h"
#include "lib/bencode/decode.h"
#include "lib/bencode/encode.h"
#include "lib/torrent/parse.h"
#include "lib/peers/handle_peer.h"
#include "lib/download/piece.h"
#include "lib/download/download_file.h"

#include "lib/nlohmann/json.hpp"

using json = nlohmann::json;

void print_info(std::string& info_hash, json& decoded_data)
{
    std::cout << "Info Hash: " << info_hash << std::endl;
    std::string tracker_url;
    decoded_data["announce"].get_to(tracker_url);
    std::cout << "Tracker URL: " << tracker_url << std::endl;
    std::cout << "Length: " << decoded_data["info"]["length"] << std::endl;
    std::cout << "Piece Length: " << decoded_data["info"]["piece length"] << std::endl;
    std::cout<<"Piece Hashes: "<<std::endl;

    int iter = 0;
    std::string pieces = decoded_data["info"]["pieces"].get<std::string>();
    std::string hexstring = hash_to_hexstring(pieces);
    std::cout<<hexstring<<std::endl;
    


}
int main(int argc, char *argv[])
{
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
        return 1;
    }

    std::string command = argv[1];

    if (command == "decode")
    {
        if (argc < 3)
        {
            std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
            return 1;
        }

        std::string encoded_value = argv[2];
        json decoded_value = decode_bencoded_value(encoded_value);
        std::cout << decoded_value.dump() << std::endl;
    }
    else if (command == "info")
    {
        std::string filename = argv[2];
        json decoded_data = parse_torrent_file(filename);
        std::string bencoded_info = encode_json(decoded_data["info"]);

        std::string info_hash = get_sha1(bencoded_info);
        print_info(info_hash, decoded_data);
    }
    else if(command=="peers")
    {
        std::string filename = argv[2];
        json mi = parse_torrent_file(filename);
        auto peers = get_peers(mi);
        for (auto peer:peers)
        {
            std::cout<<peer.ipAddr<<std::endl;
        }
    }
    else if(command=="handshake")
    {
        std::string filename = argv[2];
        json mi = parse_torrent_file(filename);
        auto peers = get_peers(mi);
        std::string ipAddress = argv[3];
        int sock;
        do_handshake(mi, ipAddress, sock);
    }
    else if(command == "download_piece")
    {
        std::string output_path = argv[3];
        std::string filename = argv[4];
        int piece_index = std::stoi(argv[5]);
        download_piece(filename, piece_index, output_path);
    }
    else if(command == "download")
    {
        std::string output_path = argv[3];
        std::string filename = argv[4];
        json mi = parse_torrent_file(filename);
        // download_piece(filename, 0, output_path);
        download_file(filename, output_path);
    }
    else
    {
        std::cerr << "unknown command: " << command << std::endl;
        return 1;
    }

    return 0;
}
