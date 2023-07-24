#ifndef vec2_h
#define vec2_h

#include <stdbool.h>
#include <SDL2/SDL.h>

#define VEC_EQUAL_EPSILON   0.001f

#define DEG_TO_RAD(x)   ((x) * M_PI / 180.0f)
#define VEC_Clear(v)    { v.x = v.y = 0; }
#define SIGN(x)         x > 0 ? 1 : x < 0 ? -1 : 0

#define VEC_ADD(a, b)       a.x += b.x; \
                            a.y += b.y;
#define VEC_SCALE(a, b)     a.x *= b; \
                            a.y *= b;

typedef float vec_t;

typedef struct
{
    vec_t x;
    vec_t y;
} Vec2;

extern const Vec2 vec2_origin;

Vec2  VEC_Add(Vec2 v1, Vec2 v2);
bool  VEC_Compare(Vec2 va, Vec2 vb);
vec_t VEC_Length(Vec2 v);
vec_t VEC_LengthSquared(Vec2 v);
Vec2  VEC_Normalize(Vec2 v);
void  VEC_Print(const char * str, Vec2 v);
void  VEC_Render(SDL_Renderer * renderer, Vec2 v);
Vec2  VEC_Rotate (Vec2 v, float radians);
Vec2  VEC_Scale(Vec2 v, vec_t scale);
Vec2  VEC_Subtract(Vec2 va, Vec2 vb);
bool  VEC_PointInRect(Vec2 pt, SDL_Rect * rect);

#endif /* vec2_h */
