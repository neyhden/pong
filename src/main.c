#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <raylib.h>

#include "ecs.h"
#include "flecs.h"

typedef Vector2 Position, Velocity, Square, AABB;
typedef float Radius;
typedef struct {
    KeyboardKey up;
    KeyboardKey down;
} Controls;

void MoveSystem(ecs_iter_t* it) {
    Position* p = ecs_field(it, Position, 1);
    Velocity* v = ecs_field(it, Velocity, 2);
    for (int i = 0; i < it->count; i++) {
        if (p[i].x < 0 || p[i].x > GetRenderWidth()) v[i].x *= -1;
        if (p[i].y < 0 || p[i].y > GetRenderHeight()) v[i].y *= -1;
        p[i].x += v[i].x * it->delta_time;
        p[i].y += v[i].y * it->delta_time;
    }
}

void DrawCircleSystem(ecs_iter_t* it) {
    Position* p = ecs_field(it, Position, 1);
    Radius* r = ecs_field(it, Radius, 2);
    Color* c = ecs_field(it, Color, 3);
    for (int i = 0; i < it->count; i++) {
        DrawCircleV(p[i], r[i], c[i]);
    }
}

void DrawSquareSystem(ecs_iter_t* it) {
    Position* p = ecs_field(it, Position, 1);
    Square* s = ecs_field(it, Square, 2);
    Color* c = ecs_field(it, Color, 3);
    for (int i = 0; i < it->count; i++) {
        DrawRectangle(p[i].x - s[i].x / 2, p[i].y - s[i].y / 2,
                      s[i].x, s[i].y, c[i]);
    }
}

void PlayerInput(ecs_iter_t* it) {
    Controls* c = ecs_field(it, Controls, 1);
    Velocity* v = ecs_field(it, Velocity, 2);
    for (int i = 0; i < it->count; i++) {
        if (IsKeyDown(c[i].up)) v[i].y = -300;
        else if (IsKeyDown(c[i].down)) v[i].y = 300;
        else v[i].y = 0;
    }
}

void CheckCollisions(ecs_iter_t* it) {
    AABB* aabb = ecs_field(it, AABB, 1);
}

int main() {
    srand(time(NULL));

    ecs_world_t* world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);
    ECS_COMPONENT_DEFINE(world, Square);
    ECS_COMPONENT_DEFINE(world, Radius);
    ECS_COMPONENT_DEFINE(world, Color);
    ECS_COMPONENT_DEFINE(world, Controls);

    ECS_SYSTEM(world, MoveSystem, EcsOnUpdate, Position, Velocity);
    ECS_SYSTEM(world, DrawCircleSystem, EcsOnStore, Position, Radius, Color);
    ECS_SYSTEM(world, DrawSquareSystem, EcsOnStore, Position, Square, Color);
    ECS_SYSTEM(world, PlayerInput, EcsOnLoad, Controls, Velocity);

    const int width = 640;
    const int height = 480;

    // Ball
    ecs_entity_t ball = ecs_new_id(world);
    ecs_set(world, ball, Position, {width / 2.0, height / 2.0});
    ecs_set(world, ball, Velocity, {150, 150});
    ecs_set(world, ball, Radius, {10});
    ecs_set(world, ball, Color, {255, 255, 255, 255});

    // Left player
    ecs_entity_t l_player = ecs_new_id(world);
    ecs_set(world, l_player, Position, {30, height / 2.0});
    ecs_set(world, l_player, Velocity, {0, 0});
    ecs_set(world, l_player, Square, {10, height / 4.0});
    ecs_set(world, l_player, Color, {255, 255, 255, 255});
    ecs_set(world, l_player, Controls, {KEY_W, KEY_S});

    // Right player
    ecs_entity_t r_player = ecs_new_id(world);
    ecs_set(world, r_player, Position, {width - 30, height / 2.0});
    ecs_set(world, r_player, Velocity, {0, 0});
    ecs_set(world, r_player, Square, {10, height / 4.0});
    ecs_set(world, r_player, Color, {255, 255, 255, 255});
    ecs_set(world, r_player, Controls, {KEY_UP, KEY_DOWN});

    // Field separator
    ecs_entity_t separator = ecs_new_id(world);
    ecs_set(world, separator, Position, {width / 2.0, height / 2.0});
    ecs_set(world, separator, Square, {10, height});
    ecs_set(world, separator, Color, {255, 255, 255, 100});


    SetTargetFPS(60);
    InitWindow(width, height, "Pong");
    SetExitKey(KEY_ESCAPE);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        ecs_progress(world, GetFrameTime());
        DrawFPS(0, 0);
        EndDrawing();
    }

    CloseWindow();
    ecs_fini(world);
}
