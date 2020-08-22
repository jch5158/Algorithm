#include "stdafx.h"
#include "CList.h"
#include "JumpPointSearchAlgorithm.h"
#include "BresenhamLine.h"
#include "JumpPointSearch.h"


extern HBRUSH       oldBrush;

// 오픈리스트
extern HBRUSH       blueBrush;

// 클로즈 리스트
extern HBRUSH       yellowBrush;

// 출발지 체크브러쉬
extern HBRUSH       greenBrush;

// 목적지 체크브러쉬
extern HBRUSH       redBrush;

// 장애물 체크브러쉬
extern HBRUSH       grayBrush;


// 목적지 노드
JumpPointSearch::NODE* destinationNode = nullptr;

// 시작지 노드
JumpPointSearch::NODE* startNode = nullptr;


CList<JumpPointSearch::NODE*> openList;

CList<JumpPointSearch::NODE*> closeList;

CList<JumpPointSearch::NODE*> routeList;

CList<JumpPointSearch::NODE*> optimizeRouteList;


HBRUSH brushBlockList[MAX_WIDTH][MAX_HEIGHT] = { nullptr, };


//===================================================================
// 타이머로 자동 함수 호출 시 로직 실행이 되지 않도록 하는 Flag
//===================================================================
bool functionFlag = false;
//
bool funcSetFlag = true;


// 목적지 노드
//JumpPointSearch::NODE* JumpPointSearch::destinationNode = nullptr;

// 시작지 노드
//JumpPointSearch::NODE* JumpPointSearch::startNode = nullptr;


JumpPointSearch::NODE* JumpPointSearch::PathFind(int startX, int startY, int destinationX, int destinationY)
{
	
	// 현재 노드
	static JumpPointSearch::NODE* curNode = nullptr;	 



	// 엔터를 입력하기 전까지 해당 함수를 호출 시 바로 리턴됩니다.
	if (functionFlag == false)
	{
		return nullptr;
	}

	if (funcSetFlag == true)
	{
		// 목적지 노드 동적할당
		destinationNode = (NODE*)malloc(sizeof(NODE));

		// 목적지 노드의 좌표 설정
		destinationNode->mX = destinationX;
		destinationNode->mY = destinationY;

		// 시작 노드 설정
		startNode = (NODE*)malloc(sizeof(NODE));

		// 시작노드 좌표설정
		startNode->mX = startX;
		startNode->mY = startY;

		startNode->mNodeDir = (BYTE)NODE_DIRECTION::NODE_DIR_ALL;

		startNode->prev = nullptr;

		// 시작노드 G 설정
		startNode->G = 0;

		// 목적지까지의 절대값	
		startNode->H += (float)(abs(destinationX - startX) + abs(destinationY - startY));

		startNode->F = startNode->G + startNode->H;

		// 클로즈 리스트에 추가
		closeList.PushBack(startNode);

		curNode = startNode;

		// 한번만 시작 셋팅되도록 설정
		funcSetFlag = false;
	}


	NODE* retval;

	// 오픈 리스트를 만들기 위해서 현재 노드 위치랑 목적지 위치를 인자로 전달한다.
	retval = InsertOpenNode(curNode, destinationNode);
	// false일 경우 목적지까지 노드를 만들었다.
	if (retval == destinationNode)
	{
		InsertRoute(destinationNode);

		PathOptimizing();

		functionFlag = false;
		return retval;
	}
	else
	{
		// openList F값 정렬
		openListBubbleSort();

		// F값이 가장 작은 노드를 선택한다.
		curNode = SelectOpenListNode();
	}

	return nullptr;
}

//======================================================
// 좌표로 openList를 찾아서 return 
//======================================================
JumpPointSearch::NODE* JumpPointSearch::FindOpenList(int openX, int openY)
{

	CList<NODE*>::Iterator iterE = openList.end();

	for (CList<NODE*>::Iterator iter = openList.begin(); iter != iterE; ++iter)
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
bool JumpPointSearch::FindCloseList(int closeX, int closeY)
{
	CList<NODE*>::Iterator iterE = closeList.end();

	for (CList<NODE*>::Iterator iter = closeList.begin(); iter != iterE; ++iter)
	{
		if (iter->mX == closeX && iter->mY == closeY)
		{
			return true;
		}
	}

	return false;
}


JumpPointSearch::NODE* JumpPointSearch::InsertOpenNode(JumpPointSearch::NODE* node, JumpPointSearch::NODE* destNode)
{
	if (node == nullptr)
	{
		return nullptr;
	}


	NODE* retOpenNode;

	NODE* openNode;

	int posX = node->mX;
	int posY = node->mY;

	int newNodePosX = 0;
	int newNodePosY = 0;

    retOpenNode = CheckDirection(node, destNode,posX, posY);
	if (retOpenNode != destNode)
	{
		return nullptr;
	}	

	return destNode;
}


JumpPointSearch::NODE* JumpPointSearch::CheckDirection(NODE* node, NODE* destNode,int x, int y)
{
	NODE* retNode;

	HBRUSH randBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));


	switch ((NODE_DIRECTION)node->mNodeDir)
	{
	case NODE_DIRECTION::NODE_DIR_RR:

		retNode = CheckRightHorizontal(node, destNode, x, y, randBrush);
		if (retNode == destNode)
		{
			return retNode;
		}

		break;
	case NODE_DIRECTION::NODE_DIR_RD:

		retNode = CheckRightDown(node, destNode, x, y, randBrush);
		if (retNode == destNode)
		{
			return retNode;
		}

		break;
	case NODE_DIRECTION::NODE_DIR_DD:
		
		retNode = CheckDownVertical(node, destNode, x, y, randBrush);
		if (retNode == destNode)
		{
			return retNode;
		}

		break;
	case NODE_DIRECTION::NODE_DIR_LD:

		retNode = CheckLeftDown(node, destNode, x, y, randBrush);
		if (retNode == destNode)
		{
			return retNode;
		}
		
		break;
	case NODE_DIRECTION::NODE_DIR_LL:
	
		retNode = CheckLeftHorizontal(node, destNode, x, y, randBrush);
		if (retNode == destNode)
		{
			return retNode;
		}
		
		break;
	case NODE_DIRECTION::NODE_DIR_LU:

		retNode = CheckLeftUp(node, destNode, x, y, randBrush);
		if (retNode == destNode)
		{
			return retNode;
		}
		
		break;
	case NODE_DIRECTION::NODE_DIR_UU:
		
		retNode = CheckUpVertical(node, destNode, x, y, randBrush);
		if (retNode == destNode)
		{
			return retNode;
		}

		break;
	case NODE_DIRECTION::NODE_DIR_RU:

		retNode = CheckRightUp(node, destNode, x, y,randBrush);
		if (retNode == destNode)
		{
			return retNode;
		}

		break;

	case NODE_DIRECTION::NODE_DIR_ALL:

		retNode = CheckRightUp(node, destNode, x, y, randBrush, false);
		if (retNode == destNode)
		{
			return retNode;
		}

		retNode = CheckRightHorizontal(node, destNode, x, y, randBrush);
		if (retNode == destNode)
		{
			return retNode;
		}

		retNode = CheckRightDown(node, destNode, x, y, randBrush ,false);
		if (retNode == destNode)
		{
			return retNode;
		}

		retNode = CheckDownVertical(node, destNode, x, y, randBrush);
		if (retNode == destNode)
		{
			return retNode;
		}

		retNode = CheckLeftDown(node, destNode, x, y, randBrush ,false);
		if (retNode == destNode)
		{
			return retNode;
		}

		retNode = CheckLeftHorizontal(node, destNode, x, y, randBrush);
		if (retNode == destNode)
		{
			return retNode;
		}

		retNode = CheckLeftUp(node, destNode, x, y, randBrush, false);
		if (retNode == destNode)
		{
			return retNode;
		}

		retNode = CheckUpVertical(node, destNode, x, y, randBrush);
		if (retNode == destNode)
		{
			return retNode;
		}	

		break;

	default:
		return nullptr;
	}

	return nullptr;

}


JumpPointSearch::NODE* JumpPointSearch::SetCornerNode(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir, int x, int y)
{
	NODE* newNode = nullptr;

	NODE* retOpenNode;

	int subX;
	int subY;



	if (FindCloseList(x, y) == false)
	{
		// 오픈리스트에 없는 노드라면은 생성
		retOpenNode = FindOpenList(x, y);
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

			newNode->H = (float)(abs(destNode->mX - newNode->mX) + abs(destNode->mY - newNode->mY));

			newNode->F = newNode->G + newNode->H;

			newNode->mNodeDir = (BYTE)nodeDir;

			newNode->prev = parentNode;

			// 블럭의 색상을 바꿔준다.

			if (x != destNode->mX && y != destNode->mY)
			{
				brushBlockList[newNode->mX][newNode->mY] = blueBrush;
			}
			
			return newNode;
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

				retOpenNode->H = (float)(abs(destNode->mX - retOpenNode->mX) + abs(destNode->mY - retOpenNode->mY));

				retOpenNode->F = retOpenNode->G + retOpenNode->H;

				retOpenNode->mNodeDir = (BYTE)nodeDir;

				retOpenNode->prev = parentNode;
				
				return newNode;
			}
		}


	}

	return nullptr;
}


//===============================================
// 오른쪽 직선 탐색 함수입니다.
//===============================================
JumpPointSearch::NODE* JumpPointSearch::CheckRightHorizontal(NODE* parentNode, NODE* destNode, int x, int y, HBRUSH randBrush)
{
	NODE* newOpenNode = nullptr;

	NODE* retNode = nullptr;

	//=================================================
	// 보조 탐색 영억을 생성하는
	//=================================================
	if (y > 0 && x + 1 < MAX_WIDTH)
	{
		if (brushBlockList[x][y - 1] == grayBrush && brushBlockList[x + 1][y - 1] == oldBrush)
		{	
			retNode = CheckRightUp(parentNode, destNode, x, y, randBrush, false);	
			if (retNode == destNode)
			{
				return retNode;
			}
		}
	}

	if (y + 1 < MAX_HEIGHT && x + 1 < MAX_WIDTH)
	{
		if (brushBlockList[x][y + 1] == grayBrush && brushBlockList[x + 1][y + 1] == oldBrush)
		{
			retNode = CheckRightDown(parentNode, destNode, x, y, randBrush, false);
			if (retNode == destNode)
			{
				return retNode;
			}
		}
	}
	
	//============================================================================
	// iCnt = x + 1 을 하는 이유는 해당 위치는 현재 노드가 있는 위치이기 때문이다. 
	// iCnt + 1 < MAX_WIDTH 인 이유는 위와 아래의 벽이 있을 경우 X + 1 좌표에 
	// 벽이 없는지 확인하기 때문으로 오버플로우가 발생되지 않도록 하기 위함입니다.
	//============================================================================
	for (int iCnt = x + 1; iCnt < MAX_WIDTH; ++iCnt)
	{
		// 벽을 만났으 경우 retur nullptr 을 한다.
		if (brushBlockList[iCnt][y] == grayBrush)
		{
			return nullptr;
		}

		// 목적지를 만날 경우 바로 이어준다. 
		if (iCnt == destNode->mX && y == destNode->mY)
		{
			destNode->prev = parentNode;

			return destNode;
		}

		// 브러쉬 리스트에 칠하기
		brushBlockList[iCnt][y] = randBrush;

		//===================================================
		// y 값이 0 이상일 경우에만 위의 벽을 확인할 수 있다.
		//===================================================
		if (y > 0 && iCnt + 1 < MAX_WIDTH )
		{
			if (brushBlockList[iCnt][y - 1] == grayBrush && brushBlockList[iCnt + 1][y - 1] == oldBrush)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RR, iCnt, y);
				if(newOpenNode != nullptr)
				{
					openList.PushBack(newOpenNode);
				}				
				return newOpenNode;
			}
		}

		if (y + 1 < MAX_WIDTH && iCnt + 1 < MAX_WIDTH)
		{
			if (brushBlockList[iCnt][y + 1] == grayBrush && brushBlockList[iCnt + 1][y + 1] == oldBrush)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RR, iCnt, y);
				if (newOpenNode != nullptr)
				{
					openList.PushBack(newOpenNode);
				}

				return newOpenNode;
			}
		}

	}

	return nullptr;
}



//===============================================
// 왼쪽 직선 탐색 함수입니다.
//===============================================
JumpPointSearch::NODE* JumpPointSearch::CheckLeftHorizontal(NODE* parentNode, NODE* destNode, int x, int y, HBRUSH randBrush)
{
	NODE* newOpenNode = nullptr;

	NODE* retNode = nullptr;

	if (y > 0 && x > 0)
	{
		if (brushBlockList[x][y - 1] == grayBrush && brushBlockList[x - 1][y - 1] == oldBrush)
		{		
			retNode = CheckLeftUp(parentNode, destNode, x, y, randBrush, false);
			if (retNode == destNode)
			{
				return retNode;
			}
		}
	}

	if (y + 1 < MAX_HEIGHT && x > 0)
	{
		if (brushBlockList[x][y + 1] == grayBrush && brushBlockList[x - 1][y + 1] == oldBrush)
		{
			retNode = CheckLeftDown(parentNode, destNode, x, y, randBrush, false);
			if (retNode == destNode)
			{
				return retNode;
			}
		}
	}

	for (int iCnt = x - 1; iCnt >= 0; --iCnt)
	{
		// 해당 좌표에 장애물을 만나면 바로 return 한다.
		if (brushBlockList[iCnt][y] == grayBrush)
		{
			return nullptr;
		}


		// 목적지를 만나면 이어주고 바로 return 한다.
		if (iCnt == destNode->mX && y == destNode->mY)
		{
			destNode->prev = parentNode;

			return destNode;
		}

		// 타일에 색상을 브러쉬를 넣어준다.
		brushBlockList[iCnt][y] = randBrush;


		if (y > 0 && iCnt - 1 >= 0)
		{
			if (brushBlockList[iCnt][y - 1] == grayBrush && brushBlockList[iCnt - 1][y - 1] == oldBrush)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LL, iCnt, y);

				if (newOpenNode != nullptr)
				{
					openList.PushBack(newOpenNode);
				}

				return newOpenNode;	
			}
		}


		if (y + 1 < MAX_HEIGHT && iCnt - 1 >= 0)
		{
			if (brushBlockList[iCnt][y + 1] == grayBrush && brushBlockList[iCnt - 1][y + 1] == oldBrush)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LL, iCnt, y);
				
				if (newOpenNode != nullptr)
				{
					openList.PushBack(newOpenNode);
				}

				return newOpenNode;
			}
		}	

	}

	return nullptr;
}


//===============================================
// 수직 직선 탐색 함수입니다.
//===============================================
JumpPointSearch::NODE* JumpPointSearch::CheckUpVertical(NODE* parentNode, NODE* destNode, int x, int y, HBRUSH randBrush)
{
	NODE* newOpenNode = nullptr;

	NODE* retNode = nullptr;


	if (x > 0 && y > 0)
	{
		if (brushBlockList[x - 1][y] == grayBrush && brushBlockList[x - 1][y - 1] == oldBrush)
		{
			retNode = CheckLeftUp(parentNode, destNode, x, y, randBrush, false);
			if (retNode == destNode)
			{
				return retNode;
			}
		}
	}

	if (x + 1 < MAX_WIDTH && y > 0)
	{
		if (brushBlockList[x + 1][y] == grayBrush && brushBlockList[x + 1][y - 1] == oldBrush)
		{ 	
			retNode = CheckRightUp(parentNode, destNode, x, y, randBrush, false);
			if (retNode == destNode)
			{
				return retNode;
			}
		}
	}

	for (int iCnt = y - 1; iCnt >= 0; --iCnt)
	{
		//=============================================
		// 해당 위치에 장애물이 있을 경우 바로 return
		//=============================================
		if (brushBlockList[x][iCnt] == grayBrush)
		{
			return nullptr;
		}


		//=============================================
		// 해당 위치에 목적지가 있을 경우 바로 return
		//=============================================
		if (x == destNode->mX && iCnt == destNode->mY)
		{
			destNode->prev = parentNode;

			return destNode;
		}

		//==========================================
		// 해당 타일에 브러쉬를 칠한다.
		//==========================================
		brushBlockList[x][iCnt] = randBrush;

		if (x > 0 && iCnt - 1 >= 0)
		{
			if (brushBlockList[x - 1][iCnt] == grayBrush && brushBlockList[x - 1][iCnt - 1] == oldBrush)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, NODE_DIRECTION::NODE_DIR_UU, x, iCnt);

				if (newOpenNode != nullptr)
				{
					openList.PushBack(newOpenNode);
				}

				return newOpenNode;
			}
		}


		if (x + 1 < MAX_WIDTH && iCnt - 1 >= 0)
		{
			if (brushBlockList[x + 1][iCnt] == grayBrush && brushBlockList[x + 1][iCnt - 1] == oldBrush)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, NODE_DIRECTION::NODE_DIR_UU, x, iCnt);

				if (newOpenNode != nullptr)
				{
					openList.PushBack(newOpenNode);
				}

				return newOpenNode;					
			}
		}	

	}
	return nullptr;
}




JumpPointSearch::NODE* JumpPointSearch::CheckDownVertical(NODE* parentNode, NODE* destNode, int x, int y, HBRUSH randBrush)
{
	NODE* newOpenNode = nullptr;

	NODE* retNode = nullptr;

	if (x > 0 && y + 1 < MAX_HEIGHT)
	{
		if (brushBlockList[x - 1][y] == grayBrush && brushBlockList[x - 1][y + 1] == oldBrush)
		{
			retNode = CheckLeftDown(parentNode, destNode, x, y, randBrush, false);
			if (retNode == destNode)
			{
				return retNode;
			}
		}
	}

	if (x + 1 < MAX_WIDTH && y + 1 < MAX_HEIGHT)
	{
		if (brushBlockList[x + 1][y] == grayBrush && brushBlockList[x + 1][y + 1] == oldBrush)
		{
			retNode = CheckRightDown(parentNode, destNode, x, y, randBrush, false);
			if (retNode == destNode)
			{
				return retNode;
			}
		}
	}


	for (int iCnt = y + 1; iCnt < MAX_HEIGHT; ++iCnt)
	{
		// 장애물을 마주칠 경우 return 한다.
		if (brushBlockList[x][iCnt] == grayBrush)
		{
			return nullptr;
		}


		// 목적지를 만났을 경우 목적지를 이어주고 return 한다.
		if (x == destNode->mX && iCnt == destNode->mY)
		{
			destNode->prev = parentNode;

			return destNode;
		}

		brushBlockList[x][iCnt] = randBrush;

		if (x > 0 && iCnt + 1 < MAX_HEIGHT )
		{
			if (brushBlockList[x - 1][iCnt] == grayBrush && brushBlockList[x - 1][iCnt + 1] == oldBrush)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, NODE_DIRECTION::NODE_DIR_DD, x, iCnt);

				if (newOpenNode != nullptr)
				{
					openList.PushBack(newOpenNode);
				}

				return newOpenNode;
			}
		}

		if (x + 1 < MAX_WIDTH && iCnt + 1 < MAX_HEIGHT)
		{
			if (brushBlockList[x + 1][iCnt] == grayBrush && brushBlockList[x + 1][iCnt + 1] == oldBrush)
			{

				newOpenNode = SetCornerNode(parentNode, destNode, NODE_DIRECTION::NODE_DIR_DD, x, iCnt);

				if (newOpenNode != nullptr)
				{
					openList.PushBack(newOpenNode);
				}

				return newOpenNode;
			}
		}		

	}

	return nullptr;
}




//======================================================
// 오른쪽 위 대각선 함수입니다. 
//======================================================
JumpPointSearch::NODE* JumpPointSearch::CheckRightUp(NODE* parentNode, NODE* destNode, int x, int y, HBRUSH randBrush ,bool firstCall)
{	
	bool retOpenNode;

	bool rightUpFlag = true;

	bool leftUpFlag = false;
	bool rightDownFlag = false;
	
	int xCount = 0;
	int yCount = 0;


	//===============================================
	// 대각선 함수 호출 시 직선 함수 호출 여부
	//===============================================
	if (firstCall)
	{	
		if (destNode == CheckRightHorizontal(parentNode, destNode, x, y, randBrush))
		{
			return destNode;
		}

		if (destNode == CheckUpVertical(parentNode, destNode, x, y, randBrush))
		{
			return destNode;
		}
	}

	//==================================================
	// 보조 왼쪽 위 대각선 호출 조건
	//==================================================
	if (x > 0 && y > 0 )
	{
		if (brushBlockList[x - 1][y] == grayBrush && brushBlockList[x - 1][y - 1] == oldBrush)
		{
			leftUpFlag = true;
		}
	}

	//==================================================
	// 보조 오른쪽 아래 대각선 호출 조건
	//==================================================
	if (x + 1 < MAX_WIDTH && y + 1 < MAX_HEIGHT)
	{
		if (brushBlockList[x][y + 1] == grayBrush && brushBlockList[x + 1][y + 1] == oldBrush)
		{
			rightDownFlag = true;
		}
	}


	while (1)
	{
		xCount += 1;

		yCount += 1;


		// 오른쪽 위 대각선 호출
		if (rightUpFlag)
		{
			do
			{
				// 배열 범위를 벗어날 경우 호출 안함 
				if (x + xCount >= MAX_WIDTH || y - yCount < 0)
				{
					rightUpFlag = false;
					break;
				}

				// 장애물을 만났을 경우 호출 안함
				if (brushBlockList[x + xCount][y - yCount] == grayBrush)
				{
					rightUpFlag = false;
					break;
				}

				//
				retOpenNode = CheckRightDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RU, x + xCount, y - yCount, randBrush);
				if (retOpenNode)
				{
					rightUpFlag = false;
					break;
				}

				retOpenNode = CheckUpDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RU, x + xCount, y - yCount, randBrush);
				if (retOpenNode)
				{
					rightUpFlag = false;
					break;
				}

			} while (0);
		}


		if (leftUpFlag)
		{
			do
			{
				if (x - xCount < 0 || y - yCount < 0)
				{
					leftUpFlag = false;
					break;
				}

				if (brushBlockList[x - xCount][y - yCount] == grayBrush)
				{
					leftUpFlag = false;
					break;
				}

				retOpenNode = CheckLeftDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LU, x - xCount, y - yCount, randBrush);
				if (retOpenNode)
				{
					leftUpFlag = false;
					break;
				}

				retOpenNode = CheckUpDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LU, x - xCount, y - yCount, randBrush);
				if (retOpenNode)
				{
					leftUpFlag = false;
					break;
				}


			} while (0);
		}

		if (rightDownFlag)
		{
			do
			{
				if (x + xCount >= MAX_WIDTH || y + yCount >= MAX_HEIGHT)
				{
					rightDownFlag = false;
					break;
				}

				if (brushBlockList[x + xCount][y + yCount] == grayBrush)
				{
					rightDownFlag = false;
					break;
				}

				retOpenNode = CheckRightDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RD, x + xCount, y + yCount, randBrush);
				if (retOpenNode)
				{
					rightDownFlag = false;
					break;
				}

				retOpenNode = CheckDownDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RD, x + xCount, y + yCount, randBrush);
				if (retOpenNode)
				{
					rightDownFlag = false;
					break;
				}

			} while (0);
		}

		if (destNode->mX == x + xCount && destNode->mY == y - yCount)
		{
			destNode->prev = parentNode;

			return destNode;
		}
		
		if (rightUpFlag == false && rightDownFlag == false && leftUpFlag == false)
		{
			break;
		}
	}

	return nullptr;
}

JumpPointSearch::NODE* JumpPointSearch::CheckRightDown(NODE* parentNode, NODE* destNode, int x, int y, HBRUSH randBrush, bool firstCall)
{
	bool retOpenNode;

	bool rightDownFlag = true;

	bool rightUpFlag = false;
	bool leftDownFlag = false;
	
	int xCount = 0;
	int yCount = 0;

	if (firstCall)
	{

		if (destNode == CheckRightHorizontal(parentNode, destNode, x, y, randBrush))
		{
			return destNode;
		}

		if (destNode == CheckDownVertical(parentNode, destNode, x, y, randBrush))
		{
			return destNode;
		}
	}

	if (x + 1 < MAX_WIDTH && y > 0)
	{
		if (brushBlockList[x][y - 1] == grayBrush && brushBlockList[x + 1][y - 1] == oldBrush)
		{	
			rightUpFlag = true;
		}
	}

	if (x > 0 && y + 1 < MAX_HEIGHT)
	{
		if (brushBlockList[x - 1][y] == grayBrush && brushBlockList[x - 1][y + 1] == oldBrush)
		{
			leftDownFlag = true;
		}
	}



	while (1)
	{
		xCount += 1;

		yCount += 1;


		if (rightDownFlag)
		{
			do
			{
				if ( x + xCount >= MAX_WIDTH || y + yCount >= MAX_HEIGHT)
				{
					rightDownFlag = false;
					break;
				}

				if (brushBlockList[x + xCount][y + yCount] == grayBrush)
				{
					rightDownFlag = false;
					break;
				}

				retOpenNode = CheckRightDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RD, x + xCount, y + yCount, randBrush);
				if (retOpenNode)
				{
					rightDownFlag = false;
					break;
				}

				retOpenNode = CheckDownDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RD, x + xCount, y + yCount, randBrush);
				if (retOpenNode)
				{
					rightDownFlag = false;
					break;
				}
			} while (0);
		}

		if (rightUpFlag)
		{
			do
			{
				if (x + xCount >= MAX_WIDTH || y - yCount < 0)
				{
					rightUpFlag = false;
					break;
				}

				if (brushBlockList[x + xCount][y - yCount] == grayBrush)
				{
					rightUpFlag = false;
					break;
				}

				retOpenNode = CheckRightDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RU, x + xCount, y - yCount, randBrush);
				if (retOpenNode)
				{
					rightUpFlag = false;
					break;
				}

				retOpenNode = CheckUpDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RU, x + xCount, y - yCount, randBrush);
				if (retOpenNode)
				{
					rightUpFlag = false;
					break;
				}	

			} while (0);
		}

		if (leftDownFlag)
		{
			do
			{
				if (x - xCount < 0 || y + yCount >= MAX_HEIGHT)
				{
					leftDownFlag = false;
					break;
				}

				if (brushBlockList[x - xCount][y + yCount] == grayBrush)
				{
					leftDownFlag = false;
					break;
				}

				retOpenNode = CheckLeftDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LD, x - xCount, y + yCount, randBrush);
				if(retOpenNode)
				{
					leftDownFlag = false;
					break;
				}

				retOpenNode = CheckDownDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LD, x - xCount, y + yCount, randBrush);
				if (retOpenNode)
				{
					leftDownFlag = false;
					break;
				}

			} while (0);
		}

		if (destNode->mX == x + xCount && destNode->mY == y + yCount)
		{
			destNode->prev = parentNode;

			return destNode;
		}

		if (rightDownFlag == false && rightUpFlag == false && leftDownFlag == false)
		{
			break;
		}

	}
}

JumpPointSearch::NODE* JumpPointSearch::CheckLeftUp(NODE* parentNode, NODE* destNode, int x, int y, HBRUSH randBrush ,bool firstCall)
{
	bool retOpenNode;
	
	bool leftUpFlag = true;

	bool leftDownFlag = false;
	bool rightUpFlag = false;

	int xCount = 0;
	int yCount = 0;

	if (firstCall)
	{


		if (destNode == CheckLeftHorizontal(parentNode, destNode, x, y, randBrush))
		{
			return destNode;
		}

		if (destNode == CheckUpVertical(parentNode, destNode, x, y, randBrush))
		{
			return destNode;
		}
	}

	if (x + 1 < MAX_WIDTH && y > 0)
	{
		if (brushBlockList[x + 1][y] == grayBrush && brushBlockList[x + 1][y - 1] == oldBrush)
		{
			rightUpFlag = true;
		}
	}

	if (x > 0 && y + 1 < MAX_HEIGHT)
	{
		if (brushBlockList[x][y + 1] == grayBrush && brushBlockList[x - 1][y + 1] == oldBrush)
		{
			leftDownFlag = true;
		}
	}



	while (1)
	{
		xCount += 1;
		yCount += 1;

		if (leftUpFlag)
		{
			do
			{
				if (x - xCount < 0 || y - yCount < 0)
				{
					leftUpFlag = false;
					break;
				}

				if (brushBlockList[x - xCount][y - yCount] == grayBrush)
				{
					leftUpFlag = false;
					break;
				}

				retOpenNode = CheckLeftDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LU, x - xCount, y - yCount, randBrush);
				if (retOpenNode)
				{
					leftUpFlag = false;
					break;
				}

				retOpenNode = retOpenNode = CheckUpDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LU, x -xCount, y - yCount, randBrush);
				if (retOpenNode)
				{
					leftUpFlag = false;
					break;
				}

			} while (0);
		}

		if (rightUpFlag)
		{
			do
			{
				if (x + xCount >= MAX_WIDTH || y - yCount < 0)
				{
					rightUpFlag = false;
					break;
				}

				if (brushBlockList[x + xCount][y - yCount] == grayBrush)
				{
					rightUpFlag = false;
					break;
				}

				retOpenNode = CheckRightDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RU, x + xCount, y - yCount, randBrush);
				if (retOpenNode)
				{
					rightUpFlag = false;
					break;
				}

				retOpenNode = CheckUpDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RU, x + xCount, y - yCount, randBrush);
				if (retOpenNode)
				{
					rightUpFlag = false;
					break;
				}

			} while (0);
		}


		if (leftDownFlag)
		{
			do
			{
				if (x - xCount < 0 || y + yCount >= MAX_HEIGHT)
				{
					leftDownFlag = false;
					break;
				}

				if (brushBlockList[x - xCount][y + yCount] == grayBrush)
				{
					leftDownFlag = false;
					break;
				}

				retOpenNode = CheckLeftDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LD, x - xCount, y + yCount, randBrush);
				if(retOpenNode)
				{
					leftDownFlag = false;
					break;
				}

				retOpenNode = CheckDownDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LD, x - xCount, y + yCount, randBrush);
				if (retOpenNode)
				{
					leftDownFlag = false;
					break;
				}
			} while (0);
		}

		if (destNode->mX == x - xCount && destNode->mY == y - yCount)
		{
			destNode->prev = parentNode;

			return destNode;
		}

		if (leftUpFlag == false && rightUpFlag == false && leftDownFlag == false)
		{
			break;
		}
	}

}

JumpPointSearch::NODE* JumpPointSearch::CheckLeftDown(NODE* parentNode, NODE* destNode, int x, int y, HBRUSH randBrush, bool firstCall)
{
	bool retOpenNode;

	bool leftDownFlag = true;

	bool leftUpFlag = false;
	bool rightDownFlag = false;

	int xCount = 0;
	int yCount = 0;

	if (firstCall == true)
	{


		if (destNode == CheckLeftHorizontal(parentNode, destNode, x, y, randBrush))
		{
			return destNode;
		}

		if (destNode == CheckDownVertical(parentNode, destNode, x, y, randBrush))
		{
			return destNode;
		}
	}

	if (x > 0 && y > 0)
	{
		if (brushBlockList[x][y - 1] == grayBrush && brushBlockList[x - 1][y - 1] == oldBrush)
		{
			leftUpFlag = true;
		}
	}

	if (x + 1 < MAX_WIDTH && y + 1 < MAX_HEIGHT)
	{
		if (brushBlockList[x + 1][y] == grayBrush && brushBlockList[x + 1][y + 1] == oldBrush)
		{
			rightDownFlag = true;
		}
	}

	while (1)
	{
		xCount += 1;
		yCount += 1;

		if (leftDownFlag)
		{
			do
			{
				if (x - xCount < 0 || y + yCount >= MAX_HEIGHT)
				{
					leftDownFlag = false;
					break;
				}

				if (brushBlockList[x- xCount][y + yCount] == grayBrush)
				{
					leftDownFlag = false;
					break;
				}

				retOpenNode = CheckLeftDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LD, x - xCount, y + yCount, randBrush);
				if (retOpenNode)
				{
					leftDownFlag = false;
					break;
				}

				retOpenNode = CheckDownDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LD, x - xCount, y + yCount, randBrush);
				if (retOpenNode)
				{
					leftDownFlag = false;
					break;
				}

			} while (0);
		}

		if (leftUpFlag)
		{
			do
			{
				if (x - xCount < 0 || y - yCount < 0)
				{
					leftUpFlag = false;
					break;
				}

				if (brushBlockList[x - xCount][y - yCount] == grayBrush)
				{
					leftUpFlag = false;
					break;
				}

				retOpenNode = CheckLeftDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LU, x - xCount, y - yCount, randBrush);
				if (retOpenNode)
				{
					leftUpFlag = false;
					break;
				}

				retOpenNode = CheckUpDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LU, x - xCount, y - yCount, randBrush);
				if (retOpenNode)
				{
					leftUpFlag = false;
					break;
				}

			} while (0);
		}

		if (rightDownFlag)
		{
			do
			{
				if (x + xCount >= MAX_WIDTH || y + yCount >= MAX_HEIGHT)
				{
					rightDownFlag = false;
					break;
				}

				if (brushBlockList[x + xCount][y + yCount] == grayBrush)
				{
					rightDownFlag = false;
					break;
				}

				retOpenNode = CheckRightDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RD, x + xCount, y + yCount, randBrush);
				if (retOpenNode)
				{
					rightDownFlag = false;
					break;
				}

				retOpenNode = CheckDownDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RD, x + xCount, y + yCount, randBrush);
				if (retOpenNode)
				{
					rightDownFlag = false;
					break;
				}

			} while (0);
		}

		if (destNode->mX == x - xCount && destNode->mY == y + yCount)
		{
			destNode->prev = parentNode;

			return destNode;
		}

		if (leftDownFlag == false && leftUpFlag == false && rightDownFlag == false)
		{
			break;
		}
		
	}
}


//=======================================================
// 대각선 함수 수평 오른쪽 방향 
//=======================================================
bool JumpPointSearch::CheckRightDiagonalHorizontal(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir,int x, int y, HBRUSH randBrush)
{

	NODE* newOpenNode;

	for (int iCnt = x; iCnt < MAX_WIDTH; ++iCnt)
	{
		if (brushBlockList[iCnt][y] == grayBrush)
		{
			return false;
		}
	
		// 목적지 노드를 만났을 경우에는 그 자리에 노드 생성후 return 
		if (iCnt == destNode->mX && y == destNode->mY)
		{
			newOpenNode = SetCornerNode(parentNode, destNode, nodeDir, x, y);

			if (newOpenNode != nullptr)
			{
				openList.PushBack(newOpenNode);
			}

			return true;
		}

		brushBlockList[iCnt][y] = randBrush;

		if (y > 0 && iCnt + 1 < MAX_WIDTH)
		{
			if (brushBlockList[iCnt][y - 1] == grayBrush && brushBlockList[iCnt + 1][y - 1] == oldBrush)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, nodeDir, x, y);

				if (newOpenNode != nullptr)
				{
					openList.PushBack(newOpenNode);
				}

				return true;
			}
		}

		if(y + 1 < MAX_HEIGHT && iCnt + 1 < MAX_WIDTH)
		{
			if (brushBlockList[iCnt][y + 1] == grayBrush && brushBlockList[iCnt + 1][y + 1] == oldBrush)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, nodeDir, x, y);

				if (newOpenNode != nullptr)
				{
					openList.PushBack(newOpenNode);
				}

				return true;
			}
		}	
	}


	return false;
}



//=======================================================
// 대각선 함수 수평 왼쪽 방향
//=======================================================
bool JumpPointSearch::CheckLeftDiagonalHorizontal(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush)
{

	NODE* newOpenNode;

	for (int iCnt = x; iCnt >= 0; --iCnt)
	{
		if (brushBlockList[iCnt][y] == grayBrush)
		{
			return false;
		}		

		//=============================================
		// 목적지를 만났을 경우 return
		//=============================================
		if (iCnt == destNode->mX && y == destNode->mY)
		{
			newOpenNode = SetCornerNode(parentNode, destNode, nodeDir, x, y);

			if (newOpenNode != nullptr)
			{
				openList.PushBack(newOpenNode);
			}

			return true;
		}

		brushBlockList[iCnt][y] = randBrush;


		
		if (y > 0 && iCnt > 0)
		{
			if (brushBlockList[iCnt][y - 1] == grayBrush && brushBlockList[iCnt - 1][y - 1] == oldBrush)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, nodeDir, x, y);

				if (newOpenNode != nullptr)
				{
					openList.PushBack(newOpenNode);
				}

				return true;
			}
		}

		if (y + 1 < MAX_HEIGHT && iCnt > 0)
		{
			if(brushBlockList[iCnt][y + 1] == grayBrush && brushBlockList[iCnt - 1][y + 1] == oldBrush)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, nodeDir, x, y);

				if (newOpenNode != nullptr)
				{
					openList.PushBack(newOpenNode);
				}

				return true;
			}
		}

		
	}

	return false;
}


//=======================================================
// 대각선 수직 위 방향
//=======================================================
bool JumpPointSearch::CheckUpDiagonalVertical(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush)
{
	NODE* newOpenNode;

	for (int iCnt = y; iCnt >= 0; --iCnt)
	{
		// 장애물 만났을 경우 return
		if (brushBlockList[x][iCnt] == grayBrush)
		{
			return false;
		}
		
		//=================================================
		// 목적지 만났을 경우 노드 생성 후 return
		//=================================================
		if (x == destNode->mX && iCnt == destNode->mY)
		{
			newOpenNode = SetCornerNode(parentNode, destNode, nodeDir, x, y);

			if (newOpenNode != nullptr)
			{
				openList.PushBack(newOpenNode);
			}

			return true;
		}

		brushBlockList[x][iCnt] = randBrush;



		if (x > 0 && iCnt > 0)
		{
			if (brushBlockList[x - 1][iCnt] == grayBrush && brushBlockList[x - 1][iCnt - 1] == oldBrush)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, nodeDir, x, y);

				if (newOpenNode != nullptr)
				{
					openList.PushBack(newOpenNode);
				}

				return true;
			}
		}

		if (x + 1 < MAX_WIDTH && iCnt > 0)
		{

			if (brushBlockList[x + 1][iCnt] == grayBrush && brushBlockList[x + 1][iCnt - 1] == oldBrush)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, nodeDir, x, y);

				if (newOpenNode != nullptr)
				{
					openList.PushBack(newOpenNode);
				}

				return true;
			}
		}
	}

	return false;
}


bool JumpPointSearch::CheckDownDiagonalVertical(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush)
{

	NODE* newOpenNode;

	for (int iCnt = y; iCnt < MAX_HEIGHT; iCnt++)
	{
		if (brushBlockList[x][iCnt] == grayBrush)
		{
			return false;
		}	

		if (x == destNode->mX && iCnt == destNode->mY)
		{
			newOpenNode = SetCornerNode(parentNode, destNode, nodeDir, x, y);

			if (newOpenNode != nullptr)
			{
				openList.PushBack(newOpenNode);
			}

			return true;
		}

		brushBlockList[x][iCnt] = randBrush;

		if (x > 0 && iCnt + 1 < MAX_HEIGHT)
		{
			if (brushBlockList[x - 1][iCnt] == grayBrush && brushBlockList[x - 1][iCnt + 1] == oldBrush)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, nodeDir, x, y);

				if (newOpenNode != nullptr)
				{
					openList.PushBack(newOpenNode);
				}

				return true;
			}
		}

		if (x + 1 < MAX_WIDTH && iCnt + 1 < MAX_HEIGHT)
		{

			if (brushBlockList[x + 1][iCnt] == grayBrush && brushBlockList[x + 1][iCnt + 1] == oldBrush)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, nodeDir, x, y);

				if (newOpenNode != nullptr)
				{
					openList.PushBack(newOpenNode);
				}

				return true;
			}
		}
	}

	return false;
}


//================================================================
// 오픈 리스트에서 정렬되어 있던 F값 노드를 뽑는다.
//================================================================
JumpPointSearch::NODE* JumpPointSearch::SelectOpenListNode()
{
	// F값이 작은 순서로 정렬해놨기 때문에 begin값을 뽑으면 첫 노드를 뽑을 수 있다.
	CList<JumpPointSearch::NODE*>::Iterator iter = openList.begin();

	if ((*iter)->data == nullptr)
	{
		return nullptr;
	}

	// 노드의 값을 복사한다.
	NODE* node = (*iter)->data;

	// 오픈 리스트에서 해당 노드를 지운다.
	openList.erase(iter);

	// closeList에 넣어준다.
	closeList.PushFront(node);

	// 클로즈리스트이니 색깔을 바꾸어준다.
	brushBlockList[node->mX][node->mY] = yellowBrush;

	return node;
}


//=============================================================
// 모든 리스트와 블럭을 초기화 시킨다.
//=============================================================
void JumpPointSearch::ResetAll()
 {	
	if (funcSetFlag == false)
	{
		free(startNode);

 		free(destinationNode);
	}

	functionFlag = false;

	funcSetFlag = true;

	ResetOpenList();

	ResetCloseList();

	ResetRouteList();

	OptimizeRouteReset();

	ResetBlock();
}


void JumpPointSearch::ReStart()
{
	if (funcSetFlag == false)
	{
		free(startNode);

		free(destinationNode);
	}

	functionFlag = true;

	funcSetFlag = true;

	ResetOpenList();

	ResetCloseList();

	ResetRouteList();

	RouteReset();

	OptimizeRouteReset();
}


//=============================================================
// 오픈리스트 노드를 리셋한다.
//=============================================================
void JumpPointSearch::ResetOpenList()
{
	CList<JumpPointSearch::NODE*>::Iterator iterE = openList.end();

	for (CList<JumpPointSearch::NODE*>::Iterator iter = openList.begin(); iter != iterE;)
	{
		iter = openList.erase(iter);
	}
}



//=============================================================
// 클로즈리스트 노드를 리셋한다.
//=============================================================
void JumpPointSearch::ResetCloseList()
{
	CList<JumpPointSearch::NODE*>::Iterator iterE = closeList.end();

	for (CList<JumpPointSearch::NODE*>::Iterator iter = closeList.begin(); iter != iterE;)
	{
		iter = closeList.erase(iter);
	}
}


void JumpPointSearch::ResetRouteList()
{
	CList<JumpPointSearch::NODE*>::Iterator iterE = routeList.end();

	for (CList<JumpPointSearch::NODE*>::Iterator iter = routeList.begin(); iter != iterE;)
	{
		iter = routeList.erase(iter);
	}
}


void JumpPointSearch::OptimizeRouteReset()
{
	CList<JumpPointSearch::NODE*>::Iterator iterE = optimizeRouteList.end();

	for (CList<JumpPointSearch::NODE*>::Iterator iter = optimizeRouteList.begin(); iter != iterE;)
	{
		iter = routeList.erase(iter);
	}
}



//=============================================================
// 블럭들을 리셋한다.
//=============================================================
void JumpPointSearch::ResetBlock()
{

	for (int iCntY = 0; iCntY < MAX_HEIGHT; iCntY++)
	{
		for (int iCntX = 0; iCntX < MAX_WIDTH; iCntX++)
		{
			brushBlockList[iCntX][iCntY] = oldBrush;
		}
	}
}



void JumpPointSearch::RouteReset()
{
	for (int iCntY = 0; iCntY < MAX_HEIGHT; iCntY++)
	{
		for (int iCntX = 0; iCntX < MAX_WIDTH; iCntX++)
		{

			if (brushBlockList[iCntX][iCntY] != redBrush && brushBlockList[iCntX][iCntY] != greenBrush && brushBlockList[iCntX][iCntY] != grayBrush)
			{
				brushBlockList[iCntX][iCntY] = oldBrush;
			}
		}
	}
}


void JumpPointSearch::InsertRoute(NODE* node)
{

	while (1)
	{	
		routeList.PushFront(node);

		optimizeRouteList.PushFront(node);

		if (node->prev == nullptr)
		{
			break;
		}

		node = node->prev;
	}

}


void JumpPointSearch::PathOptimizing()
{

	int startX;
	int startY;

	int endX;
	int endY;

	int nextEndX;
	int nextEndY;

	CList<JumpPointSearch::NODE*>::Iterator nextIter;

	CList<JumpPointSearch::NODE*>::Iterator nextNextIter;


	CList<JumpPointSearch::NODE*>::Iterator iterE = optimizeRouteList.end();

	CList<JumpPointSearch::NODE*>::Iterator iter = optimizeRouteList.begin();

	while(1)
	{
		startX = iter->mX;
		startY = iter->mY;

		nextIter = iter.NextIter();
		
		if (nextIter == iterE)
		{
			break;
		}

		endX = nextIter->mX;
		endY = nextIter->mY;

		nextNextIter = nextIter;

		while (1)
		{
			nextNextIter = nextNextIter.NextIter();

			if (nextNextIter == iterE)
			{

				--nextNextIter;

				iter = nextIter;
				break;
			}

			nextEndX = nextNextIter->mX;
			nextEndY = nextNextIter->mY;

			if (BresenhamLine::MakeLine(startX, startY, nextEndX, nextEndY))
			{
				
				(*nextIter)->deleteCheck = true;
			}
			else
			{
				--nextNextIter;

				iter = nextIter;

				break;
			}
		}
	}



	for (iter = optimizeRouteList.begin(); iter != iterE;)
	{
		if ((*iter)->deleteCheck)
		{
			iter = optimizeRouteList.erase(iter);
		}
		else
		{
			++iter;
		}
	}

}


//=============================================================
// F값 작은 순으로 정렬한다.
//=============================================================
void JumpPointSearch::openListBubbleSort() {

	CList<JumpPointSearch::NODE*>::Iterator iterE = openList.end();

	--iterE;

	for (int iCnt = 0; iCnt < openList.listLength; iCnt++)
	{
		for (CList<JumpPointSearch::NODE*>::Iterator iter = openList.begin(); iter != iterE; ++iter)
		{
			CList<JumpPointSearch::NODE*>::Iterator iterN = iter.node->next;

			if (iter->F > iterN->F)
			{
				openList.DataSwap(iter, iterN);
			}
		}

		--iterE;
	}

}