#include "LevelB.h"

LevelB::LevelB()                                      : Scene { {0.0f}, nullptr   } {}
LevelB::LevelB(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelB::~LevelB() { shutdown(); }

void LevelB::initialise()
{
   // keep this here so when resetting it doens't auto loop through the game
   mGameState.nextSceneID = 0;

   // set enemy count
   mEnemyCount = 4;

   mGameState.bgm = LoadMusicStream("assets/Majestic Hills.mp3");
   SetMusicVolume(mGameState.bgm, 0.3f);
   PlayMusicStream(mGameState.bgm);

   mGameState.jumpSound = LoadSound("assets/jump.wav");
   mGameState.lifeLossSound = LoadSound("assets/lifeloss.mp3");
   mGameState.winSound = LoadSound("assets/win.wav");
   mGameState.killEnemySound = LoadSound("assest/killSound.mp3");

   /*
      ----------- MAPS -----------
   */
   mGameState.map = new Map(
      LEVEL_WIDTH, LEVEL_HEIGHT,   // map grid cols & rows
      (unsigned int *) mLevelData, // grid data
      "assets/TilesetNature.png",   // texture filepath
      TILE_DIMENSION,              // tile size
      24, 21,                        // texture cols & rows
      mOrigin                      // in-game origin
   );

   mGameState.baseMap = new Map(
      LEVEL_WIDTH, LEVEL_HEIGHT,
      (unsigned int *) mBorderData,
      "assets/TilesetFloor.png",
      TILE_DIMENSION,
      22, 26,
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
      {mOrigin.x - 925.0f, mOrigin.y - 275.0f}, // position
      {150.0f, 150.0f},             // scale
      "assets/sword_walk2.png",
      "assets/sword_attack2.png",
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
      {mOrigin.x + 925.0f, mOrigin.y - 420.0f}, // position
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
      {mOrigin.x - 925.0f, mOrigin.y - 123.0f}, // position
      {80.0f, 80.0f},             // scale
      "assets/skull.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 4, 4 },                                 // atlas dimensions
      enemyAnimationAtlas,                    // actual atlas
      NPC                                    // entity type
   );

   mGameState.enemy->setAIType(ORBITER);
   mGameState.enemy->setAIOrbit(
      150.0f, 
      {mOrigin.x - 800.0f, mOrigin.y - 275.0f});
   mGameState.enemy->setAcceleration({0.0f, 0.0f});
   mGameState.enemy->setColliderDimensions({
      mGameState.enemy->getScale().x / 3.0f,
      mGameState.enemy->getScale().y / 2.0f
   });

   // second enemy
   mGameState.enemy2 = new Entity(
      {mOrigin.x + 750.0f, mOrigin.y - 300.0f}, // position
      {80.0f, 80.0f},             // scale
      "assets/spirit.png",                   // texture file address 
      ATLAS,                                    // single image or atlas?
      { 4, 4 },                                 // atlas dimensions
      enemyAnimationAtlas,                    // actual atlas
      NPC                                    // entity type
   );

   mGameState.enemy2->setAIType(ORBITER);
   mGameState.enemy2->setAIOrbit(
      150.0f, 
      {mOrigin.x + 800.0f, mOrigin.y - 275.0f});
   mGameState.enemy2->setAcceleration({0.0f, 0.0f});
   mGameState.enemy2->setColliderDimensions({
      mGameState.enemy2->getScale().x / 3.0f,
      mGameState.enemy2->getScale().y / 2.0f
   });

   // third enemy
   mGameState.enemy3 = new Entity(
      {mOrigin.x + 750.0f, mOrigin.y - 300.0f}, // position
      {80.0f, 80.0f},             // scale
      "assets/redSpirit.png",                   // texture file address 
      ATLAS,                                    // single image or atlas?
      { 4, 4 },                                 // atlas dimensions
      enemyAnimationAtlas,                    // actual atlas
      NPC                                    // entity type
   );

   mGameState.enemy3->setAIType(ORBITER);
   mGameState.enemy3->setAIOrbit(
      150.0f, 
      {mOrigin.x - 800.0f, mOrigin.y + 275.0f});
   mGameState.enemy3->setAcceleration({0.0f, 0.0f});
   mGameState.enemy3->setColliderDimensions({
      mGameState.enemy3->getScale().x / 3.0f,
      mGameState.enemy3->getScale().y / 2.0f
   });
   
   // fourth enemy
   mGameState.enemy4 = new Entity(
      {mOrigin.x + 750.0f, mOrigin.y - 300.0f}, // position
      {80.0f, 80.0f},             // scale
      "assets/blueSkull.png",                   // texture file address 
      ATLAS,                                    // single image or atlas?
      { 4, 4 },                                 // atlas dimensions
      enemyAnimationAtlas,                    // actual atlas
      NPC                                    // entity type
   );

   mGameState.enemy4->setAIType(ORBITER);
   mGameState.enemy4->setAIOrbit(
      150.0f, 
      {mOrigin.x + 800.0f, mOrigin.y + 275.0f});
   mGameState.enemy4->setAcceleration({0.0f, 0.0f});
   mGameState.enemy4->setColliderDimensions({
      mGameState.enemy4->getScale().x / 3.0f,
      mGameState.enemy4->getScale().y / 2.0f
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

void LevelB::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);

   // array of collidable entities
   // modified entity class to accept 
   Entity* collidableEntities[5];

   collidableEntities[0] = mGameState.goal;
   collidableEntities[1] = mGameState.enemy;
   collidableEntities[2] = mGameState.enemy2;
   collidableEntities[3] = mGameState.enemy3;
   collidableEntities[4] = mGameState.enemy4;

   mGameState.mc->update(
      deltaTime,      // delta time / fixed timestep
      nullptr,        // player
      mGameState.map, // map
      collidableEntities,        // collidable entities
      5               // col. entity count
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

   mGameState.enemy4->update(
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
         mGameState.nextSceneID = 2;
      }
   }

   if (mGameState.mc->isCollidingGoal()) {
      PlaySound(mGameState.winSound);
      mGameState.nextSceneID = 3;
   }

   panCamera(&mGameState.camera, &currentPlayerPosition);
}

void LevelB::render()
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
   mGameState.enemy4->render();
}

void LevelB::shutdown()
{
   delete mGameState.mc;
   delete mGameState.baseMap;
   delete mGameState.map;
   delete mGameState.goal;
   delete mGameState.enemy;
   delete mGameState.enemy2;
   delete mGameState.enemy3;
   delete mGameState.enemy4;

   UnloadMusicStream(mGameState.bgm);
   UnloadSound(mGameState.jumpSound);
   UnloadSound(mGameState.lifeLossSound);
   UnloadSound(mGameState.winSound);
   UnloadSound(mGameState.killEnemySound);

}