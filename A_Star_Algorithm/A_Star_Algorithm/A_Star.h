#pragma once

namespace A_Star
{
    struct NODE
    { 
        int mX;
        int mY;

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
        RED,
        GREEN,
        BLUE,
        GRAY,
        YELLOW,
        BLACK
    };


    extern BYTE blockList[MAX_WIDTH][MAX_HEIGHT];

    extern CList<NODE*> openList;

    extern CList<NODE*> closeList;

    // 타이머를 통해서 함수 로직 실행할지를 설정한다.
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
}
