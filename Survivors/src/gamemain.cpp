//
// 游戏主窗口实现文件
//

// You may need to build the project (run Qt uic code generator) to get "ui_GameMain.h" resolved

#include "gamemain.h"
#include "GameLogic/Hero.h"
#include "GameLogic/config.h"
#include "GameLogic/GameState.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include <QDebug>
#include <QKeyEvent>
#include <QPixmap>
#include <QMovie>
#include <QShortcut>
#include <QMessageBox>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMediaFormat>
#include <QUrl>
#include <iostream>
#include <QString>
#include "ui/menu.h"
#include <QFile>
#include <QIcon>

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
    maodieMovie = nullptr;
    maodieLabel = nullptr;
    maodieShortcut = nullptr;
    
    game_paused = false;
    hero_type = 0;
    
    upgradePanel = nullptr;
    upgradePanelLayout = nullptr;
    gameOverPanel = nullptr;
    gameOverLayout = nullptr;
    gameOverTitleLabel = nullptr;
    gameOverStatsLabel = nullptr;
    gameOverCloseButton = nullptr;
    
    setupUi();
    setAttribute(Qt::WA_DeleteOnClose);

    initScene();
    m_Timer.setInterval(TIMER_RATE);
    
    // 初始化猫蝶动画
    initMaodieAnimation();
    
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
        showGameOverPanel(game->getEnemyDeathCnt(), game->getHPPercent());
    });

    // 初始化升级面板
    initUpgradePanel();
    // 初始化结算面板
    initGameOverPanel();

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
     // 停止并清理动画
     if (maodieMovie) {
         maodieMovie->stop();
         delete maodieMovie;
         maodieMovie = nullptr;
     }
     // 删除快捷键
     if (maodieShortcut) {
         delete maodieShortcut;
         maodieShortcut = nullptr;
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
    
    // 设置鼠标追踪，以便能够接收鼠标事件
    setMouseTracking(true);
    
    std::cout << "[Log] GameMain::initScene 结束" << std::endl;
}

// 开始游戏
void GameMain::playGame() {
    game_paused = false;
    m_Timer.start();

    // 设置定时器回调
    connect(&m_Timer, &QTimer::timeout, [=](){
        if (game_paused) return; // 如果暂停，则不执行tick

        game->tick();
        update();
        // 更新UI显示
        enemy_cnt_label->setText("已清除敌人数量：" + QString::number(game->getEnemyDeathCnt()));
        level_label->setText("当前等级："+QString::number(game->getPlayerLevel()));
        
        // 检查游戏状态
        if(game->isGameStop()) {
            // 游戏结束处理 - 直接显示结算面板
            showGameOverPanel(game->getEnemyDeathCnt(), game->getHPPercent());
            // 注意：showGameOverPanel 内部会停止计时器和禁用按钮
            return; // 游戏已结束，不再处理升级等情况
        }
        if(game->isWaiting()) {
            // 不再创建新窗口，而是显示升级面板
            m_Timer.stop(); // 先停止计时器
            game_paused = true;
            pause_button->setEnabled(false);
            pause_button->setText("继续");
            
            // 延迟显示升级面板，避免事件循环问题
            QTimer::singleShot(100, this, [this]() {
                qDebug() << "[GameMain] 延迟显示升级面板";
                showUpgradePanel();
            });
            return;
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

// 键盘事件处理 - 添加Ctrl+M组合键直接触发Hero动画
void GameMain::keyPressEvent(QKeyEvent *event) {
    // 如果是 Ctrl+M 组合键
    if (event->key() == Qt::Key_M && (event->modifiers() & Qt::ControlModifier)) {
        // 切换Hero的动画状态
        if (game && game->getHero()) {
            if (game->getHero()->isAnimationActive()) {
                game->getHero()->stopAnimation();
            } else {
                game->getHero()->startAnimation();
            }
        }
        // 不传递事件，这里已经处理了
        event->accept();
        return;
    }
    
    // 其他按键传递给游戏逻辑
    if (game && !game_paused) {
    game->keyPressTick(event);
    }
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

// 新增函数 - 初始化猫蝶动画
void GameMain::initMaodieAnimation() {
    qDebug() << "==== 猫蝶动画初始化开始 ====";
    
    // 根据英雄类型获取对应的文件名
    QString gifFileName;
    if (hero_type == 2) {
        // 奔跑耄耋
        gifFileName = "maodie2.gif";
        qDebug() << "选择奔跑耄耋动画 (maodie2.gif)";
    } else {
        // 哈气耄耋或默认
        gifFileName = "maodie.gif";
        qDebug() << "选择哈气耄耋动画 (maodie.gif)";
    }
    
    // 尝试多种可能的文件路径
    QStringList possiblePaths;
    possiblePaths << "Assets/" + gifFileName
                << QCoreApplication::applicationDirPath() + "/Assets/" + gifFileName
                << QCoreApplication::applicationDirPath() + "/../Assets/" + gifFileName
                << "D:/Survivors/Survivors/Assets/" + gifFileName
                << "./Assets/" + gifFileName
                << "../Assets/" + gifFileName
                << ":/Assets/" + gifFileName;
    
    // 查看哪些路径存在
    qDebug() << "尝试以下猫蝶动画文件路径:";
    for (const QString& path : possiblePaths) {
        bool exists = QFile::exists(path);
        qDebug() << " - " << path << (exists ? "【存在】" : "【不存在】");
    }
    
    // 依次尝试加载
    bool loaded = false;
    for (const QString& path : possiblePaths) {
        if (QFile::exists(path)) {
            qDebug() << "尝试加载文件: " << path;
            
            // 创建QMovie对象
            maodieMovie = new QMovie(path);
            
            // 检查GIF是否有效
            if (maodieMovie->isValid()) {
                qDebug() << "★★★ 成功加载猫蝶动画: " << path << " ★★★";
                
                // 创建快捷键: Ctrl+M 显示猫蝶
                maodieShortcut = new QShortcut(QKeySequence(tr("Ctrl+M")), this);
                connect(maodieShortcut, &QShortcut::activated, this, &GameMain::onShowMaodieTriggered);
                
                loaded = true;
                break;
            } else {
                qDebug() << "✘✘✘ 文件无效: " << path << " ✘✘✘";
                delete maodieMovie;
                maodieMovie = nullptr;
            }
        }
    }
    
    // 如果所有路径都失败，尝试使用绝对路径
    if (!loaded) {
        qDebug() << "所有路径都失败，尝试硬编码的绝对路径";
        // 根据英雄类型选择正确的绝对路径
        QString absolutePath = "D:/Survivors/Survivors/Assets/" + gifFileName;
        if (QFile::exists(absolutePath)) {
            maodieMovie = new QMovie(absolutePath);
            if (maodieMovie->isValid()) {
                qDebug() << "★★★ 成功使用绝对路径加载猫蝶动画 ★★★";
                
                // 创建快捷键: Ctrl+M 显示猫蝶
                maodieShortcut = new QShortcut(QKeySequence(tr("Ctrl+M")), this);
                connect(maodieShortcut, &QShortcut::activated, this, &GameMain::onShowMaodieTriggered);
                
                loaded = true;
            } else {
                qDebug() << "✘✘✘ 绝对路径文件也无效 ✘✘✘";
                delete maodieMovie;
                maodieMovie = nullptr;
            }
        } else {
            qDebug() << "✘✘✘ 绝对路径文件不存在 ✘✘✘";
        }
    }
    
    // 加载失败最终处理
    if (!loaded) {
        qDebug() << "无法加载猫蝶动画，所有路径尝试均失败";
        maodieMovie = nullptr;
    }
    
    qDebug() << "==== 猫蝶动画初始化完成 ====";
}

// 快捷键触发显示猫蝶动画
void GameMain::onShowMaodieTriggered() {
    // 切换英雄动画状态
    if (game && game->getHero()) {
        // 切换动画状态 - 如果动画活动则停止，否则启动
        if (game->getHero()->isAnimationActive()) {
            game->getHero()->stopAnimation();
        } else {
            game->getHero()->startAnimation();
        }
        qDebug() << "英雄动画状态切换为：" << (game->getHero()->isAnimationActive() ? "动态" : "静态");
    }
    
    // 获取鼠标当前位置
    QPoint mousePos = QCursor::pos();
    // 将鼠标全局坐标转换为窗口坐标
    QPoint localPos = mapFromGlobal(mousePos);
    
    // 在鼠标位置显示动画，动画大小为100x100
    int x = localPos.x() - 50; // 减去大小的一半来居中
    int y = localPos.y() - 50;
    
    // 确保动画在窗口范围内
    x = qMax(0, qMin(x, width() - 100));
    y = qMax(0, qMin(y, height() - 100));
    
    // 显示动画
    showMaodieAnimation(x, y, 100);
}

// 新增函数 - 显示猫蝶动画
void GameMain::showMaodieAnimation(int x, int y, int size) {
    // 如果动画对象未初始化，则返回
    if (!maodieMovie) {
        qDebug() << "猫蝶动画未初始化，无法显示";
        return;
    }
    
    // 如果标签已存在，则移除它
    if (maodieLabel) {
        maodieLabel->hide();
        delete maodieLabel;
    }
    
    // 创建标签
    maodieLabel = new QLabel(this);
    maodieLabel->setMovie(maodieMovie);
    maodieLabel->setGeometry(x, y, size, size);
    maodieLabel->setAttribute(Qt::WA_TransparentForMouseEvents); // 点击穿透
    
    // 设置标签样式
    maodieLabel->setScaledContents(true); // 缩放内容以适应标签大小
    maodieLabel->setStyleSheet("background: transparent;"); // 透明背景
    
    // 显示标签
    maodieLabel->show();
    
    // 启动动画
    maodieMovie->start();
    
    // 设置定时器，自动关闭动画(5秒后)
    QTimer::singleShot(5000, this, [this]() {
        if (maodieLabel) {
            maodieLabel->hide();
            delete maodieLabel;
            maodieLabel = nullptr;
        }
    });
     }

// 使用指定英雄类型创建游戏
GameMain::GameMain(int hero_type, QWidget *parent) : QWidget(parent), hero_type(hero_type) {
    // 初始化指针为 nullptr
    bgmPlayer = nullptr;
    audioOutput = nullptr;
    maodieMovie = nullptr;
    maodieLabel = nullptr;
    maodieShortcut = nullptr;
    upgradePanel = nullptr;
    upgradePanelLayout = nullptr;
    gameOverPanel = nullptr;
    gameOverLayout = nullptr;
    gameOverTitleLabel = nullptr;
    gameOverStatsLabel = nullptr;
    gameOverCloseButton = nullptr;
    
    game_paused = false;
    
    setupUi();
    setAttribute(Qt::WA_DeleteOnClose);

    initScene();
    m_Timer.setInterval(TIMER_RATE);
    
    // 初始化猫蝶动画
    initMaodieAnimation();
    // 初始化升级面板
    initUpgradePanel();
    // 初始化结算面板
    initGameOverPanel();
    
    // 调用初始化音频函数
    initAndPlayBGM();

    // 暂停按钮点击处理
    connect(pause_button, &QPushButton::clicked, [&]()->void { pauseButtonClicked(); });
    pause_button->setText("暂停");

    connect(back_button, &QPushButton::clicked, [&]()->void {
        if(!game_paused) { pauseButtonClicked(); }
        this->hide();
        widget_parent->show();
        widget_parent->reportGamePaused();
    });

    // 修改结算按钮点击处理
    connect(end_button, &QPushButton::clicked, [&]()->void {
        if(!game_paused) { pauseButtonClicked(); }
        showGameOverPanel(game->getEnemyDeathCnt(), game->getHPPercent());
    });

    playGame();
}

// 鼠标移动事件处理
void GameMain::mouseMoveEvent(QMouseEvent *event) {
    // 只有在游戏没有暂停且当前为鼠标控制模式时才处理鼠标移动
    if (!game_paused && CONTROL_MODE == 1) {
        game->keyPressTick(nullptr); // 清除键盘状态
        
        // 使用getHero方法获取Hero对象指针
        if (game->getHero()) {
            game->getHero()->mouseMoveTick(event);
        }
    }
}

// 重写鼠标双击事件
void GameMain::mouseDoubleClickEvent(QMouseEvent *event) {
    // 切换英雄动画状态
    if (game && game->getHero()) {
        // 切换动画状态 - 如果动画活动则停止，否则启动
        if (game->getHero()->isAnimationActive()) {
            game->getHero()->stopAnimation();
        } else {
            game->getHero()->startAnimation();
        }
        qDebug() << "英雄动画状态切换为：" << (game->getHero()->isAnimationActive() ? "动态" : "静态");
    }
    
    // 在双击位置显示动画
    int x = event->position().x() - 50; // 减去大小的一半来居中
    int y = event->position().y() - 50;
    
    // 确保动画在窗口范围内
    x = qMax(0, qMin(x, width() - 100));
    y = qMax(0, qMin(y, height() - 100));
    
    // 显示动画
    showMaodieAnimation(x, y, 100);
    
    // 调用父类方法
    QWidget::mouseDoubleClickEvent(event);
}

// 在合适的位置添加新的方法
void GameMain::initUpgradePanel()
{
    // 创建升级面板（初始隐藏）
    upgradePanel = new QWidget(this);
    upgradePanel->setObjectName("upgradePanel");
    upgradePanel->setStyleSheet("QWidget#upgradePanel { background-color: rgba(0, 0, 0, 150); border-radius: 10px; }");
    upgradePanel->setGeometry(width()/2 - 150, height()/2 - 200, 300, 400);
    upgradePanel->hide();
    
    // 创建面板布局
    upgradePanelLayout = new QVBoxLayout(upgradePanel);
    
    // 创建标题标签
    QLabel *titleLabel = new QLabel("选择升级", upgradePanel);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: white; font-size: 18px; font-weight: bold; margin-bottom: 10px;");
    upgradePanelLayout->addWidget(titleLabel);
    
    // 创建升级选项按钮（初始为3个，后续可以动态调整）
    for (int i = 0; i < 3; i++) {
        QPushButton *upgradeBtn = new QPushButton(upgradePanel);
        upgradeBtn->setStyleSheet("QPushButton { color: white; background-color: #4A90E2; border-radius: 5px; padding: 10px; margin: 5px; }"
                                 "QPushButton:hover { background-color: #5AA0F2; }");
        upgradeBtn->setMinimumHeight(60);
        
        // 连接点击信号到处理函数
        connect(upgradeBtn, &QPushButton::clicked, this, [this, i]() {
            handleUpgradeChoice(i);
        });
        
        upgradeButtons.append(upgradeBtn);
        upgradePanelLayout->addWidget(upgradeBtn);
    }
    
    // 添加间隔
    upgradePanelLayout->addStretch();
}

void GameMain::showUpgradePanel()
{
    if (!upgradePanel) return;
    
    qDebug() << "[GameMain] 显示升级面板";
    
    // 根据当前英雄获取可用的升级选项（现在包含描述和图标路径）
    QVector<QPair<QString, QString>> upgradeOptions = game->getHero()->getUpgradeOptions();

    qDebug() << "获取到" << upgradeOptions.size() << "个升级选项"; // 调试：输出获取到的选项数量
    
    // 更新按钮文本和图标
    for (int i = 0; i < upgradeButtons.size(); i++) {
        if (i < upgradeOptions.size()) {
            QString descText = upgradeOptions[i].first; // 获取描述文本
            QString iconPath = upgradeOptions[i].second; // 获取图标路径

            // <<<--- 添加的调试输出 --->>>
            qDebug() << "按钮" << i << ": 描述=" << descText << ", 图标路径=" << iconPath;

            QIcon icon(iconPath); // 尝试加载图标
            if (icon.isNull()) {
                // 如果图标加载失败 (icon.isNull() 为 true)，输出警告
                qWarning() << "警告：按钮" << i << "无法加载图标！路径：" << iconPath;
            }
            // <<<-------------------->>>

            upgradeButtons[i]->setText(descText); // 设置描述文本
            upgradeButtons[i]->setIcon(icon); // 设置图标（即使是空的）
            upgradeButtons[i]->setIconSize(QSize(32, 32)); // 设置合适的图标大小
            upgradeButtons[i]->show();
        } else {
            upgradeButtons[i]->hide(); // 隐藏多余的按钮
        }
    }
    
    // 调整面板位置为居中
    upgradePanel->setGeometry(width()/2 - 150, height()/2 - 200, 300, 400);
    upgradePanel->show();
    upgradePanel->raise();
}

void GameMain::hideUpgradePanel()
{
    if (upgradePanel) {
        upgradePanel->hide();
    }
}

void GameMain::handleUpgradeChoice(int choice)
{
    qDebug() << "[GameMain] 玩家选择升级选项:" << choice;
    
    // 执行升级
    game->getHero()->upgrade(choice);
    
    // 隐藏升级面板
    hideUpgradePanel();
    
    // 恢复游戏
    resumeGame();
}

// 添加结算面板初始化方法
void GameMain::initGameOverPanel()
{
    gameOverPanel = new QWidget(this);
    gameOverPanel->setObjectName("gameOverPanel");
    gameOverPanel->setStyleSheet("QWidget#gameOverPanel { background-color: rgba(50, 50, 50, 200); border-radius: 15px; }");
    gameOverPanel->setGeometry(width()/2 - 200, height()/2 - 150, 400, 300);
    gameOverPanel->hide();

    gameOverLayout = new QVBoxLayout(gameOverPanel);
    gameOverLayout->setContentsMargins(20, 20, 20, 20);
    gameOverLayout->setSpacing(15);

    gameOverTitleLabel = new QLabel("游戏结束", gameOverPanel);
    gameOverTitleLabel->setAlignment(Qt::AlignCenter);
    gameOverTitleLabel->setStyleSheet("color: #FFD700; font-size: 24px; font-weight: bold;");
    gameOverLayout->addWidget(gameOverTitleLabel);

    gameOverStatsLabel = new QLabel("统计信息:", gameOverPanel);
    gameOverStatsLabel->setAlignment(Qt::AlignCenter);
    gameOverStatsLabel->setStyleSheet("color: white; font-size: 16px;");
    gameOverStatsLabel->setWordWrap(true);
    gameOverLayout->addWidget(gameOverStatsLabel);

    gameOverLayout->addStretch(); // 添加弹性空间

    gameOverCloseButton = new QPushButton("返回主菜单", gameOverPanel);
    gameOverCloseButton->setStyleSheet("QPushButton { color: white; background-color: #E74C3C; border-radius: 8px; padding: 12px; font-size: 16px; }" 
                                      "QPushButton:hover { background-color: #C0392B; }");
    gameOverCloseButton->setMinimumHeight(50);
    connect(gameOverCloseButton, &QPushButton::clicked, this, &GameMain::handleGameOverClose);
    gameOverLayout->addWidget(gameOverCloseButton);
}

// 添加显示结算面板方法
void GameMain::showGameOverPanel(int enemyCount, double hpPercent)
{
    if (!gameOverPanel) return;

    qDebug() << "[GameMain] 显示结算面板";
    // 停止计时器并禁用按钮（如果尚未完成）
    m_Timer.stop();
    game_paused = true;
    pause_button->setEnabled(false);
    back_button->setEnabled(false);
    end_button->setEnabled(false);
    enemy_cnt_label->setText("游戏已结束");

    // 更新统计信息
    QString statsText = QString("最终得分:\n击杀敌人数量: %1\n剩余生命值: %2%").arg(enemyCount).arg(QString::number(hpPercent * 100, 'f', 1));
    gameOverStatsLabel->setText(statsText);

    // 显示面板
    gameOverPanel->setGeometry(width()/2 - 200, height()/2 - 150, 400, 300);
    gameOverPanel->show();
    gameOverPanel->raise(); // 确保在最顶层
}

// 添加隐藏结算面板方法
void GameMain::hideGameOverPanel()
{
    if (gameOverPanel) {
        gameOverPanel->hide();
    }
}

// 添加处理结算面板关闭方法
void GameMain::handleGameOverClose()
{
    qDebug() << "[GameMain] 关闭结算面板，返回主菜单";
    this->hide();
    if (widget_parent) { // 确保父窗口指针有效
        widget_parent->show();
        widget_parent->reportGameOver(); // 通知主菜单游戏结束
    } else {
        qWarning() << "[GameMain] widget_parent 为空，无法返回主菜单";
    }
    delete this; // 删除当前游戏窗口实例
}

