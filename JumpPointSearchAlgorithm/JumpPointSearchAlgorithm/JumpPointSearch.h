#pragma once

// Ÿ���� ���� ���� ����
#define PERMETER_OF_SQUARE 20

// ���� Ÿ�� ����
#define MAX_HEIGHT 40

// ���� Ÿ�� ����
#define MAX_WIDTH 70




// TODO : JSP Ŭ����ȭ�ϱ�
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


    // TODO : private �Լ� �Լ��� ��ȯ�ϱ�  
    NODE* FindOpenList(int openX, int openY);

    bool FindCloseList(int openX, int openY);

    void InsertOpenNode(NODE* node);

    NODE* SelectOpenListNode();

    void CheckDirection(NODE* node, int x, int y);


    // TODO : mMapWidth, mMapHeight ����ϴ� �������� ����
    //===================================================================================================
    // ���� ������ Ž���ϴ� �Լ��Դϴ�.
    void CheckUpVertical(NODE* parentNode, int x, int y, HBRUSH randBrush);

    void CheckDownVertical(NODE* parentNode, int x, int y, HBRUSH randBrush);

    void CheckRightHorizontal(NODE* parentNode, int x, int y, HBRUSH randBrush);

    void CheckLeftHorizontal(NODE* parentNode,  int x, int y, HBRUSH randBrush);
    //==================================================================================================


    //==================================================================================================================
    // �밢�� ������ Ž���ϴ� �Լ��Դϴ�.
    void CheckRightUp(NODE* parentNode, int x, int y, HBRUSH randBrush ,bool firstCall = true);

    void CheckRightDown(NODE* parentNode, int x, int y, HBRUSH randBrush, bool firstCall = true);

    void CheckLeftUp(NODE* parentNode, int x, int y, HBRUSH randBrush, bool firstCall = true);
    
    void CheckLeftDown(NODE* parentNode,  int x, int y, HBRUSH randBrush, bool firstCall = true);
    //===================================================================================================================



    //===============================================================================================================================
    // �밢���� ���� Ž�� �Լ��Դϴ�.
    bool CheckRightDiagonalHorizontal(NODE* parentNode, NODE_DIRECTION nodeDir,int x, int y, HBRUSH randBrush);
    
    bool CheckLeftDiagonalHorizontal(NODE* parentNode, NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush);
    
    bool CheckUpDiagonalVertical(NODE* parentNode, NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush);
    
    bool CheckDownDiagonalVertical(NODE* parentNode,  NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush);
    //===============================================================================================================================



    void SetCornerNode(NODE* parentNode, NODE_DIRECTION nodeDir, int x, int y);

    void InsertRoute(NODE* node);


    // ��Ʈ ����ȭ
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


    // TODO : mJspMap���� ���� �����ϱ�
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


