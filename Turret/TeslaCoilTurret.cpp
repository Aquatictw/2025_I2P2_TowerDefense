#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Enemy/Enemy.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "Bullet/TeslaBullet.hpp"
#include "Scene/PlayScene.hpp"
#include "TeslaCoilTurret.hpp"
#include "Engine/AudioHelper.hpp"

const int TeslaCoilTurret::Price = 200;
TeslaCoilTurret::TeslaCoilTurret(float x, float y) : Turret("play/tower-base.png", "play/tesla-turret.png", x, y, 200, Price, 2)
{
}

void TeslaCoilTurret::CreateBullet()
{
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();

    // Create Tesla bullet that chains between enemies
    getPlayScene()->BulletGroup->AddNewObject(new TeslaBullet(Position + normalized * 36, diff, rotation, this, 1000, 5));
    AudioHelper::PlayAudio("tesla_bullet.wav");
}

void TeslaCoilTurret::Update(float deltaTime)
{
    Turret::Update(deltaTime);

    // // Find chain targets
    // chainTargets.clear();
}
