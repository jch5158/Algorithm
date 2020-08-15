#include "stdafx.h"
#include "CList.h"
#include "A_Star.h"


BYTE A_Star::blockList[MAX_WIDTH][MAX_HEIGHT] = { (BYTE)BLOCK_COLOR::BASIC, };

CList<A_Star::NODE*> A_Star::openList;

CList<A_Star::NODE*> A_Star::closeList;

bool A_Star::functionFlag = false;

A_Star::NODE* A_Star::PathFind(int startX, int startY, int destinationX, int destinationY)
{

	static bool funcSet = true;

	// 현재 노드
	static A_Star::NODE* curNode = nullptr;

	// 목적지 노드
	static NODE* destinationNode = nullptr;

	
	// 엔터를 입력하기 전까지 해당 함수를 호출 시 바로 리턴됩니다.
	if (functionFlag == false)
	{
		funcSet = true;
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

		// 시작노드의 이동거리
		startNode->G = 0;

		// 목적지까지의 절대값


		float fStartX = (float)startX;
		float fStartY = (float)startY;

		float fDestX = (float)destinationX;
		float fDestY = (float)destinationY;

		startNode->H += fabs(fDestX - fStartX);

		startNode->H += fabs(fDestY - fStartY);

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
A_Star::NODE* A_Star::FindOpenList(int openX, int openY)
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
bool A_Star::FindCloseList(int closeX, int closeY)
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


A_Star::NODE* A_Star::InsertOpenNode(A_Star::NODE* node, A_Star::NODE* destinationNode)
{
	NODE* retOpenNode;

	NODE* openNode;

	int posX = node->mX;
	int posY = node->mY;

	int newNodePosX = 0;
	int newNodePosY = 0;

	// 사선
	bool diagonal = false;

	bool areaFlag = false;

	for (int iCnt = 0; iCnt < 8; iCnt++)
	{
		if (iCnt == 0)
		{
			if (posX + 1 < MAX_WIDTH)
			{
				newNodePosX = posX + 1;
				newNodePosY = posY;
				areaFlag = true;
			}
			diagonal = false;
		}
		else if (iCnt == 1)
		{
			if (posX + 1 < MAX_WIDTH && posY + 1 < MAX_HEIGHT)
			{
				newNodePosX = posX + 1;
				newNodePosY = posY + 1;
				areaFlag = true;

			}
			diagonal = true;
		}
		else if (iCnt == 2)
		{
			if (posY + 1 < MAX_HEIGHT)
			{
				newNodePosX = posX;
				newNodePosY = posY + 1;
				areaFlag = true;

			}

			diagonal = false;
		}
		else if (iCnt == 3)
		{
			if (posX - 1 > 0 && posY + 1 < MAX_HEIGHT)
			{
				newNodePosX = posX - 1;
				newNodePosY = posY + 1;
				areaFlag = true;
			}
			diagonal = true;
		}
		else if (iCnt == 4)
		{
			if (posX - 1 > 0)
			{
				newNodePosX = posX - 1;
				newNodePosY = posY;
				areaFlag = true;
			}
			diagonal = false;
		}
		else if (iCnt == 5)
		{
			if (posX - 1 > 0 && posY - 1 > 0)
			{
				newNodePosX = posX - 1;
				newNodePosY = posY - 1;
				areaFlag = true;
			}
			diagonal = true;
		}
		else if (iCnt == 6)
		{
			if (posY - 1 > 0)
			{
				newNodePosX = posX;
				newNodePosY = posY - 1;
				areaFlag = true;
			}

			diagonal = false;
		}
		else if (iCnt == 7)
		{
			if (posX + 1 < MAX_WIDTH && posY - 1 > 0)
			{
				newNodePosX = posX + 1;
				newNodePosY = posY - 1;
				areaFlag = true;
			}
			diagonal = true;
		}



		// 해당 좌표가 closeList에 없으면은 오픈 리스트에 추가한다.
		if (FindCloseList(newNodePosX, newNodePosY) == false && blockList[newNodePosX][newNodePosY] != (BYTE)BLOCK_COLOR::GRAY && areaFlag != false)
		{
			//오픈리스트에 이미 있는 노드가 아닐 만들어서 추가한다.
			retOpenNode = FindOpenList(newNodePosX, newNodePosY);
			if (retOpenNode == nullptr)
			{
				openNode = (NODE*)malloc(sizeof(NODE));

				// 인자로 들어온 node는 보존해야 한다.
				NODE* nodeBuffer = node;

				// 대각선이면은 주변 G값들을 한번 확인해줘야 한다.
				if (diagonal == true)
				{
					CList<A_Star::NODE*>::Iterator iterE = openList.end();

					for (CList<A_Star::NODE*>::Iterator iter = openList.begin(); iter != iterE; ++iter)
					{
						if (abs(iter->mX - newNodePosX) <= 1 && abs(iter->mY - newNodePosY) <= 1)
						{
							if (iter->G < nodeBuffer->G)
							{
								nodeBuffer = (*iter)->data;
							}
						}
					}
				}
				openNode->prev = nodeBuffer;

				openNode->mX = newNodePosX;
				openNode->mY = newNodePosY;


				if (abs(openNode->mX - nodeBuffer->mX) == 1 && abs(openNode->mY - nodeBuffer->mY) == 1)
				{
					openNode->G = nodeBuffer->G + 1.5;
				}
				else
				{
					openNode->G = nodeBuffer->G + 1;
				}


				openNode->H = (float)(abs(destinationNode->mX - newNodePosX) + abs(destinationNode->mY - newNodePosY));

				openNode->F = openNode->G + openNode->H;

				if (blockList[newNodePosX][newNodePosY] == (BYTE)BLOCK_COLOR::RED)
				{
					return openNode;
				}
				else
				{
					openList.PushBack(openNode);
				}

				blockList[newNodePosX][newNodePosY] = (BYTE)BLOCK_COLOR::BLUE;
			}
		}

		// 범위 밖으로 벗어나는지 확인하는 값
		areaFlag = false;
	}

	
	return nullptr;
}


A_Star::NODE* A_Star::SelectOpenListNode()
{
	// F값이 작은 순서로 정렬해놨기 때문에 begin값을 뽑으면 첫 노드를 뽑을 수 있다.
	CList<A_Star::NODE*>::Iterator iter = openList.begin();

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


void A_Star::openListBubbleSort(){

	CList<A_Star::NODE*>::Iterator iterE = A_Star::openList.end();

	--iterE;

	for (int iCnt = 0; iCnt < A_Star::openList.listLength; iCnt++)
	{
		for (CList<A_Star::NODE*>::Iterator iter = A_Star::openList.begin(); iter != iterE; ++iter)
		{
			CList<A_Star::NODE*>::Iterator iterN = iter.node->next;

			if (iter->F > iterN->F)
			{
				A_Star::openList.DataSwap(iter, iterN);
			}
		}

		--iterE;
	}

}