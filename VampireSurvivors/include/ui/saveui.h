#ifndef SAVEUI_H
#define SAVEUI_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVector>
#include <QMessageBox>

class GameState;

// 存档管理界面
class SaveUI : public QWidget {
    Q_OBJECT
    
public:
    explicit SaveUI(QWidget *parent = nullptr);
    ~SaveUI();
    
    // 显示存档界面
    void showSaveUI(GameState *state);
    void hideSaveUI();
    void centerUI();
    
signals:
    // 关闭存档界面的信号
    void saveUIClosed();
    
private slots:
    // 保存游戏槽函数
    void onSaveSlotClicked(int slot);
    
    // 关闭界面槽函数
    void onCloseButtonClicked();
    
private:
    void setupUI();
    void updateSlotInfo();
    void showMessage(const QString& title, const QString& message);
    
    QVector<QPushButton*> save_slots;
    QVector<QLabel*> slot_info_labels;
    QPushButton *close_button;
    
    GameState *game_state;
    bool is_showing;
};

#endif // SAVEUI_H 