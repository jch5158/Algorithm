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

	// ���� ���
	static JumpPointSearch::NODE* curNode = nullptr;

	// ������ ���
	static NODE* destinationNode = nullptr;


	// ���͸� �Է��ϱ� ������ �ش� �Լ��� ȣ�� �� �ٷ� ���ϵ˴ϴ�.
	if (functionFlag == false)
	{
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

		// ���۳�� G ����
		startNode->G = 0;

		// ������������ ���밪	
		startNode->H += (float)(abs(destinationX - startX) + abs(destinationY - startY));

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
		funcSet = true;
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

// �ش� ��ǥ�� closeList�� �ִٸ��� true�� �����Ѵ�.
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


		// �ش� ��ǥ�� closeList�� �������� ���� ����Ʈ�� �߰��Ѵ�.
		if (FindCloseList(newNodePosX, newNodePosY) == false && blockList[newNodePosX][newNodePosY] != (BYTE)BLOCK_COLOR::GRAY)
		{
			//���¸���Ʈ�� �̹� �ִ� ��尡 �ƴ� ���� �߰��Ѵ�.
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
				// node -> openList���� ���� ���
				// retOpenNode   ->  �̹� ��������ִ� ���
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
	// F���� ���� ������ �����س��� ������ begin���� ������ ù ��带 ���� �� �ִ�.
	CList<JumpPointSearch::NODE*>::Iterator iter = openList.begin();

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