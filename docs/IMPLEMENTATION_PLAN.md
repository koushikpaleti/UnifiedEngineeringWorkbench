# Implementation plan

## Deadline reality
The college deadline is 2026-11-02 at 10:00 AM, which is much sooner than the assignment brief's stated 15-calendar-day window. Confirm the discrepancy with the placement coordinator immediately. Work to the college deadline unless they formally clarify it.

## Milestones
1. **Working numerical core (this starter):** 1D steady-state thermal finite-element solver, analytical comparison, midpoint error, CSV export, basic tests.
2. **File validation:** C++ ASCII STL parser and temperature CSV parser; malformed and non-finite input tests.
3. **Batch performance:** deterministic case generation, serial and bounded worker-pool implementations, repeated timings and agreement checks.
4. **Backpropagation:** handwritten derivation, C++ gradient implementation, central finite-difference checks and loss CSV.
5. **Hardware mock:** simulated microcontroller, FPGA and JTAG adapter with connect/disconnect, telemetry and timeout recovery.
6. **UI and delivery:** one coherent interface, status/errors/export, packaging, report, demo evidence, AI-use log and version-control history.

Given the short announced deadline, prioritize a real, testable C++ backend and transparent evidence. Do not claim a feature works until you have run and recorded a test.

## Suggested practical stack
- C++17 + CMake for all backend logic and numerical computation.
- A simple local web UI can be added later, but file parsing and computation must stay in C++.
- The final brief requires a standalone Windows `Launch.exe`; this starter does not yet satisfy that packaging requirement.

## Current starter scope
Implemented: thermal rod FE solver, analytical solution, maximum midpoint error, CSV export for 10/20/40/80 elements, and basic checks.
Not yet implemented: UI, STL/temperature CSV import, multithreading, hardware mocks, backpropagation, report, demo, or one-click package.
