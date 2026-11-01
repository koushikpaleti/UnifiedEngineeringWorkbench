# Simulated embedded integration design

```
Host PC (USB) -- USB CDC --> MCU board (3.3 V GPIO/ADC)
     |-- USB JTAG/SWD --> Debug adapter -- target-specific JTAG/SWD --> MCU or FPGA
     `-- USB JTAG --> FPGA board (3.3 V bank assumption)
```

| Device | Host / signals | Voltage and safety |
|---|---|---|
| MCU board | USB CDC, GPIO/ADC; SWD debug | 3.3 V GPIO assumed; board regulated from USB |
| FPGA board | USB/JTAG; adapter drives TCK/TMS/TDI, target drives TDO | Verify I/O-bank standard; 3.3 V only an assumption |
| JTAG adapter | USB debug transport only, not compute board | Sense reference voltage; never back-power target |

The C++ demonstration is simulation only. It creates MCU, FPGA and debugger adapters with discovery, capabilities, connect/disconnect and telemetry. FPGA has a recoverable 500 ms timeout. Exact board connectors/pins vary: use official schematics, verified voltage domains, common ground and a level shifter when needed.
