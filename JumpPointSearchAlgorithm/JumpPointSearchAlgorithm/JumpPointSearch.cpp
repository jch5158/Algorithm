#include "stdafx.h"
#include "CList.h"
#include "JumpPointSearchAlgorithm.h"
#include "JumpPointSearch.h"


BYTE JumpPointSearch::blockList[MAX_WIDTH][MAX_HEIGHT] = { (BYTE)BLOCK_COLOR::BASIC, };

CList<JumpPointSearch::NODE*> JumpPointSearch::openList;

CList<JumpPointSearch::NODE*> JumpPointSearch::closeList;

//===================================================================
// 타이머로 자동 함수 호출 시 로직 실행이 되지 않도록 하는 Flag
//===================================================================
bool JumpPointSearch::functionFlag = false;

bool JumpPointSearch::funcSetFlag = true;

HWND hWnd;

HDC hdc;

// 목적지 노드
JumpPointSearch::NODE* JumpPointSearch::destinationNode = nullptr;

// 시작지 노드
JumpPointSearch::NODE* JumpPointSearch::startNode = nullptr;


JumpPointSearch::NODE* JumpPointSearch::PathFind(int startX, int startY, int destinationX, int destinationY)
{
	
	// 현재 노드
	static JumpPointSearch::NODE* curNode = nullptr;	 


	hdc = GetDC(hWnd);

	MoveToEx(hdc, 10, 10, nullptr);

	LineTo(hdc, 100, 50);

	ReleaseDC(hWnd, hdc);


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

	switch ((NODE_DIRECTION)node->mNodeDir)
	{
	case NODE_DIRECTION::NODE_DIR_RR:

		retNode = CheckRightHorizontal(node, destNode, x, y);
		if (retNode == destNode)
		{
			return retNode;
		}

		break;
	case NODE_DIRECTION::NODE_DIR_RD:

		retNode = CheckRightDown(node, destNode, x, y);
		if (retNode == destNode)
		{
			return retNode;
		}

		break;
	case NODE_DIRECTION::NODE_DIR_DD:
		
		retNode = CheckDownVertical(node, destNode, x, y);
		if (retNode == destNode)
		{
			return retNode;
		}

		break;
	case NODE_DIRECTION::NODE_DIR_LD:

		retNode = CheckLeftDown(node, destNode, x, y);
		if (retNode == destNode)
		{
			return retNode;
		}
		
		break;
	case NODE_DIRECTION::NODE_DIR_LL:
	
		retNode = CheckLeftHorizontal(node, destNode, x, y);
		if (retNode == destNode)
		{
			return retNode;
		}
		
		break;
	case NODE_DIRECTION::NODE_DIR_LU:

		retNode = CheckLeftUp(node, destNode, x, y);
		if (retNode == destNode)
		{
			return retNode;
		}
		
		break;
	case NODE_DIRECTION::NODE_DIR_UU:
		
		retNode = CheckUpVertical(node, destNode, x, y);
		if (retNode == destNode)
		{
			return retNode;
		}

		break;
	case NODE_DIRECTION::NODE_DIR_RU:

		retNode = CheckRightUp(node, destNode, x, y);
		if (retNode == destNode)
		{
			return retNode;
		}

		break;

	case NODE_DIRECTION::NODE_DIR_ALL:

		retNode = CheckRightUp(node, destNode, x, y, false);
		if (retNode == destNode)
		{
			return retNode;
		}

		retNode = CheckRightHorizontal(node, destNode, x, y);
		if (retNode == destNode)
		{
			return retNode;
		}

		retNode = CheckRightDown(node, destNode, x, y,false);
		if (retNode == destNode)
		{
			return retNode;
		}

		retNode = CheckDownVertical(node, destNode, x, y);
		if (retNode == destNode)
		{
			return retNode;
		}

		retNode = CheckLeftDown(node, destNode, x, y,false);
		if (retNode == destNode)
		{
			return retNode;
		}

		retNode = CheckLeftHorizontal(node, destNode, x, y);
		if (retNode == destNode)
		{
			return retNode;
		}

		retNode = CheckLeftUp(node, destNode, x, y,false);
		if (retNode == destNode)
		{
			return retNode;
		}

		retNode = CheckUpVertical(node, destNode, x, y);
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

		retOpenNode = FindOpenList(x, y);

		if (retOpenNode == nullptr)
		{
			newNode = (NODE*)malloc(sizeof(NODE));

			newNode->mX = x;
			newNode->mY = y;
			blockList[newNode->mX][newNode->mY] = (BYTE)BLOCK_COLOR::BLUE;

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

			// 블럭 리스트의 색깔을 바꾼다.
			blockList[x][y] = (BYTE)BLOCK_COLOR::BLUE;

			return newNode;
		}
		else
		{
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
// 직선 가로, 세로, 위, 아래
//===============================================
JumpPointSearch::NODE* JumpPointSearch::CheckRightHorizontal(NODE* parentNode, NODE* destNode, int x, int y)
{
	NODE* newOpenNode = nullptr;

	//=================================
	// 보조 탐색 영억을 생성하는
	//=================================
	if (y > 0 && x + 1 < MAX_WIDTH)
	{
		if (blockList[x][y - 1] == (BYTE)BLOCK_COLOR::GRAY && blockList[x + 1][y - 1] == (BYTE)BLOCK_COLOR::BASIC)
		{	
			CheckRightUp(parentNode, destNode, x, y, false);	
		}
	}

	if (y + 1 < MAX_HEIGHT && x + 1 < MAX_WIDTH)
	{
		if (blockList[x][y + 1] == (BYTE)BLOCK_COLOR::GRAY && blockList[x + 1][y + 1] == (BYTE)BLOCK_COLOR::BASIC)
		{
			CheckRightDown(parentNode, destNode, x, y, false);
		}
	}
	
	//============================================================================
	// iCnt = x + 1 을 하는 이유는 해당 위치는 현재 노드가 있는 위치이기 때문이다. 
	// iCnt + 1 < MAX_WIDTH 인 이유는 위와 아래의 벽이 있을 경우 X + 1 좌표에 
	// 벽이 없는지 확인하기 때문으로 오버플로우가 발생되지 않도록 하기 위함입니다.
	//============================================================================
	for (int iCnt = x + 1; iCnt + 1 < MAX_WIDTH; ++iCnt)
	{
		// 벽을 만났으 경우 retur nullptr 을 한다.
		if (blockList[iCnt][y] == (BYTE)BLOCK_COLOR::GRAY)
		{
			return nullptr;
		}

		// 목적지를 만날 경우 바로 이어준다. 
		if (iCnt == destNode->mX && y == destNode->mY)
		{
			destNode->prev = parentNode;

			return destNode;
		}

		//===================================================
		// y 값이 0 이상일 경우에만 위의 벽을 확인할 수 있다.
		//===================================================
		if (y > 0)
		{
			if (blockList[iCnt][y - 1] == (BYTE)BLOCK_COLOR::GRAY && blockList[iCnt + 1][y - 1] == (BYTE)BLOCK_COLOR::BASIC)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RR, iCnt, y);
				if(newOpenNode != nullptr)
				{
					openList.PushBack(newOpenNode);
				}
				
				return newOpenNode;
			}
		}

		if (y + 1 < MAX_WIDTH)
		{
			if (blockList[iCnt][y + 1] == (BYTE)BLOCK_COLOR::GRAY && blockList[iCnt + 1][y + 1] == (BYTE)BLOCK_COLOR::BASIC)
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

JumpPointSearch::NODE* JumpPointSearch::CheckLeftHorizontal(NODE* parentNode, NODE* destNode, int x, int y)
{
	NODE* newOpenNode = nullptr;


	if (y > 0 && x > 0)
	{
		if (blockList[x][y - 1] == (BYTE)BLOCK_COLOR::GRAY && blockList[x - 1][y - 1] == (BYTE)BLOCK_COLOR::BASIC)
		{		
			CheckLeftUp(parentNode, destNode, x, y, false);
		}
	}

	if (y + 1 < MAX_HEIGHT && x > 0)
	{
		if (blockList[x][y + 1] == (BYTE)BLOCK_COLOR::GRAY && blockList[x - 1][y + 1] == (BYTE)BLOCK_COLOR::BASIC)
		{
			CheckLeftDown(parentNode, destNode, x, y, false);
		}
	}

	for (int iCnt = x - 1; iCnt - 1 > 0; --iCnt)
	{
		if (blockList[iCnt][y] == (BYTE)BLOCK_COLOR::GRAY)
		{
			return nullptr;
		}

		if (iCnt == destNode->mX && y == destNode->mY)
		{
			destNode->prev = parentNode;

			return destNode;
		}

		if (y > 0)
		{
			if (blockList[iCnt][y - 1] == (BYTE)BLOCK_COLOR::GRAY && blockList[iCnt - 1][y - 1] == (BYTE)BLOCK_COLOR::BASIC)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LL, iCnt, y);

				if (newOpenNode != nullptr)
				{
					openList.PushBack(newOpenNode);
				}

				return newOpenNode;	
			}
		}


		if (y + 1 < MAX_HEIGHT)
		{
			if (blockList[iCnt][y + 1] == (BYTE)BLOCK_COLOR::GRAY && blockList[iCnt - 1][y + 1] == (BYTE)BLOCK_COLOR::BASIC)
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

JumpPointSearch::NODE* JumpPointSearch::CheckUpVertical(NODE* parentNode, NODE* destNode, int x, int y)
{
	NODE* newOpenNode = nullptr;

	if (x > 0 && y > 0)
	{
		if (blockList[x - 1][y] == (BYTE)BLOCK_COLOR::GRAY && blockList[x - 1][y - 1] == (BYTE)BLOCK_COLOR::BASIC)
		{
			CheckLeftUp(parentNode, destNode, x, y, false);
		}
	}

	if (x + 1 < MAX_WIDTH && y > 0)
	{
		if (blockList[x + 1][y] == (BYTE)BLOCK_COLOR::GRAY && blockList[x + 1][y - 1] == (BYTE)BLOCK_COLOR::BASIC)
		{ 
		
			CheckRightUp(parentNode, destNode, x, y, false);
		}
	}

	for (int iCnt = y + 1; iCnt - 1 > 0; iCnt--)
	{

		if (blockList[x][iCnt] == (BYTE)BLOCK_COLOR::GRAY)
		{
			return nullptr;
		}

		if (x == destNode->mX && iCnt == destNode->mY)
		{
			destNode->prev = parentNode;

			return destNode;
		}

		if (x > 0)
		{
			if (blockList[x - 1][iCnt] == (BYTE)BLOCK_COLOR::GRAY && blockList[x - 1][iCnt - 1] == (BYTE)BLOCK_COLOR::BASIC)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, NODE_DIRECTION::NODE_DIR_UU, x, iCnt);

				if (newOpenNode != nullptr)
				{
					openList.PushBack(newOpenNode);
				}

				return newOpenNode;
			}
		}


		if (x + 1 < MAX_WIDTH)
		{
			if (blockList[x + 1][iCnt] == (BYTE)BLOCK_COLOR::GRAY && blockList[x + 1][iCnt - 1] == (BYTE)BLOCK_COLOR::BASIC)
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

JumpPointSearch::NODE* JumpPointSearch::CheckDownVertical(NODE* parentNode, NODE* destNode, int x, int y)
{
	NODE* newOpenNode = nullptr;

	if (x > 0 && y + 1 < MAX_HEIGHT)
	{
		if (blockList[x - 1][y] == (BYTE)BLOCK_COLOR::GRAY && blockList[x - 1][y + 1] == (BYTE)BLOCK_COLOR::BASIC)
		{
			CheckLeftDown(parentNode, destNode, x, y, false);
		}
	}

	if (x + 1 < MAX_WIDTH && y + 1 < MAX_HEIGHT)
	{
		if (blockList[x + 1][y] == (BYTE)BLOCK_COLOR::GRAY && blockList[x + 1][y + 1] == (BYTE)BLOCK_COLOR::BASIC)
		{
			CheckRightDown(parentNode, destNode, x, y, false);
		}
	}


	for (int iCnt = y; iCnt + 1 < MAX_HEIGHT; iCnt++)
	{

		if (blockList[x][iCnt] == (BYTE)BLOCK_COLOR::GRAY)
		{
			return nullptr;
		}

		if (x == destNode->mX && iCnt == destNode->mY)
		{
			destNode->prev = parentNode;

			return destNode;
		}

		if (x > 0)
		{
			if (blockList[x - 1][iCnt] == (BYTE)BLOCK_COLOR::GRAY && blockList[x - 1][iCnt + 1] == (BYTE)BLOCK_COLOR::BASIC)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, NODE_DIRECTION::NODE_DIR_DD, x, iCnt);

				if (newOpenNode != nullptr)
				{
					openList.PushBack(newOpenNode);
				}

				return newOpenNode;
			}
		}

		if (x + 1 < MAX_WIDTH)
		{
			if (blockList[x + 1][iCnt] == (BYTE)BLOCK_COLOR::GRAY && blockList[x + 1][iCnt + 1] == (BYTE)BLOCK_COLOR::BASIC)
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
//대각선 
//======================================================
JumpPointSearch::NODE* JumpPointSearch::CheckRightUp(NODE* parentNode, NODE* destNode, int x, int y, bool firstCall)
{	
	bool retOpenNode;

	bool rightUpFlag = true;

	bool leftUpFlag = false;
	bool rightDownFlag = false;
	
	int xCount = 0;
	int yCount = 0;

	if (firstCall)
	{
		if (destNode == CheckRightHorizontal(parentNode, destNode, x, y))
		{
			return destNode;
		}

		if (destNode == CheckUpVertical(parentNode, destNode, x, y))
		{
			return destNode;
		}
	}

	if (x > 0 && y > 0 )
	{
		if (blockList[x - 1][y] == (BYTE)BLOCK_COLOR::GRAY && blockList[x - 1][y - 1] == (BYTE)BLOCK_COLOR::BASIC)
		{
			leftUpFlag = true;
		}
	}

	if (x + 1 < MAX_WIDTH && y + 1 < MAX_HEIGHT)
	{
		if (blockList[x][y + 1] == (BYTE)BLOCK_COLOR::GRAY && blockList[x + 1][y + 1] == (BYTE)BLOCK_COLOR::BASIC)
		{
			rightDownFlag = true;
		}
	}

	while (1)
	{
		xCount += 1;

		yCount += 1;

		if (rightUpFlag)
		{
			do
			{
				if (x + xCount >= MAX_WIDTH || y - yCount < 0)
				{
					rightUpFlag = false;
					break;
				}

				if (blockList[x + xCount][y - yCount] == (BYTE)BLOCK_COLOR::GRAY)
				{
					rightUpFlag = false;
					break;
				}

				retOpenNode = CheckRightDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RU, x + xCount, y - yCount);
				if (retOpenNode)
				{
					rightUpFlag = false;
					break;
				}

				retOpenNode = CheckUpDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RU, x + xCount, y - yCount);
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

				if (blockList[x - xCount][y - yCount] == (BYTE)BLOCK_COLOR::GRAY)
				{
					leftUpFlag = false;
				}

				retOpenNode = CheckLeftDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LU, x - xCount, y - yCount);
				if (retOpenNode)
				{
					leftUpFlag = false;
					break;
				}

				retOpenNode = CheckUpDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LU, x - xCount, y - yCount);
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

				if (blockList[x + xCount][y + yCount] == (BYTE)BLOCK_COLOR::GRAY)
				{
					rightDownFlag = false;
					break;
				}

				retOpenNode = CheckRightDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RD, x + xCount, y + yCount);
				if (retOpenNode)
				{
					rightDownFlag = false;
					break;
				}

				retOpenNode = CheckDownDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RD, x + xCount, y + yCount);
				if (retOpenNode)
				{
					rightDownFlag = false;
					break;
				}

			} while (0);
		}
		
		if (rightUpFlag == false && rightDownFlag == false && leftUpFlag == false)
		{
			break;
		}
	}

	return nullptr;
}

JumpPointSearch::NODE* JumpPointSearch::CheckRightDown(NODE* parentNode, NODE* destNode, int x, int y,bool firstCall)
{
	bool retOpenNode;

	bool rightDownFlag = true;

	bool rightUpFlag = false;
	bool leftDownFlag = false;
	
	int xCount = 0;

	int yCount = 0;

	if (firstCall)
	{
		if (destNode == CheckRightHorizontal(parentNode, destNode, x, y))
		{
			return destNode;
		}

		if (destNode == CheckDownVertical(parentNode, destNode, x, y))
		{
			return destNode;
		}
	}

	if (x + 1 < MAX_WIDTH && y > 0)
	{
		if (blockList[x][y - 1] == (BYTE)BLOCK_COLOR::GRAY && blockList[x + 1][y - 1] == (BYTE)BLOCK_COLOR::BASIC)
		{	
			rightUpFlag = true;
		}
	}

	if (x > 0 && y + 1 < MAX_HEIGHT)
	{
		if (blockList[x - 1][y] == (BYTE)BLOCK_COLOR::GRAY && blockList[x - 1][y + 1] == (BYTE)BLOCK_COLOR::BASIC)
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

				if (blockList[x + xCount][y + yCount] == (BYTE)BLOCK_COLOR::GRAY)
				{
					rightDownFlag = false;
					break;
				}

				retOpenNode = CheckRightDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RD, x + xCount, y + yCount);
				if (retOpenNode)
				{
					rightDownFlag = false;
					break;
				}

				retOpenNode = CheckDownDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RD, x + xCount, y + yCount);
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

				if (blockList[x + xCount][y - yCount] == (BYTE)BLOCK_COLOR::GRAY)
				{
					rightUpFlag = false;
					break;
				}

				retOpenNode = CheckRightDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RU, x + xCount, y - yCount);
				if (retOpenNode)
				{
					rightUpFlag = false;
					break;
				}

				retOpenNode = CheckUpDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RU, x + xCount, y - yCount);
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

				if (blockList[x - xCount][y + yCount] == (BYTE)BLOCK_COLOR::GRAY)
				{
					leftDownFlag = false;
					break;
				}

				retOpenNode = CheckLeftDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LD, x - xCount, y + yCount);
				if(retOpenNode)
				{
					leftDownFlag = false;
					break;
				}

				retOpenNode = CheckDownDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LD, x - xCount, y + yCount);
				if (retOpenNode)
				{
					leftDownFlag = false;
					break;
				}

			} while (0);
		}

		if (rightDownFlag == false && rightUpFlag == false && leftDownFlag == false)
		{
			break;
		}

	}
}

JumpPointSearch::NODE* JumpPointSearch::CheckLeftUp(NODE* parentNode, NODE* destNode, int x, int y,bool firstCall)
{
	bool retOpenNode;
	
	bool leftUpFlag = true;
	bool leftDownFlag = false;
	bool rightUpFlag = false;

	int xCount = 0;
	int yCount = 0;

	if (firstCall == true)
	{
		if (destNode == CheckLeftHorizontal(parentNode, destNode, x, y))
		{
			return destNode;
		}

		if (destNode == CheckUpVertical(parentNode, destNode, x, y))
		{
			return destNode;
		}
	}

	if (x + 1 < MAX_WIDTH && y > 0)
	{
		if (blockList[x + 1][y] == (BYTE)BLOCK_COLOR::GRAY && blockList[x + 1][y - 1] == (BYTE)BLOCK_COLOR::BASIC)
		{
			rightUpFlag = true;
		}
	}

	if (x > 0 && y + 1 < MAX_HEIGHT)
	{
		if (blockList[x][y + 1] == (BYTE)BLOCK_COLOR::GRAY && blockList[x - 1][y + 1] == (BYTE)BLOCK_COLOR::BASIC)
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

				if (blockList[x - xCount][y - yCount] == (BYTE)BLOCK_COLOR::GRAY)
				{
					leftUpFlag = false;
					break;
				}

				retOpenNode = CheckLeftDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LU, x - xCount, y - yCount);
				if (retOpenNode)
				{
					leftUpFlag = false;
					break;
				}

				retOpenNode = retOpenNode = CheckUpDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LU, x -xCount, y - yCount);
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

				if (blockList[x + xCount][y - yCount] == (BYTE)BLOCK_COLOR::GRAY)
				{
					rightUpFlag = false;
					break;
				}

				retOpenNode = CheckRightDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RU, x + xCount, y - yCount);
				if (retOpenNode)
				{
					rightUpFlag = false;
					break;
				}

				retOpenNode = CheckUpDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RU, x + xCount, y - yCount);
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

				if (blockList[x - xCount][y + yCount] == (BYTE)BLOCK_COLOR::GRAY)
				{
					leftDownFlag = false;
					break;
				}

				retOpenNode = CheckLeftDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LD, x - xCount, y + yCount);
				if(retOpenNode)
				{
					leftDownFlag = false;
					break;
				}

				retOpenNode = CheckDownDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LD, x - xCount, y + yCount);
				if (retOpenNode)
				{
					leftDownFlag = false;
					break;
				}
			} while (0);
		}

		if (leftUpFlag == false && rightUpFlag == false && leftDownFlag == false)
		{
			break;
		}
	}

}

JumpPointSearch::NODE* JumpPointSearch::CheckLeftDown(NODE* parentNode, NODE* destNode, int x, int y,bool firstCall)
{
	bool retOpenNode;

	bool leftDownFlag = true;
	bool leftUpFlag = false;
	bool rightDownFlag = false;

	int xCount = 0;
	int yCount = 0;

	if (firstCall == true)
	{
		if (destNode == CheckLeftHorizontal(parentNode, destNode, x, y))
		{
			return destNode;
		}

		if (destNode == CheckDownVertical(parentNode, destNode, x, y))
		{
			return destNode;
		}
	}

	if (x > 0 && y > 0)
	{
		if (blockList[x][y - 1] == (BYTE)BLOCK_COLOR::GRAY && blockList[x - 1][y - 1] == (BYTE)BLOCK_COLOR::BASIC)
		{
			leftUpFlag = true;
		}
	}

	if (x + 1 < MAX_WIDTH && y + 1 < MAX_HEIGHT)
	{
		if (blockList[x + 1][y] == (BYTE)BLOCK_COLOR::GRAY && blockList[x + 1][y + 1] == (BYTE)BLOCK_COLOR::BASIC)
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
				if (x - xCount < 0 || y + yCount > MAX_HEIGHT)
				{
					leftDownFlag = false;
					break;
				}

				if (blockList[x- xCount][y + yCount] == (BYTE)BLOCK_COLOR::GRAY)
				{
					leftDownFlag = false;
					break;
				}

				retOpenNode = CheckLeftDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LD, x - xCount, y + yCount);
				if (retOpenNode)
				{
					leftDownFlag = false;
					break;
				}

				retOpenNode = CheckDownDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LD, x - xCount, y + yCount);
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

				if (blockList[x - xCount][y - yCount] == (BYTE)BLOCK_COLOR::GRAY)
				{
					leftUpFlag = false;
					break;
				}

				retOpenNode = CheckLeftDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LU, x - xCount, y - yCount);
				if (retOpenNode)
				{
					leftUpFlag = false;
					break;
				}

				retOpenNode = CheckUpDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LU, x - xCount, y - yCount);
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

				if (blockList[x + xCount][y + yCount] == (BYTE)BLOCK_COLOR::GRAY)
				{
					rightDownFlag = false;
					break;
				}

				retOpenNode = CheckRightDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RD, x + xCount, y + yCount);
				if (retOpenNode)
				{
					rightDownFlag = false;
					break;
				}

				retOpenNode = CheckDownDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RD, x + xCount, y + yCount);
				if (retOpenNode)
				{
					rightDownFlag = false;
					break;
				}

			} while (0);
		}

		if (leftDownFlag == false && leftUpFlag == false && rightDownFlag == false)
		{
			break;
		}
		
	}
}


//=======================================================
// 대각에서 수평 방향 
//=======================================================
bool JumpPointSearch::CheckRightDiagonalHorizontal(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir,int x, int y)
{

	NODE* newOpenNode;

	for (int iCnt = x; iCnt + 1 < MAX_WIDTH; ++iCnt)
	{
		if (blockList[iCnt][y] == (BYTE)BLOCK_COLOR::GRAY)
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

		if (y > 0)
		{
			if (blockList[iCnt][y - 1] == (BYTE)BLOCK_COLOR::GRAY  && blockList[iCnt + 1][y - 1] == (BYTE)BLOCK_COLOR::BASIC)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, nodeDir, x, y);

				if (newOpenNode != nullptr)
				{
					openList.PushBack(newOpenNode);
				}

				return true;
			}
		}

		if(y + 1 < MAX_HEIGHT)
		{
			if (blockList[iCnt][y + 1] == (BYTE)BLOCK_COLOR::GRAY && blockList[iCnt + 1][y + 1] == (BYTE)BLOCK_COLOR::BASIC)
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


bool JumpPointSearch::CheckLeftDiagonalHorizontal(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir, int x, int y)
{

	NODE* newOpenNode;

	for (int iCnt = x; iCnt > 0; --iCnt)
	{
		if (blockList[iCnt][y] == (BYTE)BLOCK_COLOR::GRAY)
		{
			return false;
		}		

		if (iCnt == destNode->mX && y == destNode->mY)
		{
			newOpenNode = SetCornerNode(parentNode, destNode, nodeDir, x, y);

			if (newOpenNode != nullptr)
			{
				openList.PushBack(newOpenNode);
			}

			return true;
		}

		if (y > 0)
		{
			if (blockList[iCnt][y - 1] == (BYTE)BLOCK_COLOR::GRAY && blockList[iCnt - 1][y - 1] == (BYTE)BLOCK_COLOR::BASIC)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, nodeDir, x, y);

				if (newOpenNode != nullptr)
				{
					openList.PushBack(newOpenNode);
				}

				return true;
			}
		}

		if (y + 1 < MAX_HEIGHT)
		{
			if(blockList[iCnt][y + 1] == (BYTE)BLOCK_COLOR::GRAY && blockList[iCnt - 1][y + 1] == (BYTE)BLOCK_COLOR::BASIC)
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
// 대각선 수직 방향
//=======================================================
bool JumpPointSearch::CheckUpDiagonalVertical(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir, int x, int y)
{
	NODE* newOpenNode;

	for (int iCnt = y; iCnt > 0; --iCnt)
	{
		if (blockList[x][iCnt] == (BYTE)BLOCK_COLOR::GRAY)
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

		if (x > 0)
		{
			if (blockList[x - 1][iCnt] == (BYTE)BLOCK_COLOR::GRAY && blockList[x - 1][iCnt - 1] == (BYTE)BLOCK_COLOR::BASIC)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, nodeDir, x, y);

				if (newOpenNode != nullptr)
				{
					openList.PushBack(newOpenNode);
				}

				return true;
			}
		}

		if (x + 1 < MAX_WIDTH)
		{

			if (blockList[x + 1][iCnt] == (BYTE)BLOCK_COLOR::GRAY && blockList[x + 1][iCnt - 1] == (BYTE)BLOCK_COLOR::BASIC )
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


bool JumpPointSearch::CheckDownDiagonalVertical(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir, int x, int y)
{

	NODE* newOpenNode;

	for (int iCnt = y; iCnt + 1 < MAX_HEIGHT; iCnt++)
	{
		if (blockList[x][iCnt] == (BYTE)BLOCK_COLOR::GRAY)
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

		if (x > 0)
		{
			if (blockList[x - 1][iCnt] == (BYTE)BLOCK_COLOR::GRAY && blockList[x - 1][iCnt + 1] == (BYTE)BLOCK_COLOR::BASIC)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, nodeDir, x, y);

				if (newOpenNode != nullptr)
				{
					openList.PushBack(newOpenNode);
				}

				return true;
			}
		}

		if (x + 1 < MAX_WIDTH)
		{

			if (blockList[x + 1][iCnt] == (BYTE)BLOCK_COLOR::GRAY && blockList[x + 1][iCnt + 1] == (BYTE)BLOCK_COLOR::BASIC)
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
	blockList[node->mX][node->mY] = (BYTE)BLOCK_COLOR::YELLOW;

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

	RouteReset();
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


//=============================================================
// 블럭들을 리셋한다.
//=============================================================
void JumpPointSearch::ResetBlock()
{

	for (int iCntY = 0; iCntY < MAX_HEIGHT; iCntY++)
	{
		for (int iCntX = 0; iCntX < MAX_WIDTH; iCntX++)
		{
			blockList[iCntX][iCntY] = (BYTE)BLOCK_COLOR::BASIC;
		}
	}
}



void JumpPointSearch::RouteReset()
{
	for (int iCntY = 0; iCntY < MAX_HEIGHT; iCntY++)
	{
		for (int iCntX = 0; iCntX < MAX_WIDTH; iCntX++)
		{

			if (blockList[iCntX][iCntY] == (BYTE)BLOCK_COLOR::BLUE || blockList[iCntX][iCntY] == (BYTE)BLOCK_COLOR::YELLOW)
			{
				blockList[iCntX][iCntY] = (BYTE)BLOCK_COLOR::BASIC;
			}
		}
	}
}

//=============================================================
// F값 작은 순으로 정렬한다.
//=============================================================
void JumpPointSearch::openListBubbleSort() {

	CList<JumpPointSearch::NODE*>::Iterator iterE = JumpPointSearch::openList.end();

	--iterE;

	for (int iCnt = 0; iCnt < JumpPointSearch::openList.listLength; iCnt++)
	{
		for (CList<JumpPointSearch::NODE*>::Iterator iter = JumpPointSearch::openList.begin(); iter != iterE; ++iter)
		{
			CList<JumpPointSearch::NODE*>::Iterator iterN = iter.node->next;

			if (iter->F > iterN->F)
			{
				JumpPointSearch::openList.DataSwap(iter, iterN);
			}
		}

		--iterE;
	}

}