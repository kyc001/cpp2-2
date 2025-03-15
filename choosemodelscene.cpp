#include "choosemodelscene.h"
#include <QPainter>
#include <QTimer>
#include "button.h"
ChooseModelScene::ChooseModelScene(QWidget *parent) : QMainWindow(parent)
{ // 设置窗口固定大小
    setFixedSize(1050, 725);
    // 设置标题
    setWindowTitle("坦克大战");
    // 设置图标
    setWindowIcon(QIcon("://res/tank_up.jpg"));
    // 创建场景
    playscene = new PlayScene;
    playscene2 = new PlayScene2;

    // 设置按钮
    button *oneBtn = new button("://res2/1player.png");
    button *twoBtn = new button("://res2/2player.png");
    oneBtn->setParent(this);
    twoBtn->setParent(this);
    oneBtn->move(this->width() * 0.5 - oneBtn->width() * 0.5, this->height() * 0.6);
    twoBtn->move(this->width() * 0.5 - twoBtn->width() * 0.5, this->height() * 0.7);
    connect(oneBtn, &button::clicked, [=]()
            {
       oneBtn->pop1();
       oneBtn->pop2();
       QTimer::singleShot(500,this,[=](){
           this->hide();
           playscene->show();
       });
       connect(playscene,&PlayScene::playSceneBack,[=](){
           playscene->close();
           delete playscene;
           playscene=new PlayScene;
           this->show();
       }); });
    connect(twoBtn, &button::clicked, [=]()
            {
                twoBtn->pop1();
                twoBtn->pop2();
                QTimer::singleShot(500, this, [=]()
                                   {
           this->hide();
           playscene2->show(); });
                connect(playscene2, &PlayScene2::playScene2Back, [=]()
                        {          
            playscene2->close();
            delete playscene2;
            playscene2=new PlayScene2;
            this->show(); });
            });
    // 返回按钮
    button *backBtn = new button("://res3/back.tga");
    backBtn->setParent(this);
    backBtn->move(this->width() * 0.5 - backBtn->width() * 0.5, this->height() * 0.8);
    connect(backBtn, &button::clicked, [=]()
            {
        backBtn->pop1();
        backBtn->pop2();
        QTimer::singleShot(500,this,[=](){
            emit this->chooseModelSceneBack();

        }); });
}
void ChooseModelScene::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QBrush Brush(Qt::black);
    QPixmap pixlogo;
    pixlogo.load("://res/logo.bmp");
    painter.setBrush(Brush);
    painter.drawRect(0, 0, this->width(), this->height());
    painter.drawPixmap(230, 75, 600, 200, pixlogo);
}
