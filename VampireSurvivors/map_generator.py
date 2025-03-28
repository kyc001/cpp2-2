#!/usr/bin/env python
import pygame
import numpy as np
from PIL import Image, ImageDraw
import random
import colorsys

def create_floor_tile(floor_type="grass", size=64):
    """
    生成地板贴图
    
    参数:
        floor_type: 地板类型 (grass/dirt/stone/wood)
        size: 贴图大小
    """
    img = Image.new('RGBA', (size, size))
    draw = ImageDraw.Draw(img)
    
    # 基础颜色
    base_colors = {
        "grass": (60, 120, 50),
        "dirt": (120, 85, 50),
        "stone": (130, 130, 140),
        "wood": (150, 100, 60)
    }
    
    base_color = base_colors.get(floor_type, (100, 100, 100))
    
    # 填充基础颜色
    draw.rectangle([(0, 0), (size, size)], fill=base_color)
    
    # 添加纹理
    if floor_type == "grass":
        # 草地纹理 - 小草和不同色调
        for _ in range(size * 3):
            x = random.randint(0, size-1)
            y = random.randint(0, size-1)
            r, g, b = base_color
            # 随机调整绿色
            g_variance = random.randint(-15, 15)
            grass_color = (r, min(255, max(0, g + g_variance)), b)
            
            # 随机画点或小草
            if random.random() > 0.7:
                # 画小草
                grass_height = random.randint(1, 3)
                draw.line([(x, y), (x, y-grass_height)], fill=grass_color, width=1)
            else:
                # 画点
                draw.point((x, y), fill=grass_color)
                
    elif floor_type == "dirt":
        # 泥土纹理 - 不规则纹理和小石头
        for _ in range(size * 2):
            x = random.randint(0, size-1)
            y = random.randint(0, size-1)
            r, g, b = base_color
            
            # 随机泥土颜色
            variance = random.randint(-10, 10)
            dirt_color = (r + variance, g + variance, b + variance)
            
            # 画不规则点
            size_var = random.randint(1, 3)
            if size_var == 1:
                draw.point((x, y), fill=dirt_color)
            else:
                draw.ellipse([(x-size_var//2, y-size_var//2), 
                              (x+size_var//2, y+size_var//2)], fill=dirt_color)
                
        # 添加少量小石头
        for _ in range(size // 8):
            x = random.randint(0, size-1)
            y = random.randint(0, size-1)
            stone_size = random.randint(1, 3)
            stone_color = (100 + random.randint(-20, 20), 
                          100 + random.randint(-20, 20), 
                          100 + random.randint(-20, 20))
            
            draw.ellipse([(x-stone_size, y-stone_size), 
                           (x+stone_size, y+stone_size)], fill=stone_color)
            
    elif floor_type == "stone":
        # 石头纹理 - 裂缝和不同阴影
        # 基础纹理
        for _ in range(size * 2):
            x = random.randint(0, size-1)
            y = random.randint(0, size-1)
            r, g, b = base_color
            
            # 随机石头纹理
            variance = random.randint(-20, 20)
            stone_color = (r + variance, g + variance, b + variance)
            
            draw.point((x, y), fill=stone_color)
        
        # 添加裂缝
        for _ in range(size // 10):
            x = random.randint(0, size-1)
            y = random.randint(0, size-1)
            length = random.randint(3, 10)
            angle = random.uniform(0, 2 * np.pi)
            
            end_x = x + int(length * np.cos(angle))
            end_y = y + int(length * np.sin(angle))
            
            draw.line([(x, y), (end_x, end_y)], fill=(80, 80, 90), width=1)
            
    elif floor_type == "wood":
        # 木头纹理 - 平行木纹
        plank_width = size // 4
        
        for i in range(0, size, plank_width):
            # 每块木板颜色略有不同
            r, g, b = base_color
            variance = random.randint(-10, 10)
            plank_color = (r + variance, g + variance, b + variance)
            
            # 绘制木板
            draw.rectangle([(i, 0), (i + plank_width - 1, size)], fill=plank_color)
            
            # 添加木纹
            for j in range(3):
                grain_x = i + random.randint(0, plank_width-1)
                grain_color = (r - 20, g - 20, b - 20)
                draw.line([(grain_x, 0), (grain_x, size)], fill=grain_color, width=1)
            
            # 添加木节
            if random.random() > 0.5:
                knot_x = i + plank_width // 2
                knot_y = random.randint(0, size)
                knot_size = random.randint(2, 5)
                knot_color = (r - 30, g - 30, b - 30)
                
                draw.ellipse([(knot_x - knot_size, knot_y - knot_size),
                             (knot_x + knot_size, knot_y + knot_size)], fill=knot_color)
    
    return img

def create_obstacle(obs_type="tree", size=32, is_decoration=False):
    """
    生成障碍物或装饰物的像素图
    
    参数:
        obs_type: 障碍物类型
        size: 图像大小
        is_decoration: 是否为装饰物
    """
    surface = pygame.Surface((size, size), pygame.SRCALPHA)
    
    if obs_type == "tree":
        # 树干
        trunk_width = size // 8
        trunk_height = size // 2
        trunk_color = (101, 67, 33)
        trunk_x = (size - trunk_width) // 2
        trunk_y = size - trunk_height
        
        pygame.draw.rect(surface, trunk_color, (trunk_x, trunk_y, trunk_width, trunk_height))
        
        # 树冠
        crown_radius = size // 3
        crown_center = (size // 2, size // 2 - crown_radius // 2)
        crown_color = (34, 139, 34)
        
        pygame.draw.circle(surface, crown_color, crown_center, crown_radius)
        
        # 变暗的底部树冠
        darker_crown = (24, 100, 24)
        pygame.draw.circle(surface, darker_crown, 
                          (crown_center[0], crown_center[1] + crown_radius//2), 
                          crown_radius - crown_radius//4)
        
    elif obs_type == "rock":
        # 岩石 - 不规则多边形
        rock_color = (130, 130, 140)
        center = (size // 2, size // 2)
        
        # 生成不规则多边形的顶点
        points = []
        num_points = random.randint(6, 10)
        for i in range(num_points):
            angle = i * 2 * np.pi / num_points
            radius = random.uniform(size // 4, size // 3)
            x = center[0] + radius * np.cos(angle)
            y = center[1] + radius * np.sin(angle)
            points.append((x, y))
        
        pygame.draw.polygon(surface, rock_color, points)
        
        # 添加岩石纹理
        for _ in range(size):
            x = random.randint(0, size-1)
            y = random.randint(0, size-1)
            
            if surface.get_at((x, y))[3] > 0:  # 如果不是透明点
                # 随机调整颜色
                variance = random.randint(-20, 20)
                color = (min(255, max(0, rock_color[0] + variance)),
                         min(255, max(0, rock_color[1] + variance)),
                         min(255, max(0, rock_color[2] + variance)))
                
                surface.set_at((x, y), color)
                
    elif obs_type == "wall":
        # 墙壁 - 砖块结构
        wall_color = (180, 180, 190)
        brick_height = size // 6
        brick_width = size // 3
        
        for y in range(0, size, brick_height):
            # 每行偏移半个砖块
            offset = brick_width // 2 if (y // brick_height) % 2 == 1 else 0
            
            for x in range(-offset, size, brick_width):
                if x + brick_width <= size and y + brick_height <= size:
                    # 砖块颜色随机变化
                    variance = random.randint(-15, 15)
                    brick_color = (min(255, max(0, wall_color[0] + variance)),
                                 min(255, max(0, wall_color[1] + variance)),
                                 min(255, max(0, wall_color[2] + variance)))
                    
                    # 绘制砖块
                    pygame.draw.rect(surface, brick_color, 
                                   (x, y, brick_width - 1, brick_height - 1))
                    
                    # 砖块边缘阴影
                    shadow_color = (brick_color[0] - 30, brick_color[1] - 30, brick_color[2] - 30)
                    pygame.draw.line(surface, shadow_color, 
                                   (x, y + brick_height - 1), 
                                   (x + brick_width - 1, y + brick_height - 1))
                    pygame.draw.line(surface, shadow_color, 
                                   (x + brick_width - 1, y), 
                                   (x + brick_width - 1, y + brick_height - 1))
        
    elif obs_type == "crate":
        # 木箱
        crate_color = (150, 100, 60)
        pygame.draw.rect(surface, crate_color, (0, 0, size, size))
        
        # 箱子边框
        border_color = (120, 80, 40)
        border_width = size // 16
        
        # 绘制边框
        pygame.draw.rect(surface, border_color, (0, 0, size, size), border_width)
        pygame.draw.line(surface, border_color, (0, size // 2), (size, size // 2), border_width)
        pygame.draw.line(surface, border_color, (size // 2, 0), (size // 2, size), border_width)
        
    elif obs_type == "barrel":
        # 木桶
        barrel_color = (140, 90, 50)
        barrel_radius = size // 2 - 2
        center = (size // 2, size // 2)
        
        # 主体
        pygame.draw.circle(surface, barrel_color, center, barrel_radius)
        
        # 桶箍 (金属环)
        hoop_color = (180, 180, 180)
        for offset in [barrel_radius // 3, 2 * barrel_radius // 3]:
            pygame.draw.ellipse(surface, hoop_color, 
                              (center[0] - barrel_radius, center[1] - offset // 2, 
                               barrel_radius * 2, offset), 
                              1)
        
    # 装饰物
    elif is_decoration:
        if obs_type == "flower":
            # 花 - 鲜艳的小装饰
            stem_color = (60, 120, 30)
            stem_width = 1
            stem_height = size // 3
            stem_x = size // 2
            stem_y = size - 1
            
            # 绘制花茎
            pygame.draw.line(surface, stem_color, (stem_x, stem_y), 
                           (stem_x, stem_y - stem_height), stem_width)
            
            # 随机花色
            h = random.random()  # 随机色相
            s = random.uniform(0.7, 1.0)  # 高饱和度
            v = random.uniform(0.8, 1.0)  # 高亮度
            rgb = tuple(int(c * 255) for c in colorsys.hsv_to_rgb(h, s, v))
            
            # 花朵
            flower_radius = size // 8
            flower_center = (stem_x, stem_y - stem_height)
            
            # 花瓣
            petal_count = random.randint(5, 8)
            for i in range(petal_count):
                angle = i * 2 * np.pi / petal_count
                petal_x = flower_center[0] + flower_radius * np.cos(angle)
                petal_y = flower_center[1] + flower_radius * np.sin(angle)
                
                pygame.draw.circle(surface, rgb, (int(petal_x), int(petal_y)), flower_radius // 2)
            
            # 花蕊
            pygame.draw.circle(surface, (255, 255, 0), flower_center, flower_radius // 3)
            
        elif obs_type == "grass":
            # 草 - 小的草丛
            grass_color = (60, 130, 30)
            
            # 绘制多个草叶
            for _ in range(random.randint(3, 7)):
                height = random.randint(size // 4, size // 2)
                base_x = random.randint(size // 4, 3 * size // 4)
                base_y = size - 1
                
                # 草叶弯曲方向
                curve = random.uniform(-0.5, 0.5)
                
                # 控制点
                control_x = base_x + int(height * curve)
                control_y = base_y - height // 2
                
                # 顶点
                top_x = base_x + int(height * curve * 0.5)
                top_y = base_y - height
                
                # 绘制贝塞尔曲线(简化为两条线)
                pygame.draw.line(surface, grass_color, (base_x, base_y), (control_x, control_y), 1)
                pygame.draw.line(surface, grass_color, (control_x, control_y), (top_x, top_y), 1)
            
        elif obs_type == "torch":
            # 火把 - 墙上的光源
            handle_color = (100, 70, 40)
            handle_width = size // 10
            handle_height = size // 2
            handle_x = size // 2 - handle_width // 2
            handle_y = size - handle_height
            
            # 绘制火把柄
            pygame.draw.rect(surface, handle_color, (handle_x, handle_y, handle_width, handle_height))
            
            # 火焰
            flame_center = (size // 2, handle_y - handle_width)
            flame_radius = size // 5
            
            # 绘制多层火焰
            flame_colors = [(255, 180, 0), (255, 100, 0), (255, 50, 0)]
            for i, color in enumerate(flame_colors):
                adjusted_radius = flame_radius * (3 - i) / 3
                
                # 火焰形状
                points = []
                for angle in range(0, 360, 30):
                    rad = np.radians(angle)
                    # 使火焰顶部更高
                    height_factor = 1.0 + 0.5 * np.sin(rad) if np.sin(rad) > 0 else 1.0
                    radius = adjusted_radius * height_factor
                    
                    x = flame_center[0] + radius * np.cos(rad)
                    y = flame_center[1] + radius * np.sin(rad)
                    points.append((x, y))
                
                pygame.draw.polygon(surface, color, points)
            
        elif obs_type == "gravestone":
            # 墓碑 - 墓地装饰
            stone_color = (140, 140, 150)
            
            # 墓碑形状
            width = size - size // 4
            height = size - size // 3
            base_x = (size - width) // 2
            base_y = size - height
            
            # 绘制主体
            pygame.draw.rect(surface, stone_color, (base_x, base_y, width, height))
            
            # 圆形顶部
            top_radius = width // 2
            pygame.draw.circle(surface, stone_color, (size // 2, base_y), top_radius)
            
            # 墓碑纹理
            for _ in range(size):
                x = random.randint(base_x, base_x + width - 1)
                y = random.randint(base_y - top_radius, size - 1)
                
                # 检查是否在墓碑内
                in_rect = base_x <= x < base_x + width and base_y <= y < size
                in_circle = ((x - size//2)**2 + (y - base_y)**2) <= top_radius**2 and y <= base_y
                
                if in_rect or in_circle:
                    # 随机调整颜色
                    variance = random.randint(-15, 15)
                    color = stone_color[0] + variance, stone_color[1] + variance, stone_color[2] + variance
                    surface.set_at((x, y), color)
            
            # 刻痕
            cross_x = size // 2
            cross_y = base_y + height // 3
            cross_width = width // 4
            cross_height = height // 3
            
            pygame.draw.line(surface, (70, 70, 80), 
                           (cross_x, cross_y - cross_height // 2), 
                           (cross_x, cross_y + cross_height // 2), 2)
            pygame.draw.line(surface, (70, 70, 80), 
                           (cross_x - cross_width // 2, cross_y), 
                           (cross_x + cross_width // 2, cross_y), 2)
    
    return surface

def generate_dungeon(width=200, height=200, map_type="dungeon", seed=None):
    """
    使用噪声生成地图
    
    参数:
        width, height: 地图大小
        map_type: 地图类型 (dungeon/forest/cemetery)
        seed: 随机种子
    """
    if seed:
        random.seed(seed)
        np.random.seed(seed)
    
    # 创建图像
    img = Image.new('RGB', (width, height))
    pixels = img.load()
    
    # 生成噪声
    noise = np.zeros((height, width))
    scale = 20.0  # 噪声尺度
    
    for y in range(height):
        for x in range(width):
            # 简单噪声
            nx = x / width - 0.5
            ny = y / height - 0.5
            noise[y][x] = np.sin(nx * scale) * np.cos(ny * scale)
            
            # 添加随机性
            noise[y][x] += np.random.normal(0, 0.2)
    
    # 根据地图类型选择颜色和结构
    if map_type == "dungeon":
        # 地牢 - 黑暗、紧凑的走廊
        wall_color = (50, 45, 60)
        floor_color = (100, 90, 100)
        accent_color = (150, 40, 40)  # 血迹或火把
        
        # 通过噪声阈值生成墙壁和地板
        for y in range(height):
            for x in range(width):
                value = noise[y][x]
                
                if value < -0.1:  # 墙壁
                    pixels[x,y] = wall_color
                elif value < 0.4:  # 地板
                    pixels[x,y] = floor_color
                else:              # 地板变种
                    pixels[x,y] = tuple(c + random.randint(-10, 10) for c in floor_color)
        
        # 添加随机血迹/火把
        for _ in range(width * height // 500):
            x, y = random.randint(0, width-1), random.randint(0, height-1)
            if pixels[x,y] != wall_color:  # 只在地板上添加
                # 小型斑点
                spot_size = random.randint(2, 5)
                for dx in range(-spot_size, spot_size+1):
                    for dy in range(-spot_size, spot_size+1):
                        if (0 <= x+dx < width and 0 <= y+dy < height and 
                            (dx**2 + dy**2) <= spot_size**2):
                            # 随机化斑点颜色
                            color_var = random.randint(-30, 30)
                            spot_color = (min(255, accent_color[0] + color_var), 
                                         accent_color[1], 
                                         accent_color[2])
                            pixels[x+dx, y+dy] = spot_color
    
    elif map_type == "forest":
        # 森林 - 自然区域和树木
        ground_color = (80, 120, 60)  # 草地
        path_color = (150, 130, 100)  # 小路
        obstacle_color = (40, 80, 30)  # 树木/丛林
        
        # 生成地形
        for y in range(height):
            for x in range(width):
                value = noise[y][x]
                
                if value < -0.2:  # 密集的树木
                    pixels[x,y] = obstacle_color
                elif value < 0.1:  # 草地
                    pixels[x,y] = tuple(c + random.randint(-10, 10) for c in ground_color)
                else:  # 小路
                    pixels[x,y] = path_color
        
        # 添加随机花草
        for _ in range(width * height // 300):
            x, y = random.randint(0, width-1), random.randint(0, height-1)
            if pixels[x,y] != obstacle_color:  # 在空地上添加
                flower_color = (random.randint(150, 255), 
                              random.randint(100, 200), 
                              random.randint(100, 200))
                pixels[x,y] = flower_color
    
    elif map_type == "cemetery":
        # 墓地 - 阴森恐怖的氛围
        ground_color = (100, 100, 90)  # 灰色地面
        path_color = (120, 110, 100)  # 小路
        grave_color = (150, 150, 160)  # 墓碑
        
        # 生成地形
        for y in range(height):
            for x in range(width):
                value = noise[y][x]
                
                if value < -0.1:  # 墓碑区域
                    pixels[x,y] = grave_color if random.random() > 0.7 else ground_color
                elif value < 0.3:  # 普通地面
                    pixels[x,y] = tuple(c + random.randint(-10, 10) for c in ground_color)
                else:  # 小路
                    pixels[x,y] = path_color
        
        # 添加雾气效果
        for y in range(height):
            for x in range(width):
                if random.random() < 0.1:  # 10%的像素添加雾气
                    r, g, b = pixels[x,y]
                    # 使颜色更灰
                    fog_factor = random.uniform(0.1, 0.3)
                    r = int(r * (1 - fog_factor) + 200 * fog_factor)
                    g = int(g * (1 - fog_factor) + 200 * fog_factor)
                    b = int(b * (1 - fog_factor) + 210 * fog_factor)
                    pixels[x,y] = (r, g, b)
    
    return img 