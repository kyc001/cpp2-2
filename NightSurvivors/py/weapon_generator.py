#!/usr/bin/env python
import pygame
import numpy as np
import random

def create_weapon(weapon_type="sword"):
    """
    生成不同类型武器的像素图
    
    参数:
        weapon_type: 武器类型 (sword/wand/bow/dagger/arrow)
    """
    surface = pygame.Surface((32, 32), pygame.SRCALPHA)
    
    if weapon_type == "sword":
        # 剑刃
        sword_length = 20
        sword_width = 3
        sword_handle = 8
        
        # 剑身 - 银色
        pygame.draw.line(surface, (200, 200, 210), (16, 5), (16, 5 + sword_length), sword_width)
        
        # 剑尖 - 更亮
        points = [(16, 5), (16 - sword_width//2, 5 + sword_width), (16 + sword_width//2, 5 + sword_width)]
        pygame.draw.polygon(surface, (230, 230, 240), points)
        
        # 剑柄 - 棕色
        pygame.draw.rect(surface, (139, 69, 19), (16 - sword_width//2, 5 + sword_length, sword_width, sword_handle))
        
        # 剑格 - 亮金色
        pygame.draw.rect(surface, (218, 165, 32), (16 - sword_width*2, 5 + sword_length - 1, sword_width*4, 2))
        
        # 添加像素感
        pixels = pygame.PixelArray(surface)
        for y in range(surface.get_height()):
            for x in range(surface.get_width()):
                if pixels[x, y] != 0:  # 非透明像素
                    color = surface.unmap_rgb(pixels[x, y])
                    # 随机调整颜色
                    if random.random() > 0.8:
                        r, g, b, a = color
                        r = max(0, min(255, r + random.randint(-10, 10)))
                        g = max(0, min(255, g + random.randint(-10, 10)))
                        b = max(0, min(255, b + random.randint(-10, 10)))
                        pixels[x, y] = (r, g, b, a)
        pixels.close()
        
    elif weapon_type == "wand":
        # 魔杖杆 - 深棕色
        wand_length = 22
        pygame.draw.line(surface, (120, 80, 40), (16, 8), (16, 8 + wand_length), 2)
        
        # 魔法水晶 - 蓝色
        crystal_radius = 5
        crystal_center = (16, 8)
        
        # 绘制发光的水晶
        for r in range(crystal_radius, 0, -1):
            # 随着半径减小，颜色越来越亮
            intensity = 150 + (crystal_radius - r) * 20
            color = (0, intensity, 255)
            pygame.draw.circle(surface, color, crystal_center, r)
        
        # 添加光芒线
        for angle in range(0, 360, 45):
            end_x = crystal_center[0] + int(crystal_radius * 1.5 * np.cos(np.radians(angle)))
            end_y = crystal_center[1] + int(crystal_radius * 1.5 * np.sin(np.radians(angle)))
            pygame.draw.line(surface, (150, 200, 255, 128), crystal_center, (end_x, end_y), 1)
        
    elif weapon_type == "bow":
        # 弓的主体 - 棕色木质
        bow_color = (160, 120, 80)
        bow_radius = 12
        bow_thickness = 2
        
        # 绘制弓弧
        start_angle = -60
        end_angle = 60
        rect = pygame.Rect(16 - bow_radius, 16 - bow_radius, bow_radius * 2, bow_radius * 2)
        pygame.draw.arc(surface, bow_color, rect, np.radians(start_angle), np.radians(end_angle), bow_thickness)
        
        # 弓弦 - 白色
        string_start = (16 + bow_radius * np.cos(np.radians(start_angle)), 
                        16 + bow_radius * np.sin(np.radians(start_angle)))
        string_end = (16 + bow_radius * np.cos(np.radians(end_angle)), 
                      16 + bow_radius * np.sin(np.radians(end_angle)))
        pygame.draw.line(surface, (200, 200, 200), string_start, string_end, 1)
        
        # 弓柄 - 稍深色
        grip_height = 8
        grip_pos = (16 - bow_thickness//2, 16 - grip_height//2)
        pygame.draw.rect(surface, (120, 80, 40), (*grip_pos, bow_thickness, grip_height))
        
    elif weapon_type == "dagger":
        # 匕首 - 短小锋利
        dagger_length = 14
        dagger_width = 2
        handle_length = 6
        
        # 刀身 - 银色
        pygame.draw.line(surface, (180, 180, 190), (16, 10), (16, 10 + dagger_length), dagger_width)
        
        # 刀尖 - 更尖锐
        points = [(16, 10), (16 - dagger_width, 10 + dagger_width), (16 + dagger_width, 10 + dagger_width)]
        pygame.draw.polygon(surface, (210, 210, 220), points)
        
        # 刀柄 - 棕色
        pygame.draw.rect(surface, (100, 60, 20), (16 - dagger_width//2, 10 + dagger_length, dagger_width, handle_length))
        
        # 刀格 - 深色
        pygame.draw.rect(surface, (80, 80, 100), (16 - dagger_width*2, 10 + dagger_length - 1, dagger_width*4, 2))
        
    elif weapon_type == "arrow":
        # 箭 - 细长的投射物
        arrow_length = 20
        shaft_color = (150, 100, 50)  # 棕色箭杆
        head_color = (200, 200, 200)  # 银色箭头
        
        # 箭杆
        pygame.draw.line(surface, shaft_color, (6, 16), (6 + arrow_length, 16), 1)
        
        # 箭头
        arrow_head = [(6 + arrow_length, 16), (6 + arrow_length - 4, 16 - 3), (6 + arrow_length - 4, 16 + 3)]
        pygame.draw.polygon(surface, head_color, arrow_head)
        
        # 羽毛
        feather_color = (220, 220, 220)
        feather_length = 5
        pygame.draw.line(surface, feather_color, (8, 16), (8 + feather_length, 16 - 2), 1)
        pygame.draw.line(surface, feather_color, (8, 16), (8 + feather_length, 16 + 2), 1)
    
    return surface

def create_effect(effect_type="sword", frame_count=3):
    """
    生成武器效果动画
    
    参数:
        effect_type: 效果类型 (sword/magic/dagger)
    """
    frames = []
    effect_size = 64  # 效果图通常比武器大
    
    for i in range(frame_count):
        frame = pygame.Surface((effect_size, effect_size), pygame.SRCALPHA)
        
        if effect_type == "sword":
            # 剑气效果 - 弧形切割
            angle = 30 * i  # 随着帧数增加，角度旋转
            radius = 24 + i * 4  # 半径随帧数增加
            start_angle = -30 + angle
            end_angle = 30 + angle
            
            # 弧形切割
            center = (effect_size // 2, effect_size // 2)
            rect = pygame.Rect(center[0] - radius, center[1] - radius, radius * 2, radius * 2)
            pygame.draw.arc(frame, (255, 255, 200), rect, np.radians(start_angle), np.radians(end_angle), 3)
            
            # 添加动态波纹
            for r in range(radius - 5, radius + 6, 3):
                alpha = 200 - abs(radius - r) * 30  # 中心最亮
                if alpha > 0:
                    pygame.draw.arc(frame, (255, 255, 200, alpha), 
                                    pygame.Rect(center[0] - r, center[1] - r, r * 2, r * 2),
                                    np.radians(start_angle), np.radians(end_angle), 2)
            
        elif effect_type == "magic":
            # 魔法效果 - 能量球
            center = (effect_size // 2, effect_size // 2)
            outer_radius = 15 + i * 5
            inner_radius = outer_radius - 5
            
            # 外圈光环
            for r in range(outer_radius, inner_radius, -1):
                alpha = 150 - (outer_radius - r) * 30
                pygame.draw.circle(frame, (100, 100, 255, alpha), center, r, 1)
            
            # 内部能量
            for _ in range(20):
                angle = random.uniform(0, 2 * np.pi)
                distance = random.uniform(0, inner_radius)
                x = int(center[0] + distance * np.cos(angle))
                y = int(center[1] + distance * np.sin(angle))
                size = int(random.uniform(1, 3))
                alpha = int(200 * (1 - distance / inner_radius))
                color = (150, 150, 255, alpha)
                pygame.draw.circle(frame, color, (x, y), size)
            
            # 发散光线
            for angle in range(0, 360, 30):
                length = outer_radius + random.randint(0, 10)
                end_x = center[0] + int(length * np.cos(np.radians(angle)))
                end_y = center[1] + int(length * np.sin(np.radians(angle)))
                pygame.draw.line(frame, (150, 150, 255, 100), center, (end_x, end_y), 1)
            
        elif effect_type == "dagger":
            # 匕首效果 - 急速切割
            center = (effect_size // 2, effect_size // 2)
            
            # 多重残影
            for j in range(3):
                offset = j * 5
                angle = 45 * i + j * 15
                length = 20
                
                start_x = center[0] - offset * np.cos(np.radians(angle))
                start_y = center[1] - offset * np.sin(np.radians(angle))
                end_x = start_x + length * np.cos(np.radians(angle))
                end_y = start_y + length * np.sin(np.radians(angle))
                
                # 残影颜色随距离变淡
                alpha = 255 - j * 60
                pygame.draw.line(frame, (200, 200, 100, alpha), (start_x, start_y), (end_x, end_y), 2)
                
                # 添加切割痕迹
                if j == 0:
                    for k in range(5):
                        trace_x = end_x + k * 2 * np.cos(np.radians(angle))
                        trace_y = end_y + k * 2 * np.sin(np.radians(angle))
                        size = 3 - k * 0.5
                        if size > 0:
                            pygame.draw.circle(frame, (255, 255, 150, 150 - k * 30), (trace_x, trace_y), size)
        
        frames.append(frame)
    
    return frames 