#pragma once


namespace BresenhamLine
{

    struct NODE
    {
        int mX;
        int mY;

        BYTE mNodeDir;

        NODE* prev;

        // 블럭 이동 횟수
        float G;

        // 목적지까지의 블럭의 개수
        float H;

        // G+H 값
        float F;
    };


    enum class NODE_DIRECTION
    {
        NODE_DIR_RR,
        NODE_DIR_RD,
        NODE_DIR_DD,
        NODE_DIR_LD,
        NODE_DIR_LL,
        NODE_DIR_LU,
        NODE_DIR_UU,
        NODE_DIR_RU,
        NODE_DIR_ALL
    };


    extern HBRUSH brushBlockList[MAX_WIDTH][MAX_HEIGHT];


    void MakeLine(int startX, int startY, int endX, int endY);


};

