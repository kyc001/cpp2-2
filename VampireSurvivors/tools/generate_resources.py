#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
from PIL import Image, ImageDraw

def create_default_image(size, color, filename):
    """创建默认图片"""
    img = Image.new('RGBA', size, color)
    draw = ImageDraw.Draw(img)
    
    # 添加一些基本图形
    draw.rectangle([0, 0, size[0]-1, size[1]-1], outline=(255, 255, 255))
    
    # 保存图片
    img.save(filename)

def generate_default_resources():
    """生成默认资源文件"""
    # 创建目录
    dirs = [
        'resources/images',
        'resources/sounds',
        'resources/maps'
    ]
    
    for dir_path in dirs:
        os.makedirs(dir_path, exist_ok=True)
    
    # 生成默认图片
    images = {
        'hero.png': ((32, 32), (255, 0, 0, 255)),      # 红色方块代表英雄
        'enemy.png': ((32, 32), (0, 0, 255, 255)),     # 蓝色方块代表敌人
        'bullet.png': ((16, 16), (255, 255, 0, 255)),  # 黄色方块代表子弹
        'drop.png': ((16, 16), (0, 255, 0, 255)),      # 绿色方块代表掉落物
        'weapon.png': ((32, 32), (255, 165, 0, 255)),  # 橙色方块代表武器
        'obstacle.png': ((32, 32), (128, 128, 128, 255))  # 灰色方块代表障碍物
    }
    
    for filename, (size, color) in images.items():
        create_default_image(size, color, os.path.join('resources/images', filename))
    
    # 生成默认地图
    maps = {
        'map1.png': ((800, 600), (50, 50, 50, 255)),   # 深灰色背景
        'map2.png': ((800, 600), (30, 30, 30, 255))    # 更深的灰色背景
    }
    
    for filename, (size, color) in maps.items():
        create_default_image(size, color, os.path.join('resources/maps', filename))

if __name__ == '__main__':
    generate_default_resources()
    print("Resource generation complete!") 