#include "stdafx.h"
#include "CList.h"
#include "JumpPointSearchAlgorithm.h"
#include "BresenhamLine.h"
#include "JumpPointSearch.h"


extern HBRUSH       oldBrush;

// ���¸���Ʈ
extern HBRUSH       blueBrush;

// Ŭ���� ����Ʈ
extern HBRUSH       yellowBrush;

// ����� üũ�귯��
extern HBRUSH       greenBrush;

// ������ üũ�귯��
extern HBRUSH       redBrush;

// ��ֹ� üũ�귯��
extern HBRUSH       grayBrush;


// ������ ���
JumpPointSearch::NODE* destinationNode = nullptr;

// ������ ���
JumpPointSearch::NODE* startNode = nullptr;


CList<JumpPointSearch::NODE*> openList;

CList<JumpPointSearch::NODE*> closeList;

CList<JumpPointSearch::NODE*> routeList;

CList<JumpPointSearch::NODE*> optimizeRouteList;


HBRUSH brushBlockList[MAX_WIDTH][MAX_HEIGHT] = { nullptr, };


//===================================================================
// Ÿ�̸ӷ� �ڵ� �Լ� ȣ�� �� ���� ������ ���� �ʵ��� �ϴ� Flag
//===================================================================
bool functionFlag = false;
//
bool funcSetFlag = true;


// ������ ���
//JumpPointSearch::NODE* JumpPointSearch::destinationNode = nullptr;

// ������ ���
//JumpPointSearch::NODE* JumpPointSearch::startNode = nullptr;


JumpPointSearch::NODE* JumpPointSearch::PathFind(int startX, int startY, int destinationX, int destinationY)
{
	
	// ���� ���
	static JumpPointSearch::NODE* curNode = nullptr;	 



	// ���͸� �Է��ϱ� ������ �ش� �Լ��� ȣ�� �� �ٷ� ���ϵ˴ϴ�.
	if (functionFlag == false)
	{
		return nullptr;
	}

	if (funcSetFlag == true)
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
		funcSetFlag = false;
	}


	NODE* retval;

	// ���� ����Ʈ�� ����� ���ؼ� ���� ��� ��ġ�� ������ ��ġ�� ���ڷ� �����Ѵ�.
	retval = InsertOpenNode(curNode, destinationNode);
	// false�� ��� ���������� ��带 �������.
	if (retval == destinationNode)
	{
		InsertRoute(destinationNode);

		PathOptimizing();

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
		// ���¸���Ʈ�� ���� ������� ����
		retOpenNode = FindOpenList(x, y);
		if (retOpenNode == nullptr)
		{
			newNode = (NODE*)malloc(sizeof(NODE));

			newNode->mX = x;
			newNode->mY = y;
		
			newNode->G = parentNode->G;

			// �θ� ���� �ڽ� ����� x,y ���� �� ����
			subX = abs(parentNode->mX - newNode->mX);
			subY = abs(parentNode->mY - newNode->mY);

			// subX �� subY�� ���ÿ� 1 �̻� ���̳� ��� �밢���̴�.
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

			// ���� ������ �ٲ��ش�.

			if (x != destNode->mX && y != destNode->mY)
			{
				brushBlockList[newNode->mX][newNode->mY] = blueBrush;
			}
			
			return newNode;
		}
		else
		{
			// ���� ���¸���Ʈ�� �־��� ��ġ���, �ش� ����� �θ��� G���� ���Ͽ� ���� �������ش�.
			if (retOpenNode->prev->G < parentNode->G)
			{

				retOpenNode->G = parentNode->G;

				// �θ� ���� �ڽ� ����� x,y ���� �� ����
				subX = abs(parentNode->mX - retOpenNode->mX);
				subY = abs(parentNode->mY - retOpenNode->mY);

				// subX �� subY�� ���ÿ� 1 �̻� ���̳� ��� �밢���̴�.
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
// ������ ���� Ž�� �Լ��Դϴ�.
//===============================================
JumpPointSearch::NODE* JumpPointSearch::CheckRightHorizontal(NODE* parentNode, NODE* destNode, int x, int y, HBRUSH randBrush)
{
	NODE* newOpenNode = nullptr;

	NODE* retNode = nullptr;

	//=================================================
	// ���� Ž�� ������ �����ϴ�
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
	// iCnt = x + 1 �� �ϴ� ������ �ش� ��ġ�� ���� ��尡 �ִ� ��ġ�̱� �����̴�. 
	// iCnt + 1 < MAX_WIDTH �� ������ ���� �Ʒ��� ���� ���� ��� X + 1 ��ǥ�� 
	// ���� ������ Ȯ���ϱ� �������� �����÷ο찡 �߻����� �ʵ��� �ϱ� �����Դϴ�.
	//============================================================================
	for (int iCnt = x + 1; iCnt < MAX_WIDTH; ++iCnt)
	{
		// ���� ������ ��� retur nullptr �� �Ѵ�.
		if (brushBlockList[iCnt][y] == grayBrush)
		{
			return nullptr;
		}

		// �������� ���� ��� �ٷ� �̾��ش�. 
		if (iCnt == destNode->mX && y == destNode->mY)
		{
			destNode->prev = parentNode;

			return destNode;
		}

		// �귯�� ����Ʈ�� ĥ�ϱ�
		brushBlockList[iCnt][y] = randBrush;

		//===================================================
		// y ���� 0 �̻��� ��쿡�� ���� ���� Ȯ���� �� �ִ�.
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
// ���� ���� Ž�� �Լ��Դϴ�.
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
		// �ش� ��ǥ�� ��ֹ��� ������ �ٷ� return �Ѵ�.
		if (brushBlockList[iCnt][y] == grayBrush)
		{
			return nullptr;
		}


		// �������� ������ �̾��ְ� �ٷ� return �Ѵ�.
		if (iCnt == destNode->mX && y == destNode->mY)
		{
			destNode->prev = parentNode;

			return destNode;
		}

		// Ÿ�Ͽ� ������ �귯���� �־��ش�.
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
// ���� ���� Ž�� �Լ��Դϴ�.
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
		// �ش� ��ġ�� ��ֹ��� ���� ��� �ٷ� return
		//=============================================
		if (brushBlockList[x][iCnt] == grayBrush)
		{
			return nullptr;
		}


		//=============================================
		// �ش� ��ġ�� �������� ���� ��� �ٷ� return
		//=============================================
		if (x == destNode->mX && iCnt == destNode->mY)
		{
			destNode->prev = parentNode;

			return destNode;
		}

		//==========================================
		// �ش� Ÿ�Ͽ� �귯���� ĥ�Ѵ�.
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
		// ��ֹ��� ����ĥ ��� return �Ѵ�.
		if (brushBlockList[x][iCnt] == grayBrush)
		{
			return nullptr;
		}


		// �������� ������ ��� �������� �̾��ְ� return �Ѵ�.
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
// ������ �� �밢�� �Լ��Դϴ�. 
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
	// �밢�� �Լ� ȣ�� �� ���� �Լ� ȣ�� ����
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
	// ���� ���� �� �밢�� ȣ�� ����
	//==================================================
	if (x > 0 && y > 0 )
	{
		if (brushBlockList[x - 1][y] == grayBrush && brushBlockList[x - 1][y - 1] == oldBrush)
		{
			leftUpFlag = true;
		}
	}

	//==================================================
	// ���� ������ �Ʒ� �밢�� ȣ�� ����
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


		// ������ �� �밢�� ȣ��
		if (rightUpFlag)
		{
			do
			{
				// �迭 ������ ��� ��� ȣ�� ���� 
				if (x + xCount >= MAX_WIDTH || y - yCount < 0)
				{
					rightUpFlag = false;
					break;
				}

				// ��ֹ��� ������ ��� ȣ�� ����
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
// �밢�� �Լ� ���� ������ ���� 
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
	
		// ������ ��带 ������ ��쿡�� �� �ڸ��� ��� ������ return 
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
// �밢�� �Լ� ���� ���� ����
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
		// �������� ������ ��� return
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
// �밢�� ���� �� ����
//=======================================================
bool JumpPointSearch::CheckUpDiagonalVertical(NODE* parentNode, NODE* destNode, NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush)
{
	NODE* newOpenNode;

	for (int iCnt = y; iCnt >= 0; --iCnt)
	{
		// ��ֹ� ������ ��� return
		if (brushBlockList[x][iCnt] == grayBrush)
		{
			return false;
		}
		
		//=================================================
		// ������ ������ ��� ��� ���� �� return
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
// ���� ����Ʈ���� ���ĵǾ� �ִ� F�� ��带 �̴´�.
//================================================================
JumpPointSearch::NODE* JumpPointSearch::SelectOpenListNode()
{
	// F���� ���� ������ �����س��� ������ begin���� ������ ù ��带 ���� �� �ִ�.
	CList<JumpPointSearch::NODE*>::Iterator iter = openList.begin();

	if ((*iter)->data == nullptr)
	{
		return nullptr;
	}

	// ����� ���� �����Ѵ�.
	NODE* node = (*iter)->data;

	// ���� ����Ʈ���� �ش� ��带 �����.
	openList.erase(iter);

	// closeList�� �־��ش�.
	closeList.PushFront(node);

	// Ŭ�����Ʈ�̴� ������ �ٲپ��ش�.
	brushBlockList[node->mX][node->mY] = yellowBrush;

	return node;
}


//=============================================================
// ��� ����Ʈ�� ���� �ʱ�ȭ ��Ų��.
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
// ������ �����Ѵ�.
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
// F�� ���� ������ �����Ѵ�.
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