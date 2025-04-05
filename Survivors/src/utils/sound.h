#pragma once
#include "../core/config.h"
#include "../core/platform.h"
#include "../core/defines.h"
#include "../utils/kyc_macros.h"
#include "../core/gameData.h"

// WAV File Data
struct WaveFileHeader
{
    char chunkId[4];
    uint32_t chunkSize;
    char format[4];
    char subChunkId[4];
    uint32_t subChunkSize;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t bytesPerSecond;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
};

struct WaveDataChunk
{
    char dataChunkId[4];
    uint32_t dataSize;
};

struct Sound
{
    SoundID ID;
    int sizeInBytes;
    char *data;
};

// 为SoundID添加==运算符
inline bool operator==(SoundID lhs, SoundID rhs)
{
    return static_cast<int>(lhs) == static_cast<int>(rhs);
}

struct QueuedSound
{
    SoundID ID;
    bool loop = false;
};

struct SoundState
{
    int queuedSoundCount;
    QueuedSound queuedSounds[MAX_PLAYING_SOUNDS];

    int bytesUsed;
    char *buffer;
    int allocatedSoundsCount;
    Sound allocatedSounds[MAX_ALLOCATED_SOUNDS];
};

GLOBAL SoundState *soundState;

void play_sound(SoundID soundID, bool loop = false)
{
    if (soundState->queuedSoundCount < MAX_PLAYING_SOUNDS)
    {
        soundState->queuedSounds[soundState->queuedSoundCount++] = {soundID, loop};
    }
    else
    {
        assert(0);
    }
}
