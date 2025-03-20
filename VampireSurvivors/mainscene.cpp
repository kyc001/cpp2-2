#include "mainscene.h"
#include "ui_mainscene.h"

MainScene::MainScene(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainScene)
{
    ui->setupUi(this);
    this -> setWindowTitle("Vampire Survivors");
    this -> setFixedSize(800, 600);
}

MainScene::~MainScene()
{
    delete ui;
}
