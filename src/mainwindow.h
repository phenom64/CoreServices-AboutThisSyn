/*
 * TM & (C) 2025 Syndromatic Ltd. All rights reserved.
 * Designed by Syndromatic in Manchester.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openMoreInfo();

private:
    void setupUi();
    
    // UI Elements
    QLabel *NSELogoLabel;
    QLabel *NSETitleLabel;
    QLabel *NSEVersionLabel;
    
    // Specs
    QLabel *NSEProcLabel;
    QLabel *NSEProcValue;
    
    QLabel *NSEMemLabel;
    QLabel *NSEMemValue;
    
    QLabel *NSEGfxLabel;
    QLabel *NSEGfxValue;
    
    QLabel *NSEDiskLabel;
    QLabel *NSEDiskValue;
    
    QPushButton *NSEMoreInfoButton;
    QLabel *NSECopyrightLabel;
    QLabel *NSEDesignedLabel;
};

#endif // MAINWINDOW_H
