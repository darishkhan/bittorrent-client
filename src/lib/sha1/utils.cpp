#include "utils.h"

std::string hash_to_hexstring(std::string &pieces)
{
    std::string piece;
    for(char i:pieces)
    {
        unsigned char byte = static_cast<unsigned char>(i); // cast to unsigned char to avoid sign extension
        int value = static_cast<int>(byte);
        std::stringstream ss;
        ss << std::hex << std::setw(2) << std::setfill('0') << value;
        piece += ss.str();
    }
    return piece;
}

const std::string hex_to_string(std::string in)
{
    std::string output;
    if ((in.length() % 2) != 0) {
        throw std::runtime_error("String is not valid length ...");
    }
    size_t cnt = in.length() / 2;
    for (size_t i = 0; cnt > i; ++i) {
        uint32_t s = 0;
        std::stringstream ss;
        ss << std::hex << in.substr(i * 2, 2);
        ss >> s;
        output.push_back(static_cast<unsigned char>(s));
    }
    return output;
}

std::string get_sha1(std::string bencoded_string)
{
    SHA1 sha1;
    sha1.update(bencoded_string);
    std::string string_hash = sha1.final();
    return string_hash;
}