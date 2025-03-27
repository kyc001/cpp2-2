#ifndef SAVEUI_H
#define SAVEUI_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>

class SaveFile;
class GameState;

// 存档管理界面
class SaveUI : public QWidget {
    Q_OBJECT
    
public:
    SaveUI(QWidget* parent = nullptr);
    
    // 显示存档界面
    void showSaveUI(GameState* gameState);
    
signals:
    // 关闭存档界面的信号
    void saveUIClosed();
    
private slots:
    // 保存游戏槽函数
    void onSaveGameClicked();
    
    // 加载游戏槽函数
    void onLoadGameClicked();
    
    // 导出存档槽函数
    void onExportSaveClicked();
    
    // 导入存档槽函数
    void onImportSaveClicked();
    
    // 重置存档槽函数
    void onResetSaveClicked();
    
    // 关闭界面槽函数
    void onCloseClicked();
    
private:
    // 布局
    QVBoxLayout* main_layout;
    QLabel* title_label;
    QLabel* info_label;
    
    // 存档操作按钮
    QPushButton* save_button;
    QPushButton* load_button;
    QPushButton* export_button;
    QPushButton* import_button;
    QPushButton* reset_button;
    QPushButton* close_button;
    
    // 游戏状态引用
    GameState* game_state;
    
    // 存档操作结果显示
    void showMessage(const QString& title, const QString& message);
};

#endif // SAVEUI_H 