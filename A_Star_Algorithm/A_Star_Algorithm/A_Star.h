#pragma once

namespace A_Star
{
    struct NODE
    {
        DWORD mX;
        DWORD mY;

        NODE* prev;

        // �� �̵� Ƚ��
        float G;

        // ������������ ���� ����
        float H;

        // G+H ��
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
