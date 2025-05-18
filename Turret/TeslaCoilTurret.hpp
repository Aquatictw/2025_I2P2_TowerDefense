#ifndef TESLACOILTURRET_HPP
#define TESLACOILTURRET_HPP
#include "Turret.hpp"

class TeslaCoilTurret : public Turret
{

public:
    static const int Price;
    TeslaCoilTurret(float x, float y);

    void Update(float deltaTime) override;
    void CreateBullet() override;
};
#endif // TESLACOILTURRET_HPP
