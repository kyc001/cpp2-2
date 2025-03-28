#!/usr/bin/env python
import pygame
import numpy as np
from PIL import Image, ImageDraw, ImageFont
import random
import os

def create_button(text="Button", width=120, height=40, style="normal"):
    """
    生成按钮UI元素
    
    参数:
        text: 按钮文本
        width, height: 按钮尺寸
        style: 按钮样式 (normal/fancy/wood)
    """
    # 创建表面
    surface = pygame.Surface((width, height), pygame.SRCALPHA)
    
    # 样式参数
    if style == "normal":
        # 简约现代风格
        bg_color = (80, 80, 210)
        bg_hover = (100, 100, 230)
        border_color = (40, 40, 150)
        text_color = (255, 255, 255)
        
        # 绘制按钮背景
        pygame.draw.rect(surface, bg_color, (0, 0, width, height), border_radius=5)
        
        # 绘制边框
        pygame.draw.rect(surface, border_color, (0, 0, width, height), 2, border_radius=5)
        
        # 绘制高光效果
        highlight_rect = pygame.Rect(2, 2, width-4, height//3)
        pygame.draw.rect(surface, bg_hover, highlight_rect, border_radius=5)
        
    elif style == "fancy":
        # 华丽风格
        bg_color = (180, 140, 80)
        border_color = (140, 100, 40)
        highlight_color = (220, 180, 100)
        text_color = (60, 30, 0)
        
        # 绘制基础背景
        pygame.draw.rect(surface, bg_color, (0, 0, width, height), border_radius=8)
        
        # 绘制装饰边框
        border_width = 4
        pygame.draw.rect(surface, border_color, (0, 0, width, height), border_width, border_radius=8)
        
        # 内部装饰
        inner_rect = pygame.Rect(border_width*2, border_width*2, 
                               width-border_width*4, height-border_width*4)
        pygame.draw.rect(surface, highlight_color, inner_rect, 1, border_radius=4)
        
        # 绘制角落装饰
        corner_size = min(width, height) // 6
        for corner in [(0, 0), (width-corner_size, 0), 
                      (0, height-corner_size), (width-corner_size, height-corner_size)]:
            pygame.draw.rect(surface, highlight_color, (*corner, corner_size, corner_size), 1)
            
    elif style == "wood":
        # 木质风格
        wood_color = (150, 100, 60)
        dark_wood = (120, 80, 40)
        highlight = (180, 130, 90)
        text_color = (240, 230, 200)
        
        # 绘制木质背景
        pygame.draw.rect(surface, wood_color, (0, 0, width, height), border_radius=5)
        
        # 木纹纹理
        for i in range(0, width, width//10):
            grain_color = dark_wood if i % 2 == 0 else highlight
            grain_width = 1 if i % 3 == 0 else 2
            pygame.draw.line(surface, grain_color, (i, 0), (i, height), grain_width)
        
        # 边框
        pygame.draw.rect(surface, dark_wood, (0, 0, width, height), 3, border_radius=5)
        
        # 钉子装饰
        nail_color = (180, 180, 180)
        nail_positions = [(width//4, height//4), (3*width//4, height//4), 
                         (width//4, 3*height//4), (3*width//4, 3*height//4)]
        
        for pos in nail_positions:
            pygame.draw.circle(surface, nail_color, pos, 3)
            # 高光
            pygame.draw.circle(surface, (220, 220, 220), 
                             (pos[0]-1, pos[1]-1), 1)
    
    # 渲染文本
    try:
        # 尝试加载内置字体
        font = pygame.font.SysFont('Arial', height//3)
        text_surface = font.render(text, True, text_color)
        text_rect = text_surface.get_rect(center=(width//2, height//2))
        surface.blit(text_surface, text_rect)
    except:
        # 简单文本渲染
        draw_basic_text(surface, text, text_color, (width//2, height//2), height//3)
    
    return surface

def draw_basic_text(surface, text, color, center, size):
    """简单文本渲染备用方法"""
    text_width = len(text) * size // 2
    text_height = size
    x, y = center[0] - text_width // 2, center[1] - text_height // 2
    
    for char in text:
        char_width = size // 2
        # 绘制简单像素文本
        for cy in range(text_height):
            for cx in range(char_width):
                if random.random() < 0.7:  # 简单模拟字符形状
                    surface.set_at((x + cx, y + cy), color)
        x += char_width

def create_icon(icon_type="health", size=32):
    """
    生成游戏图标
    
    参数:
        icon_type: 图标类型
        size: 图标尺寸
    """
    surface = pygame.Surface((size, size), pygame.SRCALPHA)
    center = (size // 2, size // 2)
    radius = size // 2 - 2
    
    if icon_type == "health":
        # 心形生命图标
        base_color = (220, 50, 50)
        
        # 绘制圆形背景
        pygame.draw.circle(surface, base_color, center, radius)
        
        # 绘制白色十字
        cross_width = size // 4
        cross_color = (255, 255, 255)
        
        pygame.draw.rect(surface, cross_color, 
                       (center[0] - cross_width//4, center[1] - cross_width//2, 
                        cross_width//2, cross_width))
        pygame.draw.rect(surface, cross_color, 
                       (center[0] - cross_width//2, center[1] - cross_width//4, 
                        cross_width, cross_width//2))
        
    elif icon_type == "xp":
        # 经验值图标
        base_color = (80, 220, 80)
        highlight = (160, 255, 160)
        
        # 绘制星形
        points = []
        num_points = 5
        outer_radius = radius
        inner_radius = radius // 2
        
        for i in range(num_points * 2):
            angle = np.pi / num_points * i
            r = outer_radius if i % 2 == 0 else inner_radius
            x = center[0] + r * np.cos(angle)
            y = center[1] + r * np.sin(angle)
            points.append((x, y))
        
        pygame.draw.polygon(surface, base_color, points)
        
        # 高光效果
        pygame.draw.polygon(surface, highlight, points, 1)
        
    elif icon_type == "coin":
        # 金币图标
        gold_color = (230, 200, 60)
        highlight = (255, 240, 120)
        shadow = (180, 150, 40)
        
        # 绘制圆形金币
        pygame.draw.circle(surface, gold_color, center, radius)
        
        # 边缘
        pygame.draw.circle(surface, shadow, center, radius, 2)
        
        # 高光
        highlight_rect = pygame.Rect(center[0] - radius//2, center[1] - radius//2, 
                                   radius, radius//2)
        pygame.draw.ellipse(surface, highlight, highlight_rect)
        
        # 纹理
        angle = 45  # 倾斜角度
        for i in range(-radius, radius, 3):
            start_x = center[0] + i
            start_y = center[1] - int(np.sqrt(radius*radius - i*i)) if abs(i) < radius else center[1]
            end_x = center[0] + i
            end_y = center[1] + int(np.sqrt(radius*radius - i*i)) if abs(i) < radius else center[1]
            
            pygame.draw.line(surface, shadow, (start_x, start_y), (end_x, end_y), 1)
        
    elif icon_type == "weapon":
        # 武器图标
        metal_color = (200, 200, 210)
        handle_color = (150, 100, 60)
        
        # 剑的把手
        handle_height = size // 2
        handle_width = size // 6
        pygame.draw.rect(surface, handle_color, 
                       (center[0] - handle_width//2, 
                        center[1], 
                        handle_width, handle_height))
        
        # 剑的护手
        guard_width = size // 2
        guard_height = size // 8
        pygame.draw.rect(surface, metal_color, 
                       (center[0] - guard_width//2, 
                        center[1] - guard_height//2, 
                        guard_width, guard_height))
        
        # 剑的刀刃
        blade_width = size // 4
        blade_height = 2 * size // 3
        
        # 刀刃底部(和护手接触的部分)
        pygame.draw.rect(surface, metal_color, 
                       (center[0] - blade_width//2, 
                        center[1] - blade_height, 
                        blade_width, blade_height))
        
        # 刀刃尖端(三角形)
        tip_points = [
            (center[0] - blade_width//2, center[1] - blade_height),
            (center[0] + blade_width//2, center[1] - blade_height),
            (center[0], center[1] - blade_height - size//6)
        ]
        pygame.draw.polygon(surface, metal_color, tip_points)
        
        # 刀刃高光
        highlight_color = (240, 240, 250)
        pygame.draw.line(surface, highlight_color, 
                       (center[0], center[1] - blade_height), 
                       (center[0], center[1] - blade_height - size//6), 2)
        
    elif icon_type == "shield":
        # 盾牌图标
        shield_color = (60, 90, 150)
        border_color = (180, 180, 210)
        emblem_color = (200, 40, 40)
        
        # 盾牌形状 (圆形底部的盾牌)
        shield_points = []
        for angle in range(-90, 91):
            rad = np.radians(angle)
            x = center[0] + radius * np.cos(rad)
            y = center[1] + radius * np.sin(rad) * 0.7
            shield_points.append((x, y))
        
        # 添加底部点使其成为闭合多边形
        shield_points.append((center[0] + radius//2, center[1] + radius))
        shield_points.append((center[0] - radius//2, center[1] + radius))
        
        pygame.draw.polygon(surface, shield_color, shield_points)
        pygame.draw.polygon(surface, border_color, shield_points, 2)
        
        # 盾牌上的徽章
        emblem_radius = radius // 3
        pygame.draw.circle(surface, emblem_color, center, emblem_radius)
        pygame.draw.circle(surface, border_color, center, emblem_radius, 1)
        
    elif icon_type == "magic":
        # 魔法图标
        magic_color = (100, 80, 220)
        glow_color = (180, 160, 255)
        
        # 魔法棒
        wand_length = size // 2
        wand_width = size // 10
        wand_color = (180, 140, 100)
        
        # 45度角的魔法棒
        wand_start = (center[0] - wand_length//2, center[1] + wand_length//2)
        wand_end = (center[0] + wand_length//2, center[1] - wand_length//2)
        
        pygame.draw.line(surface, wand_color, wand_start, wand_end, wand_width)
        
        # 魔法棒顶端的星星
        star_pos = (center[0] + wand_length//2, center[1] - wand_length//2)
        star_radius = size // 6
        
        # 绘制星星
        star_points = []
        for i in range(5):
            angle = np.pi/2 + i * 2*np.pi/5
            x = star_pos[0] + star_radius * np.cos(angle)
            y = star_pos[1] + star_radius * np.sin(angle)
            star_points.append((x, y))
            
            # 内点
            inner_angle = angle + np.pi/5
            inner_radius = star_radius // 2
            x = star_pos[0] + inner_radius * np.cos(inner_angle)
            y = star_pos[1] + inner_radius * np.sin(inner_angle)
            star_points.append((x, y))
        
        pygame.draw.polygon(surface, magic_color, star_points)
        pygame.draw.polygon(surface, glow_color, star_points, 1)
        
        # 魔法粒子
        for _ in range(10):
            particle_size = random.randint(1, 3)
            particle_x = random.randint(0, size-1)
            particle_y = random.randint(0, size-1)
            
            # 随机魔法颜色
            particle_color = (
                random.randint(100, 200),
                random.randint(80, 180),
                random.randint(200, 255)
            )
            
            pygame.draw.circle(surface, particle_color, 
                             (particle_x, particle_y), particle_size)
            
    return surface

def create_bar(width=200, height=20, value=0.7, bar_type="health"):
    """
    生成状态条
    
    参数:
        width, height: 状态条尺寸
        value: 填充值(0.0-1.0)
        bar_type: 状态条类型(health/mana/xp)
    """
    surface = pygame.Surface((width, height), pygame.SRCALPHA)
    
    # 颜色设置
    if bar_type == "health":
        bg_color = (60, 20, 20)
        fill_color = (220, 50, 50)
        border_color = (140, 30, 30)
    elif bar_type == "mana":
        bg_color = (20, 20, 60)
        fill_color = (50, 50, 220)
        border_color = (30, 30, 140)
    elif bar_type == "xp":
        bg_color = (20, 60, 20)
        fill_color = (50, 220, 50)
        border_color = (30, 140, 30)
    else:
        bg_color = (40, 40, 40)
        fill_color = (200, 200, 200)
        border_color = (100, 100, 100)
    
    # 绘制背景
    pygame.draw.rect(surface, bg_color, (0, 0, width, height), border_radius=height//4)
    
    # 绘制填充部分
    fill_width = int(width * value)
    if fill_width > 0:
        pygame.draw.rect(surface, fill_color, 
                       (0, 0, fill_width, height), 
                       border_radius=height//4)
    
    # 绘制边框
    pygame.draw.rect(surface, border_color, 
                   (0, 0, width, height), 
                   2, border_radius=height//4)
    
    # 细节纹理
    for i in range(0, width, width//10):
        marker_height = height // 3
        marker_y = (height - marker_height) // 2
        marker_color = (255, 255, 255, 30)  # 半透明
        
        pygame.draw.line(surface, marker_color, 
                       (i, marker_y), 
                       (i, marker_y + marker_height), 
                       1)
    
    return surface

def create_background(width=800, height=600, bg_type="menu"):
    """
    生成UI背景
    
    参数:
        width, height: 背景尺寸
        bg_type: 背景类型(menu/game/dark)
    """
    surface = pygame.Surface((width, height))
    
    if bg_type == "menu":
        # 菜单背景 - 优雅的渐变
        start_color = (40, 40, 80)
        end_color = (80, 80, 120)
        
        # 绘制垂直渐变
        for y in range(height):
            progress = y / height
            color = (
                int(start_color[0] + (end_color[0] - start_color[0]) * progress),
                int(start_color[1] + (end_color[1] - start_color[1]) * progress),
                int(start_color[2] + (end_color[2] - start_color[2]) * progress)
            )
            pygame.draw.line(surface, color, (0, y), (width, y))
        
        # 添加装饰元素
        for _ in range(50):
            x = random.randint(0, width-1)
            y = random.randint(0, height-1)
            radius = random.randint(1, 3)
            intensity = random.randint(120, 200)
            color = (intensity, intensity, intensity)
            
            pygame.draw.circle(surface, color, (x, y), radius)
        
    elif bg_type == "game":
        # 游戏背景 - 简洁暗色
        base_color = (30, 30, 40)
        surface.fill(base_color)
        
        # 添加细微的网格线
        grid_color = (40, 40, 50)
        grid_spacing = 50
        
        for x in range(0, width, grid_spacing):
            pygame.draw.line(surface, grid_color, (x, 0), (x, height), 1)
            
        for y in range(0, height, grid_spacing):
            pygame.draw.line(surface, grid_color, (0, y), (width, y), 1)
            
        # 在角落添加装饰元素
        decoration_color = (50, 50, 70)
        decoration_size = min(width, height) // 6
        
        # 左上角装饰
        pygame.draw.rect(surface, decoration_color, 
                       (0, 0, decoration_size, 2))
        pygame.draw.rect(surface, decoration_color, 
                       (0, 0, 2, decoration_size))
        
        # 右上角装饰
        pygame.draw.rect(surface, decoration_color, 
                       (width - decoration_size, 0, decoration_size, 2))
        pygame.draw.rect(surface, decoration_color, 
                       (width - 2, 0, 2, decoration_size))
        
        # 左下角装饰
        pygame.draw.rect(surface, decoration_color, 
                       (0, height - 2, decoration_size, 2))
        pygame.draw.rect(surface, decoration_color, 
                       (0, height - decoration_size, 2, decoration_size))
        
        # 右下角装饰
        pygame.draw.rect(surface, decoration_color, 
                       (width - decoration_size, height - 2, decoration_size, 2))
        pygame.draw.rect(surface, decoration_color, 
                       (width - 2, height - decoration_size, 2, decoration_size))
        
    elif bg_type == "dark":
        # 黑暗主题背景 - 用于弹出窗口等
        base_color = (20, 20, 25)
        surface.fill(base_color)
        
        # 添加微妙的噪点纹理
        for _ in range(width * height // 20):
            x = random.randint(0, width-1)
            y = random.randint(0, height-1)
            intensity = random.randint(0, 10)
            color = (
                min(255, base_color[0] + intensity),
                min(255, base_color[1] + intensity),
                min(255, base_color[2] + intensity)
            )
            surface.set_at((x, y), color)
        
        # 添加暗淡的边框
        border_color = (40, 40, 50)
        border_width = 2
        
        pygame.draw.rect(surface, border_color, 
                       (0, 0, width, height), 
                       border_width)
    
    return surface

def create_frame(width=200, height=200, style="basic"):
    """
    生成UI框架
    
    参数:
        width, height: 框架尺寸
        style: 框架样式(basic/ornate/minimal)
    """
    surface = pygame.Surface((width, height), pygame.SRCALPHA)
    
    if style == "basic":
        # 基础框架 - 简单矩形
        bg_color = (40, 40, 50, 220)  # 半透明背景
        border_color = (80, 80, 100)
        
        # 绘制半透明背景
        pygame.draw.rect(surface, bg_color, (0, 0, width, height), border_radius=5)
        
        # 绘制边框
        pygame.draw.rect(surface, border_color, (0, 0, width, height), 2, border_radius=5)
        
        # 添加简单的内侧高光
        highlight_color = (100, 100, 120)
        pygame.draw.line(surface, highlight_color, (3, 3), (width-3, 3))
        pygame.draw.line(surface, highlight_color, (3, 3), (3, height-3))
        
    elif style == "ornate":
        # 华丽框架 - 装饰角落
        bg_color = (60, 40, 30, 230)  # 半透明暗棕色
        border_color = (140, 100, 60)
        accent_color = (180, 140, 80)
        
        # 绘制背景和边框
        pygame.draw.rect(surface, bg_color, (0, 0, width, height))
        pygame.draw.rect(surface, border_color, (0, 0, width, height), 3)
        
        # 内框
        inner_margin = 8
        pygame.draw.rect(surface, accent_color, 
                       (inner_margin, inner_margin, 
                        width - inner_margin*2, height - inner_margin*2), 
                       1)
        
        # 角落装饰
        corner_size = min(width, height) // 5
        corner_positions = [
            (0, 0),
            (width - corner_size, 0),
            (0, height - corner_size),
            (width - corner_size, height - corner_size)
        ]
        
        for pos in corner_positions:
            # 角落螺旋装饰
            for i in range(0, corner_size, 3):
                pygame.draw.rect(surface, accent_color, 
                               (pos[0] + i, pos[1] + i, 
                                corner_size - i*2, corner_size - i*2), 
                               1)
        
        # 绘制中央区域分隔线
        pygame.draw.line(surface, accent_color, 
                       (0, height // 4), 
                       (width, height // 4), 
                       1)
        pygame.draw.line(surface, accent_color, 
                       (0, height * 3 // 4), 
                       (width, height * 3 // 4), 
                       1)
        
    elif style == "minimal":
        # 极简风格 - 细边框
        bg_color = (30, 30, 35, 200)  # 半透明
        border_color = (100, 100, 100)
        
        # 背景
        pygame.draw.rect(surface, bg_color, (0, 0, width, height))
        
        # 仅在顶部和底部绘制细线
        pygame.draw.line(surface, border_color, (0, 0), (width, 0), 1)
        pygame.draw.line(surface, border_color, (0, height-1), (width, height-1), 1)
        
        # 在左侧和右侧绘制更短的线条
        line_height = height // 6
        pygame.draw.line(surface, border_color, (0, 0), (0, line_height), 1)
        pygame.draw.line(surface, border_color, (0, height-line_height), (0, height), 1)
        
        pygame.draw.line(surface, border_color, (width-1, 0), (width-1, line_height), 1)
        pygame.draw.line(surface, border_color, (width-1, height-line_height), (width-1, height), 1)
    
    return surface 