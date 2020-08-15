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

	// ���� ���
	static A_Star::NODE* curNode = nullptr;

	// ������ ���
	static NODE* destinationNode = nullptr;

	
	// ���͸� �Է��ϱ� ������ �ش� �Լ��� ȣ�� �� �ٷ� ���ϵ˴ϴ�.
	if (functionFlag == false)
	{
		funcSet = true;
		return nullptr;
	}

	if (funcSet == true)
	{
		// ������ ��� �����Ҵ�
		destinationNode = (NODE*)malloc(sizeof(NODE));

		// ������ ����� ��ǥ ����
		destinationNode->mX = destinationX;
		destinationNode->mY = destinationY;

		// ���� ��� ����
		NODE* startNode = (NODE*)malloc(sizeof(NODE));

		// ���۳�� ��ǥ����
		startNode->mX = startX;
		startNode->mY = startY;

		startNode->prev = nullptr;

		// ���۳���� �̵��Ÿ�
		startNode->G = 0;

		// ������������ ���밪


		float fStartX = (float)startX;
		float fStartY = (float)startY;

		float fDestX = (float)destinationX;
		float fDestY = (float)destinationY;

		startNode->H += fabs(fDestX - fStartX);

		startNode->H += fabs(fDestY - fStartY);

		startNode->F = startNode->G + startNode->H;

		// Ŭ���� ����Ʈ�� �߰�
		closeList.PushBack(startNode);
	
		curNode = startNode;

		// �ѹ��� ���� ���õǵ��� ����
		funcSet = false;
	}


	NODE* retval;
	

	// ���� ����Ʈ�� ����� ���ؼ� ���� ��� ��ġ�� ������ ��ġ�� ���ڷ� �����Ѵ�.
	retval = InsertOpenNode(curNode, destinationNode);
	// false�� ��� ���������� ��带 �������.
	if (retval != nullptr)
	{
		functionFlag = false;
		return retval;
	}
	else
	{
		// openList F�� ����
		openListBubbleSort();

		// F���� ���� ���� ��带 �����Ѵ�.
		curNode = SelectOpenListNode();
	}


	return nullptr;
}

//======================================================
// ��ǥ�� openList�� ã�Ƽ� return 
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

// �ش� ��ǥ�� closeList�� �ִٸ��� true�� �����Ѵ�.
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

	// �缱
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



		// �ش� ��ǥ�� closeList�� �������� ���� ����Ʈ�� �߰��Ѵ�.
		if (FindCloseList(newNodePosX, newNodePosY) == false && blockList[newNodePosX][newNodePosY] != (BYTE)BLOCK_COLOR::GRAY && areaFlag != false)
		{
			//���¸���Ʈ�� �̹� �ִ� ��尡 �ƴ� ���� �߰��Ѵ�.
			retOpenNode = FindOpenList(newNodePosX, newNodePosY);
			if (retOpenNode == nullptr)
			{
				openNode = (NODE*)malloc(sizeof(NODE));

				// ���ڷ� ���� node�� �����ؾ� �Ѵ�.
				NODE* nodeBuffer = node;

				// �밢���̸��� �ֺ� G������ �ѹ� Ȯ������� �Ѵ�.
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

		// ���� ������ ������� Ȯ���ϴ� ��
		areaFlag = false;
	}

	
	return nullptr;
}


A_Star::NODE* A_Star::SelectOpenListNode()
{
	// F���� ���� ������ �����س��� ������ begin���� ������ ù ��带 ���� �� �ִ�.
	CList<A_Star::NODE*>::Iterator iter = openList.begin();

	// ����� ���� �����Ѵ�.
	NODE* node = (*iter)->data;

	// ���� ����Ʈ���� �ش� ��带 �����.
	openList.erase(iter);

	// closeList�� �־��ش�.
	closeList.PushFront(node);
	
	// Ŭ�����Ʈ�̴� ������ �ٲپ��ش�.
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