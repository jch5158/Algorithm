#include "stdafx.h"
#include "CList.h"
#include "JumpPointSearch.h"


BYTE JumpPointSearch::blockList[MAX_WIDTH][MAX_HEIGHT] = { (BYTE)BLOCK_COLOR::BASIC, };

CList<JumpPointSearch::NODE*> JumpPointSearch::openList;

CList<JumpPointSearch::NODE*> JumpPointSearch::closeList;

bool JumpPointSearch::functionFlag = false;


JumpPointSearch::NODE* JumpPointSearch::PathFind(int startX, int startY, int destinationX, int destinationY)
{
	static bool funcSet = true;

	// 현재 노드
	static JumpPointSearch::NODE* curNode = nullptr;

	// 목적지 노드
	static NODE* destinationNode = nullptr;


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
		NODE* startNode = (NODE*)malloc(sizeof(NODE));

		// 시작노드 좌표설정
		startNode->mX = startX;
		startNode->mY = startY;

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

// 해당 좌표에 closeList가 있다면은 true를 리턴한다.
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


	for (int iCnt = 0; iCnt < 8; iCnt++)
	{

		switch ((NODE_DIRECTION)iCnt)
		{
		case NODE_DIRECTION::NODE_DIR_RR:

			if (posX + 1 < MAX_WIDTH)
			{
				newNodePosX = posX + 1;
				newNodePosY = posY;
			}
			else
			{
				continue;
			}

			break;
		case NODE_DIRECTION::NODE_DIR_RD:

			if (posX + 1 < MAX_WIDTH && posY + 1 < MAX_HEIGHT)
			{
				newNodePosX = posX + 1;
				newNodePosY = posY + 1;
			}
			else
			{
				continue;
			}

			break;
		case NODE_DIRECTION::NODE_DIR_DD:

			if (posY + 1 < MAX_HEIGHT)
			{
				newNodePosX = posX;
				newNodePosY = posY + 1;
			}
			else
			{
				continue;
			}

			break;
		case NODE_DIRECTION::NODE_DIR_LD:

			if (posX - 1 >= 0 && posY + 1 < MAX_HEIGHT)
			{
				newNodePosX = posX - 1;
				newNodePosY = posY + 1;
			}
			else
			{
				continue;
			}

			break;
		case NODE_DIRECTION::NODE_DIR_LL:

			if (posX - 1 >= 0)
			{
				newNodePosX = posX - 1;
				newNodePosY = posY;
			}
			else
			{
				continue;
			}

			break;
		case NODE_DIRECTION::NODE_DIR_LU:

			if (posX - 1 >= 0 && posY - 1 >= 0)
			{
				newNodePosX = posX - 1;
				newNodePosY = posY - 1;
			}
			else
			{
				continue;
			}

			break;
		case NODE_DIRECTION::NODE_DIR_UU:

			if (posY - 1 >= 0)
			{
				newNodePosX = posX;
				newNodePosY = posY - 1;
			}
			else
			{
				continue;
			}

			break;
		case NODE_DIRECTION::NODE_DIR_RU:

			if (posX + 1 < MAX_WIDTH && posY - 1 >= 0)
			{
				newNodePosX = posX + 1;
				newNodePosY = posY - 1;
			}
			else
			{
				continue;
			}

			break;
		}


		// 해당 좌표가 closeList에 없으면은 오픈 리스트에 추가한다.
		if (FindCloseList(newNodePosX, newNodePosY) == false && blockList[newNodePosX][newNodePosY] != (BYTE)BLOCK_COLOR::GRAY)
		{
			//오픈리스트에 이미 있는 노드가 아닐 만들어서 추가한다.
			retOpenNode = FindOpenList(newNodePosX, newNodePosY);
			if (retOpenNode == nullptr)
			{
				openNode = (NODE*)malloc(sizeof(NODE));

				openNode->mX = newNodePosX;
				openNode->mY = newNodePosY;

				openNode->prev = node;

				if (abs(openNode->mX - node->mX) == 1 && abs(openNode->mY - node->mY) == 1)
				{
					openNode->G = node->G + 1.5f;
				}
				else
				{
					openNode->G = node->G + 1;
				}

				openNode->H = (float)(abs(destinationNode->mX - newNodePosX) + abs(destinationNode->mY - newNodePosY));

				openNode->F = openNode->G + openNode->H;

				if (blockList[newNodePosX][newNodePosY] == (BYTE)BLOCK_COLOR::RED)
				{
					return openNode;
				}
				else
				{
					blockList[newNodePosX][newNodePosY] = (BYTE)BLOCK_COLOR::BLUE;
					openList.PushBack(openNode);
				}
			}
			else
			{
				// node -> openList에서 뽑은 노드
				// retOpenNode   ->  이미 만들어져있는 노드
				if (retOpenNode->prev->G > node->G)
				{
					retOpenNode->prev = node;

					if (abs(retOpenNode->mX - node->mX) == 1 && abs(retOpenNode->mY - node->mY) == 1)
					{
						retOpenNode->G = node->G + 1.5f;
					}
					else
					{
						retOpenNode->G = node->G + 1;
					}

					retOpenNode->F = retOpenNode->G + retOpenNode->H;
				}
			}


		}
	}

	return nullptr;
}


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


void JumpPointSearch::ResetAll()
{
	ResetOpenList();

	ResetCloseList();

	ResetBlock();
}

void JumpPointSearch::ResetOpenList()
{
	CList<JumpPointSearch::NODE*>::Iterator iterE = openList.end();

	for (CList<JumpPointSearch::NODE*>::Iterator iter = openList.begin(); iter != iterE;)
	{
		iter = openList.erase(iter);
	}
}

void JumpPointSearch::ResetCloseList()
{
	CList<JumpPointSearch::NODE*>::Iterator iterE = closeList.end();

	for (CList<JumpPointSearch::NODE*>::Iterator iter = closeList.begin(); iter != iterE;)
	{
		iter = closeList.erase(iter);
	}
}

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