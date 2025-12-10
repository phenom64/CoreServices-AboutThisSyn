/*
 * TM & (C) 2025 Syndromatic Ltd. All rights reserved.
 * Designed by Syndromatic in Manchester.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */

#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("AboutThisSyn");
    app.setApplicationDisplayName("About This Computer");
    app.setOrganizationName("Syndromatic");
    app.setOrganizationDomain("syndromatic.com");

    // "Atmo" style usually implies system consistency.
    // We rely on platform theme, or QStyleFactory if needed.
    // Since we are "CoreServices", looking native is key.

    MainWindow win;
    win.show();

    return app.exec();
}
