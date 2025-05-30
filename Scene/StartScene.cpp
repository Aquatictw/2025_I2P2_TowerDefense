#include "StartScene.h"
#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>
#include <fstream>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Scene/StartScene.h"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void StartScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    Engine::ImageButton *btn;

    AddNewObject(new Engine::Image("black-curtain.png", halfW + 200, halfH - 150, 0, 0, 0.7, 0.7));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 + 200, 400, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Play", "pirulen.ttf", 48, halfW, halfH / 2 + 250, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::SettingsOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Settings", "pirulen.ttf", 48, halfW, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));

    AddNewObject(new Engine::Label("made by Aquatic 2025/5", "Roboto-Black.ttf", 25, halfW + 633, halfH + 345, 200, 200, 200, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Based On I2P2_TowerDefense", "Roboto-Black.ttf", 25, halfW + 600, halfH + 380, 200, 200, 200, 255, 0.5, 0.5));

    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}
void StartScene::Terminate()
{
    IScene::Terminate();
}
void StartScene::PlayOnClick(int stage)
{
    AudioHelper::StopSample(bgmInstance);
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}
void StartScene::SettingsOnClick(int stage)
{
    Engine::GameEngine::GetInstance().ChangeScene("settings");
}
