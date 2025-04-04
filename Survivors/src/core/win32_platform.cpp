#include "defines.h"
#include "../utils/my_math.h"
#include "input.h"
#include "platform.h"
#include "../utils/logger.h"
#include "common.h"
#include "config.h"
#include "shared.h"
#include "../utils/sound.h"

// 资源层
#include "assets.cpp"

// 渲染层
#include <windows.h>
#include <windowsx.h>
#include "../rendering/gl_renderer.cpp"

// 音频
#include <xaudio2.h>
#include <tlhelp32.h>

struct XAudioVoice : IXAudio2VoiceCallback
{
    bool playing;
    IXAudio2SourceVoice *voice;

    // 仅实现必要的回调函数
    void OnStreamEnd() { playing = false; }
#pragma warning(disable : 4100)
    void OnBufferStart(void *pBufferContext) { playing = true; }

    // 未使用的回调函数
    void OnVoiceProcessingPassEnd() {}
    void OnVoiceProcessingPassStart(UINT32 SamplesRequired) {}
    void OnBufferEnd(void *pBufferContext) {}
    void OnLoopEnd(void *pBufferContext) {}
    void OnVoiceError(void *pBufferContext, HRESULT Error) {}
#pragma warning(default : 4100)
};

struct XAudioState
{
    IXAudio2 *device;
    IXAudio2MasteringVoice *masteringVoice;
    XAudioVoice voices[MAX_PLAYING_SOUNDS];
};

GLOBAL XAudioState xAudioState;

// 内存管理
GLOBAL int persistentBytesUsed = 0;
GLOBAL int transientBytesUsed = 0;
GLOBAL char *persistentBuffer = 0;
GLOBAL char *transientBuffer = 0;

GLOBAL bool running = true;
GLOBAL HWND window;

// 窗口回调函数
LRESULT CALLBACK window_callback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    switch (msg)
    {
    case WM_CLOSE:
    {
        running = false;
        break;
    }

    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }

    case WM_SIZE:
    {
        // 设置屏幕尺寸
        RECT r;
        GetClientRect(window, &r);

        input->screenSize.x = (r.right - r.left);
        input->screenSize.y = (r.bottom - r.top);

        renderer_resize();
        break;
    }

    case WM_KEYDOWN:
    case WM_KEYUP:
    {
        if (wParam < KEY_COUNT)
        {
            bool isDown = !(HIWORD(lParam) & KF_UP);
            int isEcho = isDown && ((lParam >> 30) & 1);
            if (!isEcho)
            {
                input->keys[wParam].halfTransitionCount += 1;
                input->keys[wParam].isDown = isDown;
            }
        }
        break;
    }
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    {
        bool isDown = (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN)
                          ? true
                          : false;

        input->keys[wParam].isDown = isDown;
        input->keys[wParam].halfTransitionCount++;
        break;
    }

    case WM_MOUSEMOVE:
    {
        input->oldMousePos = input->mousePosScreen;

        // 鼠标位置
        input->mousePosScreen.x = (float)GET_X_LPARAM(lParam);
        input->mousePosScreen.y = (float)GET_Y_LPARAM(lParam);

        // 相对移动
        input->relMouseScreen = input->mousePosScreen - input->oldMousePos;
        break;
    }

    default:
    {
        result = DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    }

    return result;
}

// 创建窗口
STATIC bool platform_create_window(int width, int height, char *title)
{
    HINSTANCE instance = GetModuleHandleA(0);

    // 设置并注册窗口类
    HICON icon = LoadIcon(instance, IDI_APPLICATION);
    WNDCLASS wc = {};
    wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = window_callback;
    wc.hInstance = instance;
    wc.hIcon = icon;
    wc.lpszClassName = "game_window_class";

    if (!RegisterClassA(&wc))
    {
        MessageBoxA(0, "Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    // 创建窗口
    uint32_t client_x = 100;
    uint32_t client_y = 100;
    uint32_t client_width = width;
    uint32_t client_height = height;

    uint32_t window_x = client_x;
    uint32_t window_y = client_y;
    uint32_t window_width = client_width;
    uint32_t window_height = client_height;

    uint32_t window_style =
        WS_OVERLAPPED |
        WS_SYSMENU |
        WS_CAPTION |
        WS_THICKFRAME |
        WS_MINIMIZEBOX |
        WS_MAXIMIZEBOX;

    uint32_t window_ex_style = WS_EX_APPWINDOW | WS_EX_TOPMOST;
    window_ex_style = WS_EX_APPWINDOW;

    // 获取边框大小
    RECT border_rect = {};
    AdjustWindowRectEx(&border_rect,
                       (DWORD)window_style,
                       0,
                       (DWORD)window_ex_style);

    window_x += border_rect.left;
    window_y += border_rect.top;

    window_width += border_rect.right - border_rect.left;
    window_height += border_rect.bottom - border_rect.top;

    window = CreateWindowExA((DWORD)window_ex_style,
                             "game_window_class", title,
                             (DWORD)window_style, window_x, window_y, window_width, window_height,
                             0, 0, instance, 0);

    if (window == 0)
    {
        MessageBoxA(NULL, "Window creation failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    // 显示窗口
    ShowWindow(window, SW_SHOW);

    return true;
}

// 更新窗口
STATIC void platform_update_window()
{
    MSG msg;

    while (PeekMessageA(&msg, window, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

// 初始化音频系统
STATIC bool init_audio()
{
    // 分配内存
    soundState->buffer = platform_allocate_persistent(MAX_BYTES_SOUND_BUFFER);
    if (!soundState->buffer)
    {
        ASSERT(0, "分配声音缓冲区失败");
        return false;
    }

    if (CoInitializeEx(0, COINIT_MULTITHREADED) != 0)
    {
        ASSERT(0, "初始化xAudio2失败");
        return false;
    }

    if (XAudio2Create(&xAudioState.device, 0, XAUDIO2_USE_DEFAULT_PROCESSOR) != 0)
    {
        ASSERT(0, "创建xAudio2设备失败");
        return false;
    }

    if (xAudioState.device->CreateMasteringVoice(&xAudioState.masteringVoice) != 0)
    {
        ASSERT(0, "创建xAudio2主声音失败");
        return false;
    }

    // 设置WAV格式
    WAVEFORMATEX waveFormat = {0};
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nChannels = 2;          // 立体声
    waveFormat.wBitsPerSample = 16;    // 16位
    waveFormat.nSamplesPerSec = 48000; // 48 kHz
    waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    waveFormat.cbSize = 0;

    for (int voiceIdx = 0; voiceIdx < MAX_PLAYING_SOUNDS; voiceIdx++)
    {
        XAudioVoice *xAudioVoice = &xAudioState.voices[voiceIdx];

        int result = xAudioState.device->CreateSourceVoice(&xAudioVoice->voice,
                                                           &waveFormat, XAUDIO2_VOICE_NOPITCH,
                                                           XAUDIO2_DEFAULT_FREQ_RATIO,
                                                           (IXAudio2VoiceCallback *)xAudioVoice,
                                                           0);

        // 调整音量
        xAudioVoice->voice->SetVolume(0.05f);

        if (result)
        {
            ASSERT(0, "创建xAudio2声音失败");
            return false;
        }
    }

    return true;
}

typedef void(init_game_type)(GameState *, Input *, RenderData *);
typedef void(update_game_type)(GameState *, Input *, RenderData *, float);

int main()
{
    // 分配内存
    transientBuffer = (char *)malloc(TRANSIENT_BUFFER_SIZE);
    persistentBuffer = (char *)malloc(PERSISTENT_BUFFER_SIZE);

    input = (Input *)platform_allocate_persistent(sizeof(Input));
    if (!input)
    {
        FATAL("Failed to allocate Memory for Input!");
        return -1;
    }

    soundState = (SoundState *)platform_allocate_persistent(sizeof(SoundState));
    if (!input)
    {
        FATAL("Failed to allocate Memory for Sounds!");
        return -1;
    }

    RenderData *renderData = (RenderData *)platform_allocate_persistent(sizeof(RenderData));
    if (!renderData)
    {
        FATAL("Failed to allocate Memory for RenderData!");
        return -1;
    }

    GameState *gameState = (GameState *)platform_allocate_persistent(sizeof(GameState));
    if (!gameState)
    {
        FATAL("Failed to allocate Memory for the GameState!");
        return -1;
    }

    // 游戏DLL相关
    init_game_type *init_game = 0;
    update_game_type *update_game = 0;

    long long lastEditDLLTimestamp = 0;
    HMODULE gameDLL = NULL;

    // 时间相关
    GLOBAL LARGE_INTEGER ticksPerSecond;
    LARGE_INTEGER lastTickCount, currentTickCount;
    QueryPerformanceFrequency(&ticksPerSecond);
    QueryPerformanceCounter(&lastTickCount);
    float dt = 0.0f;

    platform_create_window(SCREEN_SIZE.x, SCREEN_SIZE.y, "VSClone");

    gl_init(window, renderData);
    renderer_set_vertical_sync(false);

    // 初始化音频
    init_audio();
    platform_play_sound(SOUND_BACKGROUND, true);

    bool isGameInitialized = false;

    // 随机数种子
    srand((uint32_t)__rdtsc());

    while (running)
    {
        running = !gameState->quitApp;

        // DLL热重载
        {
            long long DLLTimestamp = platform_last_edit_timestamp("game.dll");
            if (DLLTimestamp > lastEditDLLTimestamp)
            {
                lastEditDLLTimestamp = DLLTimestamp;
                printf("新DLL已找到\n");

                if (gameDLL)
                {
                    BOOL freeResult = FreeLibrary(gameDLL);
                    assert(freeResult);
                    gameDLL = NULL;
                    printf("释放库\n");
                }

                while (!CopyFile("game.dll", "game_load.dll", false))
                {
                    Sleep(10);
                }
                printf("复制DLL\n");
                while (true)
                {
                    gameDLL = LoadLibrary("game_load.dll");
                    if (gameDLL)
                    {
                        break;
                    }
                    Sleep(10);
                }
                printf("加载DLL\n");

                init_game = (init_game_type *)GetProcAddress(gameDLL, "init_game");
                ASSERT(init_game, "从游戏DLL加载init_game函数失败");
                update_game = (update_game_type *)GetProcAddress(gameDLL, "update_game");
                ASSERT(update_game, "从游戏DLL加载update_game函数失败");
            }
        }

        if (!isGameInitialized)
        {
            isGameInitialized = true;
            init_game(gameState, input, renderData);
        }

        // 重置临时内存
        transientBytesUsed = 0;

        // 计算时间增量
        {
            QueryPerformanceCounter(&currentTickCount);

            uint64_t elapsedTicks = currentTickCount.QuadPart - lastTickCount.QuadPart;

            // 转换为微秒以保持精度
            uint64_t elapsedTimeInMicroseconds = (elapsedTicks * 1000000) / ticksPerSecond.QuadPart;

            lastTickCount = currentTickCount;

            // 毫秒
            dt = (float)elapsedTimeInMicroseconds / 1000.0f;

            // 限制dt最大值为50ms
            if (dt > 50.0f)
            {
                dt = 50.0f;
            }

            // 转换为秒
            dt /= 1000.0f;
        }

        platform_update_window();

        // 保存游戏状态
        if (is_key_pressed(KEY_K))
        {
            if (platform_file_exists("gameState.bin"))
            {
                platform_delete_file("gameState.bin");
            }
            platform_write_file("gameState.bin", (char *)gameState, sizeof(GameState), true);
        }

        // 加载游戏状态
        if (is_key_pressed(KEY_L))
        {
            uint32_t fileSize;
            char *buffer = platform_read_file("gameState.bin", &fileSize);

            if (buffer)
            {
                if (fileSize == sizeof(GameState))
                {
                    memcpy(gameState, buffer, fileSize);
                }
                else
                {
                    ASSERT(0, "游戏状态大小已改变!");
                }
            }
            else
            {
                ASSERT(0, "读取游戏状态失败");
            }
        }

        update_game(gameState, input, renderData, dt);
        for (int key_i = 0; key_i < KEY_COUNT; key_i++)
        {
            input->keys[key_i].halfTransitionCount = 0;
        }

        gl_render();
    }

    return 0;
}

// platform.h实现
void platform_log(char *msg, TextColor color)
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    uint32_t colorBits = 0;

    switch (color)
    {
    case TEXT_COLOR_WHITE:
        colorBits = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
        break;

    case TEXT_COLOR_GREEN:
        colorBits = FOREGROUND_GREEN;
        break;

    case TEXT_COLOR_YELLOW:
        colorBits = FOREGROUND_GREEN | FOREGROUND_RED;
        break;

    case TEXT_COLOR_RED:
        colorBits = FOREGROUND_RED;
        break;

    case TEXT_COLOR_LIGHT_RED:
        colorBits = FOREGROUND_RED | FOREGROUND_INTENSITY;
        break;
    }

    SetConsoleTextAttribute(consoleHandle, (WORD)colorBits);

#ifdef DEBUG
    OutputDebugStringA(msg);
#endif

    WriteConsoleA(consoleHandle, msg, (int)strlen(msg), 0, 0);
}

// 输出错误信息
void platform_print_error()
{
    int error_ = GetLastError();
    LPTSTR error_text = 0;

    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER |
                      FORMAT_MESSAGE_IGNORE_INSERTS,
                  0,
                  error_,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPTSTR)&error_text,
                  0,
                  0);

    if (error_text)
    {
        ASSERT(0, "%s", error_text);
        LocalFree(error_text);
    }
}

// 读取文件
char *platform_read_file(char *path, uint32_t *fileSize)
{
    char *buffer = 0;

    if (fileSize)
    {
        HANDLE file = CreateFile(path, GENERIC_READ, FILE_SHARE_READ,
                                 0, OPEN_EXISTING, 0, 0);

        if (file != INVALID_HANDLE_VALUE)
        {
            LARGE_INTEGER fSize;
            if (GetFileSizeEx(file, &fSize))
            {
                *fileSize = (uint32_t)fSize.QuadPart;
                buffer = platform_allocate_transient(*fileSize + 1);

                if (buffer)
                {
                    DWORD bytesRead;
                    if (ReadFile(file, buffer, *fileSize, &bytesRead, 0) &&
                        *fileSize == bytesRead)
                    {
                    }
                    else
                    {
                        WARN("读取文件 %s 失败", path);
                        buffer = 0;
                    }
                }
                else
                {
                    ASSERT(0, "无法分配: %d 字节，用于加载文件 %s",
                                 *fileSize, path);
                    WARN("Could not allocate: %d bytes, to Load file %s",
                               *fileSize, path);
                }
            }
            else
            {
                WARN("获取文件 %s 大小失败", path);
            }

            CloseHandle(file);
        }
        else
        {
            WARN("打开文件 %s 失败", path);
        }
    }
    else
    {
        ASSERT(0, "未提供长度!");
        WARN("未提供长度!");
    }

    return buffer;
}

// 写入文件
unsigned long platform_write_file(char *path,
                                  char *buffer,
                                  uint32_t size,
                                  bool overwrite)
{
    DWORD bytesWritten = 0;

    HANDLE file = CreateFile(path,
                             overwrite ? GENERIC_WRITE : FILE_APPEND_DATA,
                             FILE_SHARE_WRITE, 0, OPEN_ALWAYS, 0, 0);

    if (file != INVALID_HANDLE_VALUE)
    {
        if (!overwrite)
        {
            DWORD result = SetFilePointer(file, 0, 0, FILE_END);
            if (result == INVALID_SET_FILE_POINTER)
            {
                WARN("设置文件指针到末尾失败");
            }
        }

        BOOL result = WriteFile(file, buffer, size, &bytesWritten, 0);
        if (result && size == bytesWritten)
        {
            // 成功
        }
        else
        {
            WARN("写入文件 %s 失败", path);
        }
        CloseHandle(file);
    }
    else
    {
        WARN("打开文件 %s 失败", path);
    }

    return bytesWritten;
}

// 检查文件是否存在
bool platform_file_exists(char *path)
{
    DWORD attributes = GetFileAttributes(path);

    return (attributes != INVALID_FILE_ATTRIBUTES &&
            !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}

// 删除文件
void platform_delete_file(char *path)
{
    ASSERT(DeleteFileA(path) != 0, "删除文件失败: %s", path);
}

// 获取文件最后修改时间
long long platform_last_edit_timestamp(char *path)
{
    long long time = 0;

    HANDLE file = CreateFile(path, GENERIC_READ, FILE_SHARE_WRITE,
                             0, OPEN_EXISTING, 0, 0);

    if (file != INVALID_HANDLE_VALUE)
    {
        FILETIME writeTime;
        if (GetFileTime(file, 0, 0, &writeTime))
        {
            ULARGE_INTEGER tmp = {writeTime.dwLowDateTime, writeTime.dwHighDateTime};
            time = tmp.QuadPart;
        }
        else
        {
            WARN("获取文件 %s 时间失败", path);
        }

        CloseHandle(file);
    }
    else
    {
        WARN("打开文件 %s 失败", path);
    }

    return time;
}

// 播放音效
void platform_play_sound(SoundID soundID, bool loop)
{
    Sound *sound = 0;
    for (int soundIdx = 0; soundIdx < soundState->allocatedSoundsCount; soundIdx++)
    {
        Sound *s = &soundState->allocatedSounds[soundIdx];

        if (s->ID == soundID)
        {
            sound = s;
            break;
        }
    }

    if (!sound)
    {
        if (soundState->allocatedSoundsCount < MAX_ALLOCATED_SOUNDS)
        {
            sound = &soundState->allocatedSounds[soundState->allocatedSoundsCount++];
        }

        // 从磁盘加载新的音效文件
        uint32_t fileSize = 0;
        char *soundFile = platform_read_file(SoundFiles[soundID], &fileSize);

        if (soundFile)
        {
            // 找到WAV文件的数据部分
            WaveDataChunk *dataChunk = (WaveDataChunk *)(soundFile + sizeof(WaveFileHeader));
            while (*(uint32_t *)&dataChunk->dataChunkId != FOURCC("data"))
            {
                dataChunk = (WaveDataChunk *)((char *)(dataChunk) + sizeof(WaveDataChunk) + dataChunk->dataSize);
            }

            if ((int)dataChunk->dataSize < MAX_BYTES_SOUND_BUFFER - soundState->bytesUsed)
            {
                sound->ID = soundID;
                sound->data = &soundState->buffer[soundState->bytesUsed];
                sound->sizeInBytes = dataChunk->dataSize;

                char *soundData = (char *)dataChunk + sizeof(WaveDataChunk);

                memcpy(sound->data, soundData, dataChunk->dataSize);
                soundState->bytesUsed += dataChunk->dataSize;
            }
            else
            {
                ASSERT(0, "声音缓冲区已满");
            }
        }
        else
        {
            ASSERT(0, "加载声音失败");
        }
    }

    // 将音效加载到xaudio
    {
        for (int voiceIdx = 0; voiceIdx < MAX_PLAYING_SOUNDS; voiceIdx++)
        {
            XAudioVoice xAudioVoice = xAudioState.voices[voiceIdx];

            if (xAudioVoice.playing)
            {
                continue;
            }

            XAUDIO2_BUFFER stupidBuffer = {0};
            stupidBuffer.AudioBytes = sound->sizeInBytes;
            stupidBuffer.pAudioData = (BYTE *)sound->data;
            stupidBuffer.Flags = XAUDIO2_END_OF_STREAM;
            stupidBuffer.LoopCount = loop ? XAUDIO2_MAX_LOOP_COUNT : 0;

            if (int result = xAudioVoice.voice->SubmitSourceBuffer(&stupidBuffer))
            {
                ASSERT(0, "播放声音失败: %d", sound->ID);
            }
            xAudioVoice.voice->Start(0, 0);
        }
    }
}

// 分配临时内存
char *platform_allocate_transient(uint32_t sizeInBytes)
{
    char *buffer = 0;

    if (transientBytesUsed + sizeInBytes < TRANSIENT_BUFFER_SIZE)
    {
        buffer = transientBuffer + transientBytesUsed;
        memset(buffer, 0, sizeInBytes);
        transientBytesUsed += sizeInBytes;
    }
    else
    {
        ASSERT(0, "临时存储已用尽!");
    }

    return buffer;
}

// 分配持久内存
char *platform_allocate_persistent(uint32_t sizeInBytes)
{
    char *buffer = 0;

    if (persistentBytesUsed + sizeInBytes < PERSISTENT_BUFFER_SIZE)
    {
        buffer = persistentBuffer + persistentBytesUsed;
        memset(buffer, 0, sizeInBytes);
        persistentBytesUsed += sizeInBytes;
    }
    else
    {
        ASSERT(0, "持久存储已用尽!");
    }

    return buffer;
}
