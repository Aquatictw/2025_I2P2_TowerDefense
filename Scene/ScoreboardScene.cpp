#include <functional>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "ScoreboardScene.hpp"

void ScoreboardScene::LoadScores()
{
    scores.clear();
    std::ifstream fin("scores.txt");
    std::string username;
    int money;

    while (fin >> username >> money)
    {
        GameScore score;
        score.username = username;
        score.money = money;
        scores.push_back(score);
    }
    fin.close();

    // Sort scores by money in descending order
    std::sort(scores.begin(), scores.end(), [](const GameScore &a, const GameScore &b)
              { return a.money > b.money; });
}

void ScoreboardScene::Initialize()
{
    LoadScores();

    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    AddNewObject(new Engine::Label("Scoreboard", "pirulen.ttf", 48, halfW, 50, 255, 204, 0, 255, 0.5, 0.5));

    // column headers
    AddNewObject(new Engine::Label("Player", "pirulen.ttf", 32, halfW - 100, 120, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Score", "pirulen.ttf", 32, halfW + 100, 120, 255, 255, 255, 255, 0.5, 0.5));

    // Display scores
    int yPos = 170;
    for (const auto &score : scores)
    {
        AddNewObject(new Engine::Label(score.username, "pirulen.ttf", 24, halfW - 100, yPos, 255, 255, 255, 255, 0.5, 0.5));
        AddNewObject(new Engine::Label(std::to_string(score.money), "pirulen.ttf", 24, halfW + 100, yPos, 255, 255, 255, 255, 0.5, 0.5));
        yPos += 40;
    }

    // Back button at the bottom
    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH * 7 / 4 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));
}

void ScoreboardScene::Terminate()
{
    IScene::Terminate();
}

void ScoreboardScene::BackOnClick(int stage)
{
    Engine::GameEngine::GetInstance().ChangeScene("win");
}
