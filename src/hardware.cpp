/*
 * TM & (C) 2025 Syndromatic Ltd. All rights reserved.
 * Designed by Syndromatic in Manchester.
 */

#include "hardware.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QStorageInfo>
#include <QProcess>
#include <cmath>

namespace Hardware {

QString getProcessorInfo() {
    // Use lscpu for robust parsing
    QProcess p;
    p.start("lscpu", QStringList());
    p.waitForFinished();
    QString output = p.readAllStandardOutput();
    
    QString modelName;
    int coresPerSocket = 1;
    int sockets = 1;
    double speedGhz = 0.0;

    QStringList lines = output.split('\n');
    for (const QString &line : lines) {
        if (line.startsWith("Model name:")) {
            modelName = line.section(':', 1).trimmed();
        } else if (line.startsWith("Core(s) per socket:")) {
            coresPerSocket = line.section(':', 1).trimmed().toInt();
        } else if (line.startsWith("Socket(s):")) {
            sockets = line.section(':', 1).trimmed().toInt();
        } else if (line.startsWith("CPU max MHz:")) {
            // Some distros have CPU max MHz
            double mhz = line.section(':', 1).trimmed().toDouble();
            if (mhz > 0) speedGhz = mhz / 1000.0;
        }
    }
    
    // Fallback for speed if lscpu didn't give max MHz (often it doesn't on VMs or some kernels)
    // Attempt extracting from model name e.g. "@ 2.40GHz"
    if (speedGhz == 0.0) {
        QRegularExpression speedRe("@\\s*([0-9.]+)\\s*GHz");
        QRegularExpressionMatch match = speedRe.match(modelName);
        if (match.hasMatch()) {
            speedGhz = match.captured(1).toDouble();
            // Clean the model name of the speed part
            // modelName.remove(match.captured(0)); // Optional: user requirement "Value) Speed + Physical Core Count + Model" implies we construct it.
        }
    }
    
    // Clean Model Name
    // Remove "Intel(R)", "Core(TM)", "CPU", "@ x.xxGHz"
    modelName.replace(QRegularExpression("\\(R\\)"), "");
    modelName.replace(QRegularExpression("\\(TM\\)"), "");
    modelName.replace("CPU", "", Qt::CaseInsensitive);
    modelName.replace(QRegularExpression("@.*"), ""); // Remove trailing speed if present
    modelName = modelName.trimmed();

    int totalPhysicalCores = coresPerSocket * sockets;
    
    QString speedStr;
    if (speedGhz > 0.0) {
        speedStr = QString::number(speedGhz, 'f', 1) + " GHz";
    } else {
        // Just empty if unknown
        speedStr = ""; 
    }
    
    QString coreStr;
    if (totalPhysicalCores > 0) {
        coreStr = QString("%1-core").arg(totalPhysicalCores);
    }
    
    // Format: [Speed] [Cores] [Model]
    QStringList parts;
    if (!speedStr.isEmpty()) parts << speedStr;
    if (!coreStr.isEmpty()) parts << coreStr;
    if (!modelName.isEmpty()) parts << modelName;
    
    return parts.join(" ");
}

QString getMemoryInfo() {
    // try to get detailed memory info using lshw.
    // this provides type and speed which free -m does not.
    QProcess p;
    p.start("lshw", QStringList() << "-C" << "memory");
    p.waitForFinished();
    QString output = p.readAllStandardOutput();

    // in vm/wsl environments, output might be sparse.
    // we need to handle "*-memory" blocks carefully.
    // key fields: "size:", "clock:", "description:"
    
    QString sizeStr;
    QString speedStr = "Unknown MHz";
    QString typeStr = "Unknown Type";
    
    long long totalBytes = 0;
    bool foundAnySize = false;

    QStringList lines = output.split('\n');
    for (const QString &line : lines) {
        QString trimmed = line.trimmed();

        // size parsing
        // can appear as "size: 24GiB" or "size: 512MiB"
        if (trimmed.startsWith("size:")) {
            QString s = trimmed.section(':', 1).trimmed();
            
            // if this is the first size we see, it might be the system total
            // usually grouped under *-memory
            if (!s.isEmpty()) {
                double val = 0.0;
                long long currentBytes = 0;
                
                // naive parsing for GiB/MiB/KB
                if (s.endsWith("GiB")) {
                    val = s.left(s.length() - 3).toDouble();
                    currentBytes = (long long)(val * 1024 * 1024 * 1024);
                } else if (s.endsWith("MiB")) {
                    val = s.left(s.length() - 3).toDouble();
                    currentBytes = (long long)(val * 1024 * 1024);
                } else if (s.endsWith("KiB")) { // rare for main memory
                    val = s.left(s.length() - 3).toDouble();
                    currentBytes = (long long)(val * 1024);
                } else if (s.endsWith("B")) {
                    currentBytes = s.left(s.length() - 1).toLongLong();
                }

                if (currentBytes > 0) {
                    // if we found a "system memory" bank size before, maybe we should stop?
                    // for now let's just take the first valid size we see as it's typically the total
                    // when running without sudo.
                    if (!foundAnySize) {
                        totalBytes = currentBytes;
                        foundAnySize = true;
                    }
                }
            }
        }

        // clock speed
        // "clock: 2400MHz"
        if (trimmed.startsWith("clock:")) {
            QString c = trimmed.section(':', 1).trimmed();
            if (speedStr == "Unknown MHz" && !c.isEmpty()) {
                speedStr = c;
                // add space if missing (e.g. 2400MHz -> 2400 MHz)
                if (speedStr.endsWith("MHz") && !speedStr.contains(" ")) {
                     speedStr.insert(speedStr.length() - 3, " ");
                }
            }
        }

        // type from description
        // "description: System Memory" isn't useful for type
        // look for "DDR"
        if (trimmed.startsWith("description:")) {
            QString d = trimmed.section(':', 1).trimmed();
            if (d.contains("DDR")) {
                QRegularExpression ddrRe("(DDR\\d+)");
                QRegularExpressionMatch match = ddrRe.match(d);
                if (match.hasMatch()) {
                    typeStr = match.captured(1);
                }
            }
        }
    }

    // if parsing failed entirely, fallback to a sensible "0 GB"
    if (foundAnySize && totalBytes > 0) {
        double gb = totalBytes / 1024.0 / 1024.0 / 1024.0;
        int gbInt = std::round(gb);
        sizeStr = QString::number(gbInt) + " GB";
    } else {
        sizeStr = "0 GB";
    }

    return QString("%1 %2 %3").arg(sizeStr, speedStr, typeStr);
}

QString getGraphicsInfo() {
    // Use `lspci -vmm` for easy parsing
    QProcess p;
    p.start("lspci", QStringList() << "-vmm");
    p.waitForFinished();
    QString output = p.readAllStandardOutput();
    
    // Blocks separated by newlines.
    // Class: VGA compatible controller
    // Vendor: ...
    // Device: ...
    
    QStringList blocks = output.split("\n\n"); // Split by empty line
    QString deviceName;
    
    for (const QString &block : blocks) {
        if (block.contains("VGA compatible controller") || block.contains("3D controller")) {
            QString vendor;
            QString device;
            
            QStringList lines = block.split('\n');
            for (const QString &line : lines) {
                if (line.startsWith("Vendor:")) vendor = line.section(':', 1).trimmed();
                if (line.startsWith("Device:")) device = line.section(':', 1).trimmed();
            }
            
            if (!vendor.isEmpty() && !device.isEmpty()) {
                // Heuristic: If device already contains vendor, don't repeat it?
                // Example: Vendor="NVIDIA", Device="GA102 [GeForce RTX 3080]"
                // Or Vendor="Intel", Device="Iris Xe Graphics"
                
                // Clean Device string (often contains bracketed info in -vmm output if it resolved names?)
                // Actually -vmm normally resolves names if database is present.
                
                // Let's construct "[Vendor] [Device]" but avoid redundant "Intel Intel..."
                if (device.startsWith(vendor, Qt::CaseInsensitive)) {
                    deviceName = device;
                } else {
                    deviceName = vendor + " " + device;
                }
                break; // Found primary GPU
            }
        }
    }
    
    if (deviceName.isEmpty()) return "Graphics Controller";
    
    return deviceName;
}

QString getStartupDiskInfo() {
    QStorageInfo root("/");
    double total = root.bytesTotal() / 1000.0 / 1000.0 / 1000.0; // GB decimal
    QString sizeStr;
    if (total >= 1000) {
        sizeStr = QString::number(total / 1000.0, 'f', 2) + " TB";
    } else {
        sizeStr = QString::number(total, 'f', 0) + " GB";
    }

    // "Volume Name + Capacity".
    // "Syn HD (2 TB)"
    QString name = root.name();
    if (name.isEmpty()) name = "Syn HD"; 
    
    return QString("%1 (%2)").arg(name, sizeStr);
}

QString getOSVersion() {
    QFile file("/etc/os-release");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("VERSION=")) {
                // VERSION="1.0 Canora (Beta 2)"
                QString v = line.section('=', 1);
                v.remove('"');
                return v;
            }
        }
    }
    return "Unknown Version";
}

}
