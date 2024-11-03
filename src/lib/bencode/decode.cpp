#include "decode.h"

// json decode_bencoded_value(const std::string &encoded_value, size_t &index);

json decode_bencoded_string(const std::string &encoded_value, size_t &index)
{
    size_t colon_index = encoded_value.find(':', index);
    if (colon_index != std::string::npos)
    {
        std::string number_string = encoded_value.substr(index, colon_index);
        int64_t number = std::atoll(number_string.c_str());
        std::string str = encoded_value.substr(colon_index + 1, number);
        index = colon_index + str.size() + 1;
        return json(str);
    }
    else
    {
        throw std::runtime_error("Invalid encoded value: " + encoded_value);
    }
}

json decode_bencoded_integer(const std::string &encoded_value, size_t &index)
{
    index++;
    size_t e_position = encoded_value.find('e', index);
    std::string int_as_string = encoded_value.substr(index, e_position - (index));
    index += int_as_string.size() + 1;
    long long int_value = std::stoll(int_as_string);
    return json(int_value);
}

json decode_bencoded_list(const std::string &encoded_value, size_t &index)
{
    json list = json::array();
    int curr = 0;
    index++;
    while (encoded_value[index] != 'e')
    {
        list.push_back(decode_bencoded_value(encoded_value, index));
    }
    index++;
    return list;
}

json decode_bencoded_dictionary(const std::string &encoded_value, size_t &index)
{
    json dict = json::object();
    index++;
    while (encoded_value[index] != 'e')
    {
        json key = decode_bencoded_value(encoded_value, index);
        json value = decode_bencoded_value(encoded_value, index);
        dict[key] = value;
    }
    index++;
    return dict;
}

json decode_bencoded_value(const std::string &encoded_value, size_t &index)
{
    if (std::isdigit(encoded_value[index]))
    {
        return decode_bencoded_string(encoded_value, index);
    }
    else if (encoded_value.size() >= 2 and encoded_value[index] == 'i')
    {
        return decode_bencoded_integer(encoded_value, index);
    }
    else if (encoded_value.size() >= 2 and encoded_value[index] == 'l')
    {
        return decode_bencoded_list(encoded_value, index);
    }
    else if (encoded_value.size() >= 2 and encoded_value[index] == 'd')
    {
        return decode_bencoded_dictionary(encoded_value, index);
    }
    else
    {
        throw std::runtime_error("Unhandled encoded value: " + encoded_value + "at index " + std::to_string(index));
    }
}

json decode_bencoded_value(const std::string &encoded_value)
{
    size_t index = 0;
    return decode_bencoded_value(encoded_value, index);
}