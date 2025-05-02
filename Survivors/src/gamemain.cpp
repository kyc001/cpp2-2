//
// 游戏主窗口实现文件
//

// You may need to build the project (run Qt uic code generator) to get "ui_GameMain.h" resolved

#include "gamemain.h"
#include <QString>
#include <QPushButton>
#include <iostream>
#include "dialogs/upgradedialog.h"
#include "ui/menu.h"
#include "dialogs/gameoverdialog.h"
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>
#include <QAudioDevice>
#include <QCoreApplication>
#include <QFile>
#include <QTimer>

// 添加 Windows API 头文件和链接库
#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
// 使用 Windows 多媒体音频 API
#pragma comment(lib, "winmm.lib")
#endif

GameMain::GameMain(QWidget *parent) : QWidget(parent) {
    // 初始化指针为 nullptr
    bgmPlayer = nullptr;
    audioOutput = nullptr;
    
    setupUi();
    setAttribute(Qt::WA_DeleteOnClose);

    initScene();
    m_Timer.setInterval(TIMER_RATE);
    hero_type = 1;
    
    // 调用初始化音频函数
    initAndPlayBGM();
    
    // 暂停按钮点击处理
    connect(pause_button, &QPushButton::clicked, [&]()->void { pauseButtonClicked(); });
    pause_button->setText("暂停");

    // 返回按钮点击处理
    connect(back_button, &QPushButton::clicked, [&]()->void {
        if(!game_paused) { pauseButtonClicked(); }
        this->hide();
        widget_parent->show();
        widget_parent->reportGamePaused();
    });

    // 结算按钮点击处理
    connect(end_button, &QPushButton::clicked, [&]()->void {
        if(!game_paused) { pauseButtonClicked(); }
        this->hide();
        widget_parent->reportGameOver();
        auto * new_dialog = new GameOverDialog(widget_parent, game->getEnemyDeathCnt(), game->getHPPercent());
        new_dialog->show();
        this->hide();
        delete this;
    });

    playGame();
}

void GameMain::setupUi() {
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // 游戏区域
    gameArea = new QWidget(this);
    gameArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(gameArea);
    
    // 控制区域
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    controlsLayout->setContentsMargins(10, 5, 10, 5);
    
    // 控制按钮
    pause_button = new QPushButton("暂停", this);
    back_button = new QPushButton("返回", this);
    end_button = new QPushButton("结算", this);
    
    // 信息标签
    enemy_cnt_label = new QLabel("已清除敌人数量：0", this);
    level_label = new QLabel("当前等级：1", this);
    
    // 设置字体大小
    QFont labelFont;
    labelFont.setPointSize(14);  // 增大字体大小
    labelFont.setBold(true);     // 设置为粗体
    enemy_cnt_label->setFont(labelFont);
    level_label->setFont(labelFont);
    
    // 添加组件到控制布局
    controlsLayout->addWidget(pause_button);
    controlsLayout->addWidget(back_button);
    controlsLayout->addWidget(end_button);
    controlsLayout->addStretch();
    controlsLayout->addWidget(enemy_cnt_label);
    controlsLayout->addWidget(level_label);
    
    // 添加控制布局到主布局
    mainLayout->addLayout(controlsLayout);
    
    setLayout(mainLayout);
}

GameMain::~GameMain() {
    this->hide();
     // 停止并清理音乐播放器
     if (bgmPlayer) {
         bgmPlayer->stop();
         delete bgmPlayer;
         bgmPlayer = nullptr;
     }
     if (audioOutput) {
         delete audioOutput;
         audioOutput = nullptr;
     }
    delete game;
}

// 初始化游戏场景
void GameMain::initScene() {
    std::cout << "[Log] GameMain::initScene 开始" << std::endl;
    setFixedSize(GAME_WIDTH, GAME_HEIGHT);
    setWindowTitle(GAME_TITLE);
    std::cout << "[Log] GameMain::initScene: 准备创建 GameState" << std::endl;
    game = new GameState(this);
    std::cout << "[Log] GameMain::initScene: GameState 创建完毕，准备 initMap" << std::endl;
    game->initMap(1);
    std::cout << "[Log] GameMain::initScene: initMap 调用完毕，准备 initHero" << std::endl;
    game->initHero(hero_type);
    std::cout << "[Log] GameMain::initScene: initHero 调用完毕，准备 initEnemy" << std::endl;
    game->initEnemy(1);
    std::cout << "[Log] GameMain::initScene: initEnemy 调用完毕" << std::endl;
    GAME_HALT = 0;
    FIRST_RESUME = 0;
    std::cout << "[Log] GameMain::initScene 结束" << std::endl;
}

// 开始游戏
void GameMain::playGame() {
    game_paused = false;
    m_Timer.start();

    // 设置定时器回调
    connect(&m_Timer, &QTimer::timeout, [=](){
        game->tick();
        update();
        // 更新UI显示
        enemy_cnt_label->setText("已清除敌人数量：" + QString::number(game->getEnemyDeathCnt()));
        level_label->setText("当前等级："+QString::number(game->getPlayerLevel()));
        
        // 检查游戏状态
        if(game->isGameStop()) {
            // 游戏结束处理
            m_Timer.stop();
            game_paused = true;
            pause_button->setEnabled(false);
            back_button->setEnabled(false);
            enemy_cnt_label->setText("游戏已结束，请点击结算：" + QString::number(game->getEnemyDeathCnt()));
        }
        if(game->isWaiting()) {
            // 修改升级处理逻辑，使用延迟创建对话框
            m_Timer.stop(); // 先停止计时器
            game_paused = true;
            pause_button->setEnabled(false);
            pause_button->setText("继续");
            
            // 延迟创建升级对话框，避免事件循环问题
            QTimer::singleShot(100, this, [this]() {
                qDebug() << "[GameMain] 延迟创建升级对话框";
                UpgradeDialog * up_diag = new UpgradeDialog(this);
                up_diag->setWindowModality(Qt::NonModal); // 确保非模态
                up_diag->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动删除
                up_diag->show();
                qDebug() << "[GameMain] 升级对话框已显示";
            });
        }
    });
}

// 绘制事件处理
void GameMain::paintEvent(QPaintEvent * event) {
    QPainter painter(this);

    try {
        std::vector<PaintInfo> buffer = game->paint();

        for(auto & item: buffer){
            try {
                // 确保QPixmap有效
                if (!item.getTarget().isNull()) {
                    painter.drawPixmap(item.getMX(), item.getMY(), item.getTarget());
                } else {
                    std::cout << "警告：试图绘制空QPixmap，位置：" << item.getMX() << "," << item.getMY() << std::endl;
                }
            } catch (const std::exception& e) {
                std::cout << "绘制单个项目时出错: " << e.what() << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cout << "绘制过程中出错: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "绘制过程中发生未知错误" << std::endl;
    }
}

// 按键按下事件处理
void GameMain::keyPressEvent(QKeyEvent *event) {
    game->keyPressTick(event);
}

// 按键释放事件处理
void GameMain::keyReleaseEvent(QKeyEvent *event) {
    game->keyReleaseTick(event);
}

// 暂停按钮点击处理
     void GameMain::pauseButtonClicked() {
         if(game_paused){
             m_Timer.start();
             game_paused = false;
             pause_button->setText("暂停");
             if (bgmPlayer && bgmPlayer->playbackState() == QMediaPlayer::PausedState) {
                 bgmPlayer->play();
             }
         } else {
             m_Timer.stop();
             game_paused = true;
             pause_button->setText("继续");
             if (bgmPlayer && bgmPlayer->playbackState() == QMediaPlayer::PlayingState) {
                 bgmPlayer->pause();
             }
         }
     }
// 恢复游戏
void GameMain::resumeGame() {
    qDebug() << "[GameMain] resumeGame() 开始执行";
    
    // 基本条件检查
    if(!game_paused || game->isGameStop()){
        qDebug() << "[GameMain] 条件不满足，无法恢复游戏 (game_paused=" 
                << game_paused << ", game->isGameStop()=" << game->isGameStop() << ")";
        qDebug() << "[GameMain] resumeGame() 结束执行";
        return;
    }
    
    qDebug() << "[GameMain] 条件满足，准备恢复游戏";
    
    // 使用单独的延迟任务执行每个操作，避免一次执行太多操作导致界面冻结
    
    // 1. 更新游戏状态标志
    QTimer::singleShot(0, this, [this]() {
        qDebug() << "[GameMain] 恢复步骤1: 更新状态标志";
        game_paused = false;
    });
    
    // 2. 更新UI状态
    QTimer::singleShot(50, this, [this]() {
        qDebug() << "[GameMain] 恢复步骤2: 更新UI状态";
        pause_button->setEnabled(true);
        pause_button->setText("暂停");
    });
    
    // 3. 恢复音乐播放
    QTimer::singleShot(100, this, [this]() {
        qDebug() << "[GameMain] 恢复步骤3: 尝试恢复音乐播放";
        if (bgmPlayer && bgmPlayer->playbackState() == QMediaPlayer::PausedState) {
            qDebug() << "[GameMain] 恢复音乐播放";
            bgmPlayer->play();
        }
    });
    
    // 4. 最后步骤：启动游戏计时器
    QTimer::singleShot(150, this, [this]() {
        qDebug() << "[GameMain] 恢复步骤4: 启动游戏计时器";
        m_Timer.start();
        qDebug() << "[GameMain] 游戏已成功恢复";
    });
    
    qDebug() << "[GameMain] resumeGame() 主函数结束执行，后续步骤将延迟执行";
}

// 新增函数 - 尝试使用 Windows API 播放音频
void GameMain::playBackgroundMusicWithWinAPI() {
#ifdef _WIN32
    qDebug() << "==== 尝试使用 Windows API 播放音乐 ====";
    QString fullPath = "D:/Survivors/Survivors/Assets/bgm.mp3";
    
    if (QFile::exists(fullPath)) {
        // 将路径转换为标准 Windows 路径格式
        std::wstring wideStr = fullPath.toStdWString();
        
        // 使用 Windows API 播放音频文件 - 使用 LPCWSTR 转换
        MCIERROR error = mciSendStringW(
            (L"open \"" + wideStr + L"\" type mpegvideo alias bgm").c_str(), 
            nullptr, 0, nullptr);
            
        if (error) {
            wchar_t errorText[256];
            mciGetErrorStringW(error, errorText, 256);
            qDebug() << "Windows API 音乐播放错误:" << QString::fromWCharArray(errorText);
        } else {
            // 设置重复播放
            mciSendStringW(L"play bgm repeat", nullptr, 0, nullptr);
            qDebug() << "Windows API 音乐播放已启动";
        }
    } else {
        qDebug() << "文件不存在:" << fullPath;
    }
#else
    qDebug() << "Windows API 音乐播放仅适用于 Windows 平台";
#endif
}

// 新增函数 - 用于初始化和播放背景音乐
void GameMain::initAndPlayBGM() {
    qDebug() << "==== BGM初始化开始 ====";
    
    // 释放现有的播放器以防重复初始化
    if (bgmPlayer) {
        bgmPlayer->stop();
        delete bgmPlayer;
        bgmPlayer = nullptr;
    }
    
    if (audioOutput) {
        delete audioOutput;
        audioOutput = nullptr;
    }
    
    try {
        // 创建播放器和音频输出
        bgmPlayer = new QMediaPlayer(this);
        audioOutput = new QAudioOutput(this);
        
        // 检查QMediaPlayer是否支持
        qDebug() << "QMediaPlayer可用性检查:";
        qDebug() << " - 对象创建状态: " << (bgmPlayer ? "成功" : "失败");
        
        // 设置基本参数
        bgmPlayer->setAudioOutput(audioOutput);
        audioOutput->setVolume(0.9f); // 增加音量到90%
        audioOutput->setMuted(false);
        
        // 直接尝试所有可能的路径和格式
        QStringList possibleFiles;
        possibleFiles << "bgm.mp3" << "bgm.wav" << "bgm.ogg";
        
        QStringList possibleDirs;
        possibleDirs << "D:/Survivors/Survivors/Assets/"
                    << QCoreApplication::applicationDirPath() + "/../Assets/"
                    << QCoreApplication::applicationDirPath() + "/Assets/";
                    
        // 构建所有可能的路径
        QStringList possiblePaths;
        for (const QString& dir : possibleDirs) {
            for (const QString& file : possibleFiles) {
                possiblePaths << dir + file;
            }
        }
        
        // 添加资源路径
        possiblePaths << ":/Assets/bgm.mp3";
        
        qDebug() << "尝试以下音频文件路径:";
        for (const QString& path : possiblePaths) {
            qDebug() << " - " << path << (QFile::exists(path) ? "【存在】" : "【不存在】");
        }
        
        // 依次尝试播放
        bool playAttempted = false;
        for (const QString& path : possiblePaths) {
            if (path.startsWith(":")) {
                // 资源路径
                if (!QFile::exists(path)) {
                    qDebug() << "资源文件不存在，跳过: " << path;
                    continue;
                }
                bgmPlayer->setSource(QUrl(path));
                qDebug() << "尝试播放资源文件:" << path;
            } else if (QFile::exists(path)) {
                // 本地文件
                bgmPlayer->setSource(QUrl::fromLocalFile(path));
                qDebug() << "尝试播放本地文件:" << path;
            } else {
                qDebug() << "文件不存在，跳过: " << path;
                continue; // 文件不存在则跳过
            }
            
            // 尝试播放
            qDebug() << "开始播放...";
            bgmPlayer->setLoops(QMediaPlayer::Infinite);
            bgmPlayer->play();
            playAttempted = true;
            
            // 检查播放状态
            QTimer::singleShot(500, this, [this, path]() {
                qDebug() << "检查播放状态: " << path;
                qDebug() << " - 播放状态: " << bgmPlayer->playbackState();
                qDebug() << " - 媒体状态: " << bgmPlayer->mediaStatus();
                qDebug() << " - 错误码: " << bgmPlayer->error();
                qDebug() << " - 错误信息: " << bgmPlayer->errorString();
                
                if (bgmPlayer->playbackState() == QMediaPlayer::PlayingState) {
                    qDebug() << "★★★ BGM成功播放: " << path << " ★★★";
                } else {
                    qDebug() << "✘✘✘ 播放失败: " << path << " ✘✘✘";
                }
            });
            
            break; // 只尝试第一个有效路径
        }
        
        if (!playAttempted) {
            qDebug() << "没有找到可用的音频文件！请确保 Assets 目录中有 bgm.mp3";
        }
        
        // 设置错误处理
        connect(bgmPlayer, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error error, const QString &errorString) {
            qDebug() << "播放错误: " << error << ": " << errorString;
        });
    } catch (const std::exception& e) {
        qDebug() << "初始化音频时异常: " << e.what();
    } catch (...) {
        qDebug() << "初始化音频时发生未知异常";
    }
    
    qDebug() << "==== BGM初始化完成 ====";
    
    // 尝试 Windows API 作为备用方案
    playBackgroundMusicWithWinAPI();
}

// 使用指定英雄类型创建游戏
GameMain::GameMain(int hero_type, QWidget *parent) : QWidget(parent), hero_type(hero_type) {
    // 初始化指针为 nullptr
    bgmPlayer = nullptr;
    audioOutput = nullptr;
    
    setupUi();
    setAttribute(Qt::WA_DeleteOnClose);

    initScene();
    m_Timer.setInterval(TIMER_RATE);
    
    // 调用初始化音频函数
    initAndPlayBGM();

    // 暂停按钮点击处理
    connect(pause_button, &QPushButton::clicked, [&]()->void { pauseButtonClicked(); });
    pause_button->setText("暂停");

    // 返回按钮点击处理
    connect(back_button, &QPushButton::clicked, [&]()->void {
        if(!game_paused) { pauseButtonClicked(); }
        this->hide();
        widget_parent->show();
        widget_parent->reportGamePaused();
    });

    // 结算按钮点击处理
    connect(end_button, &QPushButton::clicked, [&]()->void {
        if(!game_paused) { pauseButtonClicked(); }
        this->hide();
        widget_parent->reportGameOver();
        auto * new_dialog = new GameOverDialog(widget_parent, game->getEnemyDeathCnt(), game->getHPPercent());
        new_dialog->show();
        this->hide();
        delete this;
    });

    playGame();
}

// 鼠标移动事件处理
void GameMain::mouseMoveEvent(QMouseEvent *event) {
    // 只有在游戏没有暂停且当前为鼠标控制模式时才处理鼠标移动
    if (!game_paused && CONTROL_MODE == 1) {
        game->keyPressTick(nullptr); // 清除键盘状态
        
        // 由于已将GameMain添加为GameState的友元类，可以直接访问player成员
        if (game->player) {
            game->player->mouseMoveTick(event);
        }
    }
}

