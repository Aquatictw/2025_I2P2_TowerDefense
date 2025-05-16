#ifndef SCOREBOARDSCENE_HPP
#define SCOREBOARDSCENE_HPP
#include "Engine/IScene.hpp"
#include <vector>

struct GameScore
{
    std::string username;
    int money;
};

class ScoreboardScene final : public Engine::IScene
{
private:
    int bgmId;
    std::vector<GameScore> scores;
    void LoadScores();

public:
    explicit ScoreboardScene() = default;
    void Initialize() override;
    void Terminate() override;
    void BackOnClick(int stage);
};
#endif // SCOREBOARDSCENE_HPP
