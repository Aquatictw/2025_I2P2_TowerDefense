#include "ShovelButton.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "Turret/Turret.hpp"

bool ShovelButton::isInBuildMode = false;

ShovelButton::ShovelButton(float x, float y) : ImageButton("play/floor.png", "play/dirt.png", x, y), base("play/diamond-shovel.png", defaultX, defaultY, 70, 70, 0, 0)
{
    isSelected = false;
}

void ShovelButton::Update(float deltaTime)
{
    ImageButton::Update(deltaTime);
    Engine::Point mouse = Engine::GameEngine::GetInstance().GetMousePosition();

    if (!isSelected)
        base.Position = Engine::Point(defaultX, defaultY);
    else
    {
        mouse.x -= 20;
        mouse.y -= 20;
        base.Position = mouse;
    }
}

void ShovelButton::Draw() const
{
    ImageButton::Draw();
    base.Draw();
}

void ShovelButton::OnMouseUp(int button, int mx, int my)
{
    if (!isSelected)
        return;

    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());

    // Convert mouse position to grid position
    int x = (mx + 20) / PlayScene::BlockSize;
    int y = (my + 20) / PlayScene::BlockSize;

    // Find and remove turret at this position
    for (auto &obj : scene->TowerGroup->GetObjects())
    {
        Turret *turret = dynamic_cast<Turret *>(obj);
        if (turret)
        {
            int tx = turret->Position.x / PlayScene::BlockSize;
            int ty = turret->Position.y / PlayScene::BlockSize;
            if (tx == x && ty == y)
            {
                scene->TowerGroup->RemoveObject(turret->GetObjectIterator());
                scene->EarnMoney(turret->GetPrice());
                scene->mapState[y][x] = PlayScene::TileType::TILE_DIRT;
                break;
            }
        }
    }

    isSelected = false;
}

void ShovelButton::OnMouseDown(int button, int mx, int my)
{
    if (std::abs(mx - defaultX - 60) < 60 && std::abs(my - defaultY - 60) < 60)
        isSelected = true;
}
