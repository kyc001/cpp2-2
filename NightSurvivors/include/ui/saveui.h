#ifndef SAVEUI_H
#define SAVEUI_H

#include <QWidget>
#include <QList>
#include <QEvent>
#include <QVector>

class QLabel;
class QVBoxLayout;
class QScrollArea;
class GameState;

class SaveUI : public QWidget
{
    Q_OBJECT

public:
    explicit SaveUI(QWidget *parent = nullptr);
    virtual ~SaveUI();

    // 设置游戏状态
    void setGameState(GameState *state);

    // 显示保存或加载模式
    void showSaveMode();
    void showLoadMode();

signals:
    // 信号
    void saveClosed();
    void saveSlotSelected(int slot);
    void loadGameRequested();
    void backRequested();

    // 在 SaveUI 类的 public: 部分添加
public:
    void centerUI();

protected:
    // 事件处理
    bool eventFilter(QObject *obj, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    // 私有槽函数
    void onNewSaveClicked();
    void onDeleteClicked();
    void onLoadClicked();
    void onBackClicked();

private:
    // 设置UI
    void setupUI();
    void refreshSaveList();
    QWidget *createSaveItem(int slot);
    void selectSaveSlot(int slot);

    // 成员变量
    GameState *game_state;
    bool is_save_mode;
    int selected_slot;

    // UI组件
    QLabel *title_label;
    QScrollArea *scroll_area;
    QWidget *save_list_widget;
    QVBoxLayout *save_list_layout;
    QVector<QWidget *> save_widgets;
};

#endif // SAVEUI_H
