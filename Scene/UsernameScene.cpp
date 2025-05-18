#include <allegro5/allegro.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Engine/LOG.hpp"
#include "PlayScene.hpp"
#include "UsernameScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"

struct ScoreEntry
{
    std::string time;
    std::string username;
    int score;
};

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

    // leaderboard button
    Engine::ImageButton *btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 300, (halfH * 2 / 3) + 100, 600, 100);
    btn->SetOnClickCallback(std::bind(&UsernameScene::LeaderboardOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Enter", "pirulen.ttf", 48, halfW, halfH * 2 / 3 + 150, 0, 0, 0, 255, 0.5, 0.5));
}

void UsernameScene::Terminate()
{
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

void UsernameScene::LeaderboardOnClick()
{
    if (username.empty())
        return; // Don't start if no username entered

    WriteScore(username);

    Engine::GameEngine::GetInstance().ChangeScene("scoreboard");
}

void UsernameScene::WriteScore(std::string username)
{
    std::string scoreFile = Engine::GameEngine::BOOT_TIME + "_score.txt";
    std::ifstream checkFile(scoreFile); // check if files exist, create if not
    if (!checkFile.good())
    {
        std::ofstream createFile(scoreFile);
        createFile.close();
        Engine::LOG(Engine::INFO) << "Created new " << scoreFile << " file";
    }
    else
        checkFile.close();

    std::ifstream fin(scoreFile);
    PlayScene *playScene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));

    std::vector<ScoreEntry> scores;
    std::string time, usr;
    int score;

    while (fin >> time >> usr >> score)
        scores.push_back({time, usr, score});
    fin.close();

    auto it = std::find_if(scores.begin(), scores.end(), [&username](const ScoreEntry &entry)
                           { return entry.username == username; }); // check if same username exist in file

    bool shouldWrite = true;
    if (it != scores.end())
    {
        if (playScene->money <= it->score)
            shouldWrite = false;
        else
            scores.erase(it); // Remove old score to update it
    }

    // Write score if conditions are met
    if (shouldWrite)
    {
        ScoreEntry newEntry = {Engine::GameEngine::Get_Time(), username, playScene->money + playScene->lives * 10};
        scores.push_back(newEntry);

        std::ofstream fout(scoreFile, std::ios::trunc); // write to file
        for (const auto &entry : scores)
            fout << entry.time << " " << entry.username << " " << entry.score << "\n";
        fout.close();
        Engine::LOG(Engine::INFO) << "Wrote score: " << username << " " << playScene->money + playScene->lives * 10;
    }
    else
        Engine::LOG(Engine::INFO) << "Score not written - existing score is higher for " << username;
}
