#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QPainter>
#include<QBrush>
#include<QTimer>
#include<QDebug>
#include"button.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 设置窗口固定大小
    setFixedSize(1050, 725);
    // 设置标题
    setWindowTitle("坦克大战");
    // 设置图标
    setWindowIcon(QIcon("://res/tank_up.jpg"));
    // 模式选择场景
    choosemodelscene = new ChooseModelScene;
    // 开始按钮
    button *startBtn = new button("://res2/start.png");
    startBtn->setParent(this);
    startBtn->move(this->width() * 0.5 - startBtn->width() * 0.5, this->height() * 0.5);
    connect(startBtn, &button::clicked, [=]()
    {
        startBtn->pop1();
        startBtn->pop2();
        QTimer::singleShot(500, this, [=]()
        {
            this->hide();
            choosemodelscene->show();
            connect(choosemodelscene,&ChooseModelScene::chooseModelSceneBack,this,[=]()
            {
                this->show();
                choosemodelscene->hide();
            });
        });
    });

    // 退出按钮
    button *quitBtn = new button("://res3/quit.tga");
    quitBtn->setParent(this);
    quitBtn->move(this->width() * 0.5 - quitBtn->width() * 0.5, this->height() * 0.7);
    connect(quitBtn, &button::clicked, [=]()
    {
        quitBtn->pop1();
        quitBtn->pop2();
        QTimer::singleShot(500,this,[=]()
        {
            this->close();
        });
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QBrush Brush(Qt::black);
    QPixmap pixlogo;
    pixlogo.load("://res/logo.bmp");
    painter.setBrush(Brush);
    painter.drawRect(0, 0, this->width(), this->height());
    painter.drawPixmap(230, 75, 600, 200, pixlogo);
}
