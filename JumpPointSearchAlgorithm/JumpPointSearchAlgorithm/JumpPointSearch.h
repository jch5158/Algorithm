#pragma once

namespace JumpPointSearch
{
    struct NODE
    {
        int mX;
        int mY;

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
        RED,
        GREEN,
        BLUE,
        GRAY,
        YELLOW,
        BLACK
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
        NODE_DIR_RU
    };


    extern BYTE blockList[MAX_WIDTH][MAX_HEIGHT];

    extern CList<NODE*> openList;

    extern CList<NODE*> closeList;

    // Ÿ�̸Ӹ� ���ؼ� �Լ� ���� ���������� �����Ѵ�.
    extern bool functionFlag;


    NODE* PathFind(int startX, int startY, int destinationX, int destinationY);

    NODE* FindOpenList(int openX, int openY);

    bool FindCloseList(int openX, int openY);

    NODE* InsertOpenNode(NODE* node, NODE* destinationNode);

    NODE* SelectOpenListNode();

    void ResetOpenList();

    void ResetCloseList();

    void ResetBlock();

    void ResetAll();

    void openListBubbleSort();

};

