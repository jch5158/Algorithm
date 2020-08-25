#pragma once

// 타일의 가로 세로 길이
#define PERMETER_OF_SQUARE 20

// 세로 타일 개수
#define MAX_HEIGHT 40

// 가로 타일 개수
#define MAX_WIDTH 70




// TODO : JSP 클래스화하기
class JumpPointSearch
{
public:
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


    JumpPointSearch(int x, int y);

    ~JumpPointSearch();


    bool PathFind(int startX, int startY, int destinationX, int destinationY, RouteNode *routeNodeArray, int routeNodeArraySize);

    void SettingMapAttrivute(int posX, int posY);

    void ResetAll(RouteNode* routeNodeArray, int routeNodeArraySize);

    void ReStart(RouteNode* routeNodeArray, int routeNodeArraySize);
    
private:

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


    enum class NODE_ATTRIBUTE
    {
        NODE_UNBLOCK,
        NODE_BLOCK
    };


    // TODO : private 함수 함수명 전환하기  
    NODE* FindOpenList(int openX, int openY);

    bool FindCloseList(int openX, int openY);

    void InsertOpenNode(NODE* node);

    NODE* SelectOpenListNode();

    void CheckDirection(NODE* node, int x, int y);


    // TODO : mMapWidth, mMapHeight 사용하는 로직으로 변경
    //===================================================================================================
    // 직선 방향을 탐색하는 함수입니다.
    void CheckUpVertical(NODE* parentNode, int x, int y, HBRUSH randBrush);

    void CheckDownVertical(NODE* parentNode, int x, int y, HBRUSH randBrush);

    void CheckRightHorizontal(NODE* parentNode, int x, int y, HBRUSH randBrush);

    void CheckLeftHorizontal(NODE* parentNode,  int x, int y, HBRUSH randBrush);
    //==================================================================================================


    //==================================================================================================================
    // 대각선 방향을 탐색하는 함수입니다.
    void CheckRightUp(NODE* parentNode, int x, int y, HBRUSH randBrush ,bool firstCall = true);

    void CheckRightDown(NODE* parentNode, int x, int y, HBRUSH randBrush, bool firstCall = true);

    void CheckLeftUp(NODE* parentNode, int x, int y, HBRUSH randBrush, bool firstCall = true);
    
    void CheckLeftDown(NODE* parentNode,  int x, int y, HBRUSH randBrush, bool firstCall = true);
    //===================================================================================================================



    //===============================================================================================================================
    // 대각선의 직선 탐색 함수입니다.
    bool CheckRightDiagonalHorizontal(NODE* parentNode, NODE_DIRECTION nodeDir,int x, int y, HBRUSH randBrush);
    
    bool CheckLeftDiagonalHorizontal(NODE* parentNode, NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush);
    
    bool CheckUpDiagonalVertical(NODE* parentNode, NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush);
    
    bool CheckDownDiagonalVertical(NODE* parentNode,  NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush);
    //===============================================================================================================================



    void SetCornerNode(NODE* parentNode, NODE_DIRECTION nodeDir, int x, int y);

    void InsertRoute(NODE* node);


    // 루트 최적화
    void PathOptimizing();
 
    void ResetOpenList();

    void ResetCloseList();

    void ResetRouteList();

    void ResetBlock();

    void resetJspMap();

    void RouteReset();

    void OptimizeRouteReset();

    void resetRouteNodeArray(RouteNode* routeNodeArray, int routeNodeArraySize);

    void settingRouteArray(RouteNode* routeNodeArray, int routeNodeArraySize);

    void openListBubbleSort();


    // TODO : mJspMap으로 로직 변경하기
    char **mJspMap;
    
    int mMapWidth;
    int mMapHeight;

    bool mFuncSetFlag;

    NODE* mStartNode;

    NODE* mDestinationNode;

    CList<JumpPointSearch::NODE*> mOpenList;

    CList<JumpPointSearch::NODE*> mCloseList;

    CList<JumpPointSearch::NODE*> mRouteList;

    CList<JumpPointSearch::NODE*> mOptimizeRouteList;
};


