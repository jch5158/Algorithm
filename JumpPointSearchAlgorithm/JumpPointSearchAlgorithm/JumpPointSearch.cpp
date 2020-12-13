#include "stdafx.h"
#include "CList.h"
#include "JumpPointSearchAlgorithm.h"
#include "BresenhamLine.h"
#include "JumpPointSearch.h"



//===================================================================
// 타이머로 자동 함수 호출 시 로직 실행이 되지 않도록 하는 Flag
//===================================================================
bool functionFlag = false;



JumpPointSearch::JumpPointSearch(int mapWidth, int mapHeight)
{
	mMapWidth = mapWidth;
	mMapHeight = mapHeight;
	
	mJspMap = (char**)malloc(sizeof(char*) * mMapHeight);

	for (int iCnt = 0; iCnt < mMapHeight; ++iCnt)
	{
		mJspMap[iCnt] = (char*)malloc(sizeof(char*) * mMapWidth);
	}

	mFuncSetFlag = true;	

	for (int iCntY = 0; iCntY < mMapHeight; ++iCntY)
	{
		for (int iCntX = 0; iCntX < mMapWidth; ++iCntX)
		{
			mJspMap[iCntY][iCntX] = (char)NODE_ATTRIBUTE::NODE_UNBLOCK;
		}
	}
}


JumpPointSearch::~JumpPointSearch()
{
	for (int iCntY = 0; iCntY < mMapHeight; ++iCntY)
	{
		free(mJspMap[iCntY]);
	}

	free(mJspMap);
}



void JumpPointSearch::SettingMapAttribute(int posX, int posY)
{

	mJspMap[posY][posX] = (char)NODE_ATTRIBUTE::NODE_BLOCK;

}


void JumpPointSearch::setNodeGHF(NODE* pParentNode, int x, int y, float* pG, float* pH, float* pF)
{		
	if (pParentNode == nullptr)
	{
		*pG = 0.0f;
	}
	else
	{
		// subX 와 subY와 동시에 1 이상 차이날 경우 대각선이다.
		if (abs(pParentNode->mX - x) >= 1 && abs(pParentNode->mY - y) >= 1)
		{
			*pG += 1.5f;
		}
		else
		{
			*pG += 1.0f;
		}
	}
	
	*pH = (float)(abs(mDestinationNode->mX - x) + abs(mDestinationNode->mY - y));

	*pF = *pG + *pH;

	return;
}




bool JumpPointSearch::PathFind(int startX, int startY, int destinationX, int destinationY, RouteNode *routeNodeArray, int routeNodeArraySize, RouteNode* optimizeNodeArray, int optimizeNodeArraySize)
{
	
	// 현재 노드
	static JumpPointSearch::NODE* curNode = nullptr;	 


	// 엔터를 입력하기 전까지 해당 함수를 호출 시 바로 리턴됩니다.
	if (functionFlag == false)
	{
		return false;
	}

	if (mFuncSetFlag == true)
	{
		// 목적지 노드 동적할당
		mDestinationNode = (NODE*)malloc(sizeof(NODE));
		

		// 목적지 노드의 좌표 설정
		mDestinationNode->mX = destinationX;
		mDestinationNode->mY = destinationY;

		// 시작 노드 설정
		mStartNode = (NODE*)malloc(sizeof(NODE));

		// 시작노드 셋팅
		setNodeGHF(nullptr, startX, startY, &mStartNode->G, &mStartNode->H, &mStartNode->F);

		// 시작노드 좌표설정
		mStartNode->mX = startX;
		mStartNode->mY = startY;

		mStartNode->mNodeDir = (BYTE)NODE_DIRECTION::NODE_DIR_ALL;

		mStartNode->prev = nullptr;
		
		// 오픈 리스트에 추가
		mOpenList.PushBack(mStartNode);

		// 한 번만 시작 셋팅되도록 설정
		mFuncSetFlag = false;
	}



	curNode = selectOpenListNode();
	if (curNode->mX == mDestinationNode->mX && curNode->mY == mDestinationNode->mY)
	{
		insertRoute(curNode);

		pathOptimizing();

		settingRouteArray(routeNodeArray, routeNodeArraySize);

		settingOptimizeArray(optimizeNodeArray, optimizeNodeArraySize);

		functionFlag = false;

		return true;
	}
	
	// 길 찾기 로직 시작
	insertOpenNode(curNode);

	// openList F값 정렬
	openListBubbleSort();

	return false;
}

//======================================================
// 좌표로 openList를 찾아서 return 
//======================================================
JumpPointSearch::NODE* JumpPointSearch::findOpenList(int openX, int openY)
{

	CList<NODE*>::Iterator iterE = mOpenList.end();

	for (CList<NODE*>::Iterator iter = mOpenList.begin(); iter != iterE; ++iter)
	{
		if ((*iter)->mX == openX && (*iter)->mY == openY)
		{
			return *iter;
		}
	}

	return nullptr;
}

//======================================================
// 해당 좌표에 closeList가 있다면은 true를 리턴한다.
//======================================================
bool JumpPointSearch::findCloseList(int closeX, int closeY)
{
	CList<NODE*>::Iterator iterE = mCloseList.end();

	for (CList<NODE*>::Iterator iter = mCloseList.begin(); iter != iterE; ++iter)
	{
		if ((*iter)->mX == closeX && (*iter)->mY == closeY)
		{
			return true;
		}
	}

	return false;
}


void JumpPointSearch::insertOpenNode(JumpPointSearch::NODE* node)
{
	if (node == nullptr)
	{
		return;
	}

	int posX = node->mX;
	int posY = node->mY;

	checkDirection(node, posX, posY);

	return;
}


void JumpPointSearch::checkDirection(NODE* node, int x, int y)
{
	NODE* retNode;
	
	HBRUSH randBrush;

	for (;;)
	{
		randBrush = CreateSolidBrush(RGB(rand() % 130 + 126, rand() % 130 + 126, rand() % 130 + 126));

		if (randBrush != grayBrush && randBrush != routeBrush)
		{
			break;
		}
	}

	// 노드 방향에 맞는 탐색함수를 호출한다.
	switch ((NODE_DIRECTION)node->mNodeDir)
	{
	case NODE_DIRECTION::NODE_DIR_RR:

		checkRightHorizontal(node, x, y, randBrush);	

		break;
	case NODE_DIRECTION::NODE_DIR_RD:

		checkRightDown(node, x, y, randBrush);
		
		break;
	case NODE_DIRECTION::NODE_DIR_DD:
		
		checkDownVertical(node, x, y, randBrush);
		
		break;
	case NODE_DIRECTION::NODE_DIR_LD:

		checkLeftDown(node, x, y, randBrush);
		
		break;
	case NODE_DIRECTION::NODE_DIR_LL:
	
		checkLeftHorizontal(node, x, y, randBrush);
		
		break;
	case NODE_DIRECTION::NODE_DIR_LU:

		checkLeftUp(node, x, y, randBrush);
		
		break;
	case NODE_DIRECTION::NODE_DIR_UU:
		
		checkUpVertical(node, x, y, randBrush);
		
		break;
	case NODE_DIRECTION::NODE_DIR_RU:

		checkRightUp(node, x, y,randBrush);	

		break;
	case NODE_DIRECTION::NODE_DIR_ALL:

		checkRightUp(node, x, y, randBrush, false);
	
		checkRightHorizontal(node, x, y, randBrush);
		
		checkRightDown(node, x, y, randBrush ,false);
		
		checkDownVertical(node, x, y, randBrush);
		
		checkLeftDown(node, x, y, randBrush ,false);
		
		checkLeftHorizontal(node, x, y, randBrush);
		
		checkLeftUp(node, x, y, randBrush, false);
		
		checkUpVertical(node, x, y, randBrush);
		
		break;

	default:
		return;
	}

	return;
}


void JumpPointSearch::setCornerNode(NODE* parentNode,  NODE_DIRECTION nodeDir, int x, int y)
{
	NODE* newNode = nullptr;

	NODE* retOpenNode = nullptr;
	
	
	if (findCloseList(x, y) == false)
	{
		float tempG = 0;
		float tempH = 0;
		float tempF = 0;

		setNodeGHF(parentNode, x, y, &tempG, &tempH, &tempF);

		// 오픈리스트에 없는 노드라면은 생성
		retOpenNode = findOpenList(x, y);
		if (retOpenNode == nullptr)
		{
			newNode = (NODE*)malloc(sizeof(NODE));

			newNode->mX = x;

			newNode->mY = y;			

			newNode->G = tempG;

			newNode->H = tempH;

			newNode->F = tempF;

			newNode->mNodeDir = (BYTE)nodeDir;

			newNode->prev = parentNode;

			// 블럭의 색상을 바꿔준다.

			if (x != mDestinationNode->mX || y != mDestinationNode->mY)
			{
				brushBlockList[newNode->mX][newNode->mY] = blueBrush;
			}
		
			mOpenList.PushBack(newNode);
		}
		else
		{			
			// 기존에 오픈리스트에 있었던 노드 부모의 G 값이 parentNode의 G값 보다 크다면
			// parentNode를 부모로서 새로 이어주는 셋팅을 한다.
			if (parentNode->G < retOpenNode->prev->G)
			{				
				retOpenNode->G = tempG;

				retOpenNode->H = tempH;

				retOpenNode->F = tempF;

				retOpenNode->mNodeDir = (BYTE)nodeDir;

				retOpenNode->prev = parentNode;				
			}
		}
	}


	return;
}


//===============================================
// 오른쪽 직선 탐색 함수입니다.
//===============================================
void JumpPointSearch::checkRightHorizontal(NODE* parentNode, int x, int y, HBRUSH randBrush)
{	
	//=================================================================================
	// x 좌표가 x + 1 < mMapWidth 일 경우에만 로직을 수행합니다. 
	//=================================================================================
	if (x + 1 < mMapWidth)
	{
		//=================================================================================
		// 첫 탐색 시 오른쪽 위 보조 탐색 영억을 확인한다.
		//=================================================================================
		if (y > 0)
		{
			if (mJspMap[y - 1][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				checkRightUp(parentNode, x, y, randBrush, false);
			}
		}

		//=================================================================================
		// 첫 탐색 시 오른쪽 아래 보조 탐색 영억을 확인한다.
		//=================================================================================
		if (y + 1 < mMapHeight)
		{
			if (mJspMap[y + 1][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				checkRightDown(parentNode, x, y, randBrush, false);
			}
		}
	}	
	
	//===============================================================================
	// xIndex = x + 1 을 해서 다음 위치부터 탐색을 한다. 
	//===============================================================================
	for (int xIndex = x + 1; xIndex < mMapWidth; ++xIndex)
	{
		// 벽을 만났으 경우 아무것도 하지 않고 return 
		if (mJspMap[y][xIndex] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return;
		}

		// 목적지를 만날 경우 바로 이어주고 return 한다. 
		if (xIndex == mDestinationNode->mX && y == mDestinationNode->mY)
		{
			setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_RR, xIndex, y);

			return;
		}


		// 오픈 리스트 노드 또는 클로즈 리스트 노드에는 칠하지 않는다.
		if (brushBlockList[xIndex][y] != blueBrush && brushBlockList[xIndex][y] != yellowBrush)
		{
			brushBlockList[xIndex][y] = randBrush;
		}


		//=============================================================================
		// x 값이 mMapWidth 보다 작을 경우에만 오버플로우 없이 x+1 위치를 확인할 수 있다.
		//=============================================================================
		if (xIndex + 1 < mMapWidth)
		{		
			//=============================================================================
			// y 값이 1 이상일 경우에만 위의 벽을 확인할 수 있다.
			//=============================================================================
			if (y > 0)
			{
				if (mJspMap[y - 1][xIndex] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][xIndex + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					// 해당 위치에 노드 만들기
					setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_RR, xIndex, y);

					return;
				}
			}

			//============================================================================
			// y 값이 y + 1 < mMapHeight 일 경우에만 위의 벽을 확인할 수 있다.	
			//============================================================================
			if (y + 1 < mMapHeight)
			{
				if (mJspMap[y + 1][xIndex] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][xIndex + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_RR, xIndex, y);

					return;
				}
			}
		}
	}

	return;
}


//===============================================
// 왼쪽 직선 탐색 함수입니다.
//===============================================
void JumpPointSearch::checkLeftHorizontal(NODE* parentNode, int x, int y, HBRUSH randBrush)
{

	if (x > 0)
	{
		//=================================================================================
		// 첫 탐색 시 왼쪽 위 보조 탐색 영역을 확인합니다.
		//=================================================================================
		if (y > 0)
		{
			if (mJspMap[y - 1][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				checkLeftUp(parentNode, x, y, randBrush, false);
			}
		}

		//=================================================================================
		// 첫 탐색 시 왼쪽 아래 보조 탐색 영억을 확인합니다.
		//=================================================================================
		if (y + 1 < mMapHeight)
		{
			if (mJspMap[y + 1][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				checkLeftDown(parentNode, x, y, randBrush, false);
			}
		}
	}	


	for (int xIndex = x - 1; xIndex >= 0; --xIndex)
	{
		// 해당 좌표에 장애물을 만나면 바로 return 한다.
		if (mJspMap[y][xIndex] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return;
		}


		// 목적지를 만나면 이어주고 바로 return 한다.
		if (xIndex == mDestinationNode->mX && y == mDestinationNode->mY)
		{
			setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_LL, xIndex, y);

			return;
		}

		// 오픈 리스트 노드 또는 클로즈 리스트 노드에는 칠하지 않는다.
		if (brushBlockList[xIndex][y] != blueBrush && brushBlockList[xIndex][y] != yellowBrush)
		{
			brushBlockList[xIndex][y] = randBrush;
		}


		if (xIndex > 0)
		{
			if (y > 0)
			{
				if (mJspMap[y - 1][xIndex] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][xIndex - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_LL, xIndex, y);

					return;
				}
			}

			if (y + 1 < mMapHeight)
			{
				if (mJspMap[y + 1][xIndex] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][xIndex - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_LL, xIndex, y);

					return;
				}
			}
		}	
	}

	return;
}


//===============================================
// 수직 위 직선 탐색 함수입니다.
//===============================================
void JumpPointSearch::checkUpVertical(NODE* parentNode, int x, int y, HBRUSH randBrush)
{		
	if (y > 0)
	{
		//=================================================================================
		// 첫 탐색 시 왼쪽 위 보조 탐색 영억을 확인합니다.
		//=================================================================================
		if (x > 0)
		{
			if (mJspMap[y][x - 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				checkLeftUp(parentNode, x, y, randBrush, false);
			}
		}

		//=================================================================================
		// 첫 탐색 시 오른쪽 위 보조 탐색 영억을 확인합니다.
		//=================================================================================
		if (x + 1 < mMapWidth)
		{
			if (mJspMap[y][x + 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				checkRightUp(parentNode, x, y, randBrush, false);
			}
		}
	}
	

	for (int yIndex = y - 1; yIndex >= 0; --yIndex)
	{
		//=============================================
		// 해당 위치에 장애물이 있을 경우 바로 return
		//=============================================
		if (mJspMap[yIndex][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return;
		}


		//===================================================
		// 해당 위치에 목적지가 있을 경우 노드 생성 후 return
		//===================================================
		if (x == mDestinationNode->mX && yIndex == mDestinationNode->mY)
		{
			setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_UU, x, yIndex);

			return;
		}

		// 오픈 리스트 노드 또는 클로즈 리스트 노드에는 칠하지 않는다.
		if (brushBlockList[x][yIndex] != blueBrush && brushBlockList[x][yIndex] != yellowBrush)
		{
			brushBlockList[x][yIndex] = randBrush;
		}


		if (yIndex > 0)
		{
			if (x > 0)
			{
				if (mJspMap[yIndex][x - 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[yIndex - 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_UU, x, yIndex);

					return;
				}
			}


			if (x + 1 < mMapWidth)
			{
				if (mJspMap[yIndex][x + 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[yIndex - 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_UU, x, yIndex);

					return;
				}
			}
		}
	}

	return;
}


//===============================================
// 수직 아래 직선 탐색 함수입니다.
//===============================================
void JumpPointSearch::checkDownVertical(NODE* parentNode, int x, int y, HBRUSH randBrush)
{

	if(y + 1 < mMapHeight)
	{
		//=================================================================================
		// 첫 탐색 시 왼쪽 아래 보조 탐색 영억을 확인합니다.
		//=================================================================================	
		if (x > 0)
		{
			if (mJspMap[y][x - 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				checkLeftDown(parentNode, x, y, randBrush, false);

			}
		}

		//=================================================================================
		// 첫 탐색 시 오른쪽 아래 보조 탐색 영억을 확인합니다.
		//=================================================================================
		if (x + 1 < mMapWidth)
		{
			if (mJspMap[y][x + 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				checkRightDown(parentNode, x, y, randBrush, false);

			}
		}
	}	


	for (int yIndex = y + 1; yIndex < mMapHeight; ++yIndex)
	{
		// 장애물을 마주칠 경우 return 한다.
		if (mJspMap[yIndex][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return;
		}


		// 목적지를 만났을 경우 목적지를 이어주고 return 한다.
		if (x == mDestinationNode->mX && yIndex == mDestinationNode->mY)
		{
			setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_DD, x, yIndex);

			return;
		}

		// 오픈 리스트 노드 또는 클로즈 리스트 노드에는 칠하지 않는다.
		if (brushBlockList[x][yIndex] != blueBrush && brushBlockList[x][yIndex] != yellowBrush)
		{
			brushBlockList[x][yIndex] = randBrush;
		}

		if (x > 0 && yIndex + 1 < mMapHeight )
		{
			if (mJspMap[yIndex][x - 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[yIndex + 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_DD, x, yIndex);

				return;
			}
		}

		if (x + 1 < mMapWidth && yIndex + 1 < mMapHeight)
		{
			if (mJspMap[yIndex][x + 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[yIndex + 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_DD, x, yIndex);

				return;
			}
		}		
	}

	return;
}



//======================================================
// 오른쪽 위 대각선 함수입니다. 
//======================================================
void JumpPointSearch::checkRightUp(NODE* parentNode, int x, int y, HBRUSH randBrush ,bool firstCall)
{		
	//===============================================
	// 대각선 함수 호출 시 직선 함수 호출 여부
	//===============================================
	if (firstCall)
	{	
		//===================================================================
		// 직선 조건에 맞으면 5방향으로 탐색하게 된다.
		checkRightHorizontal(parentNode, x, y, randBrush);

		checkUpVertical(parentNode, x, y, randBrush);		
		//===================================================================
	}

	for(;;)
	{
		x += 1;

		y -= 1;

		// 배열 범위를 벗어날 경우 호출 안함 
		if (x >= mMapWidth || y < 0)
		{
			break;
		}

		// 장애물을 만났을 경우 호출 안함
		if (mJspMap[y][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			break;
		}


		// 목적지를 만났으면 목적지에서 이어주고 return 한다.
		if (mDestinationNode->mX == x && mDestinationNode->mY == y)
		{			
			setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_RU, x, y);

			return;
		}


		// TODO : 밑에 두 개의 함수 하나로 만들기 
		if (checkRightDiagonalHorizontal(parentNode, NODE_DIRECTION::NODE_DIR_RU, x, y, randBrush) == true)
		{
			return;
		}

		if (checkUpDiagonalVertical(parentNode, NODE_DIRECTION::NODE_DIR_RU, x, y, randBrush) == true)
		{
			return;
		}
	}

	return;
}


//======================================================
// 오른쪽 아래 대각선 함수입니다. 
//======================================================
void JumpPointSearch::checkRightDown(NODE* parentNode, int x, int y, HBRUSH randBrush, bool firstCall)
{		
	if (firstCall)
	{
		//===================================================================
		// 직선 조건에 맞으면 5방향으로 탐색하게 된다.
		checkRightHorizontal(parentNode, x, y, randBrush);
		
		checkDownVertical(parentNode, x, y, randBrush);	
		//===================================================================
	}

	for(;;)
	{
		x += 1;

		y += 1;

		if (x >= mMapWidth || y >= mMapHeight)
		{
			break;
		}

		if (mJspMap[y][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			break;
		}

		if (mDestinationNode->mX == x && mDestinationNode->mY == y)
		{			
			setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_RD, x, y);

			return;
		}

		if (checkRightDiagonalHorizontal(parentNode, NODE_DIRECTION::NODE_DIR_RD, x, y, randBrush) == true)
		{
			return;
		}

		if (checkDownDiagonalVertical(parentNode, NODE_DIRECTION::NODE_DIR_RD, x, y, randBrush) == true)
		{
			return;
		}
	}

	return;
}


//======================================================
// 왼쪽 위 대각선 함수입니다. 
//======================================================
void JumpPointSearch::checkLeftUp(NODE* parentNode, int x, int y, HBRUSH randBrush ,bool firstCall)
{
	if (firstCall)
	{
		//===================================================================
		// 직선 조건에 맞으면 5방향으로 탐색하게 된다.
		checkLeftHorizontal(parentNode, x, y, randBrush);	

		checkUpVertical(parentNode, x, y, randBrush);		
		//===================================================================
	}

	for(;;)
	{
		x -= 1;
		y -= 1;


		if (x < 0 || y < 0)
		{
			break;
		}

		if (mJspMap[y][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			break;
		}

		if (mDestinationNode->mX == x && mDestinationNode->mY == y)
		{
			
			setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_LU, x, y);

			return;
		}

		if (checkLeftDiagonalHorizontal(parentNode, NODE_DIRECTION::NODE_DIR_LU, x, y, randBrush) == true)
		{
			return;
		}

		if (checkUpDiagonalVertical(parentNode, NODE_DIRECTION::NODE_DIR_LU, x, y, randBrush) == true)
		{
			return;
		}
	}

	return;
}


//======================================================
// 왼쪽 아래 대각선 함수입니다. 
//======================================================
void JumpPointSearch::checkLeftDown(NODE* parentNode, int x, int y, HBRUSH randBrush, bool firstCall)
{
	if (firstCall == true)
	{
		//===================================================================
		// 직선 조건에 맞으면 5방향으로 탐색하게 된다.
		checkLeftHorizontal(parentNode, x, y, randBrush);
		

		checkDownVertical(parentNode,  x, y, randBrush);		
		//===================================================================
	}


	for(;;)
	{
		x -= 1;
		y += 1;

		if (x < 0 || y >= mMapHeight)
		{
			break;
		}

		if (mJspMap[y][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			break;
		}

		if (mDestinationNode->mX == x && mDestinationNode->mY == y)
		{
			setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_LD, x, y);

			return;
		}

		if (checkLeftDiagonalHorizontal(parentNode, NODE_DIRECTION::NODE_DIR_LD, x, y, randBrush) == true)
		{
			return;
		}

		if (checkDownDiagonalVertical(parentNode, NODE_DIRECTION::NODE_DIR_LD, x, y, randBrush) == true)
		{
			return;
		}
	}
}



//=======================================================
// 대각선 함수의 수평 오른쪽 방향 
//=======================================================
bool JumpPointSearch::checkRightDiagonalHorizontal(NODE* parentNode, NODE_DIRECTION nodeDir,int x, int y, HBRUSH randBrush)
{

	for (int xIndex = x; xIndex < mMapWidth; ++xIndex)
	{
		if (mJspMap[y][xIndex] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return false;
		}
	
		// 목적지 노드를 만났을 경우에는 그 자리에 노드 생성후 return 
		if (xIndex == mDestinationNode->mX && y == mDestinationNode->mY)
		{
			setCornerNode(parentNode, nodeDir, x, y);
			
			return true;
		}

		// 오픈 리스트 노드 또는 클로즈 리스트 노드에는 칠하지 않는다.
		if (brushBlockList[xIndex][y] != blueBrush && brushBlockList[xIndex][y] != yellowBrush)
		{
			brushBlockList[xIndex][y] = randBrush;
		}

		if (xIndex + 1 < mMapWidth)
		{
			if (y > 0)
			{
				if (mJspMap[y - 1][xIndex] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][xIndex + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, nodeDir, x, y);

					return true;
				}
			}

			if (y + 1 < mMapHeight)
			{
				if (mJspMap[y + 1][xIndex] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][xIndex + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, nodeDir, x, y);

					return true;
				}
			}
		}	
	}

	return false;
}


//=======================================================
// 대각선 함수의 수평 왼쪽 방향
//=======================================================
bool JumpPointSearch::checkLeftDiagonalHorizontal(NODE* parentNode, NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush)
{

	for (int xIndex = x; xIndex >= 0; --xIndex)
	{
		if (mJspMap[y][xIndex] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return false;
		}		

		//=============================================
		// 목적지를 만났을 경우 return
		//=============================================
		if (xIndex == mDestinationNode->mX && y == mDestinationNode->mY)
		{
			setCornerNode(parentNode, nodeDir, x, y);

			return true;
		}

		// 오픈 리스트 노드 또는 클로즈 리스트 노드에는 칠하지 않는다.
		if (brushBlockList[xIndex][y] != blueBrush && brushBlockList[xIndex][y] != yellowBrush)
		{
			brushBlockList[xIndex][y] = randBrush;
		}

		
		if (xIndex > 0)
		{
			if (y > 0)
			{
				if (mJspMap[y - 1][xIndex] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][xIndex - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, nodeDir, x, y);

					return true;
				}
			}

			if (y + 1 < mMapHeight)
			{
				if (mJspMap[y + 1][xIndex] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][xIndex - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, nodeDir, x, y);

					return true;
				}
			}
		}
	}

	return false;
}


//=======================================================
// 대각선 함수의 수직 위 방향
//=======================================================
bool JumpPointSearch::checkUpDiagonalVertical(NODE* parentNode, NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush)
{
	
	for (int yIndex = y; yIndex >= 0; --yIndex)
	{
		// 장애물 만났을 경우 return
		if (mJspMap[yIndex][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return false;
		}
		
		//=================================================
		// 목적지 만났을 경우 노드 생성 후 return
		//=================================================
		if (x == mDestinationNode->mX && yIndex == mDestinationNode->mY)
		{
			setCornerNode(parentNode, nodeDir, x, y);

			return true;
		}


		// 오픈 리스트 노드 또는 클로즈 리스트 노드에는 칠하지 않는다.
		if (brushBlockList[x][yIndex] != blueBrush && brushBlockList[x][yIndex] != yellowBrush)
		{
			brushBlockList[x][yIndex] = randBrush;
		}

		if (yIndex > 0)
		{
			if (x > 0)
			{
				if (mJspMap[yIndex][x - 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[yIndex - 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, nodeDir, x, y);

					return true;
				}
			}

			if (x + 1 < mMapWidth)
			{
				if (mJspMap[yIndex][x + 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[yIndex - 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, nodeDir, x, y);

					return true;
				}
			}
		}
	}

	return false;
}


//=======================================================
// 대각선 함수의 수직 아래 방향
//=======================================================
bool JumpPointSearch::checkDownDiagonalVertical(NODE* parentNode, NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush)
{

	for (int yIndex = y; yIndex < mMapHeight; yIndex++)
	{
		if (mJspMap[yIndex][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return false;
		}	

		if (x == mDestinationNode->mX && yIndex == mDestinationNode->mY)
		{
			setCornerNode(parentNode, nodeDir, x, y);

			return true;
		}

		// 오픈 리스트 노드 또는 클로즈 리스트 노드에는 칠하지 않는다.
		if (brushBlockList[x][yIndex] != blueBrush && brushBlockList[x][yIndex] != yellowBrush)
		{
			brushBlockList[x][yIndex] = randBrush;
		}

		if (yIndex + 1 < mMapHeight)
		{
			if (x > 0)
			{
				if (mJspMap[yIndex][x - 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[yIndex + 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, nodeDir, x, y);

					return true;
				}
			}
		
			if (x + 1 < mMapWidth)
			{
				if (mJspMap[yIndex][x + 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[yIndex + 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, nodeDir, x, y);

					return true;
				}
			}
		}		
	}

	return false;
}




//================================================================
// 오픈 리스트에서 정렬되어 있던 F값 노드를 뽑는다.
//================================================================
JumpPointSearch::NODE* JumpPointSearch::selectOpenListNode()
{
	// F값이 작은 순서로 정렬해놨기 때문에 begin값을 뽑으면 첫 노드를 뽑을 수 있다.
	CList<JumpPointSearch::NODE*>::Iterator iter = mOpenList.begin();

	if (*iter == nullptr)
	{
		return nullptr;
	}

	// 노드의 값을 복사한다.
	NODE* node = *iter;

	// 오픈 리스트에서 해당 노드를 지운다.
	mOpenList.erase(iter);

	// closeList에 넣어준다.
	mCloseList.PushFront(node);

	// 클로즈리스트이니 색깔을 바꾸어준다.

	if ((mStartNode->mX != node->mX || mStartNode->mY != node->mY) && (mDestinationNode->mX != node->mX || mDestinationNode->mY != node->mY) )
	{
		brushBlockList[node->mX][node->mY] = yellowBrush;
	}

	return node;
}


//=============================================================
// 모든 리스트와 블럭을 초기화 시킨다.
//=============================================================
void JumpPointSearch::ResetAll(RouteNode* routeNodeArray, int routeNodeArraySize, RouteNode* optimizeNodeArray, int optimizeNodeArraySize)
 {	
	if (mFuncSetFlag == false)
	{
		free(mStartNode);

 		free(mDestinationNode);
	}

	functionFlag = false;

	mFuncSetFlag = true;

	resetOpenList();

	resetCloseList();

	resetRouteList();

	resetRouteNodeArray(routeNodeArray, routeNodeArraySize);

	resetOptimizeArray(optimizeNodeArray, optimizeNodeArraySize);

	resetOptimizeRoute();

	resetJspMap();

	resetBlock();
}



//==================================================================
// 시작점과 도착점을 제외한 리스트와 블럭을 초기화하고 다시 길을 찾는다.
//==================================================================
void JumpPointSearch::ReStart(RouteNode* routeNodeArray, int routeNodeArraySize, RouteNode* optimizeNodeArray, int optimizeNodeArraySize)
{
	if (mFuncSetFlag == false)
	{
		free(mStartNode);

		free(mDestinationNode);
	}

	functionFlag = true;

	mFuncSetFlag = true;

	resetOpenList();

	resetCloseList();

	resetRouteList();

	resetJspMap();

	resetRoute();

	resetRouteNodeArray(routeNodeArray, routeNodeArraySize);

	resetOptimizeArray(optimizeNodeArray, optimizeNodeArraySize);

	resetOptimizeRoute();
}


//=============================================================
// 오픈리스트 노드를 리셋한다.
//=============================================================
void JumpPointSearch::resetOpenList()
{
	CList<JumpPointSearch::NODE*>::Iterator iterE = mOpenList.end();

	for (CList<JumpPointSearch::NODE*>::Iterator iter = mOpenList.begin(); iter != iterE;)
	{
		iter = mOpenList.erase(iter);
	}
}



//=============================================================
// 클로즈리스트 노드를 리셋한다.
//=============================================================
void JumpPointSearch::resetCloseList()
{
	CList<JumpPointSearch::NODE*>::Iterator iterE = mCloseList.end();

	for (CList<JumpPointSearch::NODE*>::Iterator iter = mCloseList.begin(); iter != iterE;)
	{
		iter = mCloseList.erase(iter);
	}
}



//=============================================================
// 시작점에서 목적지까지의 List를 reset한다.
//=============================================================
void JumpPointSearch::resetRouteList()
{
	CList<JumpPointSearch::NODE*>::Iterator iterE = mRouteList.end();

	for (CList<JumpPointSearch::NODE*>::Iterator iter = mRouteList.begin(); iter != iterE;)
	{
		iter = mRouteList.erase(iter);
	}
}


//=============================================================
// 자연스러운 시작점에서 목적지까지의 경로를 reset한다.
//=============================================================
void JumpPointSearch::resetOptimizeRoute()
{
	CList<JumpPointSearch::NODE*>::Iterator iterE = mOptimizeRouteList.end();

	for (CList<JumpPointSearch::NODE*>::Iterator iter = mOptimizeRouteList.begin(); iter != iterE;)
	{
		iter = mOptimizeRouteList.erase(iter);
	}
}



//=============================================================
// 블럭들을 리셋한다.
//=============================================================
void JumpPointSearch::resetBlock()
{
	for (int iCntY = 0; iCntY < mMapHeight; iCntY++)
	{
		for (int iCntX = 0; iCntX < mMapWidth; iCntX++)
		{
			brushBlockList[iCntX][iCntY] = oldBrush;
		}
	}
}



void JumpPointSearch::resetJspMap()
{
	for (int iCntY = 0; iCntY < mMapHeight; ++iCntY)
	{
		for (int iCntX = 0; iCntX < mMapWidth; ++iCntX)
		{
			mJspMap[iCntY][iCntX] = (char)NODE_ATTRIBUTE::NODE_UNBLOCK;
		}
	}
}


//=====================================================================
// 시작점과 목적지 장애물을 제외하고 블럭 색깔을 리셋한다.
//======================================================================
void JumpPointSearch::resetRoute()
{
	for (int iCntY = 0; iCntY < mMapHeight; iCntY++)
	{
		for (int iCntX = 0; iCntX < mMapWidth; iCntX++)
		{
			if (brushBlockList[iCntX][iCntY] != redBrush && brushBlockList[iCntX][iCntY] != greenBrush && brushBlockList[iCntX][iCntY] != grayBrush)
			{
				brushBlockList[iCntX][iCntY] = oldBrush;
			}
		}
	}
}

//==================================================================
// 정방향으로 목적지까지의 경로를 넣는다.
//==================================================================
void JumpPointSearch::insertRoute(NODE* node)
{
	for(;;)
	{	
		mRouteList.PushFront(node);

		mOptimizeRouteList.PushFront(node);

		if (node->prev == nullptr)
		{
			break;
		}

		node = node->prev;
	}
}

//====================================================
// 최적화 길을 설정하는 함수입니다.
//====================================================
void JumpPointSearch::pathOptimizing()
{

	int startX;
	int startY;

	int endX;
	int endY;

	int nextEndX;
	int nextEndY;

	// 기준 노드의 다음 노드입니다.
	CList<JumpPointSearch::NODE*>::Iterator nextIter;

	// 기준 노드의 다음 다음 노드입니다.
	CList<JumpPointSearch::NODE*>::Iterator nextNextIter;

	CList<JumpPointSearch::NODE*>::Iterator iterE = mOptimizeRouteList.end();

	CList<JumpPointSearch::NODE*>::Iterator iter = mOptimizeRouteList.begin();

	for(;;)
	{
		startX = (*iter)->mX;
		startY = (*iter)->mY;

		nextIter = iter.GetNextIter();
		//nextIter = ++iter;
		
		//========================================================================
		// iter의 다음 노드가 마지막 노드라면은 break 로직을 종료한다.
		//=========================================================================
		if (nextIter == iterE)
		{
			break;
		}
	
		nextNextIter = nextIter;

		for(;;)
		{
			nextNextIter = nextNextIter.GetNextIter();
			//nextNextIter = ++nextNextIter;


			//=====================================================================
			// nextNextIter 노드가 끝 노드라면은 그전 노드를 기준노드에 전달한다.
			//======================================================================
			if (nextNextIter == iterE)
			{
				--nextNextIter;

				iter = nextIter;
				break;
			}

			nextEndX = nextNextIter->mX;
			nextEndY = nextNextIter->mY;

			//======================================================================
			// 라인이 이어진다면은 그전 노드는 삭제할 노드로 체크한다.
			//======================================================================
			if (BresenhamLine::MakeLine(startX, startY, nextEndX, nextEndY))
			{				
				//nextIter.deleteCheck = true;

				nextIter.SetDeleteFlag();
			}
			else
			{
				//=====================================================================================
				// 장애물에 의해서 이어질 수 없는 노드라면은 nextNextIter의 그전 노드를 기준으로 바꾼다.
				//=====================================================================================
				--nextNextIter;

				iter = nextNextIter;

				break;
			}
		}
	}


	// 삭제 체크한 노드를 삭제합니다.
	for (iter = mOptimizeRouteList.begin(); iter != iterE;)
	{
		if (iter.GetDeleteFlag())
		{
			iter = mOptimizeRouteList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}



void JumpPointSearch::settingRouteArray(RouteNode* routeNodeArray, int routeArraySize)
{
	int index = 0;

	CList<NODE*>::Iterator iterE = mRouteList.end();

	for (CList<NODE*>::Iterator iter = mRouteList.begin(); iter != iterE; ++iter)
	{
		routeNodeArray[index].mPosX = iter->mX;

		routeNodeArray[index].mPosY = iter->mY;

		routeNodeArray[index].mRouteFlag = true;

		index += 1;
		
		if (index >= routeArraySize)
		{
			return;
		}

	}
}

void JumpPointSearch::resetRouteNodeArray(RouteNode* routeNodeArray, int routeNodeArraySize)
{
	for (int iCnt = 0; iCnt < routeNodeArraySize; ++iCnt)
	{
		routeNodeArray[iCnt].mRouteFlag = false;
	}
}


void JumpPointSearch::settingOptimizeArray(RouteNode* optimizeNodeArray, int optimizeNodeArraySize)
{
	int index = 0;

	CList<NODE*>::Iterator iterE = mOptimizeRouteList.end();

	for (CList<NODE*>::Iterator iter = mOptimizeRouteList.begin(); iter != iterE; ++iter)
	{
		optimizeNodeArray[index].mPosX = iter->mX;

		optimizeNodeArray[index].mPosY = iter->mY;

		optimizeNodeArray[index].mRouteFlag = true;

		index += 1;

		if (index >= optimizeNodeArraySize)
		{
			return;
		}

	}
}


void JumpPointSearch::resetOptimizeArray(RouteNode* optimizeNodeArray, int optimizeNodeArraySize)
{
	for (int iCnt = 0; iCnt < optimizeNodeArraySize; ++iCnt)
	{
		optimizeNodeArray[iCnt].mRouteFlag = false;
	}
}


//=============================================================
// F값 작은 순으로 정렬한다.
//=============================================================
void JumpPointSearch::openListBubbleSort() {

	CList<JumpPointSearch::NODE*>::Iterator iterE = mOpenList.end();

	--iterE;

	int listSize = mOpenList.GetUseSize();

	for (int iCnt = 0; iCnt < listSize; iCnt++)
	{
		for (CList<JumpPointSearch::NODE*>::Iterator iter = mOpenList.begin(); iter != iterE; ++iter)
		{
			CList<JumpPointSearch::NODE*>::Iterator iterN = iter.GetNextIter();

			if (iter->F > iterN->F)
			{
				mOpenList.DataSwap(iter, iterN);
			}
		}

		--iterE;
	}
}