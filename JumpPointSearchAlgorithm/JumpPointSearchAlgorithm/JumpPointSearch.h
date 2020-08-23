#pragma once

// Ÿ���� ���� ���� ����
#define PERMETER_OF_SQUARE 20

// ���� Ÿ�� ����
#define MAX_HEIGHT 40

// ���� Ÿ�� ����
#define MAX_WIDTH 70

extern HBRUSH brushBlockList[MAX_WIDTH][MAX_HEIGHT];


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


    NODE* PathFind(int startX, int startY, int destinationX, int destinationY);

    NODE* FindOpenList(int openX, int openY);

    bool FindCloseList(int openX, int openY);

    NODE* InsertOpenNode(NODE* node, NODE* destNode);

    NODE* SelectOpenListNode();

    NODE* CheckDirection(NODE* node,NODE* destNode , int x, int y);


    //===================================================================================================
    // ���� ������ Ž���ϴ� �Լ��Դϴ�.
    NODE* CheckUpVertical(NODE* parentNode, NODE* destNode, int x, int y, HBRUSH randBrush);

    NODE* CheckDownVertical(NODE* parentNode, NODE* destNode, int x, int y, HBRUSH randBrush);

    NODE* CheckRightHorizontal(NODE* parentNode, NODE* destNode, int x, int y, HBRUSH randBrush);

    NODE* CheckLeftHorizontal(NODE* parentNode, NODE* destNode, int x, int y, HBRUSH randBrush);
    //==================================================================================================


    //==================================================================================================================
    // �밢�� ������ Ž���ϴ� �Լ��Դϴ�.
    NODE* CheckRightUp(NODE* parentNode, NODE* destNode ,int x, int y, HBRUSH randBrush ,bool firstCall = true);

    NODE* CheckRightDown(NODE* parentNode, NODE* destNode, int x, int y, HBRUSH randBrush, bool firstCall = true);

    NODE* CheckLeftUp(NODE* parentNode, NODE* destNode, int x, int y, HBRUSH randBrush, bool firstCall = true);

    NODE* CheckLeftDown(NODE* parentNode, NODE* destNode, int x, int y, HBRUSH randBrush, bool firstCall = true);
    //===================================================================================================================



    //===============================================================================================================================
    // �밢���� ���� Ž�� �Լ��Դϴ�.
    bool CheckRightDiagonalHorizontal(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir,int x, int y, HBRUSH randBrush);

    bool CheckLeftDiagonalHorizontal(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush);

    bool CheckUpDiagonalVertical(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush);

    bool CheckDownDiagonalVertical(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush);
    //===============================================================================================================================



    NODE* SetCornerNode(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir ,int x, int y);

    void InsertRoute(NODE* node);

    void PathOptimizing();

    void ResetOpenList();

    void ResetCloseList();

    void ResetRouteList();

    void ResetBlock();

    void RouteReset();

    void OptimizeRouteReset();

    void ResetAll();

    void ReStart();

    void openListBubbleSort();
};


