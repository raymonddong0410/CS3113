#include "EndScreen.h"

EndScreen::EndScreen()                                      : Scene { {0.0f}, nullptr   } {}
EndScreen::EndScreen(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

EndScreen::~EndScreen() { shutdown(); }

void EndScreen::initialise()
{
    mGameState.nextSceneID = 0;

    mGameState.bgm = LoadMusicStream("assets/Majestic Hills.mp3");
    SetMusicVolume(mGameState.bgm, 0.40f);
    PlayMusicStream(mGameState.bgm); 
}

void EndScreen::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);
}

void EndScreen::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
}

void EndScreen::shutdown()
{
   UnloadMusicStream(mGameState.bgm);
}