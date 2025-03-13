#ifndef PLAYSCENE2_H
#define PLAYSCENE2_H

#include "baseplayscene.h"

class PlayScene2 : public baseplayscene {
    Q_OBJECT
public:
    explicit PlayScene2(QWidget *parent = nullptr);
    ~PlayScene2();

    void paintEvent(QPaintEvent *event);

signals:
    void playScene2Back();
};

#endif // PLAYSCENE2_H
