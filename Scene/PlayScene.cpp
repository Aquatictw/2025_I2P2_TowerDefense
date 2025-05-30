#include <algorithm>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include <fstream>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "Enemy/Enemy.hpp"
#include "Enemy/SoldierEnemy.hpp"
#include "Enemy/TankEnemy.hpp"
#include "Enemy/FastEnemy.hpp"
#include "Enemy/BossEnemy.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Turret/LaserTurret.hpp"
#include "Turret/MachineGunTurret.hpp"
#include "Turret/TeslaCoilTurret.hpp"
#include "Turret/TurretButton.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Animation/Plane.hpp"
#include "UI/Component/Label.hpp"
#include "Turret/ShovelButton.hpp"

bool PlayScene::DebugMode = false;
const std::vector<Engine::Point> PlayScene::directions = {Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1)};
const int PlayScene::MapWidth = 20, PlayScene::MapHeight = 13;
const int PlayScene::BlockSize = 64;
const float PlayScene::DangerTime = 7.61;
const Engine::Point PlayScene::SpawnGridPoint = Engine::Point(-1, 0);
const Engine::Point PlayScene::EndGridPoint = Engine::Point(MapWidth, MapHeight - 1);
const std::vector<int> PlayScene::code = {
    ALLEGRO_KEY_UP, ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_DOWN};
Engine::Point PlayScene::GetClientSize()
{
    return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}
void PlayScene::Initialize()
{
    mapState.clear();
    keyStrokes.clear();
    ticks = 0;
    deathCountDown = -1;
    lives = 10;
    money = 150;
    SpeedMult = 1;
    isBossActive = false;
    maxBossHP = 0;
    currentBossHP = 0;
    bossImage = nullptr;

    // Add groups from bottom to top.
    AddNewObject(TileMapGroup = new Group());
    AddNewObject(GroundEffectGroup = new Group());
    AddNewObject(DebugIndicatorGroup = new Group());
    AddNewObject(TowerGroup = new Group());
    AddNewObject(EnemyGroup = new Group());
    AddNewObject(BulletGroup = new Group());
    AddNewObject(EffectGroup = new Group());
    // Should support buttons.
    AddNewControlObject(UIGroup = new Group());
    ReadMap();
    ReadEnemyWave();
    mapDistance = CalculateBFSDistance();
    ConstructUI();
    imgTarget = new Engine::Image("play/target.png", 0, 0);
    imgTarget->Visible = false;
    preview = nullptr;
    UIGroup->AddNewObject(imgTarget);
    // Preload Lose Scene
    deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
    Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");
    // Start BGM.
    bgmId = AudioHelper::PlayBGM("creepy.mp3");
}
void PlayScene::Terminate()
{
    AudioHelper::StopBGM(bgmId);
    AudioHelper::StopSample(deathBGMInstance);
    deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void PlayScene::Update(float deltaTime)
{
    // If we use deltaTime directly, then we might have Bullet-through-paper problem.
    if (SpeedMult == 0)
        deathCountDown = -1;
    else if (deathCountDown != -1)
        SpeedMult = 1;
    std::vector<float> reachEndTimes;

    isBossActive = false;
    for (auto &it : EnemyGroup->GetObjects())
    {
        if (auto boss = dynamic_cast<BossEnemy *>(it)) // if boss exist
        {
            isBossActive = true;
            if (maxBossHP == 0)
                maxBossHP = boss->GetHP();
            currentBossHP = boss->GetHP();
            // Draw boss image
            if (!bossImage)
            {
                bossImage = new Engine::Image("play/kbtit_title.png", healthBarX + 253, healthBarY - 140, 300, 150);
                UIGroup->AddNewObject(bossImage);
            }
            break;
        }
    }
    if (!isBossActive && bossImage)
    {
        UIGroup->RemoveObject(bossImage->GetObjectIterator());
        bossImage = nullptr;
    }

    for (auto &it : EnemyGroup->GetObjects())
        reachEndTimes.push_back(dynamic_cast<Enemy *>(it)->reachEndTime);

    std::sort(reachEndTimes.begin(), reachEndTimes.end());
    float newDeathCountDown = -1;
    int danger = lives;
    for (auto &it : reachEndTimes)
    {
        if (it <= DangerTime)
        {
            danger--;
            if (danger <= 0)
            {
                // Death Countdown
                float pos = DangerTime - it;
                if (it > deathCountDown)
                {
                    // Restart Death Count Down BGM.
                    AudioHelper::StopSample(deathBGMInstance);
                    if (SpeedMult != 0)
                    {
                        AudioHelper::StopBGM(bgmId);
                        deathBGMInstance = AudioHelper::PlaySample("lose.mp3", false, AudioHelper::BGMVolume, pos);
                    }
                }
                float alpha = pos / DangerTime;
                alpha = std::max(0, std::min(255, static_cast<int>(alpha * alpha * 255)));
                dangerIndicator->Tint = al_map_rgba(255, 255, 255, alpha);
                newDeathCountDown = it;
                break;
            }
        }
    }
    deathCountDown = newDeathCountDown;
    if (SpeedMult == 0)
        AudioHelper::StopSample(deathBGMInstance);
    if (deathCountDown == -1 && lives > 0)
    {
        AudioHelper::StopSample(deathBGMInstance);
        dangerIndicator->Tint.a = 0;
    }
    if (SpeedMult == 0)
        deathCountDown = -1;
    for (int i = 0; i < SpeedMult; i++)
    {
        IScene::Update(deltaTime);
        // Check if we should create new enemy.
        ticks += deltaTime;
        if (enemyWaveData.empty()) // for some reason, merging two if statements will crash
        {
            if (enemyWaveData.empty() && EnemyGroup->GetObjects().empty())
            {
                Engine::GameEngine::GetInstance().ChangeScene("win");
            }
            continue;
        }
        auto current = enemyWaveData.front();
        if (ticks < current.second)
            continue;
        ticks -= current.second;
        enemyWaveData.pop_front();
        const Engine::Point SpawnCoordinate = Engine::Point(SpawnGridPoint.x * BlockSize + BlockSize / 2, SpawnGridPoint.y * BlockSize + BlockSize / 2);
        Enemy *enemy;
        switch (current.first)
        {
        case 1:
            EnemyGroup->AddNewObject(enemy = new SoldierEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
            break;
        case 2:
            EnemyGroup->AddNewObject(enemy = new FastEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
            break;
        case 3:
            EnemyGroup->AddNewObject(enemy = new TankEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
            break;
        case 4:
            EnemyGroup->AddNewObject(enemy = new BossEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
            break;
        default:
            continue;
        }
        enemy->UpdatePath(mapDistance);
        // Compensate the time lost.
        enemy->Update(ticks);
    }
    if (preview)
    {
        preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
        // To keep responding when paused.
        preview->Update(deltaTime);
    }
}
void PlayScene::Draw() const
{
    IScene::Draw();
    if (DebugMode)
    {
        // Draw reverse BFS distance on all reachable blocks.
        for (int i = 0; i < MapHeight; i++)
        {
            for (int j = 0; j < MapWidth; j++)
            {
                if (mapDistance[i][j] != -1)
                {
                    Engine::Label label(std::to_string(mapDistance[i][j]), "pirulen.ttf", 32, (j + 0.5) * BlockSize, (i + 0.5) * BlockSize);
                    label.Anchor = Engine::Point(0.5, 0.5);
                    label.Draw();
                }
            }
        }
    }

    // Draw boss health bar if actives (fucking primitive but it works)
    if (isBossActive)
    {
        // Draw black background bar
        al_draw_filled_rectangle(healthBarX, healthBarY,
                                 healthBarX + healthBarWidth, healthBarY + healthBarHeight,
                                 al_map_rgb(0, 0, 0));

        // Draw red health bar
        float healthPercentage = currentBossHP / maxBossHP;
        int currentWidth = healthBarWidth * healthPercentage;
        al_draw_filled_rectangle(healthBarX, healthBarY,
                                 healthBarX + currentWidth, healthBarY + healthBarHeight,
                                 al_map_rgb(255, 0, 0));

        // Draw border
        al_draw_rectangle(healthBarX, healthBarY,
                          healthBarX + healthBarWidth, healthBarY + healthBarHeight,
                          al_map_rgb(255, 255, 255), 2);

        // Draw health numbers
        std::string healthText = std::to_string(static_cast<int>(currentBossHP)) + " / " + std::to_string(static_cast<int>(maxBossHP));
        Engine::Label healthNumbers(healthText, "pirulen.ttf", 24, healthBarX + healthBarWidth / 2, healthBarY + 15);
        healthNumbers.Anchor = Engine::Point(0.5, 0.5); // Center the text
        healthNumbers.Draw();
    }
}
void PlayScene::OnMouseDown(int button, int mx, int my)
{
    if ((button & 1) && !imgTarget->Visible && preview)
    {
        // Cancel turret construct.
        UIGroup->RemoveObject(preview->GetObjectIterator());
        preview = nullptr;
    }
    IScene::OnMouseDown(button, mx, my);
}
void PlayScene::OnMouseMove(int mx, int my)
{
    IScene::OnMouseMove(mx, my);
    const int x = mx / BlockSize;
    const int y = my / BlockSize;
    if (!preview || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
    {
        imgTarget->Visible = false;
        return;
    }
    imgTarget->Visible = true;
    imgTarget->Position.x = x * BlockSize;
    imgTarget->Position.y = y * BlockSize;
}
void PlayScene::OnMouseUp(int button, int mx, int my)
{
    IScene::OnMouseUp(button, mx, my);
    if (!imgTarget->Visible)
        return;
    const int x = mx / BlockSize;
    const int y = my / BlockSize;
    if (button & 1)
    {
        if (mapState[y][x] != TILE_OCCUPIED)
        {
            if (!preview)
                return;
            // Check if valid.
            if (!CheckSpaceValid(x, y))
            {
                Engine::Sprite *sprite;
                GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
                sprite->Rotation = 0;
                return;
            }
            // Purchase.
            EarnMoney(-preview->GetPrice());
            // Remove Preview.
            preview->GetObjectIterator()->first = false;
            UIGroup->RemoveObject(preview->GetObjectIterator());
            // Construct real turret.
            preview->Position.x = x * BlockSize + BlockSize / 2;
            preview->Position.y = y * BlockSize + BlockSize / 2;
            preview->Enabled = true;
            preview->Preview = false;
            preview->Tint = al_map_rgba(255, 255, 255, 255);
            TowerGroup->AddNewObject(preview);
            // To keep responding when paused.
            preview->Update(0);
            // Remove Preview.
            preview = nullptr;

            mapState[y][x] = TILE_OCCUPIED;
            OnMouseMove(mx, my);
        }
    }
}
void PlayScene::OnKeyDown(int keyCode)
{
    IScene::OnKeyDown(keyCode);
    if (keyCode == ALLEGRO_KEY_TAB)
    {
        DebugMode = !DebugMode;
    }
    else
    {
        keyStrokes.push_back(keyCode);
        if (keyStrokes.size() > code.size())
            keyStrokes.pop_front();

        if (keyStrokes.size() == code.size())
        {
            bool match = true;
            auto it1 = keyStrokes.begin();
            auto it2 = code.begin();
            while (it1 != keyStrokes.end() && it2 != code.end()) // compare each element using iterator
            {
                if (*it1 != *it2)
                {
                    match = false;
                    break;
                }
                ++it1;
                ++it2;
            }

            if (match)
            {
                Engine::LOG(Engine::INFO) << "Konami code entered! Spawning plane...";
                // Spawn a plane when cheat code is entered
                EffectGroup->AddNewObject(new Plane());
                // Clear the key strokes after successful cheat code
                keyStrokes.clear();
            }
        }
    }
    if (keyCode == ALLEGRO_KEY_Q)
    {
        // Hotkey for MachineGunTurret.
        UIBtnClicked(0);
    }
    else if (keyCode == ALLEGRO_KEY_W)
    {
        // Hotkey for LaserTurret.
        UIBtnClicked(1);
    }
    else if (keyCode == ALLEGRO_KEY_E)
    {
        // Hotkey for TeslaCoilTurret.
        UIBtnClicked(2);
    }

    else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9)
    {
        // Hotkey for Speed up.
        SpeedMult = (keyCode - ALLEGRO_KEY_0) * 2;
    }
    else if (keyCode == ALLEGRO_KEY_MINUS)
        money -= 1000;
    else if (keyCode == ALLEGRO_KEY_EQUALS)
        money += 1000;
    UIMoney->Text = std::string("$") + std::to_string(this->money);
}
void PlayScene::Hit()
{
    lives--;
    // Update the life label
    UILives->Text = std::string("Life ❤ ") + std::to_string(lives);
    UILives->Color = al_map_rgba(255, 0, 0, 255); // Set color to red

    if (lives <= 0)
    {
        Engine::GameEngine::GetInstance().ChangeScene("lose");
    }
}
int PlayScene::GetMoney() const
{
    return money;
}
void PlayScene::EarnMoney(int money)
{
    this->money += money;
    UIMoney->Text = std::string("$") + std::to_string(this->money);
}
void PlayScene::ReadMap()
{
    std::string filename = std::string("Resource/map") + std::to_string(MapId) + ".txt";
    // Read map file.
    char c;
    std::vector<bool> mapData;
    std::ifstream fin(filename);
    while (fin >> c)
    {
        switch (c)
        {
        case '0':
            mapData.push_back(false);
            break;
        case '1':
            mapData.push_back(true);
            break;
        case '\n':
        case '\r':
            if (static_cast<int>(mapData.size()) / MapWidth != 0)
                throw std::ios_base::failure("Map data is corrupted.");
            break;
        default:
            throw std::ios_base::failure("Map data is corrupted.");
        }
    }
    fin.close();
    // Validate map data.
    if (static_cast<int>(mapData.size()) != MapWidth * MapHeight)
        throw std::ios_base::failure("Map data is corrupted.");
    // Store map in 2d array.
    mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
    for (int i = 0; i < MapHeight; i++)
    {
        for (int j = 0; j < MapWidth; j++)
        {
            const int num = mapData[i * MapWidth + j];
            mapState[i][j] = num ? TILE_FLOOR : TILE_DIRT;
            if (num)
                TileMapGroup->AddNewObject(new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            else
                TileMapGroup->AddNewObject(new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
        }
    }
}
void PlayScene::ReadEnemyWave()
{
    std::string filename = std::string("Resource/enemy") + std::to_string(MapId) + ".txt";
    // Read enemy file.
    float type, wait, repeat;
    enemyWaveData.clear();
    std::ifstream fin(filename);
    while (fin >> type && fin >> wait && fin >> repeat)
    {
        for (int i = 0; i < repeat; i++)
            enemyWaveData.emplace_back(type, wait);
    }
    fin.close();
}
void PlayScene::ConstructUI()
{
    // Background
    UIGroup->AddNewObject(new Engine::Image("play/sand.png", 1280, 0, 320, 832));
    // Text
    UIGroup->AddNewObject(new Engine::Label(std::string("Stage ") + std::to_string(MapId), "pirulen.ttf", 32, 1294, 10));
    UIGroup->AddNewObject(UIMoney = new Engine::Label(std::string("$") + std::to_string(money), "pirulen.ttf", 24, 1294, 58, 0, 255, 0));
    UIGroup->AddNewObject(UILives = new Engine::Label(std::string("Life: ") + std::to_string(lives), "pirulen.ttf", 24, 1294, 88, 255, 0, 0, 255));
    UIGroup->AddNewObject(new Engine::Label("Press + and -", "pirulen.ttf", 21, 1294, 370, 20, 20, 255, 255));
    UIGroup->AddNewObject(new Engine::Label("to cheat money", "pirulen.ttf", 21, 1294, 400, 20, 20, 255, 255));

    TurretButton *btn;
    // Button 1 - Machine Gun Turret
    btn = new TurretButton("play/floor.png", "play/dirt.png",
                           Engine::Sprite("play/tower-base.png", 1294, 136, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-1.png", 1294, 136 - 8, 0, 0, 0, 0), 1294, 136, MachineGunTurret::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 0));
    UIGroup->AddNewControlObject(btn);

    // Button 2 - Laser Turret
    btn = new TurretButton("play/floor.png", "play/dirt.png",
                           Engine::Sprite("play/tower-base.png", 1370, 136, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-2.png", 1370, 136 - 8, 0, 0, 0, 0), 1370, 136, LaserTurret::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 1));
    UIGroup->AddNewControlObject(btn);

    // Button 3 - Tesla Coil Turret
    btn = new TurretButton("play/floor.png", "play/dirt.png",
                           Engine::Sprite("play/tower-base.png", 1294, 212, 0, 0, 0, 0),
                           Engine::Sprite("play/tesla-turret.png", 1294, 212 - 8, 0, 0, 0, 0), 1294, 212, TeslaCoilTurret::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 2));
    UIGroup->AddNewControlObject(btn);

    // Add Shovel Button
    ShovelButton *shovel;
    shovel = new ShovelButton(1370, 212);
    UIGroup->AddNewControlObject(shovel);

    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    dangerIndicator = new Engine::Sprite("play/tdn.png", w / 2, h / 2);
    dangerIndicator->Tint.a = 0;
    UIGroup->AddNewObject(dangerIndicator);
}

void PlayScene::UIBtnClicked(int id)
{
    Turret *next_preview = nullptr;
    if (id == 0 && money >= MachineGunTurret::Price)
        next_preview = new MachineGunTurret(0, 0);
    else if (id == 1 && money >= LaserTurret::Price)
        next_preview = new LaserTurret(0, 0);
    else if (id == 2 && money >= TeslaCoilTurret::Price)
        next_preview = new TeslaCoilTurret(0, 0);
    if (!next_preview)
        return;

    if (preview)
        UIGroup->RemoveObject(preview->GetObjectIterator());
    preview = next_preview;

    preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
    preview->Tint = al_map_rgba(255, 255, 255, 200);
    preview->Enabled = false;
    preview->Preview = true;
    UIGroup->AddNewObject(preview);
    OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
}

bool PlayScene::CheckSpaceValid(int x, int y)
{
    if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
        return false;
    auto map00 = mapState[y][x];
    mapState[y][x] = TILE_OCCUPIED;
    std::vector<std::vector<int>> map = CalculateBFSDistance();
    mapState[y][x] = map00;
    if (map[0][0] == -1)
        return false;
    for (auto &it : EnemyGroup->GetObjects())
    {
        Engine::Point pnt;
        pnt.x = floor(it->Position.x / BlockSize);
        pnt.y = floor(it->Position.y / BlockSize);
        if (pnt.x < 0)
            pnt.x = 0;
        if (pnt.x >= MapWidth)
            pnt.x = MapWidth - 1;
        if (pnt.y < 0)
            pnt.y = 0;
        if (pnt.y >= MapHeight)
            pnt.y = MapHeight - 1;
        // Skip path check for BossEnemy
        if (dynamic_cast<BossEnemy *>(it) == nullptr && map[pnt.y][pnt.x] == -1)
            return false;
    }
    // All enemy have path to exit.
    mapState[y][x] = TILE_OCCUPIED;
    mapDistance = map;
    for (auto &it : EnemyGroup->GetObjects())
        dynamic_cast<Enemy *>(it)->UpdatePath(mapDistance);
    return true;
}
std::vector<std::vector<int>> PlayScene::CalculateBFSDistance() // returns 2d map of distance from exit
{
    // 2D array filled with -1 (unvisted)
    std::vector<std::vector<int>> map(MapHeight, std::vector<int>(std::vector<int>(MapWidth, -1)));
    std::queue<Engine::Point> que; // queue for BFS

    if (mapState[MapHeight - 1][MapWidth - 1] != TILE_DIRT) // check if exit is valid
        return map;

    que.push(Engine::Point(MapWidth - 1, MapHeight - 1));
    map[MapHeight - 1][MapWidth - 1] = 0;

    while (!que.empty())
    {
        Engine::Point p = que.front();
        que.pop();

        // BFS starting from the most right-bottom block in the map.
        // Assign the corresponding distance to the most right-bottom block

        for (const auto &dir : directions) // check all four direction
        {
            Engine::Point next = p + dir;

            // Check if in bound
            if (next.x < 0 || next.x >= MapWidth || next.y < 0 || next.y >= MapHeight)
                continue;

            // Check if the cell is TILE_DIRT and not yet visited
            if (mapState[next.y][next.x] == TILE_DIRT && map[next.y][next.x] == -1)
            {
                // Distance = parent's distance + 1
                map[next.y][next.x] = map[p.y][p.x] + 1;
                que.push(next); // add to queue
            }
        }
    }
    return map;
}
