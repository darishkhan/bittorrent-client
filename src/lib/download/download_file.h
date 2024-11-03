#pragma once
#include <vector>
#include <iostream>
// #include <winsock2.h>
#include "../bencode/decode.h"
#include "../torrent/parse.h"
#include "../peers/handle_peer.h"
#include "../sha1/sha1.hpp"
#include "piece.h"
#define SHA_DIGEST_LENGTH 20


void download_file(const std::string &filename, const std::string &output_path);
