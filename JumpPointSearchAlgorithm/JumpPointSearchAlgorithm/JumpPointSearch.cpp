#include "stdafx.h"
#include "CList.h"
#include "JumpPointSearch.h"


BYTE JumpPointSearch::blockList[MAX_WIDTH][MAX_HEIGHT] = { (BYTE)BLOCK_COLOR::BASIC, };

CList<JumpPointSearch::NODE*> JumpPointSearch::openList;

CList<JumpPointSearch::NODE*> JumpPointSearch::closeList;

bool JumpPointSearch::functionFlag = false;


// 목적지 노드
JumpPointSearch::NODE* JumpPointSearch::destinationNode = nullptr;

// 시작지 노드
JumpPointSearch::NODE* JumpPointSearch::startNode = nullptr;


JumpPointSearch::NODE* JumpPointSearch::PathFind(int startX, int startY, int destinationX, int destinationY)
{
	static bool funcSet = true;

	// 현재 노드
	static JumpPointSearch::NODE* curNode = nullptr;


	// 엔터를 입력하기 전까지 해당 함수를 호출 시 바로 리턴됩니다.
	if (functionFlag == false)
	{
		return nullptr;
	}

	if (funcSet == true)
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
		funcSet = false;
	}


	NODE* retval;

	// 오픈 리스트를 만들기 위해서 현재 노드 위치랑 목적지 위치를 인자로 전달한다.
	retval = InsertOpenNode(curNode, destinationNode);
	// false일 경우 목적지까지 노드를 만들었다.
	if (retval != nullptr)
	{
		funcSet = true;
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


JumpPointSearch::NODE* JumpPointSearch::InsertOpenNode(JumpPointSearch::NODE* node, JumpPointSearch::NODE* destinationNode)
{
	NODE* retOpenNode;

	NODE* openNode;

	int posX = node->mX;
	int posY = node->mY;

	int newNodePosX = 0;
	int newNodePosY = 0;




	



	

	return nullptr;
}


JumpPointSearch::NODE* JumpPointSearch::CheckDirection(NODE* node, int* x, int* y)
{
	NODE* retNode;

	switch ((NODE_DIRECTION)node->mNodeDir)
	{
	case NODE_DIRECTION::NODE_DIR_RR:

		retNode = CheckRightHorizontal(node, destinationNode, *x, *y);
		if (retNode != nullptr)
		{

			return retNode;
		}

		break;
	case NODE_DIRECTION::NODE_DIR_RD:

		
		break;
	case NODE_DIRECTION::NODE_DIR_DD:

		
		break;
	case NODE_DIRECTION::NODE_DIR_LD:

		
		break;
	case NODE_DIRECTION::NODE_DIR_LL:
	
		
		break;
	case NODE_DIRECTION::NODE_DIR_LU:

		
		break;
	case NODE_DIRECTION::NODE_DIR_UU:
		

		break;
	case NODE_DIRECTION::NODE_DIR_RU:

		break;

	case NODE_DIRECTION::NODE_DIR_ALL:

		break;

	default:
		return nullptr;
	}

}


JumpPointSearch::NODE* JumpPointSearch::SetCornerNode(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir, int x, int y)
{
	NODE* newNode;

	newNode = (NODE*)malloc(sizeof(NODE));

	newNode->mX = x;
	newNode->mY = y;
	blockList[newNode->mX][newNode->mY] = (BYTE)BLOCK_COLOR::BLUE;

	newNode->G = (parentNode->mX - newNode->mX) * +1.5f;

	int subX = abs(parentNode->mX - newNode->mX);
	int subY = abs(parentNode->mY - newNode->mY);

	if (subX >= 1 && subY >= 1)
	{
		newNode->G = ((float)subX) * 1.5f;
	}
	else
	{
		newNode->G = ((float)subX) * 1.0f;
	}

	newNode->H = (float)(abs(destNode->mX - newNode->mX) + abs(destNode->mY - newNode->mY));

	newNode->F = newNode->G + newNode->H;

	newNode->mNodeDir = (BYTE)nodeDir;

	newNode->prev = parentNode;

	// 블럭 리스트의 색깔을 바꾼다.
	blockList[x][y] = (BYTE)BLOCK_COLOR::BLUE;

	return newNode;
}



JumpPointSearch::NODE* JumpPointSearch::CheckRightHorizontal(NODE* parentNode, NODE* destNode, int x, int y)
{
	NODE* newOpenNode;

	NODE* assistNode;

	for (int iCnt = x; iCnt + 1 < MAX_WIDTH; ++iCnt)
	{
		if (y > 0 && y + 1 < MAX_HEIGHT)
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


			if (blockList[iCnt][y - 1] == (BYTE)BLOCK_COLOR::GRAY && blockList[iCnt + 1][y - 1] == (BYTE)BLOCK_COLOR::BASIC ||
			    blockList[iCnt][y + 1] == (BYTE)BLOCK_COLOR::GRAY && blockList[iCnt + 1][y + 1] == (BYTE)BLOCK_COLOR::BASIC)
			{
				
				newOpenNode = SetCornerNode(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RR, iCnt, y);

				openList.PushBack(newOpenNode);


				// 코너를 확인하여 수직 방향 탐색을 보낸다.
				if (blockList[x + 1][y - 1] == (BYTE)BLOCK_COLOR::BASIC)
				{	
					
					CheckUpDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RU, x + 1, y - 1);

				}

				if (blockList[x + 1][y + 1] == (BYTE)BLOCK_COLOR::BASIC)
				{

					CheckDownDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RD, x + 1, y - 1);

				}

				return newOpenNode;
			}

		}
	}

	return nullptr;
}



JumpPointSearch::NODE* JumpPointSearch::CheckLeftHorizontal(NODE* parentNode, NODE* destNode, int x, int y)
{
	NODE* newOpenNode;

	NODE* assistNode;


	for (int iCnt = x; iCnt - 1 > 0; --iCnt)
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

		if (y > 0 && y + 1 < MAX_HEIGHT)
		{
			if (blockList[iCnt][y - 1] == (BYTE)BLOCK_COLOR::GRAY && blockList[iCnt - 1][y - 1] == (BYTE)BLOCK_COLOR::BASIC || 
				blockList[iCnt][y + 1] == (BYTE)BLOCK_COLOR::GRAY && blockList[iCnt - 1][y + 1] == (BYTE)BLOCK_COLOR::BASIC)	
			{

				newOpenNode = SetCornerNode(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LL, iCnt, y);

				openList.PushBack(newOpenNode);


				// 코너에 오픈 리스트에 넣을 노드를 만든다.
				if (blockList[x - 1][y - 1] == (BYTE)BLOCK_COLOR::BASIC)
				{


				}

				if (blockList[x - 1][y + 1] == (BYTE)BLOCK_COLOR::BASIC)
				{
			
				
				}


				return newOpenNode;
			}
		
		}	
	}

	return nullptr;
}


JumpPointSearch::NODE* JumpPointSearch::CheckUpVertical(NODE* parentNode, NODE* destNode, int x, int y)
{

	NODE* newOpenNode;

	NODE* assistNode;

	for (int iCnt = y; iCnt - 1 > 0; iCnt--)
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


		if (x > 0 && x + 1 < MAX_WIDTH)
		{
			if (blockList[x - 1][iCnt] == (BYTE)BLOCK_COLOR::GRAY && blockList[x - 1][iCnt - 1] == (BYTE)BLOCK_COLOR::BASIC || 
				blockList[x + 1][iCnt] == (BYTE)BLOCK_COLOR::GRAY && blockList[x + 1][iCnt - 1] == (BYTE)BLOCK_COLOR::BASIC)
			{

				newOpenNode = SetCornerNode(parentNode, destNode, NODE_DIRECTION::NODE_DIR_UU, iCnt, y);

				openList.PushBack(newOpenNode);

				// 코너에 오픈 리스트에 넣을 노드를 만든다.
				if (blockList[x + 1][y - 1] == (BYTE)BLOCK_COLOR::BASIC)
				{
				}

				if (blockList[x - 1][y - 1] == (BYTE)BLOCK_COLOR::BASIC)
				{

				}

				return newOpenNode;
			}
		}	
	}

	return nullptr;
}

JumpPointSearch::NODE* JumpPointSearch::CheckDownVertical(NODE* parentNode, NODE* destNode, int x, int y)
{
	NODE* newOpenNode;

	NODE* assistNode;

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


		if (x > 0 && x + 1 < MAX_WIDTH)
		{
			if (blockList[x - 1][iCnt] == (BYTE)BLOCK_COLOR::GRAY && blockList[x - 1][iCnt + 1] == (BYTE)BLOCK_COLOR::BASIC ||
				blockList[x + 1][iCnt] == (BYTE)BLOCK_COLOR::GRAY && blockList[x + 1][iCnt + 1] == (BYTE)BLOCK_COLOR::BASIC)
			{

				newOpenNode = SetCornerNode(parentNode, destNode, NODE_DIRECTION::NODE_DIR_DD, iCnt, y);

				openList.PushBack(newOpenNode);

				// 코너에 오픈 리스트에 넣을 노드를 만든다.
				if (blockList[x + 1][y + 1] == (BYTE)BLOCK_COLOR::BASIC)
				{

				}

				if (blockList[x - 1][y + 1] == (BYTE)BLOCK_COLOR::BASIC)
				{

				}


				return newOpenNode;
			}
		}
	}

	return nullptr;
}




JumpPointSearch::NODE* JumpPointSearch::CheckRightDiagonalHorizontal(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir,int x, int y)
{

	NODE* newOpenNode;

	for (int iCnt = x; iCnt + 1 < MAX_WIDTH; iCnt++)
	{
		if (blockList[iCnt][y] == (BYTE)BLOCK_COLOR::GRAY)
		{
			return nullptr;
		}
	

		if (y > 0 && y + 1 < MAX_HEIGHT)
		{
			if (blockList[iCnt][y - 1] == (BYTE)BLOCK_COLOR::GRAY && blockList[iCnt + 1][y - 1] == (BYTE)BLOCK_COLOR::BASIC ||
				blockList[iCnt][y + 1] == (BYTE)BLOCK_COLOR::GRAY && blockList[iCnt + 1][y + 1] == (BYTE)BLOCK_COLOR::BASIC)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, nodeDir, x, y);

				openList.PushBack(newOpenNode);

				return newOpenNode;
			}
		}
	
	}
}


JumpPointSearch::NODE* JumpPointSearch::CheckLeftDiagonalHorizontal(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir, int x, int y)
{

	NODE* newOpenNode;

	for (int iCnt = x; iCnt - 1 > 0; iCnt--)
	{
		if (blockList[iCnt][y] == (BYTE)BLOCK_COLOR::GRAY)
		{
			return nullptr;
		}

		if (y > 0 && y + 1 < MAX_HEIGHT)
		{
			if (blockList[iCnt][y - 1] == (BYTE)BLOCK_COLOR::GRAY && blockList[iCnt - 1][y - 1] == (BYTE)BLOCK_COLOR::BASIC ||
				blockList[iCnt][y + 1] == (BYTE)BLOCK_COLOR::GRAY && blockList[iCnt - 1][y + 1] == (BYTE)BLOCK_COLOR::BASIC)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, nodeDir, x, y);

				openList.PushBack(newOpenNode);

				return newOpenNode;
			}
		}

	}

}


JumpPointSearch::NODE* JumpPointSearch::CheckUpDiagonalVertical(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir, int x, int y)
{

	NODE* newOpenNode;

	for (int iCnt = y; iCnt - 1 > 0; iCnt--)
	{
		if (blockList[x][iCnt] == (BYTE)BLOCK_COLOR::GRAY)
		{
			return nullptr;
		}

		if (x > 0 && x + 1 < MAX_WIDTH)
		{

			if (blockList[x + 1][iCnt] == (BYTE)BLOCK_COLOR::GRAY && blockList[x + 1][iCnt - 1] == (BYTE)BLOCK_COLOR::BASIC ||
				blockList[x - 1][iCnt] == (BYTE)BLOCK_COLOR::GRAY && blockList[x - 1][iCnt - 1] == (BYTE)BLOCK_COLOR::BASIC)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, nodeDir, x, y);

				openList.PushBack(newOpenNode);

				return newOpenNode;
			}

		}
	}

}


JumpPointSearch::NODE* JumpPointSearch::CheckDownDiagonalVertical(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir, int x, int y)
{

	NODE* newOpenNode;

	for (int iCnt = y; iCnt + 1 < MAX_HEIGHT; iCnt++)
	{
		if (blockList[x][iCnt] == (BYTE)BLOCK_COLOR::GRAY)
		{
			return nullptr;
		}

		if (x > 0 && x + 1 < MAX_WIDTH)
		{

			if (blockList[x + 1][iCnt] == (BYTE)BLOCK_COLOR::GRAY && blockList[x + 1][iCnt + 1] == (BYTE)BLOCK_COLOR::BASIC ||
				blockList[x - 1][iCnt] == (BYTE)BLOCK_COLOR::GRAY && blockList[x - 1][iCnt + 1] == (BYTE)BLOCK_COLOR::BASIC)
			{
				newOpenNode = SetCornerNode(parentNode, destNode, nodeDir, x, y);

				openList.PushBack(newOpenNode);

				return newOpenNode;
			}

		}
	}

}


//================================================================
// 오픈 리스트에서 정렬되어 있던 F값 노드를 뽑는다.
//================================================================
JumpPointSearch::NODE* JumpPointSearch::SelectOpenListNode()
{
	// F값이 작은 순서로 정렬해놨기 때문에 begin값을 뽑으면 첫 노드를 뽑을 수 있다.
	CList<JumpPointSearch::NODE*>::Iterator iter = openList.begin();

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
	free(startNode);

	free(destinationNode);

	ResetOpenList();

	ResetCloseList();

	ResetBlock();
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