#include "bin/saveui.h"
#include "bin/savefile.h"
#include "bin/gamestate.h"

SaveUI::SaveUI(QWidget* parent) : QWidget(parent), game_state(nullptr) {
    // 设置窗口样式
    setWindowTitle("存档管理");
    setFixedSize(400, 350);
    setStyleSheet(
        "QWidget { background-color: rgba(0, 0, 0, 180); color: white; }"
        "QLabel { font-size: 16px; }"
        "QPushButton { background-color: #4a4a4a; border: none; padding: 8px; margin: 5px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #5a5a5a; }"
        "QPushButton:pressed { background-color: #3a3a3a; }"
    );
    
    // 创建主布局
    main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(20, 20, 20, 20);
    main_layout->setSpacing(15);
    
    // 创建标题
    title_label = new QLabel("存档管理", this);
    title_label->setAlignment(Qt::AlignCenter);
    title_label->setStyleSheet("font-size: 24px; font-weight: bold; margin-bottom: 10px;");
    main_layout->addWidget(title_label);
    
    // 创建信息标签
    info_label = new QLabel("管理你的游戏存档", this);
    info_label->setAlignment(Qt::AlignCenter);
    info_label->setStyleSheet("font-size: 14px; margin-bottom: 20px;");
    main_layout->addWidget(info_label);
    
    // 创建按钮
    save_button = new QPushButton("保存游戏", this);
    load_button = new QPushButton("加载游戏", this);
    export_button = new QPushButton("导出存档", this);
    import_button = new QPushButton("导入存档", this);
    reset_button = new QPushButton("重置存档", this);
    close_button = new QPushButton("关闭", this);
    
    // 设置按钮样式
    QString button_style = "font-size: 16px; padding: 10px;";
    save_button->setStyleSheet(button_style);
    load_button->setStyleSheet(button_style);
    export_button->setStyleSheet(button_style);
    import_button->setStyleSheet(button_style);
    reset_button->setStyleSheet(button_style + "color: #ff7777;");
    close_button->setStyleSheet(button_style);
    
    // 添加按钮到布局
    main_layout->addWidget(save_button);
    main_layout->addWidget(load_button);
    main_layout->addWidget(export_button);
    main_layout->addWidget(import_button);
    main_layout->addWidget(reset_button);
    main_layout->addSpacing(10);
    main_layout->addWidget(close_button);
    
    // 连接信号和槽
    connect(save_button, &QPushButton::clicked, this, &SaveUI::onSaveGameClicked);
    connect(load_button, &QPushButton::clicked, this, &SaveUI::onLoadGameClicked);
    connect(export_button, &QPushButton::clicked, this, &SaveUI::onExportSaveClicked);
    connect(import_button, &QPushButton::clicked, this, &SaveUI::onImportSaveClicked);
    connect(reset_button, &QPushButton::clicked, this, &SaveUI::onResetSaveClicked);
    connect(close_button, &QPushButton::clicked, this, &SaveUI::onCloseClicked);
    
    // 初始隐藏窗口
    hide();
}

void SaveUI::showSaveUI(GameState* gameState) {
    game_state = gameState;
    if (game_state) {
        info_label->setText(QString("金币: %1").arg(game_state->getTotalCoins()));
        show();
    }
}

void SaveUI::onSaveGameClicked() {
    if (!game_state) {
        showMessage("错误", "游戏状态不可用");
        return;
    }
    
    if (game_state->saveGame()) {
        showMessage("成功", "游戏已保存");
    } else {
        showMessage("错误", "保存游戏失败");
    }
}

void SaveUI::onLoadGameClicked() {
    if (!game_state) {
        showMessage("错误", "游戏状态不可用");
        return;
    }
    
    QMessageBox msgBox;
    msgBox.setWindowTitle("确认");
    msgBox.setText("加载游戏将覆盖当前游戏进度，确定继续吗？");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    
    int ret = msgBox.exec();
    if (ret == QMessageBox::Yes) {
        if (game_state->loadGame()) {
            showMessage("成功", "游戏已加载");
            
            // 更新显示的金币数
            info_label->setText(QString("金币: %1").arg(game_state->getTotalCoins()));
        } else {
            showMessage("错误", "加载游戏失败");
        }
    }
}

void SaveUI::onExportSaveClicked() {
    if (!game_state) {
        showMessage("错误", "游戏状态不可用");
        return;
    }
    
    QString filename = QFileDialog::getSaveFileName(this, 
                                                    "导出存档", 
                                                    QDir::homePath() + "/VampireSurvivors_Save.txt", 
                                                    "文本文件 (*.txt)");
    
    if (filename.isEmpty()) {
        return;
    }
    
    SaveFile saveFile;
    saveFile.setTotalCoins(game_state->getTotalCoins());
    
    // 从游戏状态获取全局升级和角色解锁状态
    QVector<int> upgrades;
    for (int i = 0; i < 10; i++) {  // 假设有10种升级类型
        upgrades.append(game_state->getUpgradeLevel(i));
    }
    saveFile.setGlobalUpgrades(upgrades);
    
    if (saveFile.exportToFile(filename)) {
        showMessage("成功", "存档已导出到:\n" + filename);
    } else {
        showMessage("错误", "导出存档失败");
    }
}

void SaveUI::onImportSaveClicked() {
    if (!game_state) {
        showMessage("错误", "游戏状态不可用");
        return;
    }
    
    QString filename = QFileDialog::getOpenFileName(this, 
                                                   "导入存档", 
                                                   QDir::homePath(), 
                                                   "文本文件 (*.txt)");
    
    if (filename.isEmpty()) {
        return;
    }
    
    QMessageBox msgBox;
    msgBox.setWindowTitle("确认");
    msgBox.setText("导入存档将覆盖当前游戏进度，确定继续吗？");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    
    int ret = msgBox.exec();
    if (ret == QMessageBox::Yes) {
        SaveFile saveFile;
        if (saveFile.importFromFile(filename)) {
            // 将导入的数据应用到游戏状态
            game_state->setTotalCoins(saveFile.getTotalCoins());
            
            // 应用全局升级
            QVector<int> upgrades = saveFile.getGlobalUpgrades();
            for (int i = 0; i < upgrades.size(); i++) {
                game_state->setUpgradeLevel(i, upgrades[i]);
            }
            
            showMessage("成功", "存档已导入");
            
            // 更新显示的金币数
            info_label->setText(QString("金币: %1").arg(game_state->getTotalCoins()));
        } else {
            showMessage("错误", "导入存档失败，文件格式可能不正确");
        }
    }
}

void SaveUI::onResetSaveClicked() {
    if (!game_state) {
        showMessage("错误", "游戏状态不可用");
        return;
    }
    
    QMessageBox msgBox;
    msgBox.setWindowTitle("警告");
    msgBox.setText("重置存档将删除所有游戏进度和解锁内容，这个操作无法撤销！");
    msgBox.setInformativeText("确定要重置吗？");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.setIcon(QMessageBox::Warning);
    
    int ret = msgBox.exec();
    if (ret == QMessageBox::Yes) {
        SaveFile saveFile;
        saveFile.clear();
        
        // 将重置的数据应用到游戏状态
        game_state->setTotalCoins(saveFile.getTotalCoins());
        
        // 应用全局升级
        QVector<int> upgrades = saveFile.getGlobalUpgrades();
        for (int i = 0; i < upgrades.size(); i++) {
            game_state->setUpgradeLevel(i, upgrades[i]);
        }
        
        if (game_state->saveGame()) {
            showMessage("成功", "存档已重置");
            
            // 更新显示的金币数
            info_label->setText(QString("金币: %1").arg(game_state->getTotalCoins()));
        } else {
            showMessage("错误", "重置存档失败");
        }
    }
}

void SaveUI::onCloseClicked() {
    hide();
    emit saveUIClosed();
}

void SaveUI::showMessage(const QString& title, const QString& message) {
    QMessageBox msgBox;
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    msgBox.exec();
} 