#pragma once
#include <string>
#include <vector>

struct ThermalConfig {
    double length = 1.0;       // m
    double area = 1.0e-4;     // m^2
    double conductivity = 10.0; // W/(m K)
    double generation = 10000.0; // W/m^3
    double left_temperature = 100.0;  // deg C
    double right_temperature = 20.0;  // deg C
};

struct ThermalPoint {
    double x = 0.0;
    double temperature = 0.0;
    double exact_temperature = 0.0;
};

struct ThermalResult {
    int elements = 0;
    std::vector<ThermalPoint> nodes;
    double max_midpoint_error = 0.0;
};

ThermalResult solve_thermal_rod(const ThermalConfig& cfg, int elements);
double exact_temperature(const ThermalConfig& cfg, double x);
bool write_thermal_csv(const ThermalResult& result, const std::string& path);
bool export_thermal_suite(const std::string& directory, std::string& message);
