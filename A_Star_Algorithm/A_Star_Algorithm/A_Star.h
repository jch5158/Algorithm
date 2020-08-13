#pragma once

namespace A_Star
{
    struct NODE
    {
        DWORD mX;
        DWORD mY;

        NODE* prev;

        // 블럭 이동 횟수
        float G;

        // 목적지까지의 블럭의 개수
        float H;

        // G+H 값
        float F;
    };

    enum class BLOCK_COLOR
    {
        BASIC,
        GREEN,
        GRAY,
        RED
    };

    extern BYTE blockList[MAX_WIDTH][MAX_HEIGHT];
}
