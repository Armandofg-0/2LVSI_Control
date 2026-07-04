# 2LVSI Control

Control of a **two-level voltage source inverter (2LVSI)**, simulated in PLECS:

- **Grid-Following**:full current-controlled inverter, with the control implemented as reusable **C modules** (each a `.c`/`.h` pair) called from PLECS C-Script blocks.
- **Grid-Forming**:dispatchable Virtual Oscillator Control (**dVOC**), implemented natively in PLECS.

Each block that needs it has a PDF next to it documenting the theoretical background of the implementation.

## Repository layout

```
Code/                        C control blocks (.c + .h + docs PDF)
├── Clarke/                  clarke transform
├── Park/                    Park transform
├── PI_Controller/           Discrete PI (Obtained from PECLAB code)
├── Low_Pass_Filter/         1st-order LPF (Obtained from PECLAB code)
├── Notch_Filter/            2nd-order notch filter (Obtained from PECLAB code)
├── DSOGI_MSOGI/             DSOGI, MSOGI harmonic bank, sequence separator, MSOGI-PLL
├── Reference_Gen/           Current reference generator (CBC - IPC blend)
├── PCI/                     Proportional + Complex-Integrator current controller 
├── SVM/                     Space-vector PWM (duty-cycle output)
└── Startup_Sequencer/       Grid-connection state machine (SYNC - RAMP - RUN)

Plecs Simulations/
├── Grid Following/          Grid_Following.plecs (uses the C blocks above)
└── Grid Forming/            Grid Forming_dVOC.plecs + documentation
```

## PLECS simulations

Both models embed the C code through **C-Script blocks** with relative includes
(e.g. `#include "../../Code/PCI/pci.c"`), so the folder structure must be kept intact.

### Grid Following (`Grid_Following.plecs`)
2LVSI (800 V DC-link, RL filter) tied to a 220 V / 50 Hz grid with programmable
distortion (15% 5th, 10% 7th harmonic). Control runs at Fs = 40 kHz. 

### Grid Forming (`Grid Forming_dVOC.plecs`)
Grid-forming inverter using **dVOC** (dispatchable Virtual Oscillator Control) in per-unit,
at a 100 MVA / transmission-voltage scale. The scenario scripts load steps and
grid connection/disconnection to show black-start, islanded operation, synchronization
and grid-tied operation. Theory in `Grid Forming Documentation.pdf`.

## Disclaimer

*AI tools were used to improve grammar and code organization for easier comprehension. All design, implementation, and validation are the author's own.*
