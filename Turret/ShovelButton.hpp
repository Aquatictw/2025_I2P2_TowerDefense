#ifndef SHOVELBUTTON_HPP
#define SHOVELBUTTON_HPP
#include "Engine/Sprite.hpp"
#include "Engine/Point.hpp"
#include "UI/Component/ImageButton.hpp"

class PlayScene;
class ShovelButton : public Engine::ImageButton
{
    friend class PlayScene;

protected:
    bool isSelected;
    int const defaultX = 1366;
    int const defaultY = 208;

public:
    static bool isInBuildMode;
    ShovelButton(float x, float y);
    Engine::Sprite base;
    void Update(float deltaTime) override;
    void OnMouseDown(int button, int mx, int my) override;
    void OnMouseUp(int button, int mx, int my) override;
    void Draw() const override;
};
#endif // SHOVELBUTTON_HPP
