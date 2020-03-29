#pragma once
#include "LocalPlayerInfo.h"
#include <d3dx9math.h>
#include <xmmintrin.h>

namespace Image {
bool screen_transform(const LocalPlayerInfo& local_player, const D3DXVECTOR3& point, D3DXVECTOR3& screen)
{
    const D3DXMATRIX& w2sMatrix = local_player.get_view_matrix();
    screen.x = w2sMatrix.m[0][0] * point.x + w2sMatrix.m[0][1] * point.y + w2sMatrix.m[0][2] * point.z + w2sMatrix.m[0][3];
    screen.y = w2sMatrix.m[1][0] * point.x + w2sMatrix.m[1][1] * point.y + w2sMatrix.m[1][2] * point.z + w2sMatrix.m[1][3];
    screen.z = 0.0f;

    float w = w2sMatrix.m[3][0] * point.x + w2sMatrix.m[3][1] * point.y + w2sMatrix.m[3][2] * point.z + w2sMatrix.m[3][3];

    if (w < 0.001f) {
        screen.x *= 100000;
        screen.y *= 100000;
        return true;
    }

    float invw = 1.0f / w;
    screen.x *= invw;
    screen.y *= invw;

    return false;
}

void world_to_screen(const LocalPlayerInfo& local_player, const D3DXVECTOR3& origin, D3DXVECTOR3& screen, int w, int h)
{
    if (screen_transform(local_player, origin, screen)) {
        return;
    }
    screen.x = (w / 2.0f) + (screen.x * w) / 2;
    screen.y = (h / 2.0f) - (screen.y * h) / 2;
}

void vector_angles(D3DXVECTOR3 forward, D3DXVECTOR3& angles)
{
    float yaw;
    float pitch;
    constexpr float pi = 3.14159f;


    if (forward.z == 0 && forward.x == 0) {
        yaw = 0;
        pitch = 270;
    } else {
        float tmp;
        yaw = (atan2(forward.y, forward.x) * 180 / pi);

        if (yaw < 0)
            yaw += 360;

        tmp = sqrtf(forward.x * forward.x + forward.y * forward.y);
        pitch = (atan2(-forward.z, tmp) * 180 / pi);

        if (pitch < 0)
            pitch += 360;
    }

    if (pitch > 180)
        pitch -= 360;
    else if (pitch < -180)
        pitch += 360;

    if (yaw > 180)
        yaw -= 360;
    else if (yaw < -180)
        yaw += 360;

    if (pitch > 89)
        pitch = 89;
    else if (pitch < -89)
        pitch = -89;

    if (yaw > 180)
        yaw = 180;
    else if (yaw < -180)
        yaw = -180;

    angles.x = pitch;
    angles.y = yaw;
    angles.z = 0;
}

D3DXVECTOR3 calculate_angle(const D3DXVECTOR3& source, const D3DXVECTOR3& destination)
{
    constexpr float pi = 3.14159f;
    const auto sqrtss = [](float in)
    {
        __m128 reg = _mm_load_ss(&in);
        return _mm_mul_ss(reg, _mm_rsqrt_ss(reg)).m128_f32[0];
    };
 
 
    D3DXVECTOR3 angles;
    D3DXVECTOR3 delta = source - destination;
    float hyp = sqrtss(delta.x * delta.x + delta.y * delta.y);
 
    angles.x = asinf(delta.z / hyp) * (180 / pi);
    angles.y = atanf(delta.y / delta.x) * (180 / pi) + !((*(DWORD*)&delta.x) >> 31 & 1) * 180;
    angles.z = 0;
 
    return angles;
}

}