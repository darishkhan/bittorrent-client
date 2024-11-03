#pragma once
#include <vector>
#include <iostream>
// #include <winsock2.h>
#include "../bencode/decode.h"
#include "../torrent/parse.h"
#include "../peers/handle_peer.h"
#include "../sha1/sha1.hpp"
#define SHA_DIGEST_LENGTH 20

struct Msg {
    uint32_t length;
    uint8_t id;
} __attribute__((packed));
struct ReqMsg {
    uint32_t length;
    uint8_t id;
    uint32_t index;
    uint32_t begin;
    uint32_t length_block;
} __attribute__((packed));


int send_message(int sock, const std::vector<char>& message);
std::vector<char> recieve_message(int sock, size_t length) ;
void send_interested(int sock) ;
void send_request(int sock, uint32_t index, uint32_t begin, uint32_t length_block);
bool verify_piece(const std::string& piece_data, const std::string& expected_hash) ;
int wait_for_unchoke(int sock);
void download_piece(const std::string& filename, int piece_index, const std::string& output_path);
