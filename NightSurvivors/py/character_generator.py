#!/usr/bin/env python
import pygame
import numpy as np
import random

def create_character(width=32, height=32, palette=[(58, 47, 72), (94, 84, 112), (156, 134, 165)], 
                    character_type="warrior", is_enemy=False):
    """
    生成像素风格的角色或敌人
    
    参数:
        width, height: 图像大小
        palette: 颜色方案 (深色, 中色, 亮色)
        character_type: 角色类型
        is_enemy: 是否为敌人
    """
    surface = pygame.Surface((width, height), pygame.SRCALPHA)
    pixels = pygame.PixelArray(surface)
    
    # 随机种子基于角色类型，保证同类型角色一致
    random.seed(hash(character_type))
    
    # 身体基础矩阵 - 0是透明，1-3对应调色板
    body = np.zeros((height, width), dtype=int)
    
    # 根据角色类型定制形状
    if character_type == "warrior":
        # 战士：宽厚的身体和盔甲
        # 头部
        head_y = height // 4
        head_size = width // 3
        for y in range(head_y, head_y + head_size):
            for x in range(width // 2 - head_size // 2, width // 2 + head_size // 2):
                if (x - width//2)**2 + (y - (head_y + head_size//2))**2 < (head_size//2)**2:
                    body[y][x] = 2
        
        # 身体
        body_width = width // 2
        for y in range(head_y + head_size, height - height//4):
            for x in range(width//2 - body_width//2, width//2 + body_width//2):
                body[y][x] = 2 if abs(x - width//2) < body_width//4 else 1
        
        # 武器 (剑)
        for y in range(head_y, head_y + height//2):
            for x in range(width//2 + body_width//2, width - width//8):
                if abs(y - (head_y + height//4)) < 2:
                    body[y][x] = 3
    
    elif character_type == "mage":
        # 法师：瘦长的身体和帽子
        # 帽子
        hat_y = height // 6
        for y in range(hat_y, hat_y + height//6):
            hat_width = (height//6 - (y - hat_y)) * 2
            for x in range(width//2 - hat_width//2, width//2 + hat_width//2):
                body[y][x] = 3
        
        # 头部
        head_y = hat_y + height//6
        head_size = width // 3
        for y in range(head_y, head_y + head_size):
            for x in range(width // 2 - head_size // 2, width // 2 + head_size // 2):
                if (x - width//2)**2 + (y - (head_y + head_size//2))**2 < (head_size//2)**2:
                    body[y][x] = 2
        
        # 身体 (长袍)
        body_width = width // 3
        for y in range(head_y + head_size, height - height//6):
            curr_width = body_width + int((y - (head_y + head_size)) * 0.3)
            for x in range(width//2 - curr_width//2, width//2 + curr_width//2):
                body[y][x] = 1 if random.random() > 0.7 else 2
        
        # 魔杖
        for y in range(head_y, head_y + 2*height//3):
            if width//2 - body_width//2 - 1 < 0:
                continue
            body[y][width//2 - body_width//2 - 1] = 3
        body[head_y][width//2 - body_width//2 - 2] = 3
        
    elif character_type == "archer":
        # 弓箭手：灵活的身体和头巾
        # 头部(带头巾)
        head_y = height // 4
        head_size = width // 3
        for y in range(head_y, head_y + head_size):
            for x in range(width // 2 - head_size // 2, width // 2 + head_size // 2):
                if (x - width//2)**2 + (y - (head_y + head_size//2))**2 < (head_size//2)**2:
                    body[y][x] = 1 if y < head_y + head_size//3 else 2
        
        # 身体 (轻装)
        body_width = width // 3
        for y in range(head_y + head_size, height - height//6):
            for x in range(width//2 - body_width//2, width//2 + body_width//2):
                if abs(x - width//2) < body_width//4:
                    body[y][x] = 2
                elif random.random() > 0.5:
                    body[y][x] = 1
        
        # 弓
        bow_x = width//2 + body_width//2 + 1
        for y in range(head_y, head_y + 2*head_size):
            bow_curve = np.sin((y - head_y) / (2*head_size) * np.pi) * 2
            if bow_x + int(bow_curve) < width:
                body[y][bow_x + int(bow_curve)] = 3
    
    elif character_type == "rogue":
        # 盗贼：纤细敏捷的身体和兜帽
        # 兜帽
        hood_y = height // 5
        hood_size = width // 3
        for y in range(hood_y, hood_y + hood_size):
            for x in range(width // 2 - hood_size // 2, width // 2 + hood_size // 2):
                if (x - width//2)**2 + (y - (hood_y + hood_size//2))**2 < (hood_size//2)**2:
                    body[y][x] = 1
        
        # 面部 (黑暗中的眼睛)
        face_y = hood_y + hood_size//3
        body[face_y][width//2 - hood_size//4] = 3
        body[face_y][width//2 + hood_size//4] = 3
        
        # 身体 (斗篷)
        body_width = width // 3
        for y in range(hood_y + hood_size, height - height//6):
            curr_width = body_width + int((y - (hood_y + hood_size)) * 0.2)
            for x in range(width//2 - curr_width//2, width//2 + curr_width//2):
                body[y][x] = 1 if random.random() > 0.7 or abs(x - width//2) > curr_width//3 else 2
        
        # 匕首
        for x in range(width//2 - body_width//2 - 3, width//2 - body_width//2):
            if x >= 0:  # 确保不超出边界
                body[height//2][x] = 3
    
    # 敌人特有的形状
    elif is_enemy:
        if character_type == "melee_basic":
            # 基础近战敌人：简单的圆形怪物
            center_x, center_y = width//2, height//2
            radius = min(width, height) // 3
            
            for y in range(height):
                for x in range(width):
                    dist = ((x - center_x)**2 + (y - center_y)**2)**0.5
                    if dist < radius:
                        body[y][x] = 2 if dist < radius * 0.7 else 1
            
            # 添加眼睛
            eye_y = center_y - radius//3
            eye_space = radius // 2
            body[eye_y][center_x - eye_space//2] = 3
            body[eye_y][center_x + eye_space//2] = 3
            
            # 添加爪子
            for i in range(3):
                claw_x = center_x + radius * np.cos(np.pi/6 + i*np.pi/4)
                claw_y = center_y + radius * np.sin(np.pi/6 + i*np.pi/4)
                if 0 <= int(claw_x) < width and 0 <= int(claw_y) < height:
                    body[int(claw_y)][int(claw_x)] = 3
                
        elif character_type == "melee_fast":
            # 快速近战敌人：尖锐的三角形怪物
            for y in range(height//3, height - height//4):
                width_at_y = int((y - height//3) / (height - height//3 - height//4) * width//2)
                for x in range(width//2 - width_at_y, width//2 + width_at_y):
                    body[y][x] = 1 if random.random() > 0.8 else 2
            
            # 添加锋利的牙齿
            for x in range(width//3, width - width//3, width//10):
                tooth_height = int(height//10 * random.uniform(0.5, 1.5))
                for y in range(height - height//4, height - height//4 + tooth_height):
                    if y < height:
                        body[y][x] = 3
                        
        elif character_type == "ranged":
            # 远程敌人：圆形带触手
            center_x, center_y = width//2, height//2
            radius = min(width, height) // 4
            
            # 基础圆形身体
            for y in range(height):
                for x in range(width):
                    dist = ((x - center_x)**2 + (y - center_y)**2)**0.5
                    if dist < radius:
                        body[y][x] = 2 if dist < radius * 0.7 else 1
            
            # 添加多个触手
            for angle in range(0, 360, 45):
                rad = np.radians(angle)
                length = radius * 1.5
                start_x, start_y = center_x + radius * np.cos(rad), center_y + radius * np.sin(rad)
                end_x, end_y = center_x + length * np.cos(rad), center_y + length * np.sin(rad)
                
                steps = int(length - radius)
                for step in range(steps):
                    t = step / steps
                    x = int(start_x + t * (end_x - start_x))
                    y = int(start_y + t * (end_y - start_y))
                    if 0 <= x < width and 0 <= y < height:
                        body[y][x] = 3 if random.random() > 0.7 else 1
            
            # 添加中央眼睛
            body[center_y][center_x] = 3
            
        elif character_type == "tank":
            # 坦克敌人：坚固的方块怪物
            margin = width//8
            for y in range(margin, height - margin):
                for x in range(margin, width - margin):
                    is_edge = (x < margin*1.5 or x > width - margin*1.5 or 
                              y < margin*1.5 or y > height - margin*1.5)
                    body[y][x] = 1 if is_edge else 2
            
            # 添加棱角
            for corner in [(margin, margin), (margin, height-margin-1), 
                          (width-margin-1, margin), (width-margin-1, height-margin-1)]:
                body[corner[1]][corner[0]] = 3
                
        elif character_type == "boss":
            # Boss敌人：复杂的大型怪物
            # 身体
            center_x, center_y = width//2, height//2
            radius = min(width, height) // 2.5
            
            for y in range(height):
                for x in range(width):
                    dist = ((x - center_x)**2 + (y - center_y)**2)**0.5
                    if dist < radius:
                        # 创建放射状图案
                        angle = np.arctan2(y - center_y, x - center_x)
                        pattern = (np.sin(angle * 5) + 1) / 2  # 0到1之间的值
                        
                        if dist < radius * 0.5:
                            body[y][x] = 2
                        elif pattern > 0.7:
                            body[y][x] = 3
                        else:
                            body[y][x] = 1
            
            # 添加恐怖的眼睛
            eye_radius = radius // 4
            eye_y = center_y - radius//5
            for eye_x in [center_x - radius//3, center_x + radius//3]:
                for y in range(int(eye_y - eye_radius), int(eye_y + eye_radius)):
                    for x in range(int(eye_x - eye_radius), int(eye_x + eye_radius)):
                        if 0 <= y < height and 0 <= x < width:
                            eye_dist = ((x - eye_x)**2 + (y - eye_y)**2)**0.5
                            if eye_dist < eye_radius:
                                body[y][x] = 3 if eye_dist < eye_radius * 0.5 else 0
    
    # 将矩阵转换为像素
    for y in range(height):
        for x in range(width):
            if body[y][x] > 0:
                color = palette[body[y][x] - 1]
                # 添加随机噪点增强像素感
                if np.random.random() > 0.8:
                    color = tuple(min(255, c + np.random.randint(-10, 10)) for c in color)
                pixels[x][y] = color
    
    # 释放PixelArray以便使用Surface
    pixels.close()
    return surface

def generate_animation_frames(base_sprite, frame_count=4, animation_type="walk"):
    """
    基于基础精灵生成动画帧
    
    参数:
        base_sprite: 基础精灵图像
        frame_count: 要生成的帧数
        animation_type: 动画类型 (walk/attack/cast/shoot/special)
    """
    frames = []
    width, height = base_sprite.get_size()
    
    for i in range(frame_count):
        frame = base_sprite.copy()
        
        if animation_type == "walk":
            # 行走动画：轻微的上下摆动和左右摆动
            offset_y = int(2 * np.sin(i * np.pi / 2))
            shift = pygame.Surface((width, height), pygame.SRCALPHA)
            shift.blit(frame, (0, offset_y))
            frame = shift
            
            # 添加走路时的左右微晃效果
            offset_x = int(1 * np.sin(i * np.pi))
            shift = pygame.Surface((width, height), pygame.SRCALPHA)
            shift.blit(frame, (offset_x, 0))
            frame = shift
            
        elif animation_type == "attack":
            # 攻击动画：向前倾斜并增加亮度
            # 略微向前倾斜
            angle = 5 if i == 1 else -5 if i == 2 else 0
            frame = pygame.transform.rotate(frame, angle)
            
            # 调整亮度
            pixels = pygame.PixelArray(frame)
            for y in range(height):
                for x in range(width):
                    try:
                        if pixels[x, y] != 0:  # 跳过透明像素
                            r, g, b, a = frame.unmap_rgb(pixels[x, y])
                            # 增加红色强调攻击感
                            r = min(255, r + 30)
                            pixels[x, y] = (r, g, b, a)
                    except IndexError:
                        continue  # 旋转后可能超出边界
            pixels.close()
            
        elif animation_type == "cast":
            # 施法动画：添加魔法光效
            glow = pygame.Surface((width, height), pygame.SRCALPHA)
            glow_radius = int(10 + 5 * np.sin(i * np.pi / 2))
            glow_center = (width // 2, height // 3)
            
            for y in range(height):
                for x in range(width):
                    dist = ((x - glow_center[0])**2 + (y - glow_center[1])**2)**0.5
                    if dist < glow_radius:
                        alpha = int(255 * (1 - dist / glow_radius))
                        glow.set_at((x, y), (100, 100, 255, alpha))
            
            frame.blit(glow, (0, 0), special_flags=pygame.BLEND_ADD)
            
        elif animation_type == "shoot":
            # 射箭动画：弓的绷紧和放松
            # 模拟拉弓的动作
            stretch = 1 - 0.2 * i  # 第一帧拉满，然后逐渐放松
            scaled = pygame.transform.scale(frame, (int(width * stretch), height))
            new_frame = pygame.Surface((width, height), pygame.SRCALPHA)
            new_frame.blit(scaled, ((width - scaled.get_width()) // 2, 0))
            frame = new_frame
            
        elif animation_type == "special":
            # 特殊技能动画：强烈的闪光和扭曲
            # 添加强烈的光晕
            glow = pygame.Surface((width, height), pygame.SRCALPHA)
            for radius in range(10, 30, 5):
                intensity = int(255 * (1 - radius / 30) * np.sin(i * np.pi / 2 + radius / 10))
                # 确保intensity在有效范围内
                intensity = max(0, min(255, intensity))
                pygame.draw.circle(glow, (255, 50, 50, intensity), (width // 2, height // 2), radius, 2)
            
            # 扭曲效果
            pixels = pygame.PixelArray(frame)
            for y in range(height):
                for x in range(width):
                    if 0 <= x < width and 0 <= y < height and pixels[x, y] != 0:
                        # 随机轻微偏移
                        offset_x = x + int(2 * np.sin(y / 5 + i))
                        offset_y = y + int(2 * np.cos(x / 5 + i))
                        
                        if 0 <= offset_x < width and 0 <= offset_y < height:
                            pixels[offset_x, offset_y] = pixels[x, y]
                            pixels[x, y] = 0
            pixels.close()
            
            frame.blit(glow, (0, 0), special_flags=pygame.BLEND_ADD)
        
        frames.append(frame)
    
    return frames 