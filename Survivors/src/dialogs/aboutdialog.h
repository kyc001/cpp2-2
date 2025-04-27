//
// 游戏关于对话框头文件
//

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QTextBrowser>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

private slots:
    void onCloseClicked();

private:
    QTextBrowser *contentBrowser;
    QPushButton *closeButton;
    
    void setupUi();
};

#endif // ABOUTDIALOG_H 