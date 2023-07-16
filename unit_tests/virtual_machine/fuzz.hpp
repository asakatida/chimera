#include <tao/json.hpp>

#include <string>

auto base64_decode(std::string data) -> std::string;
auto json_from_file(std::string path) -> tao::json::value;
void TestOne(std::string data);
