#include "imports.hpp"
#include <cmath>
#include <fstream>
#include <sstream>

namespace { bool finite(double v) { return std::isfinite(v); }
std::vector<std::string> split(const std::string& line) { std::vector<std::string> v; std::stringstream s(line); std::string x; while(std::getline(s,x,',')) v.push_back(x); return v; }
bool number(const std::string& s,double& v) { try { size_t n=0; v=std::stod(s,&n); return n==s.size() && finite(v); } catch(...) { return false; } }
}
ImportResult parse_ascii_stl(const std::string& path, std::vector<Triangle>& output) {
    output.clear(); std::ifstream in(path); if(!in) return {false,"Cannot open STL file.",0};
    std::string line, word; std::array<double,9> values{}; int vertex=0; size_t lines=0;
    while(std::getline(in,line)) { if(++lines>1000000) return {false,"STL exceeds tested 1,000,000-line limit.",0}; std::stringstream s(line); s>>word;
        if(word=="vertex") { double x,y,z; if(!(s>>x>>y>>z)||!finite(x)||!finite(y)||!finite(z)) return {false,"Invalid or non-finite STL vertex at line "+std::to_string(lines)+".",0}; if(vertex>=3) return {false,"Facet has more than three vertices.",0}; values[vertex*3]=x;values[vertex*3+1]=y;values[vertex*3+2]=z; ++vertex; }
        if(word=="endloop") { if(vertex!=3) return {false,"Facet does not contain exactly three vertices.",0}; output.push_back({values}); vertex=0; }
    }
    if(vertex) return {false,"Unterminated STL facet.",0}; if(output.empty()) return {false,"STL contains no triangles.",0}; return {true,"Parsed ASCII STL triangles (units are source-file units).",output.size()};
}
ImportResult parse_temperature_csv(const std::string& path, std::vector<TemperatureNode>& output) {
    output.clear(); std::ifstream in(path); if(!in) return {false,"Cannot open CSV file.",0}; std::string line; if(!std::getline(in,line)) return {false,"CSV is empty.",0};
    if(line!="node_id,x_m,y_m,z_m,temperature_C") return {false,"Expected header: node_id,x_m,y_m,z_m,temperature_C",0}; size_t row=1;
    while(std::getline(in,line)) { ++row; if(line.empty()) return {false,"Empty row "+std::to_string(row)+" is not allowed.",0}; if(row>1000002) return {false,"CSV exceeds tested 1,000,000-node limit.",0}; auto f=split(line); double id,x,y,z,t; if(f.size()!=5||!number(f[0],id)||std::floor(id)!=id||id<0||!number(f[1],x)||!number(f[2],y)||!number(f[3],z)||!number(f[4],t)) return {false,"Invalid/non-finite data at CSV row "+std::to_string(row)+".",0}; output.push_back({static_cast<long long>(id),x,y,z,t}); }
    if(output.empty()) return {false,"CSV has header but no nodes.",0}; return {true,"Parsed temperature nodes. Coordinates: m; temperature: C.",output.size()};
}
