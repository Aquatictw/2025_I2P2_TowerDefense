#ifndef ENEMY_HPP
#define ENEMY_HPP
#include <list>
#include <string>
#include <vector>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"

class Bullet;
class PlayScene;
class Turret;

class Enemy : public Engine::Sprite
{
protected:
    std::vector<Engine::Point> path;
    float speed;
    float hp;
    int money;
    PlayScene *getPlayScene();
    virtual void OnExplode();

public:
    float reachEndTime;
    float timeSinceHit;               // Track time since last hit
    const float hitEffectTime = 1.0f; // Duration of red tint effect
    float CollisionRadius;
    std::list<Turret *> lockedTurrets;
    std::list<Bullet *> lockedBullets;
    Enemy(std::string img, float x, float y, float w, float h, float radius, float speed, float hp, int money);
    void Hit(float damage);
    void UpdatePath(const std::vector<std::vector<int>> &mapDistance);
    void Update(float deltaTime) override;
    void Draw() const override;
    float GetHP() const { return hp; }
};
#endif // ENEMY_HPP
