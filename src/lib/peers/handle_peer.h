#pragma once 
#include <string>
#include <vector>
#include "../http/HTTPRequest.hpp"
#include "../nlohmann/json.hpp"
#include "../sha1/utils.h"
#include "../bencode/encode.h"
#include "../bencode/decode.h"
#ifndef WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

using json = nlohmann::json;

std::string func(char c);

struct Peer
{
    char ip[4];
    std::string ipAddr;
    int port;
    Peer(char ip1, char ip2, char ip3, char ip4, int p)
    {
        ip[0] = ip1;
        ip[1] = ip2;
        ip[2] = ip3;
        ip[3] = ip4;
        this->port = p;
        ipAddr = func(ip[0]) + '.' + func(ip[1]) + '.' + func(ip[2]) + '.' + func(ip[3]) + ':' + std::to_string(this->port % 65536);
    }
};

std::string url_encode(const std::string &hash);
std::vector<Peer> decode_peers(std::string &encoded_peers);
std::vector<Peer> get_peers(json &mi);
void prepare_handshake(std::string &handshake_msg, std::string info_hash);
int do_handshake(json &mi, std::string ipAddress, int &sock);
