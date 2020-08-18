#pragma once

namespace JumpPointSearch
{
    struct NODE
    {
        int mX;
        int mY;

        BYTE mNodeDir;

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
        NODE_DIR_RU,
        NODE_DIR_ALL
    };


    extern BYTE blockList[MAX_WIDTH][MAX_HEIGHT];

    extern CList<NODE*> openList;

    extern CList<NODE*> closeList;


    // ������ ���
    extern NODE* destinationNode;

    // ������ ���
    extern NODE* startNode;


    // Ÿ�̸Ӹ� ���ؼ� �Լ� ���� ���������� �����Ѵ�.
    extern bool functionFlag;


    NODE* PathFind(int startX, int startY, int destinationX, int destinationY);

    NODE* FindOpenList(int openX, int openY);

    bool FindCloseList(int openX, int openY);

    NODE* InsertOpenNode(NODE* node, NODE* destinationNode);

    NODE* SelectOpenListNode();


    NODE* CheckDirection(NODE* node, int* x, int* y);


    NODE* CheckUpVertical(NODE* parentNode, NODE* destNode, int x, int y);

    NODE* CheckDownVertical(NODE* parentNode, NODE* destNode, int x, int y);

    NODE* CheckRightHorizontal(NODE* parentNode, NODE* destNode, int x, int y);

    NODE* CheckLeftHorizontal(NODE* parentNode, NODE* destNode, int x, int y);


    NODE* CheckRightDiagonalHorizontal(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir,int x, int y);

    NODE* CheckLeftDiagonalHorizontal(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir, int x, int y);

    NODE* CheckUpDiagonalVertical(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir, int x, int y);

    NODE* CheckDownDiagonalVertical(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir, int x, int y);




    NODE* SetCornerNode(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir ,int x, int y);


    void ResetOpenList();

    void ResetCloseList();

    void ResetBlock();

    void ResetAll();

    void openListBubbleSort();
};

