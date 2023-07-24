#include "vec2.h"

#include <stdio.h>

const Vec2 vec2_origin = { 0.0f, 0.0f };

float VEC_Length(Vec2 v)
{
    return sqrtf(v.x * v.x + v.y * v.y);
}

float VEC_LengthSquared(Vec2 v)
{
    return v.x * v.x + v.y * v.y;
}

bool VEC_Compare(Vec2 va, Vec2 vb)
{
    vec_t dx = fabsf(va.x - vb.x);
    vec_t dy = fabsf(va.y - vb.y);
    return dx <= VEC_EQUAL_EPSILON && dy <= VEC_EQUAL_EPSILON;
}

Vec2 VEC_Add(Vec2 va, Vec2 vb)
{
    Vec2 out;
    out.x = va.x + vb.x;
    out.y = va.y + vb.y;
    
    return out;
}

Vec2 VEC_Subtract(Vec2 va, Vec2 vb)
{
    Vec2 out;
    out.x = va.x - vb.x;
    out.y = va.y - vb.y;
    
    return out;
}

Vec2 VEC_Scale(Vec2 v, vec_t scale)
{
    Vec2 out;
    out.x = v.x * scale;
    out.y = v.y * scale;

    return out;
}

Vec2 VEC_Normalize(Vec2 v)
{
    vec_t length, ilength;
    Vec2 out = { 0, 0 };

    length = sqrtf(v.x * v.x + v.y * v.y);
    if ( length == 0.0f ) {
        return out;
    }

    ilength = 1.0/length;
    out.x = v.x * ilength;
    out.y = v.y * ilength;

    return out;
}

Vec2 VEC_Rotate (Vec2 v, float radians)
{
    Vec2 out;
    out.x = cos(radians) * v.x - sin(radians) * v.y;
    out.y = sin(radians) * v.x + cos(radians) * v.y;
    
    return out;
}

void VEC_Print(const char * str, Vec2 v)
{
    if ( str ) {
        printf("%s ", str);
    }
    printf("(%.2f, %.2f)\n", v.x, v.y);
}

void VEC_Render(SDL_Renderer * renderer, Vec2 v)
{
    SDL_RenderDrawLine(renderer, 0, 0, v.x, v.y);
}

bool VEC_PointInRect(Vec2 pt, SDL_Rect * rect)
{
    int top = rect->y;
    int bottom = rect->y + rect->h;
    int left = rect->x;
    int right = rect->x + rect->w;
    
    if ( pt.x < left || pt.x > right )
        return false;
    
    if ( pt.y < top || pt.y > bottom )
        return false;
    
    return true;
}
