/**
* Author: Raymond Dong
* Assignment: Pong Clone
* Date due: 2025-10-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "starter/cs3113.h"

enum MEATSTATUS {ACTIVE, INACTIVE};
enum WINNER {NONE, LUFFY, ZORO};

// Global Constants
constexpr int SCREEN_WIDTH  = 1600 / 2,
              SCREEN_HEIGHT = 900 / 2,
              FPS           = 60,
              SIZE          = 120,
              SPEED         = 200;

constexpr char    BG_COLOUR[]    = "#F8F1C8";
constexpr Vector2 ORIGIN         = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  BASE_SIZE      = { (float) SIZE, (float) SIZE },
                  MEAT_SIZE      = { BASE_SIZE.x / 2, BASE_SIZE.y / 2},
                  INIT_POS  = { ORIGIN.x, ORIGIN.y },
                  LUFFY_INIT_POS = { 0 + SIZE/2, ORIGIN.y },
                  ZORO_INIT_POS = { ORIGIN.x + SCREEN_WIDTH / 2 - SIZE / 2, ORIGIN.y };

// One piece images
constexpr char LUFFY_FP[]  = "assets/luffy.png";
constexpr char ZORO_FP[] = "assets/zoro.png";
constexpr char MEAT_FP[] = "assets/meat.png";
constexpr char BACKGROUND_FP[] = "assets/background.jpg";
constexpr char LUFFYWINNER_FP[] = "assets/luffyWinner.jpeg";
constexpr char ZOROWINNER_FP[] = "assets/zoroWinner.jpg";


// Global Variables
AppStatus gAppStatus     = RUNNING;
float     gAngle         = 0.0f,
          gPreviousTicks = 0.0f;

Vector2 gLuffyPosition  = LUFFY_INIT_POS,
        gLuffyMovement  = { 0.0f, 0.0f },
        gLuffyScale     = BASE_SIZE,

        gZoroPosition = ZORO_INIT_POS,
        gZoroMovement = { 0.0f, 0.0f },
        gZoroScale    = BASE_SIZE,

        // diff movements + init positions to introduce pseudo randomness
        gFirstMeatPosition  = INIT_POS,
        gFirstMeatMovement  = { 0.75f, 1.0f },

        gSecondMeatPosition  = {INIT_POS.x - 100.0f, INIT_POS.y},
        gSecondMeatMovement  = { 1.0f, 1.0f },

        gThirdMeatPosition  = {INIT_POS.x + 100.0f, INIT_POS.y},
        gThirdMeatMovement  = { -0.75f, 1.0f },
        
        gMeatScale = {BASE_SIZE.x / 2, BASE_SIZE.y / 2},

        // background for winners
        gBackgroundScale = {SCREEN_WIDTH, SCREEN_HEIGHT};

MEATSTATUS  gFirstMeatStatus = ACTIVE,
            gSecondMeatStatus = INACTIVE,
            gThirdMeatStatus = INACTIVE;

WINNER  victor = NONE;

Texture2D   gLuffyTexture,
            gZoroTexture,
            gFirstMeatTexture,
            gSecondMeatTexture,
            gThirdMeatTexture,
            gBackgroundTexture,
            gLuffyWinnerTexture,
            gZoroWinnerTexture;

unsigned int startTime;

// to toggle between the one and two players
bool isSinglePlayer = false;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB);
void updateMeat(const Vector2 *position, const Vector2 *movement, MEATSTATUS status, float deltaTime);

// Function Definitions

/**
 * @brief Checks for a square collision between 2 Rectangle objects.
 * 
 * @see 
 * 
 * @param positionA The position of the first object
 * @param scaleA The scale of the first object
 * @param positionB The position of the second object
 * @param scaleB The scale of the second object
 * @return true if a collision is detected,
 * @return false if a collision is not detected
 */
bool isColliding(const Vector2 *positionA,  const Vector2 *scaleA, 
                 const Vector2 *positionB, const Vector2 *scaleB)
{
    float xDistance = fabs(positionA->x - positionB->x) - ((scaleA->x + scaleB->x) / 2.0f);
    float yDistance = fabs(positionA->y - positionB->y) - ((scaleA->y + scaleB->y) / 2.0f);

    if (xDistance < 0.0f && yDistance < 0.0f) return true;

    return false;
}

// avoid repeating the logic for deciding whether a meat sprite would be spawned
void updateMeat(Vector2 *position, Vector2 *movement, MEATSTATUS status, float deltaTime)
{
    if (status == ACTIVE) {
        position->x += movement->x * SPEED * deltaTime;
        position->y += movement->y * SPEED * deltaTime;
        // if touches top of bottom of screen change the y direction (reflect)
        if (position->y < gMeatScale.y / 2 || position->y + gMeatScale.y / 2 > SCREEN_HEIGHT)
            movement->y = -movement->y;
        // controls the bouncing off of the other two characters
        if (isColliding(&gLuffyPosition, &gLuffyScale, position, &gMeatScale))
            movement->x = -movement->x;
        if (isColliding(&gZoroPosition, &gZoroScale, position, &gMeatScale)) 
            movement->x = -movement->x;

        if (position->x < 0) victor = ZORO;
        else if (position->x > SCREEN_WIDTH) victor = LUFFY;
    }
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
        gAngle, WHITE
    );
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "User Input / Collision Detection");

    startTime = time(NULL);

    gLuffyTexture  = LoadTexture(LUFFY_FP);
    gZoroTexture = LoadTexture(ZORO_FP);
    gFirstMeatTexture = LoadTexture(MEAT_FP);
    gSecondMeatTexture = LoadTexture(MEAT_FP);
    gThirdMeatTexture = LoadTexture(MEAT_FP);
    gBackgroundTexture = LoadTexture(BACKGROUND_FP);
    gLuffyWinnerTexture = LoadTexture(LUFFYWINNER_FP);
    gZoroWinnerTexture = LoadTexture(ZOROWINNER_FP);

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (IsKeyPressed(KEY_T)) {
        // switch between two modes
       isSinglePlayer = !isSinglePlayer;
    }

    if (IsKeyPressed(KEY_ONE)) {
        gFirstMeatStatus = ACTIVE;
        gSecondMeatStatus = INACTIVE;
        gThirdMeatStatus = INACTIVE;
    }
    else if (IsKeyPressed(KEY_TWO)) {
        gFirstMeatStatus = ACTIVE;
        gSecondMeatStatus = ACTIVE;
        gThirdMeatStatus = INACTIVE;
    }
    else if (IsKeyPressed(KEY_THREE)) {
        gFirstMeatStatus = ACTIVE;
        gSecondMeatStatus = ACTIVE;
        gThirdMeatStatus = ACTIVE;
    }

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    // Delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    // if single player then auto right side (zoro)
    if (isSinglePlayer) {
        gLuffyMovement = {0.0f, 0.0f};
        // using else if so can't go up and down
        if (IsKeyDown(KEY_W)) gLuffyMovement.y = -0.75;
        else if (IsKeyDown(KEY_S)) gLuffyMovement.y = 0.75;

        if (gZoroMovement.y == 0) // make it so it goes up first if auto
            gZoroMovement.y = -0.75;
        // set bounds (change direction if it touches top or bottom yk)
        if (gZoroPosition.y - gZoroScale.y / 2 <= 0) gZoroMovement.y = 0.75;
        else if (gZoroPosition.y + gZoroScale.y / 2 >=  SCREEN_HEIGHT) gZoroMovement.y = -0.75;
    }
    else {
        gLuffyMovement = {0.0f, 0.0f};
        if (IsKeyDown(KEY_W)) gLuffyMovement.y = -0.75;
        else if (IsKeyDown(KEY_S)) gLuffyMovement.y = 0.75;

        gZoroMovement = {0.0f, 0.0f};
        if (IsKeyDown(KEY_UP)) gZoroMovement.y = -0.75;
        else if (IsKeyDown(KEY_DOWN)) gZoroMovement.y = 0.75;
    }

    // luffy movement
    gLuffyPosition = {
        gLuffyPosition.x,
        gLuffyPosition.y + SPEED * gLuffyMovement.y * deltaTime
    };
    // set bounds
    if (gLuffyPosition.y - gLuffyScale.y / 2 <= 0) gLuffyPosition.y = gLuffyScale.y / 2;
    if (gLuffyPosition.y + gLuffyScale.y / 2 >= SCREEN_HEIGHT) gLuffyPosition.y = SCREEN_HEIGHT - gLuffyScale.y / 2;

    // zoro movement
    gZoroPosition = {
        gZoroPosition.x,
        gZoroPosition.y + SPEED * gZoroMovement.y * deltaTime
    };
    // set bounds
    if (gZoroPosition.y - gZoroScale.y / 2 <= 0) gZoroPosition.y = gZoroScale.y / 2;
    if (gZoroPosition.y + gZoroScale.y / 2 >= SCREEN_HEIGHT) gZoroPosition.y = SCREEN_HEIGHT - gZoroScale.y / 2;

    // active meat movement
    // collisions based on meat 
    // the meat determine the win
    updateMeat(&gFirstMeatPosition, &gFirstMeatMovement, gFirstMeatStatus, deltaTime);
    updateMeat(&gSecondMeatPosition, &gSecondMeatMovement, gSecondMeatStatus, deltaTime);
    updateMeat(&gThirdMeatPosition, &gThirdMeatMovement, gThirdMeatStatus, deltaTime);
}

void render()
{
    BeginDrawing();
    // don't need this since using image as background
    // ClearBackground(ColorFromHex(BG_COLOUR));

    // render background
    renderObject(&gBackgroundTexture, &ORIGIN, &gBackgroundScale);

    // render Luffy
    renderObject(&gLuffyTexture, &gLuffyPosition, &gLuffyScale);

    // render Zoro
    renderObject(&gZoroTexture, &gZoroPosition, &gZoroScale);

    // render the meats
    if (gFirstMeatStatus == ACTIVE) renderObject(&gFirstMeatTexture, &gFirstMeatPosition, &gMeatScale);
    if (gSecondMeatStatus == ACTIVE) renderObject(&gSecondMeatTexture, &gSecondMeatPosition, &gMeatScale);
    if (gThirdMeatStatus == ACTIVE) renderObject(&gThirdMeatTexture, &gThirdMeatPosition, &gMeatScale);

    // winner screens
    if (victor == LUFFY) renderObject(&gLuffyWinnerTexture, &ORIGIN, &gBackgroundScale);
    else if (victor == ZORO) renderObject(&gZoroWinnerTexture, &ORIGIN, &gBackgroundScale);

    EndDrawing();
}

void shutdown() 
{ 
    CloseWindow(); 
    UnloadTexture(gLuffyTexture);
    UnloadTexture(gZoroTexture);
    UnloadTexture(gFirstMeatTexture);
    UnloadTexture(gFirstMeatTexture);
    UnloadTexture(gSecondMeatTexture);
    UnloadTexture(gThirdMeatTexture);
    UnloadTexture(gBackgroundTexture);
    UnloadTexture(gLuffyWinnerTexture);
    UnloadTexture(gZoroWinnerTexture);
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