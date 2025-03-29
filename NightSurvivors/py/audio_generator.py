#!/usr/bin/env python
import numpy as np
import os
import random
from scipy.io import wavfile
from PIL import Image

def generate_tone(freq, duration, sample_rate=44100, volume=0.5):
    """
    生成单一频率的音调
    
    参数:
        freq: 频率(Hz)
        duration: 持续时间(秒)
        sample_rate: 采样率
        volume: 音量(0.0-1.0)
    """
    t = np.linspace(0, duration, int(sample_rate * duration), False)
    tone = np.sin(freq * 2 * np.pi * t) * volume
    
    # 应用淡入淡出以避免爆音
    fade_duration = min(0.01, duration / 10)
    fade_samples = int(fade_duration * sample_rate)
    
    # 淡入
    if fade_samples > 0:
        tone[:fade_samples] *= np.linspace(0, 1, fade_samples)
        
        # 淡出
        tone[-fade_samples:] *= np.linspace(1, 0, fade_samples)
    
    return tone

def generate_white_noise(duration, sample_rate=44100, volume=0.1):
    """生成白噪声"""
    samples = np.random.normal(0, 1, int(sample_rate * duration))
    return samples * volume

def generate_brown_noise(duration, sample_rate=44100, volume=0.3):
    """生成褐噪声(更加低沉)"""
    samples = np.random.normal(0, 1, int(sample_rate * duration))
    
    # 通过积分将白噪声转换为褐噪声
    brown = np.cumsum(samples)
    
    # 归一化
    brown = brown / np.max(np.abs(brown))
    
    return brown * volume

def apply_reverb(audio, delay=0.1, decay=0.5, sample_rate=44100):
    """应用简单的混响效果"""
    delay_samples = int(delay * sample_rate)
    reverb = np.zeros_like(audio)
    
    # 添加延迟并衰减原始信号
    reverb[delay_samples:] += audio[:-delay_samples] * decay
    
    return audio + reverb

def apply_envelope(audio, attack=0.01, decay=0.1, sustain=0.7, release=0.2, sample_rate=44100):
    """
    应用ADSR包络
    
    参数:
        audio: 输入音频
        attack: 起音时间(秒)
        decay: 衰减时间(秒)
        sustain: 持续电平(0.0-1.0)
        release: 释放时间(秒)
    """
    total_samples = len(audio)
    envelope = np.ones_like(audio)
    
    # 计算各阶段的样本数
    attack_samples = int(attack * sample_rate)
    decay_samples = int(decay * sample_rate)
    release_samples = int(release * sample_rate)
    
    # 计算各阶段的结束位置
    attack_end = attack_samples
    decay_end = attack_end + decay_samples
    sustain_end = total_samples - release_samples
    
    # 构建包络
    if attack_samples > 0:
        envelope[:attack_end] = np.linspace(0, 1, attack_samples)
    
    if decay_samples > 0:
        envelope[attack_end:decay_end] = np.linspace(1, sustain, decay_samples)
    
    if decay_end < sustain_end:
        envelope[decay_end:sustain_end] = sustain
    
    if release_samples > 0:
        envelope[sustain_end:] = np.linspace(sustain, 0, release_samples)
    
    return audio * envelope

def create_music_track(duration=30, type="main_theme", bpm=120, sample_rate=44100):
    """
    生成音乐轨道
    
    参数:
        duration: 持续时间(秒)
        type: 音乐类型
        bpm: 每分钟节拍数
        sample_rate: 采样率
    """
    # 根据类型设置参数
    if type == "main_theme":
        key_freq = 440.0  # A4
        scale_type = "major"
        rhythm_density = 0.7
        
    elif type == "battle":
        key_freq = 392.0  # G4
        scale_type = "minor"
        rhythm_density = 0.9
        bpm = 140
        
    elif type == "boss":
        key_freq = 311.1  # Eb4
        scale_type = "diminished"
        rhythm_density = 0.8
        bpm = 160
        
    elif type == "victory":
        key_freq = 523.3  # C5
        scale_type = "major"
        rhythm_density = 0.6
        bpm = 100
        
    elif type == "game_over":
        key_freq = 349.2  # F4
        scale_type = "minor"
        rhythm_density = 0.5
        bpm = 80
        
    else:
        key_freq = 440.0  # A4
        scale_type = "major"
        rhythm_density = 0.7
    
    # 计算音阶频率
    # 主音阶
    if scale_type == "major":
        intervals = [0, 2, 4, 5, 7, 9, 11]  # 大调
    elif scale_type == "minor":
        intervals = [0, 2, 3, 5, 7, 8, 10]  # 小调
    elif scale_type == "diminished":
        intervals = [0, 2, 3, 5, 6, 8, 9, 11]  # 减音阶
    else:
        intervals = [0, 2, 4, 5, 7, 9, 11]  # 默认大调
        
    # 计算所有音符的频率
    base_note = key_freq / (2 ** (intervals[0] / 12))
    scale_freqs = []
    
    # 两个八度的音阶
    for octave in range(2):
        for semitone in intervals:
            freq = base_note * (2 ** ((semitone + 12 * octave) / 12))
            scale_freqs.append(freq)
    
    # 计算节拍长度
    beat_duration = 60 / bpm
    
    # 创建空轨道
    total_samples = int(sample_rate * duration)
    track = np.zeros(total_samples)
    
    # 生成音乐结构 (简单的4小节循环)
    beats_per_bar = 4
    bars = int(duration / (beats_per_bar * beat_duration))
    
    # 创建低音轨道
    bass_pattern = []
    for bar in range(bars):
        for beat in range(beats_per_bar):
            # 每小节的第一拍和第三拍放低音
            if beat == 0 or beat == 2:
                root_freq = scale_freqs[0] / 2  # 降一个八度
                fifth_freq = scale_freqs[4] / 2  # 五度
                
                # 随机选择根音或五度
                freq = root_freq if bar % 2 == 0 or beat == 0 else fifth_freq
                
                start_sample = int((bar * beats_per_bar + beat) * beat_duration * sample_rate)
                end_sample = start_sample + int(beat_duration * sample_rate)
                
                if start_sample < total_samples:
                    end_sample = min(end_sample, total_samples)
                    bass_note = generate_tone(freq, beat_duration, sample_rate, volume=0.4)
                    
                    # 应用低通滤波来模拟低音
                    bass_note = apply_envelope(bass_note, attack=0.01, decay=0.1, sustain=0.4, release=0.2, sample_rate=sample_rate)
                    
                    track[start_sample:end_sample] += bass_note[:end_sample-start_sample]
    
    # 创建旋律轨道
    melody_pattern = []
    prev_note_idx = 0
    
    for bar in range(bars):
        for beat in range(beats_per_bar):
            # 根据节奏密度决定是否放音符
            if random.random() < rhythm_density:
                # 旋律音符 - 趋向于顺阶进行
                max_jump = 2  # 限制音符跳跃
                available_notes = list(range(max(0, prev_note_idx - max_jump), 
                                          min(len(scale_freqs), prev_note_idx + max_jump + 1)))
                
                note_idx = random.choice(available_notes)
                freq = scale_freqs[note_idx]
                prev_note_idx = note_idx
                
                # 确定音符长度 (1/4, 1/8, 1/16 音符)
                duration_options = [beat_duration, beat_duration/2, beat_duration/4]
                note_duration = random.choice(duration_options)
                
                start_sample = int((bar * beats_per_bar + beat) * beat_duration * sample_rate)
                end_sample = start_sample + int(note_duration * sample_rate)
                
                if start_sample < total_samples:
                    end_sample = min(end_sample, total_samples)
                    melody_note = generate_tone(freq, note_duration, sample_rate, volume=0.3)
                    
                    # 应用包络
                    melody_note = apply_envelope(melody_note, attack=0.01, decay=0.1, sustain=0.6, release=0.1, sample_rate=sample_rate)
                    
                    # 添加简单的颤音
                    if random.random() < 0.2:  # 20%的音符添加颤音
                        vibrato_freq = 6  # 颤音频率
                        vibrato_depth = 0.03  # 颤音深度
                        t = np.linspace(0, note_duration, len(melody_note), False)
                        vibrato = 1.0 + vibrato_depth * np.sin(vibrato_freq * 2 * np.pi * t)
                        melody_note = melody_note * vibrato
                    
                    track[start_sample:end_sample] += melody_note[:end_sample-start_sample]
    
    # 添加打击乐
    if type in ["battle", "boss"]:
        for bar in range(bars):
            for beat in range(beats_per_bar):
                # 鼓点 - 在第一拍和第三拍
                if beat == 0 or beat == 2:
                    start_sample = int((bar * beats_per_bar + beat) * beat_duration * sample_rate)
                    
                    # 低鼓
                    kick_duration = 0.1
                    kick = generate_tone(60, kick_duration, sample_rate, volume=0.4)
                    kick = apply_envelope(kick, attack=0.001, decay=0.1, sustain=0.0, release=0.0, sample_rate=sample_rate)
                    
                    end_sample = start_sample + len(kick)
                    if end_sample <= total_samples:
                        track[start_sample:end_sample] += kick
                
                # 军鼓 - 在第二拍和第四拍
                if beat == 1 or beat == 3:
                    start_sample = int((bar * beats_per_bar + beat) * beat_duration * sample_rate)
                    
                    # 军鼓
                    snare_duration = 0.1
                    snare_base = generate_white_noise(snare_duration, sample_rate, volume=0.3)
                    snare = apply_envelope(snare_base, attack=0.001, decay=0.05, sustain=0.0, release=0.05, sample_rate=sample_rate)
                    
                    end_sample = start_sample + len(snare)
                    if end_sample <= total_samples:
                        track[start_sample:end_sample] += snare
    
    # 添加氛围
    if type in ["boss", "game_over"]:
        ambient_noise = generate_brown_noise(duration, sample_rate, volume=0.1)
        track += ambient_noise
    
    # 应用整体效果
    if type in ["main_theme", "victory"]:
        track = apply_reverb(track, delay=0.2, decay=0.3)
    elif type in ["battle", "boss"]:
        track = apply_reverb(track, delay=0.1, decay=0.2)
    
    # 归一化到 [-1, 1] 范围
    max_amplitude = np.max(np.abs(track))
    if max_amplitude > 0:
        track = track / max_amplitude * 0.9  # 留出一些余量避免削波
    
    # 转换为16位PCM
    track = (track * 32767).astype(np.int16)
    
    return track, sample_rate

def create_sound_effect(effect_type="attack", duration=0.5, sample_rate=44100):
    """
    生成游戏音效
    
    参数:
        effect_type: 音效类型
        duration: 持续时间(秒)
        sample_rate: 采样率
    """
    # 创建空音效
    effect = np.zeros(int(sample_rate * duration))
    
    if effect_type == "attack":
        # 攻击音效 - 快速上升的音调
        base_freq = 300
        sweep_duration = 0.15
        t = np.linspace(0, sweep_duration, int(sample_rate * sweep_duration))
        sweep = np.sin(2 * np.pi * (base_freq + 200 * t) * t) * 0.7
        
        # 应用快速衰减
        sweep = apply_envelope(sweep, attack=0.001, decay=0.1, sustain=0.1, release=0.05, sample_rate=sample_rate)
        
        # 添加一些噪声作为打击感
        attack_noise = generate_white_noise(sweep_duration, sample_rate, volume=0.3)
        attack_noise = apply_envelope(attack_noise, attack=0.001, decay=0.05, sustain=0.0, release=0.02, sample_rate=sample_rate)
        
        sweep += attack_noise
        effect[:len(sweep)] += sweep
        
    elif effect_type == "hit":
        # 受击音效 - 闷响
        thud_freq = 150
        thud_duration = 0.2
        t = np.linspace(0, thud_duration, int(sample_rate * thud_duration))
        thud = np.sin(2 * np.pi * thud_freq * t) * 0.8
        
        # 添加噪声作为打击感
        hit_noise = generate_white_noise(thud_duration, sample_rate, volume=0.4)
        
        # 混合并应用快速衰减
        thud += hit_noise
        thud = apply_envelope(thud, attack=0.001, decay=0.07, sustain=0.0, release=0.1, sample_rate=sample_rate)
        
        effect[:len(thud)] += thud
        
    elif effect_type == "death":
        # 死亡音效 - 下降音调和噪声
        fall_duration = 0.5
        base_freq = 300
        t = np.linspace(0, fall_duration, int(sample_rate * fall_duration))
        
        # 音调从高到低
        freq_mod = base_freq * np.exp(-3 * t)
        fall = np.sin(2 * np.pi * freq_mod * t) * 0.6
        
        # 结合噪声
        noise = generate_brown_noise(fall_duration, sample_rate, volume=0.3)
        
        # 混合并应用包络
        fall += noise
        fall = apply_envelope(fall, attack=0.01, decay=0.2, sustain=0.4, release=0.3, sample_rate=sample_rate)
        
        effect[:len(fall)] += fall
        
    elif effect_type == "level_up":
        # 升级音效 - 上升的琶音
        arpeggio_notes = [261.63, 329.63, 392.0, 523.25]  # C4, E4, G4, C5
        note_duration = 0.1
        
        start_sample = 0
        for note in arpeggio_notes:
            tone = generate_tone(note, note_duration, sample_rate, volume=0.6)
            tone = apply_envelope(tone, attack=0.01, decay=0.05, sustain=0.6, release=0.04, sample_rate=sample_rate)
            
            end_sample = start_sample + len(tone)
            if end_sample <= len(effect):
                effect[start_sample:end_sample] += tone
            
            start_sample += int(note_duration * 0.9 * sample_rate)  # 略微重叠
        
        # 添加明亮的尾音
        bright_freq = 1047  # C6
        bright_duration = 0.3
        bright_tone = generate_tone(bright_freq, bright_duration, sample_rate, volume=0.4)
        bright_tone = apply_envelope(bright_tone, attack=0.02, decay=0.1, sustain=0.2, release=0.2, sample_rate=sample_rate)
        
        # 在最后一个音符之后添加
        start_sample = int(note_duration * len(arpeggio_notes) * 0.8 * sample_rate)
        end_sample = start_sample + len(bright_tone)
        if end_sample <= len(effect):
            effect[start_sample:end_sample] += bright_tone
        
    elif effect_type == "pickup":
        # 拾取物品音效 - 短促上升音调
        pickup_freq = 800
        pickup_duration = 0.15
        t = np.linspace(0, pickup_duration, int(sample_rate * pickup_duration))
        pickup = np.sin(2 * np.pi * (pickup_freq + 300 * t**2) * t) * 0.5
        
        # 应用快速衰减
        pickup = apply_envelope(pickup, attack=0.001, decay=0.1, sustain=0.1, release=0.05, sample_rate=sample_rate)
        
        effect[:len(pickup)] += pickup
        
    elif effect_type == "menu_click":
        # 菜单点击音效 - 简单清脆的点击
        click_freq = 1200
        click_duration = 0.1
        click = generate_tone(click_freq, click_duration, sample_rate, volume=0.5)
        
        # 添加高频分量使声音更清脆
        click_high = generate_tone(click_freq * 1.5, click_duration, sample_rate, volume=0.3)
        click += click_high
        
        # 应用非常快速的衰减
        click = apply_envelope(click, attack=0.001, decay=0.03, sustain=0.0, release=0.02, sample_rate=sample_rate)
        
        effect[:len(click)] += click
    
    # 归一化
    max_amplitude = np.max(np.abs(effect))
    if max_amplitude > 0:
        effect = effect / max_amplitude * 0.9
    
    # 转换为16位PCM
    effect = (effect * 32767).astype(np.int16)
    
    return effect, sample_rate

def save_wav_file(audio_data, sample_rate, filename):
    """保存音频为WAV文件"""
    wavfile.write(filename, sample_rate, audio_data)
    print(f"已保存音频文件: {filename}")

def generate_audio_spectrogram(audio_data, sample_rate, output_file):
    """从音频生成频谱图可视化"""
    from matplotlib import pyplot as plt
    
    plt.figure(figsize=(10, 4))
    
    # 计算频谱
    plt.specgram(audio_data, Fs=sample_rate, cmap='viridis')
    plt.title('Audio Spectrogram')
    plt.xlabel('Time (s)')
    plt.ylabel('Frequency (Hz)')
    plt.colorbar(label='Intensity (dB)')
    
    plt.tight_layout()
    plt.savefig(output_file)
    plt.close()
    
    print(f"已保存频谱图: {output_file}")

def generate_all_audio(output_dir='resources/audio'):
    """生成所有音频资源"""
    # 确保目录存在
    os.makedirs(os.path.join(output_dir, 'music'), exist_ok=True)
    os.makedirs(os.path.join(output_dir, 'sfx'), exist_ok=True)
    
    # 生成音乐
    music_types = ['main_theme', 'battle', 'boss', 'victory', 'game_over']
    for music_type in music_types:
        print(f"生成 {music_type} 音乐...")
        music_duration = 30 if music_type == 'main_theme' else 20
        
        music, rate = create_music_track(duration=music_duration, type=music_type)
        music_path = os.path.join(output_dir, 'music', f"{music_type}.wav")
        save_wav_file(music, rate, music_path)
        
        # 生成频谱图
        spec_path = os.path.join(output_dir, 'music', f"{music_type}_spectrogram.png")
        generate_audio_spectrogram(music, rate, spec_path)
    
    # 生成音效
    effect_types = ['attack', 'hit', 'death', 'level_up', 'pickup', 'menu_click']
    for effect_type in effect_types:
        print(f"生成 {effect_type} 音效...")
        effect, rate = create_sound_effect(effect_type=effect_type)
        effect_path = os.path.join(output_dir, 'sfx', f"{effect_type}.wav")
        save_wav_file(effect, rate, effect_path)
        
        # 生成频谱图
        spec_path = os.path.join(output_dir, 'sfx', f"{effect_type}_spectrogram.png")
        generate_audio_spectrogram(effect, rate, spec_path)
    
    print("所有音频资源生成完成!")

if __name__ == "__main__":
    # 测试生成单个音频
    music, rate = create_music_track(duration=10, type="main_theme")
    save_wav_file(music, rate, "test_music.wav")
    
    effect, rate = create_sound_effect(effect_type="level_up")
    save_wav_file(effect, rate, "test_effect.wav")
    
    # 生成所有音频资源
    # generate_all_audio() 