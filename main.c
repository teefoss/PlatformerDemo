#include "vec2.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define MS_PER_FRAME 16
#define RENDER_W 256
#define RENDER_H 240
#define SCALE 3

#define TILE_SIZE 16
#define LEVEL_W (RENDER_W / TILE_SIZE)
#define LEVEL_H (RENDER_H / TILE_SIZE)
#define GROUND_CHAR '#'
#define BLOCK_CHAR 'O'
#define SKY_CHAR '.'

#define SPRITE_TILE_W 16
#define NUM_RUNNING_FRAMES 3

// horizontal tile number location in spritesheet:
typedef enum {
    MARIO_SPRITE_STANDING,
    MARIO_SPRITE_RUNNING1,
    MARIO_SPRITE_RUNNING2,
    MARIO_SPRITE_RUNNING3,
    MARIO_SPRITE_TURNING,
    MARIO_SPRITE_JUMPING,
    MARIO_SPRITE_CROUCHING
} MarioSprite;

typedef enum
{
    TILE_GROUND,
    TILE_BLOCK
} TileSprite;

typedef struct { int left, top, right, bottom; } Box;

typedef struct
{
    Vec2 position;
    Vec2 velocity;
    int w, h;
    enum { RIGHT, LEFT } facing;
    int frame; // running animation frame
    bool is_grounded;
} Mario;


SDL_Window * window;
SDL_Renderer * renderer;
const Uint8 * keys;
SDL_Texture * mario_texture;
SDL_Texture * blocks_texture;

Mario mario = {
    .w = 13,
    .h = 16,
};

const char level[LEVEL_H][LEVEL_W] = {
    "................",
    "................",
    ".....OOOOO......",
    "OO.......O....OO",
    ".........O......",
    "........OO......",
    "...O.....O......",
    "...O............",
    ".OOO............",
    "OO......OO.....O",
    "O...OO.OOO....OO",
    "O....O.OO....OOO",
    "O....O......OOOO",
    "################",
    "################"
};


SDL_Texture * LoadTexture(const char * path)
{
    SDL_Texture * t = IMG_LoadTexture(renderer, path);
    if ( t == NULL ) {
        fprintf(stderr, "Failed to load %s! (%s)\n", path, SDL_GetError());
        exit(EXIT_FAILURE);
    }
    
    return t;
}


// get the sprite for current state
MarioSprite GetMarioSprite()
{
    if ( mario.velocity.y ) {
        return MARIO_SPRITE_JUMPING;
    } else {
        if ( mario.velocity.x == 0 ) {
            return MARIO_SPRITE_STANDING;
        } else {
            return MARIO_SPRITE_RUNNING1 + mario.frame;
        }
    }
}


void DrawMario()
{
    SDL_Rect src = {
        .x = GetMarioSprite() * SPRITE_TILE_W,
        .y = 32, // tiny mario for now
        .w = SPRITE_TILE_W,
        .h = mario.h
    };
    
    SDL_Rect dst = {
        .x = mario.position.x - 1,
        .y = mario.position.y,
        .w = SPRITE_TILE_W,
        .h = mario.h
    };
    
    SDL_RenderCopyEx(renderer, mario_texture, &src, &dst, 0, NULL, mario.facing);
}


void DrawLevel()
{
    SDL_Rect src = { 0, 0, TILE_SIZE, TILE_SIZE };
    SDL_Rect dst = { 0, 0, TILE_SIZE, TILE_SIZE };
    
    for ( int y = 0; y < LEVEL_H; y++ ) {
        for ( int x = 0; x < LEVEL_W; x++ ) {
            
            switch ( level[y][x] ) {
                case GROUND_CHAR:
                    src.x = TILE_GROUND * TILE_SIZE;
                    break;
                case BLOCK_CHAR:
                    src.x = TILE_BLOCK * TILE_SIZE;
                    break;
                default:
                    continue;
            }

            dst.x = x * TILE_SIZE;
            dst.y = y * TILE_SIZE;
            SDL_RenderCopy(renderer, blocks_texture, &src, &dst);
        }
    }
}


// get which tile the sides of an object are in
Box GetTileSides(float x, float y, int w, int h)
{
    Box box = {
        .left = x / TILE_SIZE,
        .top = y / TILE_SIZE,
        .right = (x + w) / TILE_SIZE,
        .bottom = (y + h) / TILE_SIZE
    };
    
    return box;
}


// check level tiles for collision with right or left side of object
bool HorizontalCollision(int tile_side)
{
    int tile_top = mario.position.y / TILE_SIZE;
    int tile_bottom = (mario.position.y + mario.h - 1) /  TILE_SIZE;
    
    for ( int y = tile_top; y <= tile_bottom; y++ ) {
        if ( level[y][tile_side] != SKY_CHAR ) {
            return true;
        }
    }
    
    return false;
}


// check level tiles for collision with top or bottom side of object
bool VerticalCollision(int tile_side)
{
    int tile_left = mario.position.x / TILE_SIZE; // old sides
    int tile_right = (mario.position.x + mario.w - 1) /  TILE_SIZE;
    
    for ( int x = tile_left; x <= tile_right; x++ ) {
        if ( level[tile_side][x] != SKY_CHAR ) {
            return true;
        }
    }
    
    return false;
}


void TryMove(float dt)
{
    //Box tile = GetTileSides(mario.x, mario.y, mario.w, mario.h);
//    tile.right--; // make sure the to check within the current tile
//    tile.bottom--;
    Vec2 velocity = VEC_Scale(mario.velocity, dt);
    Vec2 position = VEC_Add(mario.position, velocity);

    Box new_tile = GetTileSides(position.x, position.y, mario.w, mario.h);
    
    // handle horizontal collisions
    
    if ( mario.velocity.x < 0 && HorizontalCollision(new_tile.left) ) {
        position.x = (new_tile.left + 1) * TILE_SIZE; // clip to side
        mario.velocity.x = 0;
    } else if ( mario.velocity.x > 0 && HorizontalCollision(new_tile.right) ) {
        position.x = (new_tile.right * TILE_SIZE) - mario.w;
        mario.velocity.x = 0;
    }
    
    // handle vertical collisions
    
    if ( mario.velocity.y < 0 && VerticalCollision(new_tile.top) ) {
        position.y = (new_tile.top + 1) * TILE_SIZE;
        mario.velocity.y = 0;
    } else if ( mario.velocity.y > 0 && VerticalCollision(new_tile.bottom) ) {
        position.y = (new_tile.bottom * TILE_SIZE) - mario.h;
        mario.velocity.y = 0;
        mario.is_grounded = true;
    } else {
        mario.is_grounded = false;
    }

    mario.position = position;

    // wrap around
    if ( mario.position.x > RENDER_W ) {
        mario.position.x = 0;
    } else if ( mario.position.x < 0 ) {
        mario.position.x = RENDER_W - 1;
    }
}


void ApplyHorizontalInertia()
{
    mario.velocity.x *= 0.9f;
    
    if ( fabsf(mario.velocity.x) < 1.0f ) {
        mario.velocity.x = 0.0f;
    }
}


void DoLeftRightKeys()
{
    const float movement_speed = 10.0f;
    
    if ( keys[SDL_SCANCODE_A] ) {
        mario.velocity.x -= movement_speed;
        mario.facing = LEFT;
    }
    
    if ( keys[SDL_SCANCODE_D] ) {
        mario.velocity.x += movement_speed;
        mario.facing = RIGHT;
    }
}

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Mario Demo", 0, 0, RENDER_W * SCALE, RENDER_H * SCALE, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, RENDER_W, RENDER_H);
    keys = SDL_GetKeyboardState(NULL);
    
    mario_texture = LoadTexture("assets/graphics/mario.png");
    blocks_texture = LoadTexture("assets/graphics/blocks.png");
    
    int ticks = 0;
    int frame_time = 0;
    bool jump_pressed = false;
    bool quit_requested = false;

    while ( !quit_requested ) {
        
        ++ticks;
        while ( !SDL_TICKS_PASSED(SDL_GetTicks(), frame_time + MS_PER_FRAME) ) {
            SDL_Delay(1);
        }
        float dt = (float)(SDL_GetTicks() - frame_time) / 1000.0f;
        frame_time = SDL_GetTicks();

        SDL_Event event;
        while ( SDL_PollEvent(&event) ) {
            quit_requested = (event.type == SDL_QUIT);
            if ( event.type == SDL_KEYDOWN ) {
                switch ( event.key.keysym.sym ) {
                    case SDLK_w:
                        if ( mario.is_grounded && !jump_pressed ) {
                            mario.velocity.y = -10000.0f * dt;
                            mario.is_grounded = false;
                            jump_pressed = true;
                        }
                    default:
                        break;
                }
            }
        }
        
        // handle movement input
        
        //DoJumpKey();
        if ( keys[SDL_SCANCODE_W] && mario.velocity.y < 0.0f ) {
            mario.velocity.y -= 4.0f;
        } else {
            jump_pressed = false;
        }

        DoLeftRightKeys();
        
        // update mario
        
        if ( ticks % 5 == 0 ) {
            mario.frame = (mario.frame + 1) % NUM_RUNNING_FRAMES;
        }
        
        mario.velocity.y += 500.0f * dt; // apply gravity
        //printf("%f\n", mario.velocity.y);
        if ( mario.velocity.y > 200.0f ) {
            mario.velocity.y = 200.0f;
        }

        ApplyHorizontalInertia();
        TryMove(dt);
        
        // render
        
        SDL_SetRenderDrawColor(renderer, 159, 158, 255, 255); // sky color
        SDL_RenderClear(renderer);
        DrawLevel();
        DrawMario();
        
        SDL_RenderPresent(renderer);        
    };
    
    SDL_DestroyTexture(mario_texture);
    SDL_DestroyTexture(blocks_texture);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    SDL_Quit();

	return 0;
}
