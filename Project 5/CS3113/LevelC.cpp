#include "LevelC.h"

LevelC::LevelC()                                      : Scene { {0.0f}, nullptr   } {}
LevelC::LevelC(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelC::~LevelC() { shutdown(); }

void LevelC::initialise()
{
   // keep this here so when resetting it doens't auto loop through the game
   mGameState.nextSceneID = 0;

   // set enemy count
   mEnemyCount = 3;

   mGameState.bgm = LoadMusicStream("assets/Majestic Hills.mp3");
   SetMusicVolume(mGameState.bgm, 0.3f);
   PlayMusicStream(mGameState.bgm);

   mGameState.jumpSound = LoadSound("assets/jump.wav");
   mGameState.lifeLossSound = LoadSound("assets/lifeloss.mp3");
   mGameState.winSound = LoadSound("assets/win.wav");
   mGameState.boomSound = LoadSound("assets/boom.mp3");
   mGameState.killEnemySound = LoadSound("assest/killSound.mp3");

   /*
      ----------- MAPS -----------
   */
   mGameState.map = new Map(
      LEVEL_WIDTH, LEVEL_HEIGHT,   // map grid cols & rows
      (unsigned int *) mLevelData, // grid data
      "assets/TilesetDungeon.png",   // texture filepath
      TILE_DIMENSION,              // tile size
      12, 4,                        // texture cols & rows
      mOrigin                      // in-game origin
   );

   mGameState.baseMap = new Map(
      LEVEL_WIDTH, LEVEL_HEIGHT,
      (unsigned int *) mBorderData,
      "assets/TilesetInteriorFloor.png",
      TILE_DIMENSION,
      22, 17,
      mOrigin
   );

   /*
      ----------- PROTAGONIST -----------
   */
   std::map<Direction, std::vector<int>> mcAnimationAtlas = {
      {DOWN,  {  0,  1,  2,  3, 4, 5 }},
      {LEFT,  {  6,  7, 8, 9, 10, 11 }},
      {RIGHT,    { 12, 13, 14, 15, 16, 17 }},
      {UP, { 18, 19, 20, 21, 22, 23 }},
      // for attack
      {ATTACK_DOWN,  {  0,  1,  2,  3, 4, 5 }},
      {ATTACK_LEFT,  {  6,  7, 8, 9, 10, 11 }},
      {ATTACK_RIGHT,    { 12, 13, 14, 15, 16, 17 }},
      {ATTACK_UP, { 18, 19, 20, 21, 22, 23 }},
   };

   mGameState.mc = new Entity(
      {mOrigin.x - 925.0f, mOrigin.y}, // position
      {150.0f, 150.0f},             // scale
      "assets/sword_walk3.png",
      "assets/sword_attack3.png",
      ATLAS,                                    // single image or atlas?
      { 4, 6 },                                 // atlas dimensions
      mcAnimationAtlas,                    // actual atlas
      PLAYER                                    // entity type
   );

   mGameState.mc->setJumpingPower(550.0f);
   mGameState.mc->setColliderDimensions({
      mGameState.mc->getScale().x / 2.5f,
      mGameState.mc->getScale().y / 2.5f
   });
   mGameState.mc->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});

   /*
      ----------- TROPHY -----------
   */
   mGameState.goal = new Entity(
      {mOrigin.x + 925.0f, mOrigin.y}, // position
      {40.0f, 40.0f},             // scale
      "assets/goal.png",                   // texture file address
      GOAL                                    // entity type
   );

   mGameState.goal->setColliderDimensions({
      mGameState.goal->getScale().x / 5.0f,
      mGameState.goal->getScale().y
   });

   /*
      ----------- GATE -----------
   */
   mGameState.wall1 = new Entity(
      {mOrigin.x - 400.0f, mOrigin.y - 110.0f}, // position
      {75.0f, 75.0f},             // scale
      "assets/Rock.png",                   // texture file address
      GATE                                    // entity type
   );

   mGameState.wall1->setColliderDimensions({
      mGameState.wall1->getScale().x,
      mGameState.wall1->getScale().y
   });

   mGameState.wall2 = new Entity(
      {mOrigin.x - 400.0f, mOrigin.y - 37.0f}, // position
      {75.0f, 75.0f},             // scale
      "assets/Rock.png",                   // texture file address
      GATE                                    // entity type
   );

   mGameState.wall2->setColliderDimensions({
      mGameState.wall2->getScale().x,
      mGameState.wall2->getScale().y
   });



   mGameState.wall3 = new Entity(
      {mOrigin.x - 400.0f, mOrigin.y + 37.0f}, // position
      {75.0f, 75.0f},             // scale
      "assets/Rock.png",                   // texture file address
      GATE                                    // entity type
   );

   mGameState.wall3->setColliderDimensions({
      mGameState.wall3->getScale().x,
      mGameState.wall3->getScale().y
   });



   mGameState.wall4 = new Entity(
      {mOrigin.x - 400.0f, mOrigin.y + 110.0f}, // position
      {75.0f, 75.0f},             // scale
      "assets/Rock.png",                   // texture file address
      GATE                                    // entity type
   );

   mGameState.wall4->setColliderDimensions({
      mGameState.wall4->getScale().x,
      mGameState.wall4->getScale().y
   });

   mGameState.wall1->deactivate();
   mGameState.wall2->deactivate();
   mGameState.wall3->deactivate();
   mGameState.wall4->deactivate();

   /*
      ----------- ENEMY -----------
   */

   std::map<Direction, std::vector<int>> enemyAnimationAtlas = {
      {UP,  {  0,  4,  8,  12 }},
      {DOWN,  {  1, 5, 9, 13 }},
      {LEFT,    { 2, 6, 10, 14 }},
      {RIGHT, { 3, 7, 11, 15 }},
   };

   mGameState.enemy = new Entity(
      {mOrigin.x, mOrigin.y - 250.0f}, // position
      {50.0f, 50.0f},             // scale
      "assets/dragon.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 4, 4 },                                 // atlas dimensions
      enemyAnimationAtlas,                    // actual atlas
      NPC                                    // entity type
   );

   mGameState.enemy->setAIType(FOLLOWER);
   mGameState.enemy->setAIState(IDLE);
   mGameState.enemy->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
   mGameState.enemy->setColliderDimensions({
      mGameState.enemy->getScale().x / 3.0f,
      mGameState.enemy->getScale().y / 2.0f
   });

   // second enemy
   mGameState.enemy2 = new Entity(
      {mOrigin.x + 250.0f, mOrigin.y}, // position
      {50.0f, 50.0f},             // scale
      "assets/Beast.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 4, 4 },                                 // atlas dimensions
      enemyAnimationAtlas,                    // actual atlas
      NPC                                    // entity type
   );

   mGameState.enemy2->setAIType(FOLLOWER);
   mGameState.enemy2->setAIState(IDLE);
   mGameState.enemy2->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
   mGameState.enemy2->setColliderDimensions({
      mGameState.enemy2->getScale().x / 3.0f,
      mGameState.enemy2->getScale().y / 2.0f
   });

   // third enemy
   mGameState.enemy3 = new Entity(
      {mOrigin.x, mOrigin.y + 250.0f}, // position
      {50.0f, 50.0f},             // scale
      "assets/yellowDragon.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 4, 4 },                                 // atlas dimensions
      enemyAnimationAtlas,                    // actual atlas
      NPC                                    // entity type
   );

   mGameState.enemy3->setAIType(FOLLOWER);
   mGameState.enemy3->setAIState(IDLE);
   mGameState.enemy3->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
   mGameState.enemy3->setColliderDimensions({
      mGameState.enemy3->getScale().x / 3.0f,
      mGameState.enemy3->getScale().y / 2.0f
   });

   /*
      ----------- CAMERA -----------
   */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mGameState.mc->getPosition(); // camera follows player
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom
}

void LevelC::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);

   // array of collidable entities
   // modified entity class to accept 
   Entity* collidableEntities[8];

   collidableEntities[0] = mGameState.goal;
   collidableEntities[1] = mGameState.enemy;
   collidableEntities[2] = mGameState.enemy2;
   collidableEntities[3] = mGameState.enemy3;
   collidableEntities[4] = mGameState.wall1;
   collidableEntities[5] = mGameState.wall2;
   collidableEntities[6] = mGameState.wall3;
   collidableEntities[7] = mGameState.wall4;

   mGameState.mc->update(
      deltaTime,      // delta time / fixed timestep
      nullptr,        // player
      mGameState.map, // map
      collidableEntities,        // collidable entities
      8               // col. entity count
   );

   // update enemy
   mGameState.enemy->update(
      deltaTime,
      mGameState.mc, // mc as target
      mGameState.map,
      nullptr,
      0
   );

   mGameState.enemy2->update(
      deltaTime,
      mGameState.mc, // mc as target
      mGameState.map,
      nullptr,
      0
   );

   mGameState.enemy3->update(
      deltaTime,
      mGameState.mc, // mc as target
      mGameState.map,
      nullptr,
      0
   );

   Vector2 currentPlayerPosition = { mGameState.mc->getPosition().x, mGameState.mc->getPosition().y };
   
   // play sound if kill enemy
   if (mGameState.mc->killedEnemy())
   {
       PlaySound(mGameState.killEnemySound);
       mEnemyCount--;
   }

   // check if falling or colliding w enemy and lose a life
   if (mGameState.mc->getPosition().y > 800.0f || mGameState.mc->isCollidingEnemy()) {
      PlaySound(mGameState.lifeLossSound);
      lives--;
      if (lives == 0){
         mGameState.nextSceneID = 5;
      }
      else{
         mGameState.nextSceneID = 3;
      }
   }

   if (mGameState.mc->getPosition().x > mOrigin.x - 335.0f && !mGameState.wall1->isActive()) {
      mGameState.wall1->activate();
      mGameState.wall2->activate();
      mGameState.wall3->activate();
      mGameState.wall4->activate();
      PlaySound(mGameState.boomSound);
   }

   if (mGameState.mc->isCollidingGoal()) {
      PlaySound(mGameState.winSound);
      mGameState.nextSceneID = 4;
   }

   panCamera(&mGameState.camera, &currentPlayerPosition);
}

void LevelC::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   // render baseMap first so background is set
   // no collidable walls which are set in map
   mGameState.baseMap->render();
   mGameState.map->render();
   mGameState.mc->render();
   if (mEnemyCount == 0) {
      mGameState.goal->render();
   }
   mGameState.enemy->render();
   mGameState.enemy2->render();
   mGameState.enemy3->render();
   mGameState.wall1->render();
   mGameState.wall2->render();
   mGameState.wall3->render();
   mGameState.wall4->render();
}

void LevelC::shutdown()
{
   delete mGameState.mc;
   delete mGameState.baseMap;
   delete mGameState.map;
   delete mGameState.goal;
   delete mGameState.enemy;
   delete mGameState.enemy2;
   delete mGameState.enemy3;
   delete mGameState.wall1;
   delete mGameState.wall2;
   delete mGameState.wall3;
   delete mGameState.wall4;

   UnloadMusicStream(mGameState.bgm);
   UnloadSound(mGameState.jumpSound);
   UnloadSound(mGameState.lifeLossSound);
   UnloadSound(mGameState.winSound);
   UnloadSound(mGameState.boomSound);
   UnloadSound(mGameState.killEnemySound);
}