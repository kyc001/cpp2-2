#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
painter.setPen(Qt::blue);
painter.drawText(100, 100, "MainWindow Test"); // 添加简单的绘制代码
    QMainWindow::paintEvent(event);
}
