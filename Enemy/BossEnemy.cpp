#include <allegro5/base.h>
#include <random>
#include <string>
#include <cmath>

#include "Engine/Point.hpp"
#include "BossEnemy.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"

BossEnemy::BossEnemy(int x, int y)
    : Enemy("play/boss_kbtit.png", x, y, 300, 400, 150, 10, 1000, 100)
{
}

void BossEnemy::Draw() const
{
    Enemy::Draw();
}

void BossEnemy::Update(float deltaTime)
{
    timeSinceHit += deltaTime;
    if (timeSinceHit < hitEffectTime)
    {
        float alpha = 1.0f - (timeSinceHit / hitEffectTime);
        Tint = al_map_rgba(255, 255 * (1 - alpha), 255 * (1 - alpha), 255);
    }
    else
        Tint = al_map_rgba(255, 255, 255, 255); // Reset to normal color

    // Calculate the target position (end point)
    Engine::Point target = PlayScene::EndGridPoint * PlayScene::BlockSize + Engine::Point(PlayScene::BlockSize / 2, PlayScene::BlockSize / 2);
    Engine::Point vec = target - Position;

    // Calculate remaining time to reach end
    reachEndTime = vec.Magnitude() / speed;

    // Move directly towards the end point
    if (vec.Magnitude() > speed * deltaTime)
    {
        Engine::Point normalized = vec.Normalize();
        Velocity = normalized * speed;
        Position = Position + (Velocity * deltaTime);
    }
    else
    {
        Engine::GameEngine::GetInstance().ChangeScene("lose");
    }

    Rotation = 0; // Keep boss sprite upright
    Sprite::Update(deltaTime);
}
