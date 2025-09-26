/**
* Author: Raymond Dong
* Assignment: Simple 2D Scene
* Date due: 2025-09-27, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "starter/cs3113.h"
#include <math.h>

// Global Constants
constexpr int   SCREEN_WIDTH  = 1600,
                SCREEN_HEIGHT = 900,
                FPS           = 60,
                SIZE          = 200;
constexpr float MAX_AMP       = 50.0f,
                RADIUS        = 150.0f, // radius of the orbit
                ORBIT_SPEED   = 0.03f;  // the speed of orbit

constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
constexpr Vector2 BASE_SIZE  = { (float) SIZE, (float) SIZE };

/* SPRITES */
// ICHIGO //
constexpr char ICHIGO_FP[]  = "assets/ichigo.png";
Vector2 ichigoPosition = ORIGIN;
Vector2 ichigoScale = BASE_SIZE;
float   ichigoAngle = 0.0f;
float   ichigoRotation = 0.0f; // differentiate from angle so texture does not rotate 

// FIREBALL //
constexpr char FIREBALL_FP[] = "assets/fireball.png";
Vector2 fireballPosition = ichigoPosition; // based off ichigo origin
Vector2 fireballScale = { (float) 30, (float) 30};
float   fireballAngle = 0.0f;

// LUFFY //
constexpr char LUFFY_FP[] = "assets/luffy.png";
enum Direction {LEFT = 0, RIGHT = 1};
Direction luffyXDirection = LEFT;
Vector2 luffyPosition = ORIGIN;
Vector2 luffyScale = BASE_SIZE;
float luffyAngle = 0.0f;
float luffyRotation = 0.0f;
/* END OF SPRITE SECTION */


// Global Variables
AppStatus gAppStatus     = RUNNING;
float     gScaleFactor   = SIZE,
          gPulseTime     = 0.0f;
float     gPreviousTicks = 0.0f; // used to get delta time

// Changing Background Color
enum Colors {Purple = 0, Red = 1, Blue = 2};
void changeBGColor(Colors &color);
Colors color = Purple;
const char * BG_COLOUR = "#B2AAC6";

// Textures for sprites
Texture2D ichigoTexture;
Texture2D fireballTexture;
Texture2D luffyTexture;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

// Function Definitions
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Project 1");

    ichigoTexture = LoadTexture(ICHIGO_FP);
    fireballTexture = LoadTexture(FIREBALL_FP);
    luffyTexture = LoadTexture(LUFFY_FP);

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{

    // Delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    gPulseTime += 1.0f * deltaTime;

    // ICHIGO SPRITE //
    // Oval Orbit Effect
    ichigoAngle += ORBIT_SPEED;
    ichigoPosition.x = ORIGIN.x + RADIUS * cos(ichigoAngle) * 2;
    ichigoPosition.y = ORIGIN.y + RADIUS * sin(ichigoAngle);

    // Scaling size of Ichigo based on Delta Time
    ichigoScale = {
        BASE_SIZE.x + MAX_AMP * cos(gPulseTime),
        BASE_SIZE.y + MAX_AMP * cos(gPulseTime)
    };

    // FIREBALL SPRITE //
    // Circular orbit effect around ichigo
    fireballAngle += ORBIT_SPEED; // to make the fireball rotate
    fireballPosition.x = ichigoPosition.x + RADIUS * cos(fireballAngle) / 2;
    fireballPosition.y = ichigoPosition.y + RADIUS * sin(fireballAngle) / 2;

    // LUFFY SPRITE //
    // Lhanging BG color based on luffy position
    // Luffy position following left right path with sinusoidal oscillation 
    if (luffyXDirection == 0) {
        luffyPosition.x -= 7;
        if (luffyPosition.x <= 100) {
            luffyXDirection = RIGHT;
            changeBGColor(color);
        }
    }
    else if (luffyXDirection == 1) {
        luffyPosition.x += 7;
        if (luffyPosition.x >= SCREEN_WIDTH - 100) {
            luffyXDirection = LEFT;
            changeBGColor(color);
        }
    }

    luffyAngle += ORBIT_SPEED;
    luffyPosition.y = ORIGIN.y + RADIUS * sin(luffyAngle) * 2;

    // Scaling size of luffy based on Delta Time
    luffyScale = {
        BASE_SIZE.x + MAX_AMP * cos(gPulseTime),
        BASE_SIZE.y + MAX_AMP * cos(gPulseTime)
    };

}

void render()
{
    BeginDrawing();
    // sets background color
    ClearBackground(ColorFromHex(BG_COLOUR));

    // ICHIGO //
    // Whole texture (UV coordinates)
    Rectangle ichigoTextureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(ichigoTexture.width),
        static_cast<float>(ichigoTexture.height)
    };

    // Destination rectangle – centered at ichigo position
    Rectangle ichigoDestinationArea = {
        ichigoPosition.x,
        ichigoPosition.y,
        static_cast<float>(ichigoScale.x),
        static_cast<float>(ichigoScale.y)
    };

    // Origin inside the ichigo texture (centre of the texture)
    Vector2 ichigoObjectOrigin = {
        static_cast<float>(ichigoScale.x) / 2.0f,
        static_cast<float>(ichigoScale.y) / 2.0f
    };

    DrawTexturePro(
        ichigoTexture, 
        ichigoTextureArea, 
        ichigoDestinationArea, 
        ichigoObjectOrigin, 
        ichigoRotation, 
        WHITE
    );

    // FIREBALL //
    Rectangle fireballTextureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(fireballTexture.width),
        static_cast<float>(fireballTexture.height)
    };

    // Destination rectangle – centered at ichigo position (fireball origin)
    Rectangle fireballDestinationArea = {
        fireballPosition.x,
        fireballPosition.y,
        static_cast<float>(fireballScale.x),
        static_cast<float>(fireballScale.y)
    };

    // Origin inside the fireball texture
    Vector2 fireballObjectOrigin = {
        static_cast<float>(fireballScale.x) / 2.0f,
        static_cast<float>(fireballScale.y) / 2.0f
    };

    DrawTexturePro(
        fireballTexture,
        fireballTextureArea,
        fireballDestinationArea,
        fireballObjectOrigin,
        fireballAngle,
        WHITE
    );

    // LUFFY //
    Rectangle luffyTextureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(luffyTexture.width),
        static_cast<float>(luffyTexture.height)
    };

    // Destination rectangle – centered at luffy position
    Rectangle luffyDestinationArea = {
        luffyPosition.x,
        luffyPosition.y,
        static_cast<float>(luffyScale.x) / 2, // scale luffy smaller
        static_cast<float>(luffyScale.y) / 2  // scale luffy smaller
    };

    // Origin inside the luffy texture
    Vector2 luffyObjectOrigin = {
        static_cast<float>(luffyScale.x) / 2.0f,
        static_cast<float>(luffyScale.y) / 2.0f
    };

    DrawTexturePro(
        luffyTexture,
        luffyTextureArea,
        luffyDestinationArea,
        luffyObjectOrigin,
        luffyRotation,
        WHITE
    );

    EndDrawing();
}

void shutdown() { CloseWindow(); }

// helper function to change color of Background
void changeBGColor(Colors &color) {
    switch (color) {
        case Purple:
            color = Red;
            BG_COLOUR = "#FF6B6B";
            break;
        case Red:
            color = Blue;
            BG_COLOUR = "#6BAAFF";
            break;
        case Blue:
            color = Purple;
            BG_COLOUR = "#B2AAC6";
            break;
    }
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