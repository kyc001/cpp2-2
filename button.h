#ifndef BUTTON_H
#define BUTTON_H

#include <QPushButton>

class button : public QPushButton {
    Q_OBJECT
public:
    button(QString normalImg);
    QString normalImgPath;

    // 弹跳特效
    void pop1(); // 向下跳
    void pop2(); // 向上跳
    void createAnimation(int startYOffset, int endYOffset);
    void setupButton(const QPixmap& pix);

signals:

public slots:
};

#endif // BUTTON_H
