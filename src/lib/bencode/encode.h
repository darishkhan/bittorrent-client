#include<string>
#include "../nlohmann/json.hpp"

using json = nlohmann::json;

std::string encode_json(json &json_content);