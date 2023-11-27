// raylib-brickout.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include "raylib.h"

const int screenWidth = 500;
const int screenHeight = 600;

struct Ball {
    Vector2 pos;
    Vector2 acceleration{ 1.0f, 1.0f };
    float velocity;
    float radius = 5.0f;
};

struct Brick {
    Rectangle rect;
    Color color;
    float w = 50.0f;
    float h = 20.0f;
};

struct Player {
    Rectangle rect;
    float velocity = 250.0f;
    int score = 0;
    float w = 75.0f;
    float h = 10.0f;
};

Texture2D texBackground;
Player player;
Ball ball;

std::vector<Brick> bricks;

Sound fxBrickHit[2];
Sound fxPaddleHit;

void GameStartup() {

    // loading the background image into video memory
    Image imgBackground = LoadImage("assets/background.png");
    texBackground = LoadTextureFromImage(imgBackground);
    UnloadImage(imgBackground);

    // loading the player paddle data
    player.rect = Rectangle{ 250.0f, 540.0f, player.w, player.h };
    player.score = 0;

    // loading the ball data
    ball.pos = Vector2{ 300, 300 };
    ball.velocity = 300.0f;

    std::vector<Color> COLORS{ RED, ORANGE, PURPLE, BROWN, PINK, MAGENTA, DARKGRAY };


    Brick newBrick;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            newBrick.rect = Rectangle{
                float(40 + (i * 55)),
                float(50 + (j * 26)),
                newBrick.w,
                newBrick.h
            };
            newBrick.color = COLORS[rand() % COLORS.size()];
            bricks.push_back(newBrick);
        }
    }

    fxBrickHit[0] = LoadSound("assets/brick-hit-1.wav");
    fxBrickHit[1] = LoadSound("assets/brick-hit-2.wav");
    fxPaddleHit = LoadSound("assets/paddle-hit-1.wav");

}

void GameUpdate() {

    float frameTime = GetFrameTime();

    // update player position based on keyboard input
    if (IsKeyDown(KEY_LEFT)) {
        player.rect.x = player.rect.x - (player.velocity * frameTime);
    }
    if (IsKeyDown(KEY_RIGHT)) {
        player.rect.x = player.rect.x + (player.velocity * frameTime);
    }

    // update the ball position
    ball.pos.x = ball.pos.x + ((ball.velocity * ball.acceleration.x) * frameTime);
    ball.pos.y = ball.pos.y + ((ball.velocity * ball.acceleration.y) * frameTime);

    // check collision between each brick and the ball
    Brick brick;
    int random;
    for (int i = 0; i < bricks.size(); i++) {
        brick = bricks[i];
        if (CheckCollisionCircleRec(
            ball.pos,
            ball.radius,
            brick.rect
        )) {
            ball.acceleration.y = ball.acceleration.y * -1;
            bricks.erase(bricks.begin() + i);
            player.score = player.score + 10;
            random = rand() % 2;
            PlaySound(fxBrickHit[random]);
            break;
        }
    }


    // check collision between walls and ball
    if (ball.pos.x > screenWidth || ball.pos.x < 10) {
        ball.acceleration.x = ball.acceleration.x * -1;
    }
    if (ball.pos.y > screenHeight || ball.pos.y < 10) {
        ball.acceleration.y = ball.acceleration.y * -1;
    }

    // check collision between wall and player
    if (player.rect.x < 0) {
        player.rect.x = 0;
    }

    if (player.rect.x > (screenWidth - player.rect.width)) {
        player.rect.x = (screenWidth - player.rect.width);
    }

    // check collision between ball and player
    if (CheckCollisionCircleRec(ball.pos, ball.radius, player.rect)) {
        ball.acceleration.x = ball.acceleration.x * -1;
        ball.acceleration.y = ball.acceleration.y * -1;
        PlaySound(fxPaddleHit);
    }

}

void GameRender() {

    // render the background
    DrawTexture(texBackground, 0, 0, RAYWHITE);

    // render the bricks
    Brick brick;
    for (int i = 0; i < bricks.size(); i++) {
        brick = bricks[i];
        DrawRectangle(
            brick.rect.x,
            brick.rect.y,
            brick.rect.width,
            brick.rect.height,
            brick.color
        );
    }

    // render the ball
    DrawCircle(ball.pos.x, ball.pos.y, ball.radius, RAYWHITE);

    // render the player
    DrawRectangle(player.rect.x, player.rect.y, player.rect.width, player.rect.height, YELLOW);

    // render the score
    // TODO: converting to a std::stringstream here might be ok too
    std::string sScore = std::to_string(player.score);
    std::string temp1 = "SCORE: ";
    std::string temp2 = temp1 + sScore;
    char const* text = temp2.c_str();
    DrawText(text, 10, 10, 30, YELLOW);


}

void GameShutdown() {

    UnloadSound(fxBrickHit[0]);
    UnloadSound(fxBrickHit[1]);
    UnloadSound(fxPaddleHit);

    UnloadTexture(texBackground);

    CloseAudioDevice();

}


int main()
{
    srand(time(NULL));

    InitWindow(screenWidth, screenHeight, "Raylib :: Brickout");
    InitAudioDevice();

    SetTargetFPS(60);

    GameStartup();

    while (!WindowShouldClose()) {

        GameUpdate();

        BeginDrawing();
        ClearBackground(BLUE);

        GameRender();

        EndDrawing();
    }

    GameShutdown();

    CloseWindow();
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
