#ifndef CHOOSEMODELSCENE_H
#define CHOOSEMODELSCENE_H

#include <QMainWindow>
#include "playscene1.h"
#include "playscene2.h"

class ChooseModelScene : public QMainWindow {
    Q_OBJECT
public:
    explicit ChooseModelScene(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event);

    PlayScene1* playscene1;
    PlayScene2* playscene2;

signals:
    void chooseModelSceneBack();

public slots:
};

#endif // CHOOSEMODELSCENE_H
