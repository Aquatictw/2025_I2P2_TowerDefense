#ifndef USERNAMESCENE_HPP
#define USERNAMESCENE_HPP

#include <allegro5/allegro_audio.h>
#include <memory>
#include <string>

#include "Engine/IScene.hpp"

class UsernameScene final : public Engine::IScene
{
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    std::string username;
    int selectedStage;
    Engine::Label *usernameLabel;

public:
    explicit UsernameScene() = default;
    void Initialize() override;
    void Terminate() override;
    void OnKeyDown(int keyCode) override;
    void StartGame();
    void SetStage(int stage) { selectedStage = stage; }
};

#endif // USERNAMESCENE_HPP
