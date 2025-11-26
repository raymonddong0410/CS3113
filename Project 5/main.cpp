/**
* Author: Raymond Dong
* Assignment: Warrior's Steps
* Date due: 12/5/25, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "CS3113/ShaderProgram.h"

// Global Constants
constexpr int SCREEN_WIDTH     = 1000,
              SCREEN_HEIGHT    = 600,
              FPS              = 120,
              NUMBER_OF_LEVELS = 2;

constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
            
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

constexpr char BACKGROUND_FP[] = "assets/background.jpg";

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

Scene *gCurrentScene = nullptr;
std::vector<Scene*> gLevels = {};

// set lives to be read by levels
int Scene::lives = 3;

StartScreen *gStartScreen = nullptr;
LevelA *gLevelA = nullptr;
LevelB *gLevelB = nullptr;
LevelC *gLevelC = nullptr;
EndScreen *gEndScreen = nullptr;
RestartScreen *gRestartScreen = nullptr;

// for shaders
ShaderProgram gShader;
Vector2 gLightPosition = { 0.0f, 0.0f };

Texture2D gBackgroundTexture;

// Function Declarations
void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(Scene *scene)
{   
    gCurrentScene = scene;
    gCurrentScene->initialise();
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Project 5");
    InitAudioDevice();

    gBackgroundTexture = LoadTexture(BACKGROUND_FP);

    gShader.load("shaders/vertex.glsl", "shaders/fragment.glsl");

    gStartScreen = new StartScreen(ORIGIN, "#000000");
    gLevelA = new LevelA(ORIGIN, "#011627");
    gLevelB = new LevelB(ORIGIN, "#011627");
    gLevelC = new LevelC(ORIGIN, "#011627");
    gEndScreen = new EndScreen(ORIGIN, "#000000");
    gRestartScreen = new RestartScreen(ORIGIN, "#000000");

    gLevels.push_back(gStartScreen);
    gLevels.push_back(gLevelA);
    gLevels.push_back(gLevelB);
    gLevels.push_back(gLevelC);
    gLevels.push_back(gEndScreen);
    gLevels.push_back(gRestartScreen);

    switchToScene(gLevels[0]);

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (gCurrentScene->getState().mc != nullptr) {
        gCurrentScene->getState().mc->resetMovement();

        if      (IsKeyDown(KEY_A)) gCurrentScene->getState().mc->moveLeft();
        else if (IsKeyDown(KEY_D)) gCurrentScene->getState().mc->moveRight();
        else if (IsKeyDown(KEY_W)) gCurrentScene->getState().mc->moveUp();
        else if (IsKeyDown(KEY_S)) gCurrentScene->getState().mc->moveDown();

        if (IsKeyPressed(KEY_W) && 
            gCurrentScene->getState().mc->isCollidingBottom())
        {
            gCurrentScene->getState().mc->jump();
            PlaySound(gCurrentScene->getState().jumpSound);
        }

        if (IsKeyDown(KEY_SPACE))
        {
            gCurrentScene->getState().mc->attack();
        }

        if (GetLength(gCurrentScene->getState().mc->getMovement()) > 1.0f) 
            gCurrentScene->getState().mc->normaliseMovement();
    }

    if (gCurrentScene == gStartScreen) {
        // reset lives counter to 3
        Scene::lives = 3;
        if (IsKeyPressed(KEY_ENTER)) switchToScene(gLevels[1]);
    }
    if (gCurrentScene == gRestartScreen) {
        if (IsKeyPressed(KEY_ENTER)) switchToScene(gLevels[0]);
    }
    else if (gCurrentScene == gEndScreen) {
        if (IsKeyPressed(KEY_R)) switchToScene(gLevels[0]);
    }

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;

        if (gCurrentScene == gLevelC && gCurrentScene->getState().mc != nullptr) {
            gLightPosition = gCurrentScene->getState().mc->getPosition();
        }
    }
}

void render()
{
    BeginDrawing();
    BeginMode2D(gCurrentScene->getState().camera);

    if (gCurrentScene == gLevelC) {
        gShader.begin();
        gShader.setVector2("lightPosition", gLightPosition);
    }

    gCurrentScene->render();

    if (gCurrentScene == gLevelC) {
        gShader.end();
    }

    EndMode2D();

    // To render text on non playable screens
    if (gCurrentScene == gStartScreen) {

        // pulled from renderObject function defined in earlier weeks
        DrawTexturePro(
            gBackgroundTexture,
            {0.0f, 0.0f, (float)gBackgroundTexture.width, (float)gBackgroundTexture.height}, // texture
            {0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT}, // dest rect
            {0.0f, 0.0f}, // offset
            0.0f, WHITE
        );

        DrawText("Warrior's Steps", SCREEN_WIDTH / 2 - 110, SCREEN_HEIGHT / 2 - 75.0f, 30, WHITE);
        DrawText("WASD to Move, Space to Attack", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 10.0f, 20, WHITE);
        DrawText("Kill all Enemies to unlock the Trophy!", SCREEN_WIDTH / 2 - 175, SCREEN_HEIGHT / 2 + 50.0f, 20, WHITE);
        DrawText("Press Enter to Start", SCREEN_WIDTH / 2 - 110, SCREEN_HEIGHT / 2 + 110.0f, 20, WHITE);
    }
    if (gCurrentScene == gEndScreen) {
        DrawTexturePro(
            gBackgroundTexture,
            {0.0f, 0.0f, (float)gBackgroundTexture.width, (float)gBackgroundTexture.height}, // texture
            {0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT}, // dest rect
            {0.0f, 0.0f}, // offset
            0.0f, WHITE
        );
        DrawText("Congrats, You Win!", SCREEN_WIDTH / 2 - 125, SCREEN_HEIGHT / 2 - 100.0f, 30, WHITE);
        DrawText("Press R to Reset", SCREEN_WIDTH / 2 - 75, SCREEN_HEIGHT / 2, 20, WHITE);
    }
    if (gCurrentScene == gRestartScreen) {
        DrawTexturePro(
            gBackgroundTexture,
            {0.0f, 0.0f, (float)gBackgroundTexture.width, (float)gBackgroundTexture.height}, // texture
            {0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT}, // dest rect
            {0.0f, 0.0f}, // offset
            0.0f, WHITE
        );
        DrawText("You Lose", SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 - 100.0f, 30, WHITE);
        DrawText("Press Enter to Restart", SCREEN_WIDTH / 2 - 110, SCREEN_HEIGHT / 2, 20, WHITE);    }

    // draw lives on level a, b, and c
    if (gCurrentScene == gLevelA || gCurrentScene == gLevelB || gCurrentScene == gLevelC) {
        char livesText[12];
        snprintf(livesText, 12, "LIVES: %d", Scene::lives);
        DrawText(livesText, 10, 10, 20, WHITE);
    }

    EndDrawing();
}

void shutdown() 
{
    delete gStartScreen;
    delete gLevelA;
    delete gLevelB;
    delete gLevelC;
    delete gEndScreen;
    delete gRestartScreen;

    for (int i = 0; i < NUMBER_OF_LEVELS; i++) gLevels[i] = nullptr;

    gShader.unload();

    UnloadTexture(gBackgroundTexture);

    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();

        if (gCurrentScene->getState().nextSceneID > 0)
        {
            int id = gCurrentScene->getState().nextSceneID;
            switchToScene(gLevels[id]);
        }

        render();
    }

    shutdown();

    return 0;
}