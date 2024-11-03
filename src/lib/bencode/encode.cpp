#include "encode.h"

std::string encode_json(json &json_content)
{
    std::ostringstream bencoded_string;
    if (json_content.is_object())
    {
        // json items = json_content.items();
        // json::(items.begin(), items.end());
        bencoded_string << 'd';
        for (auto &i : json_content.items())
        {
            bencoded_string << i.key().size() << ':' << i.key();
            bencoded_string << encode_json(i.value());
        }
        bencoded_string << 'e';
    }
    else if (json_content.is_array())
    {
        bencoded_string << 'l';
        for (auto &i : json_content)
        {
            bencoded_string << encode_json(i);
        }
        bencoded_string << 'e';
    }
    else if (json_content.is_number_integer())
    {
        bencoded_string << 'i';
        bencoded_string << json_content.get<long long>();
        bencoded_string << 'e';
    }
    else if (json_content.is_string())
    {
        std::string string_value = json_content.get<std::string>();
        bencoded_string << std::to_string(string_value.size()) << ':' << string_value;
    }
    return bencoded_string.str();
}