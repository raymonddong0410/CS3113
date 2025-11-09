#include "StartScreen.h"

StartScreen::StartScreen()                                      : Scene { {0.0f}, nullptr   } {}
StartScreen::StartScreen(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

StartScreen::~StartScreen() { shutdown(); }

void StartScreen::initialise()
{
    mGameState.nextSceneID = 0;

    mGameState.bgm = LoadMusicStream("assets/Majestic Hills.mp3");
   SetMusicVolume(mGameState.bgm, 0.40f);
   PlayMusicStream(mGameState.bgm);
}

void StartScreen::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);
}

void StartScreen::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
}

void StartScreen::shutdown()
{
   UnloadMusicStream(mGameState.bgm);
}