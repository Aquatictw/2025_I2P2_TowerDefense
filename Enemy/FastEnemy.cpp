#include <string>
#include "FastEnemy.hpp"

FastEnemy::FastEnemy(int x, int y) : Enemy("play/enemy-2.png", x, y, 70, 70, 15, 100, 3, 10)
{
    // Fast enemy with:
    // - radius: 15 (smaller hitbox)
    // - speed: 100 (very fast)
    // - hp: 3 (very low health)
    // - money: 10 (reward for killing)
}
