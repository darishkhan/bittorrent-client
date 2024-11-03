#include<string>
#include "../nlohmann/json.hpp"
using json = nlohmann::json;

json decode_bencoded_value(const std::string &encoded_value, size_t &index);
json decode_bencoded_string(const std::string &encoded_value, size_t &index);
json decode_bencoded_integer(const std::string &encoded_value, size_t &index);
json decode_bencoded_list(const std::string &encoded_value, size_t &index);
json decode_bencoded_dictionary(const std::string &encoded_value, size_t &index);
json decode_bencoded_value(const std::string &encoded_value, size_t &index);
json decode_bencoded_value(const std::string &encoded_value);