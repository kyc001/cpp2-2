#include "../include/ui/mainscene.h"
#include <QApplication>
#include <QFontDatabase>
#include <QFile>
#include <QDebug>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 设置应用程序信息
    QApplication::setApplicationName("暗夜幸存者");
    QApplication::setOrganizationName("NightSurvivors Team");
    
    // 尝试加载自定义字体（如果不存在则使用系统默认字体）
    /* 
    int fontId = QFontDatabase::addApplicationFont(":/ui/fonts/game_font.ttf");
    if (fontId != -1) {
        QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
        QFont gameFont(fontFamily, 12);
        a.setFont(gameFont);
    } else {
        qDebug() << "无法加载游戏字体";
    }
    */
    
    // 加载全局样式表
    QFile styleFile(":/ui/styles/style.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = styleFile.readAll();
        a.setStyleSheet(styleSheet);
        styleFile.close();
    } else {
        qDebug() << "无法加载样式表文件";
    }
    
    // 创建主窗口
    MainScene w;
    
    // 获取屏幕尺寸并设置窗口大小
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int width = qMin(1280, screenGeometry.width() - 100);
    int height = qMin(720, screenGeometry.height() - 100);
    w.resize(width, height);
    
    // 居中显示窗口
    w.move((screenGeometry.width() - width) / 2, (screenGeometry.height() - height) / 2);
    
    // 显示窗口
    w.show();
    
    return a.exec();
} 