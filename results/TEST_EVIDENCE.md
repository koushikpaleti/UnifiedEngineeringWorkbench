# Verified local test evidence

The project was compiled with `g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic` on 2026-11-02. The first compile exposed incompatibilities with the installed old MinGW toolchain (`<filesystem>` and `std::thread`); these were corrected using `_mkdir` and explicit Windows worker threads. Final build completed with warnings only (misleading-indentation style warnings in `imports.cpp`).

`Launch.exe test` output:

```
[TEST] thermal FE + midpoint convergence: PASS
[TEST] valid STL/CSV and malformed non-finite rejection: PASS
```

`Launch.exe all 200 40` completed Thermal, Performance, Hardware and Backprop exports. A subsequent 20,000-case, 80-element performance run was used for the benchmark summary. This is actual development-machine evidence; it is not a claim that the package was clean-machine tested.
