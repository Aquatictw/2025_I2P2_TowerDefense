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
    std::ifstream fin(scoreFile);
    std::string time, username;
    int money;

    while (fin >> time >> username >> money)
    {
        GameScore score;
        score.time = time;
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

    // Debug button
    Engine::ImageButton *debugBtn = new Engine::ImageButton("win/dirt.png", "win/floor.png", w - 120, 20, 100, 40);
    debugBtn->SetOnClickCallback(std::bind(&ScoreboardScene::DebugAddEntriesOnClick, this, 2));
    AddNewControlObject(debugBtn);
    AddNewObject(new Engine::Label("DEBUG", "pirulen.ttf", 20, w - 70, 40, 255, 0, 0, 255, 0.5, 0.5));

    // column headers
    AddNewObject(new Engine::Label("Time", "pirulen.ttf", 32, halfW - 250, 120, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Player", "pirulen.ttf", 32, halfW, 120, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Score", "pirulen.ttf", 32, halfW + 250, 120, 255, 255, 255, 255, 0.5, 0.5));

    // Back button
    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH * 7 / 4 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));

    Engine::ImageButton *prev_btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 500, halfH * 7 / 4 - 50, 200, 100);
    prev_btn->SetOnClickCallback(std::bind(&ScoreboardScene::PrevPageOnClick, this, 2));
    AddNewControlObject(prev_btn);
    AddNewObject(new Engine::Label("Prev", "pirulen.ttf", 32, halfW - 400, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));

    Engine::ImageButton *next_btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW + 300, halfH * 7 / 4 - 50, 200, 100);
    next_btn->SetOnClickCallback(std::bind(&ScoreboardScene::NextPageOnClick, this, 2));
    AddNewControlObject(next_btn);
    AddNewObject(new Engine::Label("Next", "pirulen.ttf", 32, halfW + 400, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));

    UpdateScoreLabels();
}

void ScoreboardScene::UpdateScoreLabels()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    // Remove existing labels
    for (auto it = objects.begin(); it != objects.end();)
    {
        if ((it->second->Position.y >= 170 && it->second->Position.y <= 170 + ENTRIES_PER_PAGE * 40) || it->second->Position.y == halfH * 7 / 4 + 80)
        {
            delete it->second;
            it = objects.erase(it);
        }
        else
            ++it;
    }

    // Display scores for current page
    int startIdx = currentPage * ENTRIES_PER_PAGE;
    int yPos = 170;
    for (int i = 0; i < ENTRIES_PER_PAGE && startIdx + i < scores.size(); i++)
    {
        const auto &score = scores[startIdx + i];
        AddNewObject(new Engine::Label(score.time, "pirulen.ttf", 24, halfW - 250, yPos, 255, 255, 255, 255, 0.5, 0.5));
        AddNewObject(new Engine::Label(score.username, "pirulen.ttf", 24, halfW, yPos, 255, 255, 255, 255, 0.5, 0.5));
        AddNewObject(new Engine::Label(std::to_string(score.money), "pirulen.ttf", 24, halfW + 250, yPos, 255, 255, 255, 255, 0.5, 0.5));
        yPos += 40;
    }

    // Update page number display
    int totalPages = (scores.size() + ENTRIES_PER_PAGE - 1) / ENTRIES_PER_PAGE;
    if (totalPages > 0)
    {
        std::string pageText = "Page " + std::to_string(currentPage + 1) + " of " + std::to_string(totalPages);
        AddNewObject(new Engine::Label(pageText, "pirulen.ttf", 24, halfW, halfH * 7 / 4 + 80, 255, 255, 255, 255, 0.5, 0.5));
    }
}

void ScoreboardScene::Terminate()
{
    IScene::Terminate();
}
void ScoreboardScene::Draw() const
{
    IScene::Draw();
}

void ScoreboardScene::BackOnClick(int stage)
{
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}

void ScoreboardScene::NextPageOnClick(int stage)
{
    if ((currentPage + 1) * ENTRIES_PER_PAGE < scores.size())
    {
        currentPage++;
        UpdateScoreLabels();
    }
}

void ScoreboardScene::PrevPageOnClick(int stage)
{
    if (currentPage > 0)
    {
        currentPage--;
        UpdateScoreLabels();
    }
}

void ScoreboardScene::DebugAddEntriesOnClick(int stage)
{
    for (int i = 0; i < 20; i++) // add test entries
    {
        GameScore score;
        score.time = Engine::GameEngine::BOOT_TIME;
        score.username = "Test_" + std::to_string(i + 1);
        score.money = i * 1000;
        scores.push_back(score);
    }

    std::sort(scores.begin(), scores.end(), [](const GameScore &a, const GameScore &b)
              { return a.money > b.money; });

    // Save to file
    std::ofstream fout(scoreFile);
    for (const auto &score : scores)
    {
        fout << score.time << " " << score.username << " " << score.money << "\n";
    }
    fout.close();

    UpdateScoreLabels();
}
