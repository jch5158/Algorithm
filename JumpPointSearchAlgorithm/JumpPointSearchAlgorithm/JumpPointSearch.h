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

        // �� �̵� Ƚ��
        float G;

        // ������������ ���� ����
        float H;

        // G+H ��
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

    // ��ֹ� Ȯ�� enum
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
    // ���� ������ Ž���ϴ� �Լ��Դϴ�.
    void checkUpVertical(NODE* parentNode, int x, int y, HBRUSH randBrush);

    void checkDownVertical(NODE* parentNode, int x, int y, HBRUSH randBrush);

    void checkRightHorizontal(NODE* parentNode, int x, int y, HBRUSH randBrush);

    void checkLeftHorizontal(NODE* parentNode,  int x, int y, HBRUSH randBrush);
    //==================================================================================================


    //==================================================================================================================
    // �밢�� ������ Ž���ϴ� �Լ��Դϴ�.
    void checkRightUp(NODE* parentNode, int x, int y, HBRUSH randBrush ,bool firstCall = true);

    void checkRightDown(NODE* parentNode, int x, int y, HBRUSH randBrush, bool firstCall = true);

    void checkLeftUp(NODE* parentNode, int x, int y, HBRUSH randBrush, bool firstCall = true);
    
    void checkLeftDown(NODE* parentNode,  int x, int y, HBRUSH randBrush, bool firstCall = true);
    //===================================================================================================================



    //===============================================================================================================================
    // �밢���� ���� Ž�� �Լ��Դϴ�.
    bool checkRightDiagonalHorizontal(NODE* parentNode, NODE_DIRECTION nodeDir,int x, int y, HBRUSH randBrush);
    
    bool checkLeftDiagonalHorizontal(NODE* parentNode, NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush);
    
    bool checkUpDiagonalVertical(NODE* parentNode, NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush);
    
    bool checkDownDiagonalVertical(NODE* parentNode,  NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush);
    //===============================================================================================================================


    void setCornerNode(NODE* parentNode, NODE_DIRECTION nodeDir, int x, int y);
 
    // ����� G,H,F ���� �����Ѵ�.
    void setNodeGHF(NODE* pParentNode, int x, int y, float* pG, float* pH, float* pF);

    void insertRoute(NODE* node);

    // ��Ʈ ����ȭ
    void pathOptimizing();
 
    // ���� ����Ʈ ����
    void resetOpenList();

    // Ŭ���� ����Ʈ ����
    void resetCloseList();

    // �Ϲ� ��Ʈ ����Ʈ ����
    void resetRouteList();
    
    // ����ȭ ��Ʈ ����Ʈ ����
    void resetOptimizeRoute();

    // brush �� ����
    void resetBlock();

    // jsp �� ����
    void resetJspMap();

    // brush ������ ��ֹ� ���� �����ϰ� ����
    void resetRoute();
    

    // �迭 ��Ʈ ����
    void settingRouteArray(RouteNode* routeNodeArray, int routeNodeArraySize);

    // ��Ʈ �迭 ����
    void resetRouteNodeArray(RouteNode* routeNodeArray, int routeNodeArraySize);


    // TODO : ����ȭ �迭 ����
    void settingOptimizeArray(RouteNode* optimizeNodeArray, int optimizeNodeArraySize);

    // TODO : ����ȭ �迭 �ʱ�ȭ
    void resetOptimizeArray(RouteNode* optimizeNodeArray, int optimizeNodeArraySize);



    // F �� ����
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


