#include <QApplication>
#include <QStyleFactory>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // 設定現代化樣式
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // 深色調色板
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(30, 30, 30));
    darkPalette.setColor(QPalette::WindowText, QColor(240, 240, 240));
    darkPalette.setColor(QPalette::Base, QColor(50, 50, 50));
    darkPalette.setColor(QPalette::AlternateBase, QColor(60, 60, 60));
    darkPalette.setColor(QPalette::ToolTipBase, QColor(30, 30, 30));
    darkPalette.setColor(QPalette::ToolTipText, QColor(240, 240, 240));
    darkPalette.setColor(QPalette::Text, QColor(240, 240, 240));
    darkPalette.setColor(QPalette::Button, QColor(50, 50, 50));
    darkPalette.setColor(QPalette::ButtonText, QColor(240, 240, 240));
    darkPalette.setColor(QPalette::BrightText, QColor(255, 255, 255));
    darkPalette.setColor(QPalette::Link, QColor(0, 160, 255));
    darkPalette.setColor(QPalette::Highlight, QColor(0, 160, 255));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    app.setPalette(darkPalette);
    
    MainWindow w;
    w.show();
    return app.exec();
}
