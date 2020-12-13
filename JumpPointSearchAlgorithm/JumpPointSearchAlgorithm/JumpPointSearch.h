#pragma once


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


    bool PathFind(int startX, int startY, int destinationX, int destinationY, RouteNode *routeNodeArray, int routeNodeArraySize, RouteNode* optimizeNodeArray, int optimizeNodeArraySize);

    void SettingMapAttribute(int posX, int posY);

    void ResetAll(RouteNode* routeNodeArray, int routeNodeArraySize, RouteNode* optimizeNodeArray, int optimizeNodeArraySize);

    void ReStart(RouteNode* routeNodeArray, int routeNodeArraySize, RouteNode* optimizeNodeArray, int optimizeNodeArraySize);
    
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

    // 장애물 확인 enum
    enum class NODE_ATTRIBUTE
    {
        NODE_UNBLOCK,
        NODE_BLOCK
    };


    NODE* findOpenList(int openX, int openY);

    bool findCloseList(int openX, int openY);

    void insertOpenNode(NODE* node);

    NODE* selectOpenListNode();

    void checkDirection(NODE* node, int x, int y);


    //===================================================================================================
    // 직선 방향을 탐색하는 함수입니다.
    void checkUpVertical(NODE* parentNode, int x, int y, HBRUSH randBrush);

    void checkDownVertical(NODE* parentNode, int x, int y, HBRUSH randBrush);

    void checkRightHorizontal(NODE* parentNode, int x, int y, HBRUSH randBrush);

    void checkLeftHorizontal(NODE* parentNode,  int x, int y, HBRUSH randBrush);
    //==================================================================================================


    //==================================================================================================================
    // 대각선 방향을 탐색하는 함수입니다.
    void checkRightUp(NODE* parentNode, int x, int y, HBRUSH randBrush ,bool firstCall = true);

    void checkRightDown(NODE* parentNode, int x, int y, HBRUSH randBrush, bool firstCall = true);

    void checkLeftUp(NODE* parentNode, int x, int y, HBRUSH randBrush, bool firstCall = true);
    
    void checkLeftDown(NODE* parentNode,  int x, int y, HBRUSH randBrush, bool firstCall = true);
    //===================================================================================================================



    //===============================================================================================================================
    // 대각선의 직선 탐색 함수입니다.
    bool checkRightDiagonalHorizontal(NODE* parentNode, NODE_DIRECTION nodeDir,int x, int y, HBRUSH randBrush);
    
    bool checkLeftDiagonalHorizontal(NODE* parentNode, NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush);
    
    bool checkUpDiagonalVertical(NODE* parentNode, NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush);
    
    bool checkDownDiagonalVertical(NODE* parentNode,  NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush);
    //===============================================================================================================================


    void setCornerNode(NODE* parentNode, NODE_DIRECTION nodeDir, int x, int y);
 
    // 노드이 G,H,F 값을 셋팅한다.
    void setNodeGHF(NODE* pParentNode, int x, int y, float* pG, float* pH, float* pF);

    void insertRoute(NODE* node);

    // 루트 최적화
    void pathOptimizing();
 
    // 오픈 리스트 리셋
    void resetOpenList();

    // 클로즈 리스트 리셋
    void resetCloseList();

    // 일반 루트 리스트 리셋
    void resetRouteList();
    
    // 최적화 루트 리스트 리셋
    void resetOptimizeRoute();

    // brush 블럭 리셋
    void resetBlock();

    // jsp 맵 리셋
    void resetJspMap();

    // brush 시작점 장애물 끝점 제외하고 리셋
    void resetRoute();
    

    // 배열 루트 셋팅
    void settingRouteArray(RouteNode* routeNodeArray, int routeNodeArraySize);

    // 루트 배열 리셋
    void resetRouteNodeArray(RouteNode* routeNodeArray, int routeNodeArraySize);


    // TODO : 최적화 배열 셋팅
    void settingOptimizeArray(RouteNode* optimizeNodeArray, int optimizeNodeArraySize);

    // TODO : 최적화 배열 초기화
    void resetOptimizeArray(RouteNode* optimizeNodeArray, int optimizeNodeArraySize);



    // F 값 정렬
    void openListBubbleSort();


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


