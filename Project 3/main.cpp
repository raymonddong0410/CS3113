/**
* Author: Raymond Dong
* Assignment: Lunar Lander
* Date due: 2025-10-27, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/Entity.h"

struct GameState
{
    Entity *ship;
    Entity *flames;
    Entity *platforms;
};

// Global Constants
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 600,
              FPS           = 60;

constexpr char    BG_COLOUR[]      = "#C0897E";
constexpr char BACKGROUND_FP[] = "assets/space.jpeg";
constexpr Vector2 ORIGIN           = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  gBackgroundScale = {SCREEN_WIDTH, SCREEN_HEIGHT},
                  ATLAS_DIMENSIONS = { 4, 4 },
                  FLAME_DIMENSIONS = { 1, 5 };

constexpr int   NUMBER_OF_FLAMES        = 62,
                NUMBER_OF_PLATFORMS     = 4;
constexpr float TILE_DIMENSION          = 50.0f,
                // in m/ms², since delta time is in ms
                ACCELERATION_OF_GRAVITY = 7.0f,
                HORIZONTAL_DECELERATION = 0.5f,
                MAX_ANGLE               = 90.0f,
                FIXED_TIMESTEP          = 1.0f / 60.0f,
                END_GAME_THRESHOLD      = 800.0f;

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

float angle = 0;

int fuel = 1500;
const char *lostReason = "MISSION FAILED: CRASHED";

GameState gState;

Texture2D   gBackgroundTexture;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
void renderTexture();
void resetGame();

void resetGame() {
    // no reset of platforms
    // add some randomness to the game

    // reset fuel, deafault losing reason, and angle
    fuel = 1500;
    lostReason = "MISSION FAILED: CRASHED";
    angle = 0.0f;

    // reset ship
    // must reset velocity and acceleration so doesn't auto fly off the screen
    gState.ship->setPosition({ORIGIN.x - 400.0f, ORIGIN.y - 200.0f});
    gState.ship->setVelocity({0.0f, 0.0f});
    gState.ship->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    gState.ship->setAngle(angle);
    gState.ship->setGameStatus(PLAYING);
}

void renderObject(const Texture2D *texture, const Vector2 *position, 
                  const Vector2 *scale)
{
    // Whole texture (UV coordinates)
    Rectangle textureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)
    };

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {
        position->x,
        position->y,
        static_cast<float>(scale->x),
        static_cast<float>(scale->y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {
        static_cast<float>(scale->x) / 2.0f,
        static_cast<float>(scale->y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        *texture, 
        textureArea, destinationArea, originOffset,
        0, WHITE
    );
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Project 3");

    gBackgroundTexture = LoadTexture(BACKGROUND_FP);

    /*
        ----------- SHIP -----------
    */
    std::map<EntityMovement, std::vector<int>> shipAnimationAtlas = {
        {IDLE,  {1}},
        {MOVING, {4, 5}},
    };

    gState.ship = new Entity(
        {ORIGIN.x - 400.0f, ORIGIN.y - 200.0f}, // position
        {35.0f, 35.0f},           // scale
        "assets/ship.png",                 // texture file address
        ATLAS,                                  // single image or atlas?
        ATLAS_DIMENSIONS,                       // atlas dimensions
        shipAnimationAtlas,                  // actual atlas
        PLAYER                                  // entity type
    );

    gState.ship->setColliderDimensions({
        gState.ship->getScale().x / 1.25f,
        gState.ship->getScale().y / 1.25f
    });
    gState.ship->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});

    /*
        ----------- FLAMES -----------
    */
    gState.flames = new Entity[NUMBER_OF_FLAMES];

    // Compute the left‑most x coordinate so that the entire row is centred
    float leftMostX = ORIGIN.x - (NUMBER_OF_FLAMES * TILE_DIMENSION) / 2.0f;

    // got flames from here: https://tekilor.itch.io/2d-fires-pack-2
    std::map<EntityMovement, std::vector<int>> flameAnimationAtlas = {
        // only idle since not controlling
        {IDLE, {0, 1, 2, 3, 4}},
    };

    for (int i = 0; i < NUMBER_OF_FLAMES; i++) 
    {
        gState.flames[i].setTexture("assets/flame.png");
        gState.flames[i].setEntityType(FLAME);
        gState.flames[i].setScale({TILE_DIMENSION, TILE_DIMENSION * 2});
        gState.flames[i].setColliderDimensions({TILE_DIMENSION, TILE_DIMENSION * 2});
        gState.flames[i].setPosition({
            leftMostX + i * TILE_DIMENSION / 1.5f, 
            SCREEN_HEIGHT - TILE_DIMENSION
        });
        gState.flames[i].setTextureType(ATLAS);
        gState.flames[i].setSpriteSheetDimensions(FLAME_DIMENSIONS);
        gState.flames[i].setAnimationAtlas(flameAnimationAtlas);
        // set frame speed so it animates
        gState.flames[i].setFrameSpeed(5);
    }

    /*
        ----------- PLATFORMS -----------
    */
    gState.platforms = new Entity[NUMBER_OF_PLATFORMS];

    for (int i = 0; i < NUMBER_OF_PLATFORMS; i++) 
    {
        gState.platforms[i].setTexture("assets/platform.png");
        gState.platforms[i].setEntityType(PLATFORM);
        gState.platforms[i].setScale({TILE_DIMENSION * 2, TILE_DIMENSION * 2});
        gState.platforms[i].setColliderDimensions(
            {TILE_DIMENSION / 1.4 , TILE_DIMENSION / 1.4});
        gState.platforms[i].setSpeed(10.0f);
    }

    gState.platforms[0].setPosition(
        {ORIGIN.x - TILE_DIMENSION * 3, ORIGIN.y - TILE_DIMENSION * 2.5f});
    gState.platforms[1].setPosition(
        {ORIGIN.x, ORIGIN.y + TILE_DIMENSION * 1.5f});
    gState.platforms[2].setPosition(
        {ORIGIN.x + TILE_DIMENSION * 7, ORIGIN.y - TILE_DIMENSION * 4});
    gState.platforms[3].setPosition(
        {ORIGIN.x - TILE_DIMENSION * 6, ORIGIN.y - TILE_DIMENSION});

    // set initial movement for 2 of the platforms
    gState.platforms[1].setMovement({-5.5f, 0.0f});
    gState.platforms[3].setMovement({4.5f, 0.0f});



    SetTargetFPS(FPS);
}

void processInput() 
{

    // if the game is not over yet allow process input
    if (gState.ship->getGameStatus() == PLAYING) {
        gState.ship->resetMovement();

        float currYAccel = ACCELERATION_OF_GRAVITY;
        float currXAccel = 0;

        // set deafult movement state
        // logic faulty if I change state in header file
        // this helps reset when nothing pressed
        EntityMovement movementState = IDLE;

        // use horizontal velocity so we know which direction to inverse
        float horizontalDecel = gState.ship->getVelocity().x * -HORIZONTAL_DECELERATION;

        // ship movement upwards
        if (IsKeyDown(KEY_W) && fuel > 0) {
            fuel--;
            gState.ship->moveUp(); 
            currYAccel = gState.ship->getAcceleration().y;
            movementState = MOVING;
            if (angle == 0) {}
            else if (angle > 0 && angle <= MAX_ANGLE + 1) angle -= 0.85;
            else if (angle < 0 && angle >= -MAX_ANGLE - 1) angle += 0.85;
        }

        // ship movement right
        if (IsKeyDown(KEY_D) && fuel > 0) { 
            fuel--;
            gState.ship->moveRight();
            currXAccel = gState.ship->getAcceleration().x;
            movementState = MOVING;
            if (angle <= MAX_ANGLE) angle += 0.85;
            }
        // ship movement left
        else if (IsKeyDown(KEY_A) && fuel > 0) {
            fuel--;
            gState.ship->moveLeft(); 
            currXAccel = gState.ship->getAcceleration().x;
            movementState = MOVING;
            if (angle >= -MAX_ANGLE) angle -= 0.85;
        }
        else {
            currXAccel = horizontalDecel;
        }

        gState.ship->setAcceleration({currXAccel, currYAccel});
        gState.ship->setEntityMovement(movementState);
        // change angle of ship to make it correlate w/ acceleration
        gState.ship->setAngle(angle);
    }
    else {
        if (IsKeyPressed(KEY_R)) {
                resetGame();
            }
    }

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    // check if the game is still going on
    // if run out of fuel lose
    if (fuel == 0) {
        gState.ship->setGameStatus(LOSE);
        lostReason = "MISSION FAILED: NO FUEL";
    }
    // if out of bounds lose
    Vector2 shipPosition = gState.ship->getPosition();
    Vector2 shipSize = gState.ship->getScale();
    if (shipPosition.x < 0 || 
        shipPosition.x > SCREEN_WIDTH ||
        shipPosition.y < 0) {
            gState.ship->setGameStatus(LOSE);
            lostReason = "MISSION FAILED: OFF SCREEN";
        }
        
    if (gState.ship->getGameStatus() != PLAYING) return;

    // Delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    // Fixed timestep
    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gState.ship->update(FIXED_TIMESTEP, nullptr, gState.flames, 
            NUMBER_OF_FLAMES, gState.platforms, NUMBER_OF_PLATFORMS);

        for (int i = 0; i < NUMBER_OF_PLATFORMS; i++) {
            gState.platforms[i].update(FIXED_TIMESTEP, nullptr, nullptr, 0, 
                nullptr, 0);

            // deal with changing movement
            if (gState.platforms[i].getPosition().x < 100) {
                gState.platforms[i].setMovement(
                    {-gState.platforms[i].getMovement().x, 0.0f}
                );
            }
            else if (gState.platforms[i].getPosition().x > SCREEN_WIDTH - 100) {
                gState.platforms[i].setMovement(
                    {-gState.platforms[i].getMovement().x, 0.0f}
                );
            }
        }

        for (int i = 0; i < NUMBER_OF_FLAMES; i++) 
            gState.flames[i].update(FIXED_TIMESTEP, nullptr, nullptr, 0, 
                nullptr, 0);

        deltaTime -= FIXED_TIMESTEP;
    }

    if (gState.ship->getPosition().y > END_GAME_THRESHOLD) 
        gAppStatus = TERMINATED;
}

void render()
{
    BeginDrawing();

    if (gState.ship->getGameStatus() == PLAYING) {
        // ClearBackground(ColorFromHex(BG_COLOUR));
        renderObject(&gBackgroundTexture, &ORIGIN, &gBackgroundScale);

        gState.ship->render();

        for (int i = 0; i < NUMBER_OF_FLAMES;  i++) gState.flames[i].render();
        for (int i = 0; i < NUMBER_OF_PLATFORMS; i++) gState.platforms[i].render();

        //render fuel text
        char fuelText[20];
        snprintf(fuelText, 20, "Fuel: %d", fuel);

        DrawText(fuelText, 10, 10, 20, WHITE);
    }
    else {
        renderObject(&gBackgroundTexture, &ORIGIN, &gBackgroundScale);

        if (gState.ship->getGameStatus() == WIN) {
            DrawText("MISSION ACCOMPLISHED", ORIGIN.x - SCREEN_WIDTH / 2 + 75, ORIGIN.y - 50.0f, 50, WHITE);
        }
        else {
            DrawText(lostReason, ORIGIN.x - SCREEN_WIDTH / 2 + 75, ORIGIN.y - 50.0f, 50, WHITE);
        }

        DrawText("Press Q to Quit", ORIGIN.x - SCREEN_WIDTH / 2 + 75, ORIGIN.y + 50.0f, 20, WHITE);
        DrawText("Press R to Reset", ORIGIN.x - SCREEN_WIDTH / 2 + 75, ORIGIN.y + 100.0f, 20, WHITE);
    }

    EndDrawing();
}

void shutdown() 
{
    delete   gState.ship;
    delete[] gState.flames;
    delete[] gState.platforms;
    UnloadTexture(gBackgroundTexture);

    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}