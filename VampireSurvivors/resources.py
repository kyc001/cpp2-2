import pygame
import numpy as np
from PIL import Image
import os
import random

# 初始化pygame
pygame.init()

# ========================
# 1. 基础角色生成（修正版）
# ========================
def create_pixel_character(width=32, height=32, palette=[(45,34,56), (111,60,112), (219,123,125)]):
    """生成随机像素风角色"""
    surface = pygame.Surface((width, height), pygame.SRCALPHA)
    
    # 使用numpy数组代替PixelArray
    pixels = np.zeros((height, width, 4), dtype=np.uint8)
    
    # 身体基础形状
    body = np.random.choice([0, 1, 2], (height, width), p=[0.6, 0.3, 0.1])
    
    for y in range(height):
        for x in range(width):
            if body[y][x] > 0:
                color = list(palette[body[y][x]-1])
                # 添加随机噪点增强像素感
                if random.random() > 0.2:
                    color = [min(255, c + random.randint(-15,15)) for c in color]
                pixels[y, x] = [*color, 255]  # 添加alpha通道
    
    # 将numpy数组转为Surface
    pygame.pixelcopy.array_to_surface(surface, pixels)
    return surface

# ========================
# 2. 动画生成（修正版）
# ========================
def generate_animation_frames(base_sprite, frame_count=4, animation_type="walk"):
    """生成动画帧"""
    frames = []
    width, height = base_sprite.get_size()
    
    for i in range(frame_count):
        # 创建副本的正确方式
        frame = pygame.Surface((width, height), pygame.SRCALPHA)
        frame.blit(base_sprite, (0, 0))
        
        # 获取像素数据
        pixels = pygame.surfarray.pixels3d(frame)
        alpha = pygame.surfarray.pixels_alpha(frame)
        
        if animation_type == "walk":
            offset = int(3 * np.sin(i * np.pi / 2))
            # 使用numpy的roll函数
            pixels[:] = np.roll(pixels, offset, axis=0)
            alpha[:] = np.roll(alpha, offset, axis=0)
            
        elif animation_type == "attack":
            # 定义武器区域
            weapon_area = slice(10, 22), slice(5, 27)  # 调整到合理范围
            # 高亮处理
            pixels[weapon_area] = np.clip(pixels[weapon_area] + [50, 0, 0], 0, 255)
        
        del pixels  # 必须释放surface锁
        del alpha
        
        frames.append(frame)
    return frames

# ========================
# 3. 武器生成（保持不变）
# ========================
def create_weapon(weapon_type="sword"):
    """生成武器精灵"""
    weapon = pygame.Surface((32, 32), pygame.SRCALPHA)
    
    if weapon_type == "sword":
        pygame.draw.line(weapon, (200,200,200), (16,5), (16,25), 3)
        pygame.draw.rect(weapon, (139,69,19), (14,22,4,6))
    elif weapon_type == "wand":
        pygame.draw.line(weapon, (218,165,32), (16,5), (16,25), 2)
        pygame.draw.circle(weapon, (0,191,255), (16,8), 4)
    
    return weapon

# ========================
# 4. 特效生成（修正版）
# ========================
def create_effect(effect_type="slash"):
    """生成特效动画帧"""
    frames = []
    for i in range(3):
        frame = pygame.Surface((64,64), pygame.SRCALPHA)
        
        if effect_type == "slash":
            angle = i * 30
            # 使用更简单的线条表示
            pygame.draw.line(frame, (255,255,0,200), 
                           (32,32),
                           (32 + 25*np.cos(np.radians(angle)), 
                            32 + 25*np.sin(np.radians(angle))), 
                           3)
            
        frames.append(frame)
    return frames

# ========================
# 5. 简化版地图生成（移除noise依赖）
# ========================
def generate_dungeon(width=100, height=100):
    """生成随机地牢地图"""
    img = Image.new('RGB', (width, height))
    pixels = img.load()
    
    # 简单的随机地图生成
    for y in range(height):
        for x in range(width):
            r = random.random()
            if r < 0.1:  # 墙壁
                pixels[x,y] = (70, 65, 60)
            elif r < 0.6: # 地板
                pixels[x,y] = (120, 110, 100)
            else:         # 路径
                pixels[x,y] = (180, 170, 160)
    
    return img

# ========================
# 6. 障碍物生成（保持不变）
# ========================
def create_obstacle(obs_type="tree"):
    """生成环境障碍物"""
    surf = pygame.Surface((32,48), pygame.SRCALPHA)
    
    if obs_type == "tree":
        pygame.draw.rect(surf, (101,67,33), (14,30,4,18))
        pygame.draw.circle(surf, (34,139,34), (16,20), 12)
    
    return surf

# ========================
# 7. 主生成函数（修正版）
# ========================
def generate_all_assets():
    """生成所有游戏资源"""
    # 创建目录
    os.makedirs("resources/heroes/warrior", exist_ok=True)
    os.makedirs("resources/weapons/sword", exist_ok=True)
    os.makedirs("resources/maps/presets", exist_ok=True)
    os.makedirs("resources/maps/obstacles", exist_ok=True)

    # 1. 生成角色
    character_types = {
        "warrior": [(58,47,72), (94,84,112)],
        "mage": [(70,30,80), (150,80,160)],
        "archer": [(40,60,30), (90,120,60)]
    }

    for char_type, palette in character_types.items():
        os.makedirs(f"resources/heroes/{char_type}", exist_ok=True)
        
        char = create_pixel_character(palette=palette)
        pygame.image.save(char, f"resources/heroes/{char_type}/{char_type}_idle.png")
        
        walk_frames = generate_animation_frames(char, animation_type="walk")
        for i, frame in enumerate(walk_frames):
            pygame.image.save(frame, f"resources/heroes/{char_type}/{char_type}_walk{i+1}.png")

    # 2. 生成武器
    sword = create_weapon("sword")
    pygame.image.save(sword, "resources/weapons/sword/sword.png")
    
    slash_effects = create_effect("slash")
    for i, frame in enumerate(slash_effects):
        pygame.image.save(frame, f"resources/weapons/sword/sword_effect{i+1}.png")

    # 3. 生成地图
    dungeon = generate_dungeon()
    dungeon.save("resources/maps/presets/dungeon_map.png")

    # 4. 生成障碍物
    tree = create_obstacle("tree")
    pygame.image.save(tree, "resources/maps/obstacles/tree.png")

# ========================
# 执行生成
# ========================
if __name__ == "__main__":
    generate_all_assets()
    print("所有游戏资源已生成到resources目录！")