// [main.cpp]
// This is the entry point of your game.
// You can register your scenes here, and start the game.
#include <chrono>
#include <iomanip>
#include <sstream>
#include "Engine/GameEngine.hpp"
#include "Engine/LOG.hpp"
#include "Scene/LoseScene.hpp"
#include "Scene/PlayScene.hpp"
#include "Scene/StageSelectScene.hpp"
#include "Scene/WinScene.hpp"
#include "Scene/StartScene.h"
#include "Scene/SettingsScene.hpp"
#include "Scene/ScoreboardScene.hpp"
#include "Scene/UsernameScene.hpp"

int main(int argc, char **argv)
{
	Engine::LOG::SetConfig(true);
	Engine::GameEngine &game = Engine::GameEngine::GetInstance();
	Engine::GameEngine::Initialize_Time();

	game.AddNewScene("start", new StartScene()); // add start scene
	game.AddNewScene("stage-select", new StageSelectScene());
	game.AddNewScene("username", new UsernameScene());
	game.AddNewScene("play", new PlayScene());
	game.AddNewScene("lose", new LoseScene());
	game.AddNewScene("win", new WinScene());
	game.AddNewScene("settings", new SettingsScene()); // add settings scene
	game.AddNewScene("scoreboard", new ScoreboardScene());

	game.Start("start", 60, 1600, 832);
	return 0;
}
