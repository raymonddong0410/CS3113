#include "LevelB.h"

#ifndef LEVELC_H
#define LEVELC_H

class LevelC : public Scene {
private:
    unsigned int mLevelData[LEVEL_WIDTH * LEVEL_HEIGHT] = {
        56, 0,  0,  0,  0, 0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0, 0,  0,  0,  0,  0,  0,  56,
        56, 0,  0,  0,  0, 0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0, 0,  0,  0,  0,  0,  0,  56,
        56, 0,  0,  0,  0, 0,   0,   0,  30,  30, 0,  0,  0,  0,  0,  0,  0,  30,0, 0,  0, 0,  0,  0,  0,  0,  0,  56,
        56, 0,  0,  0,  0, 30,  0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  30,0,  0,  0,  0,  0,  0,  56,
        56, 0,  0,  0,  0, 0,   0,   0,   0,  0,  0,  0,  0,  133,134,95, 0,  0, 0, 0,  0, 0,  0,  0,  0,  0,  30, 56,
        56, 94,134,135, 0, 0,   0,   0,   0,  0,  0,  0,  0,  0,  0, 96,  0,  0, 0, 0,  0, 0,  0,  0,  0,  0,  0,  56,
        56, 96, 0,  0,  0, 0,   0,   0,   0,  0,  0,  0,  0,  0,  0,116,  0,  0, 0, 0,  0, 0,  0,  0,  0,  30, 0,  56,
        56, 116,0,  0,  0, 0,   0,   0,   0,  0,  0,  0,  0,  0,  0,116,  0,  0, 0, 0,  0, 0,  0,  0,  0,  0,  0,  56,
    };

public:
    static constexpr float TILE_DIMENSION       = 75.0f,
                        ACCELERATION_OF_GRAVITY = 981.0f,
                        END_GAME_THRESHOLD      = 800.0f;

    LevelC();
    LevelC(Vector2 origin, const char *bgHexCode);
    ~LevelC();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif