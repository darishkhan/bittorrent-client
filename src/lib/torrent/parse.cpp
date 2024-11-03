#include "parse.h"

json parse_torrent_file(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        return "File not Found!";
    }
    std::string file_contents;
    file.seekg(0, std::ios::end);
    file_contents.reserve(file.tellg());
    file.seekg(0, std::ios::beg);
    file_contents.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    return decode_bencoded_value(file_contents);
}