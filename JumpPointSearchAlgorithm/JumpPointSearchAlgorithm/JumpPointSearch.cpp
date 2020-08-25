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



void JumpPointSearch::SettingMapAttrivute(int posX, int posY)
{

	mJspMap[posY][posX] = (char)NODE_ATTRIBUTE::NODE_BLOCK;

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

		// 시작노드 좌표설정
		mStartNode->mX = startX;
		mStartNode->mY = startY;

		mStartNode->mNodeDir = (BYTE)NODE_DIRECTION::NODE_DIR_ALL;

		mStartNode->prev = nullptr;

		// 시작노드 G 설정
		mStartNode->G = 0;

		// 목적지까지의 절대값	
		mStartNode->H += (float)(abs(destinationX - startX) + abs(destinationY - startY));

		mStartNode->F = mStartNode->G + mStartNode->H;

		// 클로즈 리스트에 추가
		mOpenList.PushBack(mStartNode);

		// 한번만 시작 셋팅되도록 설정
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
		if (iter->mX == openX && iter->mY == openY)
		{
			return (*iter)->data;
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
		if (iter->mX == closeX && iter->mY == closeY)
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

	int newNodePosX = 0;
	int newNodePosY = 0;

	checkDirection(node, posX, posY);

	return;
}


void JumpPointSearch::checkDirection(NODE* node, int x, int y)
{
	NODE* retNode;
	
	HBRUSH randBrush;

	do
	{
		randBrush = CreateSolidBrush(RGB(rand() % 130 + 126, rand() % 130 + 126, rand() % 130 + 126));

		if (randBrush == grayBrush || randBrush == routeBrush)
		{
			continue;
		}

	} while (0);


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

	NODE* retOpenNode;

	int subX;
	int subY;

	if (findCloseList(x, y) == false)
	{
		// 오픈리스트에 없는 노드라면은 생성
		retOpenNode = findOpenList(x, y);
		if (retOpenNode == nullptr)
		{
			newNode = (NODE*)malloc(sizeof(NODE));

			newNode->mX = x;
			newNode->mY = y;
		
			newNode->G = parentNode->G;

			// 부모 노드와 자식 노드의 x,y 차이 값 저장
			subX = abs(parentNode->mX - newNode->mX);
			subY = abs(parentNode->mY - newNode->mY);

			// subX 와 subY와 동시에 1 이상 차이날 경우 대각선이다.
			if (subX >= 1 && subY >= 1)
			{
				newNode->G += ((float)subX) * 1.5f;
			}
			else
			{
				if (subX == 0)
				{
					newNode->G += ((float)subY) * 1.0f;
				}
				else
				{
					newNode->G += ((float)subX) * 1.0f;
				}
			}

			newNode->H = (float)(abs(mDestinationNode->mX - newNode->mX) + abs(mDestinationNode->mY - newNode->mY));

			newNode->F = newNode->G + newNode->H;

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
			// 기존 오픈리스트에 있었던 위치라면, 해당 노드의 부모의 G값을 비교하여 새로 연결해준다.
			if (retOpenNode->prev->G < parentNode->G)
			{

				retOpenNode->G = parentNode->G;

				// 부모 노드와 자식 노드의 x,y 차이 값 저장
				subX = abs(parentNode->mX - retOpenNode->mX);
				subY = abs(parentNode->mY - retOpenNode->mY);

				// subX 와 subY와 동시에 1 이상 차이날 경우 대각선이다.
				if (subX >= 1 && subY >= 1)
				{
					retOpenNode->G += ((float)subX) * 1.5f;
				}
				else
				{
					if (subX == 0)
					{
						retOpenNode->G += ((float)subY) * 1.0f;
					}
					else
					{
						retOpenNode->G += ((float)subX) * 1.0f;
					}
				}

				retOpenNode->H = (float)(abs(mDestinationNode->mX - retOpenNode->mX) + abs(mDestinationNode->mY - retOpenNode->mY));

				retOpenNode->F = retOpenNode->G + retOpenNode->H;

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
	// 오른쪽 위 보조 탐색 영억을 생성하는
	//=================================================================================
	if (y > 0 && x + 1 < mMapWidth)
	{
		if (mJspMap[y - 1][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
		{	
			checkRightUp(parentNode, x, y, randBrush, false);				
		}
	}

	//=================================================================================
	// 오른쪽 아래 보조 탐색 영억을 생성하는
	//=================================================================================
	if (y + 1 < mMapHeight && x + 1 < mMapWidth)
	{
		if (mJspMap[y + 1][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK )
		{
			checkRightDown(parentNode, x, y, randBrush, false);	
		}
	}
	
	//============================================================================
	// iCnt = x + 1 을 하는 이유는 해당 위치는 현재 노드가 있는 위치이기 때문이다. 
	// iCnt + 1 < mMapWidth 인 이유는 위와 아래의 벽이 있을 경우 X + 1 좌표에 
	// 벽이 없는지 확인하기 때문으로 오버플로우가 발생되지 않도록 하기 위함입니다.
	//============================================================================
	for (int iCnt = x + 1; iCnt < mMapWidth; ++iCnt)
	{
		// 벽을 만났으 경우 retur nullptr 을 한다.
		if (mJspMap[y][iCnt] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return;
		}

		// 목적지를 만날 경우 바로 이어주고 return 한다. 
		if (iCnt == mDestinationNode->mX && y == mDestinationNode->mY)
		{
			setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_RR, iCnt, y);

			return;
		}


		// 오픈 리스트 노드 또는 클로즈 리스트 노드에는 칠하지 않는다.
		if (brushBlockList[iCnt][y] != blueBrush && brushBlockList[iCnt][y] != yellowBrush)
		{
			brushBlockList[iCnt][y] = randBrush;
		}


		//====================================================================
		// y 값이 0 이상일 경우에만 위의 벽을 확인할 수 있다.
		//====================================================================
		if (y > 0 && iCnt + 1 < mMapWidth )
		{
			if (mJspMap[y - 1][iCnt] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][iCnt + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_RR, iCnt, y);
				
				return;
			}
		}


		//====================================================================
		// y 값이 y + 1 < mMapHeight 일 경우에만 위의 벽을 확인할 수 있다.
		//====================================================================
		if (y + 1 < mMapHeight && iCnt + 1 < mMapWidth)
		{
			if (mJspMap[y + 1][iCnt] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][iCnt + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_RR, iCnt, y);
			
				return;
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

	//=================================================================================
	// 왼쪽 위 보조 탐색 영억을 생성하는
	//=================================================================================
	if (y > 0 && x > 0)
	{
		if (mJspMap[y - 1][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
		{		
			checkLeftUp(parentNode, x, y, randBrush, false);			
		}
	}

	//=================================================================================
	// 왼쪽 아래 보조 탐색 영억을 생성하는
	//=================================================================================
	if (y + 1 < mMapHeight && x > 0)
	{
		if (mJspMap[y + 1][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK )
		{
			checkLeftDown(parentNode, x, y, randBrush, false);		
		}
	}

	for (int iCnt = x - 1; iCnt >= 0; --iCnt)
	{
		// 해당 좌표에 장애물을 만나면 바로 return 한다.
		if (mJspMap[y][iCnt] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return;
		}


		// 목적지를 만나면 이어주고 바로 return 한다.
		if (iCnt == mDestinationNode->mX && y == mDestinationNode->mY)
		{
			setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_LL, iCnt, y);

			return;
		}

		// 오픈 리스트 노드 또는 클로즈 리스트 노드에는 칠하지 않는다.
		if (brushBlockList[iCnt][y] != blueBrush && brushBlockList[iCnt][y] != yellowBrush)
		{
			brushBlockList[iCnt][y] = randBrush;
		}

		if (y > 0 && iCnt - 1 >= 0)
		{
			if (mJspMap[y - 1][iCnt] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][iCnt - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_LL, iCnt, y);

				return;
			}
		}


		if (y + 1 < mMapHeight && iCnt - 1 >= 0)
		{
			if (mJspMap[y + 1][iCnt] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][iCnt - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_LL, iCnt, y);

				return;
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
	
	//=================================================================================
	// 왼쪽 위 보조 탐색 영억을 생성하는
	//=================================================================================
	if (x > 0 && y > 0)
	{
		if (mJspMap[y][x - 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
		{
			checkLeftUp(parentNode, x, y, randBrush, false);	
		}
	}

	//=================================================================================
	// 오른쪽 위 보조 탐색 영억을 생성하는
	//=================================================================================
	if (x + 1 < mMapWidth && y > 0)
	{
		if (mJspMap[y][x + 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
		{ 	
			checkRightUp(parentNode, x, y, randBrush, false);	
		}
	}

	for (int iCnt = y - 1; iCnt >= 0; --iCnt)
	{
		//=============================================
		// 해당 위치에 장애물이 있을 경우 바로 return
		//=============================================
		if (mJspMap[iCnt][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return;
		}


		//===================================================
		// 해당 위치에 목적지가 있을 경우 노드 생성 후 return
		//===================================================
		if (x == mDestinationNode->mX && iCnt == mDestinationNode->mY)
		{
			setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_UU, x, iCnt);

			return;
		}

		// 오픈 리스트 노드 또는 클로즈 리스트 노드에는 칠하지 않는다.
		if (brushBlockList[x][iCnt] != blueBrush && brushBlockList[x][iCnt] != yellowBrush)
		{
			brushBlockList[x][iCnt] = randBrush;
		}


		if (x > 0 && iCnt - 1 >= 0)
		{
			if (mJspMap[iCnt][x - 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[iCnt - 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_UU, x, iCnt);

				return;
			}
		}


		if (x + 1 < mMapWidth && iCnt - 1 >= 0)
		{
			if (mJspMap[iCnt][x + 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[iCnt - 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK )
			{
				setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_UU, x, iCnt);

				return;
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

	//=================================================================================
	// 왼쪽 아래 보조 탐색 영억을 생성하는
	//=================================================================================
	if (x > 0 && y + 1 < mMapHeight)
	{
		if (mJspMap[y][x - 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK )
		{
			checkLeftDown(parentNode, x, y, randBrush, false);
			
		}
	}

	//=================================================================================
	// 오른쪽 아래 보조 탐색 영억을 생성하는
	//=================================================================================
	if (x + 1 < mMapWidth && y + 1 < mMapHeight)
	{
		if (mJspMap[y][x + 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
		{
			checkRightDown(parentNode, x, y, randBrush, false);
			
		}
	}


	for (int iCnt = y + 1; iCnt < mMapHeight; ++iCnt)
	{
		// 장애물을 마주칠 경우 return 한다.
		if (mJspMap[iCnt][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return;
		}


		// 목적지를 만났을 경우 목적지를 이어주고 return 한다.
		if (x == mDestinationNode->mX && iCnt == mDestinationNode->mY)
		{
			setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_DD, x, iCnt);

			return;
		}

		// 오픈 리스트 노드 또는 클로즈 리스트 노드에는 칠하지 않는다.
		if (brushBlockList[x][iCnt] != blueBrush && brushBlockList[x][iCnt] != yellowBrush)
		{
			brushBlockList[x][iCnt] = randBrush;
		}

		if (x > 0 && iCnt + 1 < mMapHeight )
		{
			if (mJspMap[iCnt][x - 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[iCnt + 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_DD, x, iCnt);

				return;
			}
		}

		if (x + 1 < mMapWidth && iCnt + 1 < mMapHeight)
		{
			if (mJspMap[iCnt][x + 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[iCnt + 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_DD, x, iCnt);

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
	bool setOpenNodeFlag;

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

	while (1)
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

		setOpenNodeFlag = checkRightDiagonalHorizontal(parentNode, NODE_DIRECTION::NODE_DIR_RU, x, y, randBrush);
		if (setOpenNodeFlag)
		{
			return;
		}


		setOpenNodeFlag = checkUpDiagonalVertical(parentNode, NODE_DIRECTION::NODE_DIR_RU, x, y, randBrush);
		if (setOpenNodeFlag)
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
	bool setOpenNodeFlag;

	if (firstCall)
	{
		//===================================================================
		// 직선 조건에 맞으면 5방향으로 탐색하게 된다.
		checkRightHorizontal(parentNode, x, y, randBrush);
		
		checkDownVertical(parentNode, x, y, randBrush);	
		//===================================================================
	}

	while (1)
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

		setOpenNodeFlag = checkRightDiagonalHorizontal(parentNode, NODE_DIRECTION::NODE_DIR_RD, x, y, randBrush);
		if (setOpenNodeFlag)
		{
			return;
		}

		setOpenNodeFlag	= checkDownDiagonalVertical(parentNode, NODE_DIRECTION::NODE_DIR_RD, x, y, randBrush);
		if (setOpenNodeFlag)
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
	bool setOpenNodeFlag;

	if (firstCall)
	{
		//===================================================================
		// 직선 조건에 맞으면 5방향으로 탐색하게 된다.
		checkLeftHorizontal(parentNode, x, y, randBrush);	

		checkUpVertical(parentNode, x, y, randBrush);		
		//===================================================================
	}

	while (1)
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

		setOpenNodeFlag = checkLeftDiagonalHorizontal(parentNode, NODE_DIRECTION::NODE_DIR_LU, x, y, randBrush);
		if (setOpenNodeFlag)
		{
			return;
		}

		setOpenNodeFlag = checkUpDiagonalVertical(parentNode, NODE_DIRECTION::NODE_DIR_LU, x, y, randBrush);
		if (setOpenNodeFlag)
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
	bool setOpenNodeFlag;

	if (firstCall == true)
	{
		//===================================================================
		// 직선 조건에 맞으면 5방향으로 탐색하게 된다.
		checkLeftHorizontal(parentNode, x, y, randBrush);
		

		checkDownVertical(parentNode,  x, y, randBrush);		
		//===================================================================
	}


	while (1)
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

		setOpenNodeFlag = checkLeftDiagonalHorizontal(parentNode, NODE_DIRECTION::NODE_DIR_LD, x, y, randBrush);
		if (setOpenNodeFlag)
		{
			return;
		}

		setOpenNodeFlag = checkDownDiagonalVertical(parentNode, NODE_DIRECTION::NODE_DIR_LD, x, y, randBrush);
		if (setOpenNodeFlag)
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

	for (int iCnt = x; iCnt < mMapWidth; ++iCnt)
	{
		if (mJspMap[y][iCnt] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return false;
		}
	
		// 목적지 노드를 만났을 경우에는 그 자리에 노드 생성후 return 
		if (iCnt == mDestinationNode->mX && y == mDestinationNode->mY)
		{
			setCornerNode(parentNode, nodeDir, x, y);
			
			return true;
		}

		// 오픈 리스트 노드 또는 클로즈 리스트 노드에는 칠하지 않는다.
		if (brushBlockList[iCnt][y] != blueBrush && brushBlockList[iCnt][y] != yellowBrush)
		{
			brushBlockList[iCnt][y] = randBrush;
		}

		if (y > 0 && iCnt + 1 < mMapWidth)
		{
			if (mJspMap[y - 1][iCnt] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][iCnt + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				setCornerNode(parentNode, nodeDir, x, y);

				return true;
			}
		}

		if(y + 1 < mMapHeight && iCnt + 1 < mMapWidth)
		{
			if (mJspMap[y + 1][iCnt] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][iCnt + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				setCornerNode(parentNode, nodeDir, x, y);

				return true;
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

	for (int iCnt = x; iCnt >= 0; --iCnt)
	{
		if (mJspMap[y][iCnt] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return false;
		}		

		//=============================================
		// 목적지를 만났을 경우 return
		//=============================================
		if (iCnt == mDestinationNode->mX && y == mDestinationNode->mY)
		{
			setCornerNode(parentNode, nodeDir, x, y);

			return true;
		}

		// 오픈 리스트 노드 또는 클로즈 리스트 노드에는 칠하지 않는다.
		if (brushBlockList[iCnt][y] != blueBrush && brushBlockList[iCnt][y] != yellowBrush)
		{
			brushBlockList[iCnt][y] = randBrush;
		}

		
		if (y > 0 && iCnt > 0)
		{
			if (mJspMap[y - 1][iCnt] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][iCnt - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
			    setCornerNode(parentNode, nodeDir, x, y);

				return true;
			}
		}

		if (y + 1 < mMapHeight && iCnt > 0)
		{
			if(mJspMap[y + 1][iCnt] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][iCnt - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				setCornerNode(parentNode, nodeDir, x, y);
				
				return true;
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
	
	for (int iCnt = y; iCnt >= 0; --iCnt)
	{
		// 장애물 만났을 경우 return
		if (mJspMap[iCnt][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return false;
		}
		
		//=================================================
		// 목적지 만났을 경우 노드 생성 후 return
		//=================================================
		if (x == mDestinationNode->mX && iCnt == mDestinationNode->mY)
		{
			setCornerNode(parentNode, nodeDir, x, y);

			return true;
		}


		// 오픈 리스트 노드 또는 클로즈 리스트 노드에는 칠하지 않는다.
		if (brushBlockList[x][iCnt] != blueBrush && brushBlockList[x][iCnt] != yellowBrush)
		{
			brushBlockList[x][iCnt] = randBrush;
		}

		if (x > 0 && iCnt > 0)
		{
			if (mJspMap[iCnt][x - 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[iCnt - 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				setCornerNode(parentNode, nodeDir, x, y);
				
				return true;
			}
		}

		if (x + 1 < mMapWidth && iCnt > 0)
		{
			if (mJspMap[iCnt][x + 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[iCnt - 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				setCornerNode(parentNode, nodeDir, x, y);

				return true;
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

	for (int iCnt = y; iCnt < mMapHeight; iCnt++)
	{
		if (mJspMap[iCnt][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return false;
		}	

		if (x == mDestinationNode->mX && iCnt == mDestinationNode->mY)
		{
			setCornerNode(parentNode, nodeDir, x, y);

			return true;
		}

		// 오픈 리스트 노드 또는 클로즈 리스트 노드에는 칠하지 않는다.
		if (brushBlockList[x][iCnt] != blueBrush && brushBlockList[x][iCnt] != yellowBrush)
		{
			brushBlockList[x][iCnt] = randBrush;
		}

		if (x > 0 && iCnt + 1 < mMapHeight)
		{
			if (mJspMap[iCnt][x - 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[iCnt + 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				setCornerNode(parentNode, nodeDir, x, y);
				
				return true;
			}
		}

		if (x + 1 < mMapWidth && iCnt + 1 < mMapHeight)
		{

			if (mJspMap[iCnt][x + 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[iCnt + 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				setCornerNode(parentNode, nodeDir, x, y);				

				return true;
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

	if ((*iter)->data == nullptr)
	{
		return nullptr;
	}

	// 노드의 값을 복사한다.
	NODE* node = (*iter)->data;

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

	optimizeRouteReset();

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

	routeReset();

	resetRouteNodeArray(routeNodeArray, routeNodeArraySize);

	resetOptimizeArray(optimizeNodeArray, optimizeNodeArraySize);

	optimizeRouteReset();
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
void JumpPointSearch::optimizeRouteReset()
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
void JumpPointSearch::routeReset()
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
	while (1)
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

	while(1)
	{
		startX = iter->mX;
		startY = iter->mY;

		nextIter = iter.NextIter();
		
		//========================================================================
		// iter의 다음 노드가 마지막 노드라면은 break 로직을 종료한다.
		//=========================================================================
		if (nextIter == iterE)
		{
			break;
		}
	
		nextNextIter = nextIter;

		while (1)
		{
			nextNextIter = nextNextIter.NextIter();


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
				(*nextIter)->deleteCheck = true;
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
		if ((*iter)->deleteCheck)
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

	for (int iCnt = 0; iCnt < mOpenList.listLength; iCnt++)
	{
		for (CList<JumpPointSearch::NODE*>::Iterator iter = mOpenList.begin(); iter != iterE; ++iter)
		{
			CList<JumpPointSearch::NODE*>::Iterator iterN = iter.node->next;

			if (iter->F > iterN->F)
			{
				mOpenList.DataSwap(iter, iterN);
			}
		}

		--iterE;
	}

}