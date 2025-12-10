# AboutThisSyn

**AboutThisSyn** is a mini System Profiler designed for SynOS. It presents a beautiful, user-friendly "About This Computer" window that displays the user's hardware specifications (Processor, Memory, Graphics, Storage) and operating system version in an elegant portrait card.

While built for SynOS Canora, it follows standard Linux hardware probing practices (`lshw`, `lscpu`, `free`, `lspci`) and can be adapted for other distributions.

![License](https://img.shields.io/badge/license-GPLv2-blue.svg)

## Features

- **Atmo Design Language**: Polished UI with native font integration and "NSE" code styling.
- **Robust Hardware Probing**: Accurately detects memory speed/type (`lshw`), physical cores (`lscpu`), and GPU model (`lspci`), even in virtualised environments like WSL.
- **User Mode**: Runs entirely without `root` privileges (no sudo required).
- **Responsive Text**: Handles long hardware component names gracefully with text wrapping.
- **Integration**: "More Info..." launches the native KDE `kinfocenter`.

## Technology Stack

- **Language**: C++ 17
- **Framework**: Qt 5 (Widgets)
- **Target**: KDE Plasma 5 / SynOS Canora

## Build Instructions

### Prerequisites
- CMake 3.16+
- Qt 5 (Core, Gui, Widgets)
- Standard Linux utilities: `lshw`, `lscpu`, `free`, `lspci`.

### Building

Follow these steps to compile `AboutThisSyn` using all available processor cores for maximum speed:

```bash
# 1. Clone or navigate to the source directory
cd AboutThisSyn

# 2. Create a build directory
mkdir build
cd build

# 3. Generate Makefiles (Targeting /usr/bin for install)
cmake -DCMAKE_INSTALL_PREFIX=/usr ..

# 4. Compile (using all cores)
make -j$(nproc)

# 5. Run
./AboutThisSyn
```

### Installation

To install the application to `/usr/bin/`:

```bash
sudo make install
```

### Uninstallation

To remove the application:

```bash
sudo rm /usr/bin/AboutThisSyn
```


## License

This project is licensed under the **GNU General Public License v2.0 (GPLv2)**.

---
*TM & © 2025 Syndromatic Ltd. All rights reserved.*
*Designed by Syndromatic in Manchester.*
