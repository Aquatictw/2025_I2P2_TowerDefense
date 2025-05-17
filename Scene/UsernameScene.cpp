#include <allegro5/allegro.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "UsernameScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"

void UsernameScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    // Add title
    AddNewObject(new Engine::Label("Enter Your Username", "pirulen.ttf", 48, halfW, halfH / 3, 255, 255, 255, 255, 0.5, 0.5));

    // Add username display
    usernameLabel = new Engine::Label(username, "pirulen.ttf", 36, halfW, halfH / 2 + 50, 255, 255, 255, 255, 0.5, 0.5);
    AddNewObject(usernameLabel);

    // Add Start Game button
    Engine::ImageButton *btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 300, (halfH * 2 / 3) + 100, 600, 100);
    btn->SetOnClickCallback(std::bind(&UsernameScene::StartGame, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Start Game", "pirulen.ttf", 48, halfW, halfH * 2 / 3 + 150, 0, 0, 0, 255, 0.5, 0.5));

    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}

void UsernameScene::Terminate()
{
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

void UsernameScene::OnKeyDown(int keyCode)
{
    if (keyCode == ALLEGRO_KEY_BACKSPACE && !username.empty())
    {
        username.pop_back();
    }
    else if (keyCode >= ALLEGRO_KEY_A && keyCode <= ALLEGRO_KEY_Z)
    {
        char c = 'a' + (keyCode - ALLEGRO_KEY_A); // to lowercase
        if (username.length() < 10)               // limit username length
            username += c;
    }

    usernameLabel->Text = username;
}

void UsernameScene::StartGame()
{
    if (username.empty())
        return; // Don't start if no username entered

    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
    scene->MapId = selectedStage;
    scene->username = username;
    Engine::GameEngine::GetInstance().ChangeScene("play");
}
