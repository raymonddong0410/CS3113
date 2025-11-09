#include "LevelC.h"

LevelC::LevelC()                                      : Scene { {0.0f}, nullptr   } {}
LevelC::LevelC(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelC::~LevelC() { shutdown(); }

void LevelC::initialise()
{
   // keep this here so when resertting it doens't auto loop through the game
   mGameState.nextSceneID = 0;

   mGameState.bgm = LoadMusicStream("assets/Majestic Hills.mp3");
   SetMusicVolume(mGameState.bgm, 0.40f);
   PlayMusicStream(mGameState.bgm);

   mGameState.jumpSound = LoadSound("assets/jump.wav");
   mGameState.lifeLossSound = LoadSound("assets/lifeloss.mp3");
   mGameState.winSound = LoadSound("assets/win.wav");

   /*
      ----------- MAP -----------
   */
   mGameState.map = new Map(
      LEVEL_WIDTH, LEVEL_HEIGHT,   // map grid cols & rows
      (unsigned int *) mLevelData, // grid data
      "assets/tilemap.png",   // texture filepath
      TILE_DIMENSION,              // tile size
      20, 9,                        // texture cols & rows
      mOrigin                      // in-game origin
   );

   /*
      ----------- PROTAGONIST -----------
   */
   std::map<Direction, std::vector<int>> mcAnimationAtlas = {
      {DOWN,  {  0,  1,  2,  3 }},
      {LEFT,  {  4,  5,  6,  7 }},
      {RIGHT,    { 8, 9, 10, 11 }},
      {UP, { 12, 13, 14, 15 }},
   };

   mGameState.mc = new Entity(
      {mOrigin.x - 900.0f, mOrigin.y - 200.0f}, // position
      {100.0f, 100.0f},             // scale
      "assets/mc.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 4, 4 },                                 // atlas dimensions
      mcAnimationAtlas,                    // actual atlas
      PLAYER                                    // entity type
   );

   mGameState.mc->setJumpingPower(550.0f);
   mGameState.mc->setColliderDimensions({
      mGameState.mc->getScale().x / 6.0f,
      mGameState.mc->getScale().y / 1.2f
   });
   mGameState.mc->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});

   /*
      ----------- TROPHY -----------
   */
   mGameState.goal = new Entity(
      {mOrigin.x + 925.0f, mOrigin.y - 50.0f}, // position
      {40.0f, 40.0f},             // scale
      "assets/goal.png",                   // texture file address
      GOAL                                    // entity type
   );

   mGameState.goal->setColliderDimensions({
      mGameState.goal->getScale().x / 5.0f,
      mGameState.goal->getScale().y
   });

   /*
      ----------- ENEMY -----------
   */

   std::map<Direction, std::vector<int>> enemyAnimationAtlas = {
      {DOWN,  {  0,  1,  2,  3 }},
      {LEFT,  {  4,  5,  6,  7 }},
      {RIGHT,    { 8, 9, 10, 11 }},
      {UP, { 12, 13, 14, 15 }},
   };

   mGameState.enemy = new Entity(
      {mOrigin.x, mOrigin.y - 100.0f}, // position
      {100.0f, 100.0f},             // scale
      "assets/enemy4.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 4, 4 },                                 // atlas dimensions
      enemyAnimationAtlas,                    // actual atlas
      NPC                                    // entity type
   );

   mGameState.enemy->setAIType(ORBITER);
   mGameState.enemy->setAIOrbit(
      150.0f, 
      {mOrigin.x - 375.0f, mOrigin.y - 150.0f});
   mGameState.enemy->setAcceleration({0.0f, 0.0f});
   mGameState.enemy->setColliderDimensions({
      mGameState.enemy->getScale().x / 7.0f,
      mGameState.enemy->getScale().y / 2.0f
   });

   // second enemy
   mGameState.enemy2 = new Entity(
      {mOrigin.x, mOrigin.y - 100.0f}, // position
      {100.0f, 100.0f},             // scale
      "assets/enemy5.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 4, 4 },                                 // atlas dimensions
      enemyAnimationAtlas,                    // actual atlas
      NPC                                    // entity type
   );

   mGameState.enemy2->setAIType(ORBITER);
   mGameState.enemy2->setAIOrbit(
      125.0f, 
      {mOrigin.x + 375.0f, mOrigin.y - 100.0f});
   mGameState.enemy2->setAcceleration({0.0f, 0.0f});
   mGameState.enemy2->setColliderDimensions({
      mGameState.enemy2->getScale().x / 7.0f,
      mGameState.enemy2->getScale().y / 2.0f
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
   Entity* collidableEntities[2];

   collidableEntities[0] = mGameState.goal;
   collidableEntities[1] = mGameState.enemy;

   mGameState.mc->update(
      deltaTime,      // delta time / fixed timestep
      nullptr,        // player
      mGameState.map, // map
      collidableEntities,        // collidable entities
      2               // col. entity count
   );

   // update enemies
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

   Vector2 currentPlayerPosition = { mGameState.mc->getPosition().x, mOrigin.y };

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

   if (mGameState.mc->isCollidingGoal()) {
      PlaySound(mGameState.winSound);
      mGameState.nextSceneID = 4;
   }

   panCamera(&mGameState.camera, &currentPlayerPosition);
}

void LevelC::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   mGameState.mc->render();
   mGameState.map->render();
   mGameState.goal->render();
   mGameState.enemy->render();
   mGameState.enemy2->render();
}

void LevelC::shutdown()
{
   delete mGameState.mc;
   delete mGameState.map;
   delete mGameState.goal;
   delete mGameState.enemy;
   delete mGameState.enemy2;

   UnloadMusicStream(mGameState.bgm);
   UnloadSound(mGameState.jumpSound);
   UnloadSound(mGameState.lifeLossSound);
   UnloadSound(mGameState.winSound);
}