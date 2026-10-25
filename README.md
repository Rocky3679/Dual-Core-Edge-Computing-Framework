## 📂 Repository Structure

Navigate directly to key engineering assets using the links below:

```text
├── LICENSE
├── README.md                          # Main architectural overview & quickstart
│
├── Code/
│   ├── [Dual_Core_Edge_Framework.ino](./Code/Dual_Core_Edge_Framework.ino)   # Core 0 ISR sampling + Core 1 LDM RTOS firmware
│   ├── [NN_Weights.h](./Code/NN_Weights.h)                                   # Quantized FFNN weight & bias matrices
│   └── [README.md](./Code/README.md)                                         # Task scheduler, memory budget & timing breakdown
│
├── Documentation/
│   ├── [Working_Principle.md](./Documentation/Working_Principle.md)          # True RMS mathematics, FFNN math & hysteresis logic
│   ├── [Troubleshooting.md](./Documentation/Troubleshooting.md)              # Ground bounce, ADC noise & relay chatter resolution
│   ├── [Project_Specification.md](./Documentation/Project_Specification.md)  # Electrical limits, latency budgets & tolerances
│   └── [Future_Improvements.md](./Documentation/Future_Improvements.md)      # Hardware Rev 2.0 (SSRs, SPI ADC, active filters)
│
├── Hardware/
│   ├── [Connections.md](./Hardware/Connections.md)                           # Complete pinout, Star Grounding & schematic wiring
│   ├── [Components_and_BOM.md](./Hardware/Components_and_BOM.md)             # Component list, industrial specs & estimated cost
│   └── [README.md](./Hardware/README.md)                                     # Physical layout & test bench setup description
│
├── Research/
│   ├── [Abstract.md](./Research/Abstract.md)                                 # Peer-reviewed paper abstract & empirical benchmarks
│   ├── [README.md](./Research/README.md)                                     # Publication status & IEEE policy compliance notice
│   └── KCET_Conference_Paper.pdf                                             # Peer-reviewed manuscript (presented at KCET)
│
├── Diagrams/
│   ├── Workflow.png                                                          # Edge dual-core data flow & FreeRTOS queue IPC
│   ├── Matlab Simulink.png                                                   # Digital twin circuit model & transient response
│   └── [README.md](./Diagrams/README.md)                                     # Diagram descriptions & simulation correlation
│
└── Videos/
    └── [README.md](./Videos/README.md)                                       # Hardware demo links & transient response captures