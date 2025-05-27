#ifndef BOSSENEMY_HPP
#define BOSSENEMY_HPP
#include "Enemy.hpp"
#include "Engine/Sprite.hpp"

class BossEnemy : public Enemy
{

public:
    BossEnemy(int x, int y);
    void Draw() const override;
    void Update(float deltaTime) override;
};
#endif // BOSSENEMY_HPP
