#include<string>
#include <iomanip>
#include "sha1.hpp"

std::string get_sha1(std::string bencoded_string);
const std::string hex_to_string(std::string in);
std::string hash_to_hexstring(std::string &pieces);
