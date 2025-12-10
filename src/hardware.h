/*
 * TM & (C) 2025 Syndromatic Ltd. All rights reserved.
 * Designed by Syndromatic in Manchester.
 */

#ifndef HARDWARE_H
#define HARDWARE_H

#include <QString>

namespace Hardware {

// Returns e.g. "3.2 GHz 6-core Intel Core i5"
QString getProcessorInfo();

// Returns e.g. "16 GB 2400 MHz DDR4" or just "16 GB" if details unavailable
QString getMemoryInfo();

// Returns e.g. "AMD Radeon RX 580 8 GB"
QString getGraphicsInfo();

// Returns e.g. "Macintosh HD (500 GB)" - Volume name + Capacity
QString getStartupDiskInfo();

// Returns e.g. "10.8.3" or "24.04 (Noble Numbat)"
QString getOSVersion();

}

#endif // HARDWARE_H
