#ifndef TESLABULLET_HPP
#define TESLABULLET_HPP
#include "Bullet.hpp"
#include <vector>

class Enemy;
class Turret;

class TeslaBullet : public Bullet
{
private:
    float chainRange;
    int maxChainCount;

public:
    TeslaBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret *parent, float chainRange, int maxChainCount, std::vector<Enemy *> hitTargets = std::vector<Enemy *>());
    void OnExplode(Enemy *enemy) override;
    void FindNextChainTarget(Enemy *currentEnemy);
};
#endif // TESLABULLET_HPP
