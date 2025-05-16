#include <functional>
#include <string>
#include <fstream>
#include <map>
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Point.hpp"
#include "PlayScene.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "WinScene.hpp"

void WinScene::Initialize()
{
    WriteScore();
    ticks = 0;

    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    AddNewObject(new Engine::Image("win/tnok.png", halfW + 100, halfH + 20, 0, 0, 0.7, 0.7));
    AddNewObject(new Engine::Image("win/ktc.jpg", halfW + 700, halfH + 20, 0, 0, 0.7, 0.7));
    AddNewObject(new Engine::Image("win/ktc2.jpg", halfW - 580, halfH + 20, 0, 0, 0.7, 0.7));

    AddNewObject(new Engine::Label("You Win!", "pirulen.ttf", 48, halfW + 10, halfH / 4 - 10, 255, 255, 255, 255, 0.5, 0.5));

    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 350, halfH * 7 / 4 - 50, 300, 100);
    btn->SetOnClickCallback(std::bind(&WinScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW - 200, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));

    Engine::ImageButton *btn2;
    btn2 = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW + 70, halfH * 7 / 4 - 50, 400, 100);
    btn2->SetOnClickCallback(std::bind(&WinScene::ScoreboardOnClick, this, 2));
    AddNewControlObject(btn2);
    AddNewObject(new Engine::Label("Scoreboard", "pirulen.ttf", 38, halfW + 270, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));

    bgmId = AudioHelper::PlayAudio("win.wav");
}
void WinScene::Terminate()
{
    IScene::Terminate();
    AudioHelper::StopBGM(bgmId);
}
void WinScene::Update(float deltaTime)
{
    ticks += deltaTime;
    if (ticks > 4 && ticks < 100 &&
        dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"))->MapId == 2)
    {
        ticks = 100;
        bgmId = AudioHelper::PlayBGM("happy.ogg");
    }
}

void WinScene::WriteScore()
{
    PlayScene *playScene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));

    std::map<std::string, int> scores; // user/scores value pair
    std::ifstream fin("scores.txt");
    std::string username;
    int score;
    while (fin >> username >> score)
    {
        scores[username] = score;
    }
    fin.close();

    // Check if new score is higher or username doesn't exist
    bool shouldWrite = true;
    if (scores.find(playScene->username) != scores.end())
    {
        if (playScene->money <= scores[playScene->username])
            shouldWrite = false;
    }

    // Write score if conditions are met
    if (shouldWrite)
    {
        scores[playScene->username] = playScene->money; // update player score

        std::ofstream fout("scores.txt", std::ios::trunc); // write to file
        for (const auto &pair : scores)
        {
            fout << pair.first << " " << pair.second << "\n";
        }
        fout.close();
        Engine::LOG(Engine::INFO) << "Wrote score: " << playScene->username << " " << playScene->money;
    }
    else
    {
        Engine::LOG(Engine::INFO) << "Score not written - existing score is higher for " << playScene->username;
    }
}

void WinScene::BackOnClick(int stage)
{
    // Change to select scene.
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}
void WinScene::ScoreboardOnClick(int stage)
{
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard");
}
