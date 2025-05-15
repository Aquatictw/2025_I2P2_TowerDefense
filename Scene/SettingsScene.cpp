#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Scene/SettingsScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void SettingsScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    // Add title
    AddNewObject(new Engine::Label("Settings", "pirulen.ttf", 120, halfW, halfH / 3 + 50, 10, 255, 255, 255, 0.5, 0.5));

    // Add BGM Volume Slider
    AddNewObject(new Engine::Label("BGM Volume", "pirulen.ttf", 36, halfW - 200, halfH - 55, 255, 255, 255, 255, 0.5, 0.5));
    Slider *bgmSlider = new Slider(halfW, halfH - 70, 400, 40);
    bgmSlider->SetValue(AudioHelper::BGMVolume);
    bgmSlider->SetOnValueChangedCallback(std::bind(&SettingsScene::BGMSlideOnValueChanged, this, std::placeholders::_1));
    AddNewControlObject(bgmSlider);

    // Add SFX Volume Slider
    AddNewObject(new Engine::Label("SFX Volume", "pirulen.ttf", 36, halfW - 200, halfH + 55, 255, 255, 255, 255, 0.5, 0.5));
    Slider *sfxSlider = new Slider(halfW, halfH + 40, 400, 40);
    sfxSlider->SetValue(AudioHelper::SFXVolume);
    sfxSlider->SetOnValueChangedCallback(std::bind(&SettingsScene::SFXSlideOnValueChanged, this, std::placeholders::_1));
    AddNewControlObject(sfxSlider);

    // Add Back Button
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&SettingsScene::BackOnClick, this, 0));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));

    // Initialize BGM
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}

void SettingsScene::Terminate()
{
    AudioHelper::StopSample(bgmInstance);
    IScene::Terminate();
}

void SettingsScene::BackOnClick(int stage)
{
    Engine::GameEngine::GetInstance().ChangeScene("start");
}

void SettingsScene::BGMSlideOnValueChanged(float value)
{
    AudioHelper::ChangeSampleVolume(bgmInstance, value);
    AudioHelper::BGMVolume = value;
}

void SettingsScene::SFXSlideOnValueChanged(float value)
{
    AudioHelper::SFXVolume = value;
}
