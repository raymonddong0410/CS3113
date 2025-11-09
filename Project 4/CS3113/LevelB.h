#include "LevelA.h"

#ifndef LEVELB_H
#define LEVELB_H

class LevelB : public Scene {
private:
    unsigned int mLevelData[LEVEL_WIDTH * LEVEL_HEIGHT] = {
        56, 0,  0,  0,  0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,  56,
        56, 0,  0,  0,  0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,  56,
        56,104, 0,  0,  0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 49, 50,51,  0,  0,  0,  0,  0,  56,
        56, 123, 104,0,  0,   0,   0,   0,   101,0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,0,0,  0,  0,  0,  0,  0,  56,
        56, 123, 123, 104,0,   0,   0,   102, 123, 103, 103, 103, 104, 0,  0,  0,  102, 103, 0, 0,  0, 0,  0,  0,  102, 103, 103, 56,
        56, 123, 123, 123, 103, 103, 103, 123,  123, 123, 123, 123, 123, 103, 103, 103, 123, 123, 0, 0,  0, 0, 0, 103, 123, 123, 123, 56,
        56, 123, 123, 123, 123,  123,  123,  123,  123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 0, 0,  0, 0, 0, 123, 123, 123, 123, 56,
        56, 123, 123, 123, 123,  123,  123,  123,  123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 0, 0,  0, 0, 0, 123, 123, 123, 123, 56,
    };

public:
    static constexpr float TILE_DIMENSION       = 75.0f,
                        ACCELERATION_OF_GRAVITY = 981.0f,
                        END_GAME_THRESHOLD      = 800.0f;

    LevelB();
    LevelB(Vector2 origin, const char *bgHexCode);
    ~LevelB();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif