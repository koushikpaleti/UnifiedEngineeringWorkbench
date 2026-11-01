#pragma once
#include <array>
#include <string>
#include <vector>
struct Triangle { std::array<double, 9> vertices{}; };
struct TemperatureNode { long long id=0; double x=0,y=0,z=0,temperature=0; };
struct ImportResult { bool ok=false; std::string message; size_t count=0; };
ImportResult parse_ascii_stl(const std::string& path, std::vector<Triangle>& output);
ImportResult parse_temperature_csv(const std::string& path, std::vector<TemperatureNode>& output);
