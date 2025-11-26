#include "Entity.h"

#ifndef SCENE_H
#define SCENE_H

struct GameState
{
    Entity *mc;
    Entity *goal;
    Entity *enemy;
    Entity *enemy2;
    Entity *enemy3;
    Entity *enemy4;

    Entity *wall1;
    Entity *wall2;
    Entity *wall3;
    Entity *wall4;

    Map *map;
    Map *baseMap;

    Music bgm;
    Sound jumpSound;
    Sound lifeLossSound;
    Sound winSound;
    Sound boomSound;
    Sound killEnemySound;

    Camera2D camera;

    int nextSceneID;
};

class Scene 
{
protected:
    GameState mGameState;
    Vector2 mOrigin;
    const char *mBGColourHexCode = "#000000";
    
public:
    Scene();
    Scene(Vector2 origin, const char *bgHexCode);

    // public to allow main.cpp to access (for drawing number of lives)
    static int lives;

    virtual void initialise() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;
    
    GameState   getState()           const { return mGameState; }
    Vector2     getOrigin()          const { return mOrigin;    }
    const char* getBGColourHexCode() const { return mBGColourHexCode; }
};

#endif