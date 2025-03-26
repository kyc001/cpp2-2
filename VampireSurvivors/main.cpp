#include "bin/mainscene.h"
#include <QApplication>
#include <QTime>
#include <cstdlib>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Seed random number generator
    srand(QTime::currentTime().msec());
    
    // Create and show the main scene
    MainScene w;
    w.show();
    
    return a.exec();
}
