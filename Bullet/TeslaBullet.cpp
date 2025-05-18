#include <allegro5/base.h>
#include <random>
#include <string>

#include "Enemy/Enemy.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "Engine/LOG.hpp"
#include "TeslaBullet.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Scene/PlayScene.hpp"
#include "Turret/Turret.hpp"

TeslaBullet::TeslaBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret *parent, float chainRange, int maxChainCount, std::vector<Enemy *> hitTargets)
    : Bullet("play/tesla-bullet.png", 200, 6, position, forwardDirection, rotation - ALLEGRO_PI / 2, parent)
{
    this->chainRange = chainRange;
    this->maxChainCount = maxChainCount;
}

void TeslaBullet::OnExplode(Enemy *enemy)
{

    if (maxChainCount > 0) // if there are still chain targets to find
        FindNextChainTarget(enemy);

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(2, 5);
    getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/light-2.png", dist(rng), enemy->Position.x, enemy->Position.y));
}

void TeslaBullet::FindNextChainTarget(Enemy *currentEnemy)
{
    Enemy *nextTarget = nullptr;
    float minDistance = chainRange;

    // Find the nearest enemy within range
    for (auto &it : getPlayScene()->EnemyGroup->GetObjects())
    {
        Enemy *enemy = dynamic_cast<Enemy *>(it);
        if (enemy != nullptr && enemy != currentEnemy)
        {
            float distance = (enemy->Position - currentEnemy->Position).Magnitude();
            if (distance <= chainRange && distance < minDistance)
            {
                minDistance = distance;
                nextTarget = enemy;
            }
        }
    }

    if (nextTarget)
    {
        // Create new bullet targeting the next enemy
        Engine::Point diff = nextTarget->Position - currentEnemy->Position;
        float rotation = atan2(diff.y, diff.x);
        Engine::Point normalized = diff.Normalize();

        // Create a new bullet at the current enemy's position, passing along the list of hit targets
        getPlayScene()->BulletGroup->AddNewObject(
            new TeslaBullet(currentEnemy->Position, normalized, rotation, parent, chainRange, maxChainCount - 1));
    }
}
