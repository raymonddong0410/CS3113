#include "RestartScreen.h"

RestartScreen::RestartScreen()                                      : Scene { {0.0f}, nullptr   } {}
RestartScreen::RestartScreen(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

RestartScreen::~RestartScreen() { shutdown(); }

void RestartScreen::initialise()
{
    mGameState.nextSceneID = 0;

    mGameState.bgm = LoadMusicStream("assets/Majestic Hills.mp3");
    SetMusicVolume(mGameState.bgm, 0.40f);
    PlayMusicStream(mGameState.bgm);
}

void RestartScreen::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);
}

void RestartScreen::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
}

void RestartScreen::shutdown()
{
   UnloadMusicStream(mGameState.bgm);
}