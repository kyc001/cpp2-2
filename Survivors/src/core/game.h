#pragma once

#include "common.h"
#include "../entities/enemy.h"
#include "../entities/entity.h"
#include "../entities/player.h"
#include "../weapons/weapon.h"
#include "gameData.h"

// 声明game.cpp中所有公共函数接口
void spawn_enemy(EnemyType type, Vec2 pos);
void player_add_weapon(WeaponID ID);
Circle get_collider(Player *p);
Circle get_collider(Entity *e);
Weapon *get_weapon(WeaponID weaponID);
bool is_damaging_area_hitting_entity(DamagingArea *da, Entity *e);
void inflict_damage(Entity *e, int damage);
void add_damaging_area(WeaponID weaponID, Vec2 pos, Vec2 size, int damage, float duration);

// 游戏主循环和初始化函数
void init_game();
void update_game(float dt);
void render_game();
