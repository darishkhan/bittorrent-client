#include <string>
#include <fstream>
#include "../nlohmann/json.hpp"
#include "../bencode/decode.h"
using json = nlohmann::json;

json parse_torrent_file(const std::string &filename);