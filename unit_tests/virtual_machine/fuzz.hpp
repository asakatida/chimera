#include <tao/json.hpp>

#include <string>

auto base64_decode(const std::string &data) -> std::string;
auto json_from_file(const std::string &path) -> tao::json::value;
void TestOne(const std::string &data);
