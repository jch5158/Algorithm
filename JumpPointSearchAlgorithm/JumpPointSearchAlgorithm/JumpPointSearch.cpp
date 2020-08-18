#include "stdafx.h"
#include "CList.h"
#include "JumpPointSearch.h"


BYTE JumpPointSearch::blockList[MAX_WIDTH][MAX_HEIGHT] = { (BYTE)BLOCK_COLOR::BASIC, };

CList<JumpPointSearch::NODE*> JumpPointSearch::openList;

CList<JumpPointSearch::NODE*> JumpPointSearch::closeList;

bool JumpPointSearch::functionFlag = false;


// ������ ���
JumpPointSearch::NODE* JumpPointSearch::destinationNode = nullptr;

// ������ ���
JumpPointSearch::NODE* JumpPointSearch::startNode = nullptr;


JumpPointSearch::NODE* JumpPointSearch::PathFind(int startX, int startY, int destinationX, int destinationY)
{
	static bool funcSet = true;

	// ���� ���
	static JumpPointSearch::NODE* curNode = nullptr;


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
		startNode = (NODE*)malloc(sizeof(NODE));

		// ���۳�� ��ǥ����
		startNode->mX = startX;
		startNode->mY = startY;

		startNode->mNodeDir = (BYTE)NODE_DIRECTION::NODE_DIR_ALL;

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

//======================================================
// �ش� ��ǥ�� closeList�� �ִٸ��� true�� �����Ѵ�.
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

	// �� ����Ʈ�� ������ �ٲ۴�.
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


				// �ڳʸ� Ȯ���Ͽ� ���� ���� Ž���� ������.
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


				// �ڳʿ� ���� ����Ʈ�� ���� ��带 �����.
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

				// �ڳʿ� ���� ����Ʈ�� ���� ��带 �����.
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

				// �ڳʿ� ���� ����Ʈ�� ���� ��带 �����.
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
// ���� ����Ʈ���� ���ĵǾ� �ִ� F�� ��带 �̴´�.
//================================================================
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


//=============================================================
// ��� ����Ʈ�� ���� �ʱ�ȭ ��Ų��.
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
// ���¸���Ʈ ��带 �����Ѵ�.
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
// Ŭ�����Ʈ ��带 �����Ѵ�.
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
// ������ �����Ѵ�.
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
// F�� ���� ������ �����Ѵ�.
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