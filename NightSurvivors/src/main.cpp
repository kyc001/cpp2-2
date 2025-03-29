#include <QApplication>
#include "../include/ui/mainscene.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 创建并显示主场景
    MainScene mainWindow;
    mainWindow.show();
    
    return app.exec();
} 