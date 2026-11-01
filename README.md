# Unified Engineering Workbench

Offline C++17 engineering prototype. Parsing, thermal calculation, gradient computation, and worker-pool computation are native C++17; it has no network or third-party runtime dependency.

## Build and run

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
.\build\bin\Launch.exe all 1000 80
```

Commands: `thermal`, `geometry data/example_ascii.stl`, `temperatures data/synthetic_temperature_nodes.csv`, `performance [cases] [mesh]`, `hardware`, `backprop`, `test`, and `all`. Results go under `results/`.

## Architecture and formats

`thermal.cpp` owns the two-node FE model and tridiagonal solve. `imports.cpp` owns validated ASCII STL/CSV parsing. `performance.cpp` uses deterministic independent cases (seed `20261102`) and standard threads. `hardware.cpp` is explicitly simulation-only. `backprop.cpp` has no automatic differentiation. `main.cpp` is the command/presentation layer.

CSV header is exactly `node_id,x_m,y_m,z_m,temperature_C`; coordinates are m, temperature is C. ASCII STL accepts triangular `vertex` facets. Both reject empty/malformed/non-finite records. Tested limits: 1,000,000 CSV nodes and 1,000,000 STL lines. Supplied STL and CSV are self-created; the CSV is synthetic, not an ANSYS export.

## Thermal verification

Model: L=1 m, A=1e-4 m2, k=10 W/(m K), q=10000 W/m3, T(0)=100 C, T(L)=20 C, insulated sides. Element terms are `kA/h [[1,-1],[-1,1]]` and `qAh/2 [1,1]`; exact solution is `100 - 80x/L + q/(2k)x(L-x)`. `thermal_convergence.csv` reports midpoint interpolation error. A debugging insight: nodal error is near round-off but midpoint error is nonzero and decreases about 4x on mesh doubling.

## Limitations

This is a backend-first console interface, not a production CAD/ANSYS viewer: there is no interactive 3D viewport, cancellation control, native GUI, physical board access, automatic plots, or verified public URL. Benchmark numbers are machine-dependent; record final hardware/compiler facts honestly. Read `docs/HARDWARE_DESIGN.md` before considering any physical wiring. AI disclosure is in `docs/AI_USE_LOG.md`.
