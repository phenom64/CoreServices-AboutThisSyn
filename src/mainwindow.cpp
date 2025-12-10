/*
 * TM & (C) 2025 Syndromatic Ltd. All rights reserved.
 * Designed by Syndromatic in Manchester.
 */

#include "mainwindow.h"
#include "hardware.h"
#include <QVBoxLayout>
#include <QFile>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QProcess>
#include <QPixmap>
#include <QApplication>
#include <QPainter>
#include <QStyle>
#include <QDebug>
#include <QFont>
#include <QPalette>
#include <QLinearGradient>
#include <QGraphicsDropShadowEffect>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Window Frame Constraints
    // Fixed size, strictly locked. 
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    
    // Explicitly lock size
    setFixedSize(320, 480); 
    setMinimumSize(320, 480);
    setMaximumSize(320, 480);
    
    setupUi();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    QWidget *NSECentralWidget = new QWidget(this);
    setCentralWidget(NSECentralWidget);

    // Main Layout: Vertical
    QVBoxLayout *NSEMainLayout = new QVBoxLayout(NSECentralWidget);
    // Tighter margins for narrower width
    NSEMainLayout->setContentsMargins(20, 30, 20, 20);
    NSEMainLayout->setSpacing(10);
    
    // 1. Logo and Header Block
    
    // Logo
    NSELogoLabel = new QLabel(this);
    NSELogoLabel->setAlignment(Qt::AlignCenter);
    QString logoPath = "/usr/share/synos/icons/syn-chrome.png";
    QPixmap logoPix(logoPath);
    
    if (logoPix.isNull()) {
        // Fallback
        logoPix = QPixmap(96, 96); // Slightly smaller for 320px width
        logoPix.fill(Qt::transparent);
        QPainter p(&logoPix);
        p.setRenderHint(QPainter::Antialiasing);
        
        QFont f = QApplication::font();
        f.setPixelSize(72);
        f.setBold(true);
        p.setFont(f);
        p.setPen(QColor(180, 180, 180));
        p.drawText(logoPix.rect(), Qt::AlignCenter, "?");
    } else {
        if (logoPix.width() > 96) logoPix = logoPix.scaled(96, 96, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    
    NSELogoLabel->setPixmap(logoPix);
    if (!QFile::exists(logoPath)) {
        QGraphicsDropShadowEffect *eff = new QGraphicsDropShadowEffect(this);
        eff->setBlurRadius(10);
        eff->setOffset(0, 2);
        eff->setColor(QColor(0, 0, 0, 50));
        NSELogoLabel->setGraphicsEffect(eff);
    }
    NSEMainLayout->addWidget(NSELogoLabel);
    
    // Title: "SynOS"
    NSETitleLabel = new QLabel("SynOS", this);
    NSETitleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = QApplication::font();
    titleFont.setPixelSize(24);
    titleFont.setBold(true); 
    NSETitleLabel->setFont(titleFont);
    QPalette pal = NSETitleLabel->palette();
    pal.setColor(QPalette::WindowText, QColor(30, 30, 30));
    NSETitleLabel->setPalette(pal);
    NSEMainLayout->addWidget(NSETitleLabel);
    
    // Version
    QString ver = Hardware::getOSVersion();
    // Wrap version if super long, but usually it fits.
    // "Version X.Y"
    QString shortVer = ver.split(' ').first(); 
    NSEVersionLabel = new QLabel(QString("Version %1").arg(shortVer), this);
    NSEVersionLabel->setAlignment(Qt::AlignCenter);
    QFont verFont = QApplication::font();
    verFont.setBold(true);
    NSEVersionLabel->setFont(verFont);
    pal.setColor(QPalette::WindowText, QColor(120, 120, 120));
    NSEVersionLabel->setPalette(pal);
    NSEMainLayout->addWidget(NSEVersionLabel);
    
    NSEMainLayout->addSpacing(15);
    
    // 2. Hardware Specs Grid
    QGridLayout *NSESpecsLayout = new QGridLayout();
    NSESpecsLayout->setHorizontalSpacing(10);
    NSESpecsLayout->setVerticalSpacing(12); // slightly more breathing room for wrapped lines
    // Force column 1 (Values) to take available space
    NSESpecsLayout->setColumnStretch(1, 1);
    
    int row = 0;
    
    auto addSpec = [&](const QString &label, const QString &value) {
        QLabel *NSELabel = new QLabel(label, this);
        QFont f = QApplication::font();
        f.setBold(true);
        NSELabel->setFont(f);
        // Align Top-Right so if value wraps, label stays at top
        NSELabel->setAlignment(Qt::AlignRight | Qt::AlignTop);
        // Prevent label from taking too much space? Natural size is usually fine.
        
        QLabel *NSEValue = new QLabel(value, this);
        NSEValue->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        NSEValue->setWordWrap(true); // Crucial for long CPU names
        
        NSESpecsLayout->addWidget(NSELabel, row, 0);
        NSESpecsLayout->addWidget(NSEValue, row, 1);
        row++;
    };
    
    addSpec("Processor", Hardware::getProcessorInfo());
    addSpec("Memory", Hardware::getMemoryInfo());
    addSpec("Graphics", Hardware::getGraphicsInfo());
    addSpec("Startup Disk", Hardware::getStartupDiskInfo());
    
    NSEMainLayout->addLayout(NSESpecsLayout);
    
    NSEMainLayout->addSpacing(15);
    
    // 3. More Info Button
    NSEMoreInfoButton = new QPushButton("More Info...", this);
    NSEMoreInfoButton->setFixedWidth(120);
    connect(NSEMoreInfoButton, &QPushButton::clicked, this, &MainWindow::openMoreInfo);
    
    QHBoxLayout *NSEBtnLayout = new QHBoxLayout();
    NSEBtnLayout->addStretch();
    NSEBtnLayout->addWidget(NSEMoreInfoButton);
    NSEBtnLayout->addStretch();
    NSEMainLayout->addLayout(NSEBtnLayout);
    
    NSEMainLayout->addStretch(); 
    
    // 4. Footer
    NSECopyrightLabel = new QLabel("™ and © 2025 Syndromatic Ltd. All rights reserved.", this);
    NSECopyrightLabel->setAlignment(Qt::AlignCenter);
    QFont footerFont = QApplication::font();
    footerFont.setPixelSize(10); 
    NSECopyrightLabel->setFont(footerFont);
    pal.setColor(QPalette::WindowText, QColor(100, 100, 100));
    NSECopyrightLabel->setPalette(pal);
    
    NSEDesignedLabel = new QLabel("Designed by Syndromatic in Manchester", this);
    NSEDesignedLabel->setAlignment(Qt::AlignCenter);
    NSEDesignedLabel->setFont(footerFont);
    NSEDesignedLabel->setPalette(pal);
    
    NSEMainLayout->addWidget(NSECopyrightLabel);
    NSEMainLayout->addWidget(NSEDesignedLabel);
    
    // Helper to add 'embossed' text effect (light grey shadow)
    auto addEmboss = [this](QLabel *lbl) {
        QGraphicsDropShadowEffect *eff = new QGraphicsDropShadowEffect(this);
        eff->setBlurRadius(0); // Sharp shadow
        eff->setOffset(0, 1);
        eff->setColor(QColor(225, 225, 225)); // Light grey shadow
        lbl->setGraphicsEffect(eff);
    };
    
    // Apply effect to all text labels
    addEmboss(NSETitleLabel);
    addEmboss(NSEVersionLabel);
    addEmboss(NSECopyrightLabel);
    addEmboss(NSEDesignedLabel);
    
    // Note: The specs created inside the lambda above need the effect too.
    // Since we created them in a loop before defining this helper, we can't easily retro-apply without iterating children.
    // However, we can just iterate the Specs Layout items.
    
    for (int i = 0; i < NSESpecsLayout->count(); ++i) {
        QWidget *w = NSESpecsLayout->itemAt(i)->widget();
        if (QLabel *lbl = qobject_cast<QLabel*>(w)) {
            addEmboss(lbl);
        }
    }
}

void MainWindow::openMoreInfo()
{
    QProcess::startDetached("kinfocenter", QStringList());
}
