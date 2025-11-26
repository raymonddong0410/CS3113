#include "Scene.h"

#ifndef STARTSCREEN_H
#define STARTSCREEN_H

constexpr int LEVEL_WIDTH = 28,
              LEVEL_HEIGHT = 14;

class StartScreen : public Scene {
public:
    static constexpr float TILE_DIMENSION       = 75.0f,
                        ACCELERATION_OF_GRAVITY = 981.0f,
                        END_GAME_THRESHOLD      = 800.0f;

    StartScreen();
    StartScreen(Vector2 origin, const char *bgHexCode);
    ~StartScreen();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif