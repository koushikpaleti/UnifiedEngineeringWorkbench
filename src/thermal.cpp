#include "thermal.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <vector>

double exact_temperature(const ThermalConfig& cfg, double x) {
    return cfg.left_temperature
        + (cfg.right_temperature - cfg.left_temperature) * x / cfg.length
        + (cfg.generation / (2.0 * cfg.conductivity)) * x * (cfg.length - x);
}

ThermalResult solve_thermal_rod(const ThermalConfig& cfg, int elements) {
    if (elements < 1) throw std::invalid_argument("Element count must be >= 1.");
    if (cfg.length <= 0 || cfg.area <= 0 || cfg.conductivity <= 0)
        throw std::invalid_argument("Length, area, and conductivity must be positive.");

    const int node_count = elements + 1;
    const double h = cfg.length / elements;
    const double ke = cfg.conductivity * cfg.area / h;
    const double fe = cfg.generation * cfg.area * h / 2.0;

    // The reduced system is tridiagonal. This Thomas solve avoids O(N^3)
    // dense elimination while retaining the exact two-node element assembly.
    std::vector<double> F(node_count, cfg.generation * cfg.area * h);
    F.front() = fe; F.back() = fe;
    std::vector<double> lower(node_count, -ke), diag(node_count, 2.0 * ke), upper(node_count, -ke);
    diag.front() = diag.back() = ke;
    F[1] -= lower[1] * cfg.left_temperature;
    F[node_count - 2] -= upper[node_count - 2] * cfg.right_temperature;
    std::vector<double> solution(node_count, 0.0);
    solution.front() = cfg.left_temperature; solution.back() = cfg.right_temperature;
    const int internal = node_count - 2;
    if (internal > 0) {
        std::vector<double> a(internal), b(internal), c(internal), d(internal);
        for (int r=0;r<internal;++r) { int i=r+1; a[r]=(r?lower[i]:0); b[r]=diag[i]; c[r]=(r+1<internal?upper[i]:0); d[r]=F[i]; }
        for (int i=1;i<internal;++i) { const double m=a[i]/b[i-1]; b[i]-=m*c[i-1]; d[i]-=m*d[i-1]; }
        solution[node_count-2]=d.back()/b.back();
        for (int i=internal-2;i>=0;--i) solution[i+1]=(d[i]-c[i]*solution[i+2])/b[i];
    }

    ThermalResult result;
    result.elements = elements;
    result.nodes.reserve(node_count);
    for (int i = 0; i < node_count; ++i) {
        const double x = i * h;
        result.nodes.push_back({x, solution[i], exact_temperature(cfg, x)});
    }

    // Evaluate the piecewise-linear FE interpolation at each element midpoint.
    for (int e = 0; e < elements; ++e) {
        const double xmid = (e + 0.5) * h;
        const double fe_mid = 0.5 * (solution[e] + solution[e + 1]);
        const double error = std::abs(fe_mid - exact_temperature(cfg, xmid));
        result.max_midpoint_error = std::max(result.max_midpoint_error, error);
    }
    return result;
}

bool write_thermal_csv(const ThermalResult& result, const std::string& path) {
    std::ofstream out(path);
    if (!out) return false;
    out << "x_m,temperature_FE_C,temperature_exact_C\n";
    out << std::setprecision(12);
    for (const auto& p : result.nodes)
        out << p.x << ',' << p.temperature << ',' << p.exact_temperature << '\n';
    return static_cast<bool>(out);
}

bool export_thermal_suite(const std::string& directory, std::string& message) {
    std::ofstream convergence(directory + "/thermal_convergence.csv");
    if (!convergence) { message="Cannot create thermal output directory."; return false; }
    convergence << "elements,nodes,max_midpoint_absolute_error_C\n";
    for (int n : {10,20,40,80}) { const auto r=solve_thermal_rod(ThermalConfig{},n); if (!write_thermal_csv(r,directory+"/thermal_N"+std::to_string(n)+".csv")) return false; convergence<<n<<','<<r.nodes.size()<<','<<std::setprecision(12)<<r.max_midpoint_error<<'\n'; }
    message="Exported N=10,20,40,80 nodal CSVs and thermal_convergence.csv."; return true;
}
