#!/usr/bin/env python
import os
import sys
import pygame
import numpy as np
from PIL import Image
import random
import shutil

# 导入生成器模块
try:
    from character_generator import create_character, generate_animation_frames
    from weapon_generator import create_weapon, create_effect
    from map_generator import create_floor_tile, create_obstacle, generate_dungeon
    from ui_generator import create_button, create_icon, create_bar, create_background, create_frame
except ImportError:
    print("无法导入生成器模块，请确保相关文件在同一目录下")
    sys.exit(1)

# 资源目录
RESOURCES_DIR = 'resources'

def ensure_directories():
    """确保所有必要的目录结构存在"""
    # 英雄目录
    hero_types = ['warrior', 'mage', 'archer', 'rogue']
    for hero_type in hero_types:
        os.makedirs(os.path.join(RESOURCES_DIR, 'heroes', hero_type), exist_ok=True)
    
    # 敌人目录
    enemy_types = ['melee_basic', 'melee_fast', 'ranged', 'tank', 'boss']
    for enemy_type in enemy_types:
        os.makedirs(os.path.join(RESOURCES_DIR, 'enemies', enemy_type), exist_ok=True)
    
    # 武器目录
    weapon_types = ['sword', 'wand', 'bow', 'dagger']
    for weapon_type in weapon_types:
        os.makedirs(os.path.join(RESOURCES_DIR, 'weapons', weapon_type), exist_ok=True)
        os.makedirs(os.path.join(RESOURCES_DIR, 'weapons', weapon_type, 'effects'), exist_ok=True)
    
    # 地图目录
    map_dirs = ['floors', 'obstacles', 'decorations', 'presets']
    for map_dir in map_dirs:
        os.makedirs(os.path.join(RESOURCES_DIR, 'maps', map_dir), exist_ok=True)
    
    # UI目录
    ui_dirs = ['buttons', 'icons', 'bars', 'backgrounds', 'frames']
    for ui_dir in ui_dirs:
        os.makedirs(os.path.join(RESOURCES_DIR, 'ui', ui_dir), exist_ok=True)
    
    # 音频目录
    audio_dirs = ['music', 'sfx']
    for audio_dir in audio_dirs:
        os.makedirs(os.path.join(RESOURCES_DIR, 'audio', audio_dir), exist_ok=True)

def create_placeholder_audio(path):
    """创建占位音频文件"""
    with open(path, 'w') as f:
        f.write(f"# 这是一个音频占位文件: {os.path.basename(path)}\n")
        f.write("# 请用真实音频文件替换此文件\n")

def generate_hero_resources():
    """生成英雄资源"""
    hero_types = ['warrior', 'mage', 'archer', 'rogue']
    
    for hero_type in hero_types:
        print(f"生成{hero_type}英雄资源...")
        
        # 生成英雄素材
        hero_dir = os.path.join(RESOURCES_DIR, 'heroes', hero_type)
        
        # 生成英雄静止图像
        idle_img = create_character(character_type=hero_type)
        pygame.image.save(idle_img, os.path.join(hero_dir, f'{hero_type}_idle.png'))
        
        # 生成行走动画帧
        walk_frames = generate_animation_frames(idle_img, frame_count=4, animation_type="walk")
        for i, frame in enumerate(walk_frames):
            pygame.image.save(frame, os.path.join(hero_dir, f'{hero_type}_walk_{i}.png'))
        
        # 生成攻击动画帧
        attack_frames = generate_animation_frames(idle_img, frame_count=3, animation_type="attack")
        for i, frame in enumerate(attack_frames):
            pygame.image.save(frame, os.path.join(hero_dir, f'{hero_type}_attack_{i}.png'))

def generate_enemy_resources():
    """生成敌人资源"""
    enemy_types = ['melee_basic', 'melee_fast', 'ranged', 'tank', 'boss']
    
    for enemy_type in enemy_types:
        print(f"生成{enemy_type}敌人资源...")
        
        # 生成敌人素材
        enemy_dir = os.path.join(RESOURCES_DIR, 'enemies', enemy_type)
        
        # 生成敌人静止图像
        idle_img = create_character(character_type=enemy_type, is_enemy=True)
        pygame.image.save(idle_img, os.path.join(enemy_dir, f'{enemy_type}_idle.png'))
        
        # 生成行走动画帧
        walk_frames = generate_animation_frames(idle_img, frame_count=4, animation_type="walk")
        for i, frame in enumerate(walk_frames):
            pygame.image.save(frame, os.path.join(enemy_dir, f'{enemy_type}_walk_{i}.png'))
        
        # 生成攻击动画帧
        attack_frames = generate_animation_frames(idle_img, frame_count=3, animation_type="attack")
        for i, frame in enumerate(attack_frames):
            pygame.image.save(frame, os.path.join(enemy_dir, f'{enemy_type}_attack_{i}.png'))
        
        # 为Boss生成特殊动画
        if enemy_type == 'boss':
            special_frames = generate_animation_frames(idle_img, frame_count=5, animation_type="special")
            for i, frame in enumerate(special_frames):
                pygame.image.save(frame, os.path.join(enemy_dir, f'{enemy_type}_special_{i}.png'))

def generate_weapon_resources():
    """生成武器资源"""
    weapon_types = ['sword', 'wand', 'bow', 'dagger']
    
    for weapon_type in weapon_types:
        print(f"生成{weapon_type}武器资源...")
        
        # 生成武器素材
        weapon_dir = os.path.join(RESOURCES_DIR, 'weapons', weapon_type)
        effects_dir = os.path.join(weapon_dir, 'effects')
        
        # 生成武器图像
        weapon_img = create_weapon(weapon_type=weapon_type)
        pygame.image.save(weapon_img, os.path.join(weapon_dir, f'{weapon_type}.png'))
        
        # 生成武器效果动画
        effect_frames = create_effect(effect_type=weapon_type, frame_count=4)
        for i, frame in enumerate(effect_frames):
            pygame.image.save(frame, os.path.join(effects_dir, f'{weapon_type}_effect_{i}.png'))
        
        # 为弓箭额外生成箭矢
        if weapon_type == 'bow':
            arrow_img = create_weapon(weapon_type='arrow')
            pygame.image.save(arrow_img, os.path.join(weapon_dir, 'arrow.png'))

def generate_map_resources():
    """生成地图资源"""
    print("生成地图资源...")
    
    # 生成地板纹理
    floor_types = ['grass', 'dirt', 'stone', 'wood']
    floors_dir = os.path.join(RESOURCES_DIR, 'maps', 'floors')
    
    for floor_type in floor_types:
        floor_img = create_floor_tile(floor_type=floor_type)
        floor_img.save(os.path.join(floors_dir, f'{floor_type}.png'))
    
    # 生成障碍物
    obstacle_types = ['tree', 'rock', 'wall', 'crate', 'barrel']
    obstacles_dir = os.path.join(RESOURCES_DIR, 'maps', 'obstacles')
    
    for obs_type in obstacle_types:
        obs_img = create_obstacle(obs_type=obs_type)
        pygame.image.save(obs_img, os.path.join(obstacles_dir, f'{obs_type}.png'))
    
    # 生成装饰物
    decoration_types = ['flower', 'grass', 'torch', 'gravestone']
    decorations_dir = os.path.join(RESOURCES_DIR, 'maps', 'decorations')
    
    for dec_type in decoration_types:
        dec_img = create_obstacle(obs_type=dec_type, is_decoration=True)
        pygame.image.save(dec_img, os.path.join(decorations_dir, f'{dec_type}.png'))
    
    # 生成预设地图
    map_types = ['dungeon', 'forest', 'cemetery']
    presets_dir = os.path.join(RESOURCES_DIR, 'maps', 'presets')
    
    for map_type in map_types:
        map_img = generate_dungeon(width=512, height=512, map_type=map_type)
        map_img.save(os.path.join(presets_dir, f'{map_type}.png'))

def generate_ui_resources():
    """生成UI资源"""
    print("生成UI资源...")
    
    # 生成按钮
    button_styles = ['normal', 'fancy', 'wood']
    buttons_dir = os.path.join(RESOURCES_DIR, 'ui', 'buttons')
    
    for style in button_styles:
        button_img = create_button(text="Button", style=style)
        pygame.image.save(button_img, os.path.join(buttons_dir, f'button_{style}.png'))
    
    # 生成图标
    icon_types = ['health', 'xp', 'coin', 'weapon', 'shield', 'magic']
    icons_dir = os.path.join(RESOURCES_DIR, 'ui', 'icons')
    
    for icon_type in icon_types:
        icon_img = create_icon(icon_type=icon_type)
        pygame.image.save(icon_img, os.path.join(icons_dir, f'icon_{icon_type}.png'))
    
    # 生成状态条
    bar_types = ['health', 'mana', 'xp']
    bars_dir = os.path.join(RESOURCES_DIR, 'ui', 'bars')
    
    for bar_type in bar_types:
        bar_img = create_bar(bar_type=bar_type)
        pygame.image.save(bar_img, os.path.join(bars_dir, f'bar_{bar_type}.png'))
    
    # 生成背景
    bg_types = ['menu', 'game', 'dark']
    backgrounds_dir = os.path.join(RESOURCES_DIR, 'ui', 'backgrounds')
    
    for bg_type in bg_types:
        bg_img = create_background(width=800, height=600, bg_type=bg_type)
        pygame.image.save(bg_img, os.path.join(backgrounds_dir, f'bg_{bg_type}.png'))
    
    # 生成框架
    frame_styles = ['basic', 'ornate', 'minimal']
    frames_dir = os.path.join(RESOURCES_DIR, 'ui', 'frames')
    
    for style in frame_styles:
        frame_img = create_frame(style=style)
        pygame.image.save(frame_img, os.path.join(frames_dir, f'frame_{style}.png'))

def generate_audio_resources():
    """生成音频资源"""
    print("生成音频占位资源...")
    
    # 背景音乐
    music_dir = os.path.join(RESOURCES_DIR, 'audio', 'music')
    for track in ['main_theme', 'battle', 'boss', 'victory', 'game_over']:
        create_placeholder_audio(os.path.join(music_dir, f'{track}.txt'))
    
    # 音效
    sfx_dir = os.path.join(RESOURCES_DIR, 'audio', 'sfx')
    for effect in ['attack', 'hit', 'death', 'level_up', 'pickup', 'menu_click']:
        create_placeholder_audio(os.path.join(sfx_dir, f'{effect}.txt'))

def create_qrc_file():
    """创建Qt资源文件"""
    print("生成Qt资源文件...")
    
    qrc_path = os.path.join(RESOURCES_DIR, 'game_resources.qrc')
    
    with open(qrc_path, 'w') as f:
        f.write('<!DOCTYPE RCC>\n<RCC version="1.0">\n<qresource>\n')
        
        # 遍历资源目录，添加所有文件
        for root, _, files in os.walk(RESOURCES_DIR):
            for file in files:
                # 排除.qrc文件本身
                if file.endswith('.qrc'):
                    continue
                
                # 获取相对路径
                rel_path = os.path.relpath(os.path.join(root, file), RESOURCES_DIR)
                f.write(f'    <file>{rel_path}</file>\n')
        
        f.write('</qresource>\n</RCC>\n')

def main():
    """主函数"""
    # 初始化pygame
    pygame.init()
    
    # 检查资源目录是否存在
    if os.path.exists(RESOURCES_DIR):
        response = input(f"资源目录 '{RESOURCES_DIR}' 已存在. 是否覆盖? (y/n): ")
        if response.lower() == 'y':
            shutil.rmtree(RESOURCES_DIR)
        else:
            print("操作已取消")
            return
    
    # 创建目录结构
    print("创建目录结构...")
    ensure_directories()
    
    # 生成所有资源
    generate_hero_resources()
    generate_enemy_resources()
    generate_weapon_resources()
    generate_map_resources()
    generate_ui_resources()
    generate_audio_resources()
    
    # 创建Qt资源文件
    create_qrc_file()
    
    print(f"资源生成完成! 所有资源已保存在 '{RESOURCES_DIR}' 目录中")
    print("注意: 这些都是占位资源, 建议在实际游戏中替换为专业设计的资源")

if __name__ == "__main__":
    main() 