#include "StartScreen.h"

#ifndef LEVELA_H
#define LEVELA_H

class LevelA : public Scene {
private:
    unsigned int mLevelData[LEVEL_WIDTH * LEVEL_HEIGHT] = {
        56, 0,  0,  0,  0, 0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,  56,
        56, 0,  0,  0,  0, 0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,  56,
        56, 0,  0,  0,  0, 0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,  56,
        56, 0,  0,  0,  0, 154, 155, 156, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 15, 16,0,  0,  0,  0,  0,  0,  56,
        56, 0,  0,  18, 0, 0,   0,   0,   0,  0,  18, 19, 20, 0,  0,  0,  18, 19, 0, 0,  0, 0,  0,  0,  18, 19, 19, 56,
        56, 19, 19, 39, 0, 0,   0,   0,   19, 19, 39, 39, 39, 19, 19, 19, 39, 39, 0, 0,  0, 19, 19, 19, 39, 39, 39, 56,
        56, 39, 39, 39, 0, 0,   0,   0,   39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 0, 0,  0, 39, 39, 39, 39, 39, 39, 56,
        56, 39, 39, 39, 0, 0,   0,   0,   39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 0, 0,  0, 39, 39, 39, 39, 39, 39, 56,
    };

public:
    static constexpr float TILE_DIMENSION       = 75.0f,
                        ACCELERATION_OF_GRAVITY = 981.0f,
                        END_GAME_THRESHOLD      = 800.0f;

    LevelA();
    LevelA(Vector2 origin, const char *bgHexCode);
    ~LevelA();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif