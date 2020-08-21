#pragma once

namespace JumpPointSearch
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


    extern CList<NODE*> openList;

    extern CList<NODE*> closeList;


    // 목적지 노드
    extern NODE* destinationNode;

    // 시작지 노드
    extern NODE* startNode;


    // 타이머를 통해서 함수 로직 실행할지를 설정한다.
    extern bool functionFlag;

    extern bool funcSetFlag;



    NODE* PathFind(int startX, int startY, int destinationX, int destinationY);

    NODE* FindOpenList(int openX, int openY);

    bool FindCloseList(int openX, int openY);

    NODE* InsertOpenNode(NODE* node, NODE* destNode);

    NODE* SelectOpenListNode();


    NODE* CheckDirection(NODE* node,NODE* destNode , int x, int y);


    NODE* CheckUpVertical(NODE* parentNode, NODE* destNode, int x, int y);

    NODE* CheckDownVertical(NODE* parentNode, NODE* destNode, int x, int y);

    NODE* CheckRightHorizontal(NODE* parentNode, NODE* destNode, int x, int y);

    NODE* CheckLeftHorizontal(NODE* parentNode, NODE* destNode, int x, int y);


    // 5방향을 체크한다.
    NODE* CheckRightUp(NODE* parentNode, NODE* destNode ,int x, int y, bool firstCall = true);

    NODE* CheckRightDown(NODE* parentNode, NODE* destNode, int x, int y, bool firstCall = true);

    NODE* CheckLeftUp(NODE* parentNode, NODE* destNode, int x, int y, bool firstCall = true);

    NODE* CheckLeftDown(NODE* parentNode, NODE* destNode, int x, int y, bool firstCall = true);



    bool CheckRightDiagonalHorizontal(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir,int x, int y);

    bool CheckLeftDiagonalHorizontal(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir, int x, int y);

    bool CheckUpDiagonalVertical(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir, int x, int y);

    bool CheckDownDiagonalVertical(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir, int x, int y);


    NODE* SetCornerNode(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir ,int x, int y);


    void ResetOpenList();

    void ResetCloseList();

    void ResetBlock();

    void RouteReset();

    void ResetAll();

    void ReStart();

    void openListBubbleSort();
};

