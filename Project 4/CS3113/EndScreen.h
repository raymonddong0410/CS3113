#include "LevelC.h"

#ifndef ENDSCREEN_H
#define ENDSCREEN_H

class EndScreen : public Scene {
public:
    static constexpr float TILE_DIMENSION       = 75.0f,
                        ACCELERATION_OF_GRAVITY = 981.0f,
                        END_GAME_THRESHOLD      = 800.0f;

    EndScreen();
    EndScreen(Vector2 origin, const char *bgHexCode);
    ~EndScreen();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif