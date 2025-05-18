#ifndef SCOREBOARDSCENE_HPP
#define SCOREBOARDSCENE_HPP
#include "Engine/IScene.hpp"
#include <vector>

struct GameScore
{
    std::string time;
    std::string username;
    int money;
};

class ScoreboardScene final : public Engine::IScene
{
private:
    std::string scoreFile = Engine::GameEngine::BOOT_TIME + "_score.txt";
    int bgmId;
    std::vector<GameScore> scores;
    int currentPage;
    static const int ENTRIES_PER_PAGE = 8;
    void LoadScores();
    void UpdateScoreLabels();

public:
    explicit ScoreboardScene() : currentPage(0) {}
    void Initialize() override;
    void Terminate() override;
    void Draw() const override;
    void BackOnClick(int stage);
    void NextPageOnClick(int stage);
    void PrevPageOnClick(int stage);
    void DebugAddEntriesOnClick(int stage);
};
#endif // SCOREBOARDSCENE_HPP
