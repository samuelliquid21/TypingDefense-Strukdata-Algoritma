#include "raylib.h"

int main() {
    // 1. Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib basic window");

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    // 2. Main Game Loop
    // Detect window close button or ESC key
    while (!WindowShouldClose()) {    
        // Update logic goes here (animations, inputs, etc.)

        // 3. Drawing
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawText("Hello World", 0, 0, 100, LIGHTGRAY);

        EndDrawing();
    }

    // 4. Cleanup
    CloseWindow();        

    return 0;
}