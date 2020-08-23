#include "stdafx.h"
#include "CList.h"
#include "JumpPointSearchAlgorithm.h"
#include "BresenhamLine.h"
#include "JumpPointSearch.h"


// ������ ���
JumpPointSearch::NODE* destinationNode = nullptr;

// ������ ���
JumpPointSearch::NODE* startNode = nullptr;


CList<JumpPointSearch::NODE*> openList;

CList<JumpPointSearch::NODE*> closeList;

CList<JumpPointSearch::NODE*> routeList;

CList<JumpPointSearch::NODE*> optimizeRouteList;


HBRUSH brushBlockList[MAX_WIDTH][MAX_HEIGHT];


//===================================================================
// Ÿ�̸ӷ� �ڵ� �Լ� ȣ�� �� ���� ������ ���� �ʵ��� �ϴ� Flag
//===================================================================
bool functionFlag = false;

bool funcSetFlag = true;


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
	NODE* retNode = nullptr;

	//=================================================================================
	// ������ �� ���� Ž�� ������ �����ϴ�
	//=================================================================================
	if (y > 0 && x + 1 < MAX_WIDTH)
	{
		if (brushBlockList[x][y - 1] == grayBrush && (brushBlockList[x + 1][y - 1] == oldBrush || brushBlockList[x + 1][y - 1] == redBrush))
		{	
			retNode = CheckRightUp(parentNode, destNode, x, y, randBrush, false);	
			if (retNode == destNode)
			{
				return retNode;
			}
		}
	}

	//=================================================================================
	// ������ �Ʒ� ���� Ž�� ������ �����ϴ�
	//=================================================================================
	if (y + 1 < MAX_HEIGHT && x + 1 < MAX_WIDTH)
	{
		if (brushBlockList[x][y + 1] == grayBrush && (brushBlockList[x + 1][y + 1] == oldBrush || brushBlockList[x + 1][y + 1] == redBrush))
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

		// �������� ���� ��� �ٷ� �̾��ְ� return �Ѵ�. 
		if (iCnt == destNode->mX && y == destNode->mY)
		{
			destNode->prev = parentNode;

			return destNode;
		}


		// ���� ����Ʈ ��� �Ǵ� Ŭ���� ����Ʈ ��忡�� ĥ���� �ʴ´�.
		if (brushBlockList[iCnt][y] != blueBrush && brushBlockList[iCnt][y] != yellowBrush)
		{
			brushBlockList[iCnt][y] = randBrush;
		}


		//====================================================================
		// y ���� 0 �̻��� ��쿡�� ���� ���� Ȯ���� �� �ִ�.
		//====================================================================
		if (y > 0 && iCnt + 1 < MAX_WIDTH )
		{
			if (brushBlockList[iCnt][y - 1] == grayBrush && (brushBlockList[iCnt + 1][y - 1] == oldBrush || brushBlockList[iCnt + 1][y - 1] == redBrush))
			{
				retNode = SetCornerNode(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RR, iCnt, y);
				if(retNode != nullptr)
				{
					openList.PushBack(retNode);
				}				
				return retNode;
			}
		}


		//====================================================================
		// y ���� y + 1 < MAX_HEIGHT �� ��쿡�� ���� ���� Ȯ���� �� �ִ�.
		//====================================================================
		if (y + 1 < MAX_HEIGHT && iCnt + 1 < MAX_WIDTH)
		{
			if (brushBlockList[iCnt][y + 1] == grayBrush && (brushBlockList[iCnt + 1][y + 1] == oldBrush || brushBlockList[iCnt + 1][y + 1] == redBrush))
			{
				retNode = SetCornerNode(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RR, iCnt, y);
				if (retNode != nullptr)
				{
					openList.PushBack(retNode);
				}

				return retNode;
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
	NODE* retNode = nullptr;

	//=================================================================================
	// ���� �� ���� Ž�� ������ �����ϴ�
	//=================================================================================
	if (y > 0 && x > 0)
	{
		if (brushBlockList[x][y - 1] == grayBrush && (brushBlockList[x - 1][y - 1] == oldBrush || brushBlockList[x - 1][y - 1] == redBrush))
		{		
			retNode = CheckLeftUp(parentNode, destNode, x, y, randBrush, false);
			if (retNode == destNode)
			{
				return retNode;
			}
		}
	}

	//=================================================================================
	// ���� �Ʒ� ���� Ž�� ������ �����ϴ�
	//=================================================================================
	if (y + 1 < MAX_HEIGHT && x > 0)
	{
		if (brushBlockList[x][y + 1] == grayBrush && (brushBlockList[x - 1][y + 1] == oldBrush || brushBlockList[x - 1][y + 1] == redBrush))
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

		// ���� ����Ʈ ��� �Ǵ� Ŭ���� ����Ʈ ��忡�� ĥ���� �ʴ´�.
		if (brushBlockList[iCnt][y] != blueBrush && brushBlockList[iCnt][y] != yellowBrush)
		{
			brushBlockList[iCnt][y] = randBrush;
		}

		if (y > 0 && iCnt - 1 >= 0)
		{
			if (brushBlockList[iCnt][y - 1] == grayBrush && (brushBlockList[iCnt - 1][y - 1] == oldBrush || brushBlockList[iCnt - 1][y - 1] == redBrush))
			{
				retNode = SetCornerNode(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LL, iCnt, y);

				if (retNode != nullptr)
				{
					openList.PushBack(retNode);
				}

				return retNode;
			}
		}


		if (y + 1 < MAX_HEIGHT && iCnt - 1 >= 0)
		{
			if (brushBlockList[iCnt][y + 1] == grayBrush && (brushBlockList[iCnt - 1][y + 1] == oldBrush || brushBlockList[iCnt - 1][y + 1] == redBrush))
			{
				retNode = SetCornerNode(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LL, iCnt, y);

				if (retNode != nullptr)
				{
					openList.PushBack(retNode);
				}

				return retNode;
			}
		}
	}

	return nullptr;
}


//===============================================
// ���� �� ���� Ž�� �Լ��Դϴ�.
//===============================================
JumpPointSearch::NODE* JumpPointSearch::CheckUpVertical(NODE* parentNode, NODE* destNode, int x, int y, HBRUSH randBrush)
{	
	NODE* retNode = nullptr;

	//=================================================================================
	// ���� �� ���� Ž�� ������ �����ϴ�
	//=================================================================================
	if (x > 0 && y > 0)
	{
		if (brushBlockList[x - 1][y] == grayBrush && (brushBlockList[x - 1][y - 1] == oldBrush || brushBlockList[x - 1][y - 1] == redBrush))
		{
			retNode = CheckLeftUp(parentNode, destNode, x, y, randBrush, false);
			if (retNode == destNode)
			{
				return retNode;
			}
		}
	}

	//=================================================================================
	// ������ �� ���� Ž�� ������ �����ϴ�
	//=================================================================================
	if (x + 1 < MAX_WIDTH && y > 0)
	{
		if (brushBlockList[x + 1][y] == grayBrush && (brushBlockList[x + 1][y - 1] == oldBrush || brushBlockList[x + 1][y - 1] == redBrush))
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

		// ���� ����Ʈ ��� �Ǵ� Ŭ���� ����Ʈ ��忡�� ĥ���� �ʴ´�.
		if (brushBlockList[x][iCnt] != blueBrush && brushBlockList[x][iCnt] != yellowBrush)
		{
			brushBlockList[x][iCnt] = randBrush;
		}


		if (x > 0 && iCnt - 1 >= 0)
		{
			if (brushBlockList[x - 1][iCnt] == grayBrush && (brushBlockList[x - 1][iCnt - 1] == oldBrush || brushBlockList[x - 1][iCnt - 1] == redBrush))
			{
				retNode = SetCornerNode(parentNode, destNode, NODE_DIRECTION::NODE_DIR_UU, x, iCnt);

				if (retNode != nullptr)
				{
					openList.PushBack(retNode);
				}

				return retNode;
			}
		}


		if (x + 1 < MAX_WIDTH && iCnt - 1 >= 0)
		{
			if (brushBlockList[x + 1][iCnt] == grayBrush && (brushBlockList[x + 1][iCnt - 1] == oldBrush || brushBlockList[x + 1][iCnt - 1] == redBrush))
			{
				retNode = SetCornerNode(parentNode, destNode, NODE_DIRECTION::NODE_DIR_UU, x, iCnt);

				if (retNode != nullptr)
				{
					openList.PushBack(retNode);
				}

				return retNode;
			}
		}	
	}

	return nullptr;
}


//===============================================
// ���� �Ʒ� ���� Ž�� �Լ��Դϴ�.
//===============================================
JumpPointSearch::NODE* JumpPointSearch::CheckDownVertical(NODE* parentNode, NODE* destNode, int x, int y, HBRUSH randBrush)
{
	NODE* retNode = nullptr;

	//=================================================================================
	// ���� �Ʒ� ���� Ž�� ������ �����ϴ�
	//=================================================================================
	if (x > 0 && y + 1 < MAX_HEIGHT)
	{
		if (brushBlockList[x - 1][y] == grayBrush && (brushBlockList[x - 1][y + 1] == oldBrush || brushBlockList[x - 1][y + 1] == redBrush))
		{
			retNode = CheckLeftDown(parentNode, destNode, x, y, randBrush, false);
			if (retNode == destNode)
			{
				return retNode;
			}
		}
	}

	//=================================================================================
	// ������ �Ʒ� ���� Ž�� ������ �����ϴ�
	//=================================================================================
	if (x + 1 < MAX_WIDTH && y + 1 < MAX_HEIGHT)
	{
		if (brushBlockList[x + 1][y] == grayBrush && (brushBlockList[x + 1][y + 1] == oldBrush || brushBlockList[x + 1][y + 1] == redBrush))
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

		// ���� ����Ʈ ��� �Ǵ� Ŭ���� ����Ʈ ��忡�� ĥ���� �ʴ´�.
		if (brushBlockList[x][iCnt] != blueBrush && brushBlockList[x][iCnt] != yellowBrush)
		{
			brushBlockList[x][iCnt] = randBrush;
		}

		if (x > 0 && iCnt + 1 < MAX_HEIGHT )
		{
			if (brushBlockList[x - 1][iCnt] == grayBrush && (brushBlockList[x - 1][iCnt + 1] == oldBrush || brushBlockList[x - 1][iCnt + 1] == redBrush))
			{
				retNode = SetCornerNode(parentNode, destNode, NODE_DIRECTION::NODE_DIR_DD, x, iCnt);

				if (retNode != nullptr)
				{
					openList.PushBack(retNode);
				}

				return retNode;
			}
		}

		if (x + 1 < MAX_WIDTH && iCnt + 1 < MAX_HEIGHT)
		{
			if (brushBlockList[x + 1][iCnt] == grayBrush && (brushBlockList[x + 1][iCnt + 1] == oldBrush || brushBlockList[x + 1][iCnt + 1] == redBrush))
			{
				retNode = SetCornerNode(parentNode, destNode, NODE_DIRECTION::NODE_DIR_DD, x, iCnt);

				if (retNode != nullptr)
				{
					openList.PushBack(retNode);
				}

				return retNode;
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
	bool retOpenNodeFlag = false;

	//===============================================
	// �밢�� �Լ� ȣ�� �� ���� �Լ� ȣ�� ����
	//===============================================
	if (firstCall)
	{	
		//===================================================================
		// ���� ���ǿ� ������ 5�������� Ž���ϰ� �ȴ�.
		if (destNode == CheckRightHorizontal(parentNode, destNode, x, y, randBrush))
		{
			return destNode;
		}

		if (destNode == CheckUpVertical(parentNode, destNode, x, y, randBrush))
		{
			return destNode;
		}
		//===================================================================
	}

	while (1)
	{
		x += 1;

		y -= 1;


		// �迭 ������ ��� ��� ȣ�� ���� 
		if (x >= MAX_WIDTH || y < 0)
		{
			break;
		}

		// ��ֹ��� ������ ��� ȣ�� ����
		if (brushBlockList[x][y] == grayBrush)
		{
			break;
		}


		// �������� �������� ���������� �̾��ְ� return �Ѵ�.
		if (destNode->mX == x && destNode->mY == y)
		{
			destNode->prev = parentNode;

			return destNode;
		}

		retOpenNodeFlag = CheckRightDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RU, x, y, randBrush);
		if (retOpenNodeFlag)
		{
			break;
		}

		retOpenNodeFlag = CheckUpDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RU, x, y, randBrush);
		if (retOpenNodeFlag)
		{
			break;
		}
	}

	return nullptr;
}


//======================================================
// ������ �Ʒ� �밢�� �Լ��Դϴ�. 
//======================================================
JumpPointSearch::NODE* JumpPointSearch::CheckRightDown(NODE* parentNode, NODE* destNode, int x, int y, HBRUSH randBrush, bool firstCall)
{
	
	bool retOpenNodeFlag = false;
	
	if (firstCall)
	{
		//===================================================================
		// ���� ���ǿ� ������ 5�������� Ž���ϰ� �ȴ�.
		if (destNode == CheckRightHorizontal(parentNode, destNode, x, y, randBrush))
		{
			return destNode;
		}

		if (destNode == CheckDownVertical(parentNode, destNode, x, y, randBrush))
		{
			return destNode;
		}
		//===================================================================
	}

	while (1)
	{
		x += 1;

		y += 1;

		if (x >= MAX_WIDTH || y >= MAX_HEIGHT)
		{
			break;
		}

		if (brushBlockList[x][y] == grayBrush)
		{
			break;
		}

		if (destNode->mX == x && destNode->mY == y)
		{
			destNode->prev = parentNode;

			return destNode;
		}

		retOpenNodeFlag = CheckRightDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RD, x, y, randBrush);
		if (retOpenNodeFlag)
		{
			break;
		}

		retOpenNodeFlag = CheckDownDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_RD, x, y, randBrush);
		if (retOpenNodeFlag)
		{
			break;
		}
	}

	return nullptr;
}


//======================================================
// ���� �� �밢�� �Լ��Դϴ�. 
//======================================================
JumpPointSearch::NODE* JumpPointSearch::CheckLeftUp(NODE* parentNode, NODE* destNode, int x, int y, HBRUSH randBrush ,bool firstCall)
{
	bool retOpenNodeFlag = false;
		
	if (firstCall)
	{
		//===================================================================
		// ���� ���ǿ� ������ 5�������� Ž���ϰ� �ȴ�.
		if (destNode == CheckLeftHorizontal(parentNode, destNode, x, y, randBrush))
		{
			return destNode;
		}

		if (destNode == CheckUpVertical(parentNode, destNode, x, y, randBrush))
		{
			return destNode;
		}
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

		if (brushBlockList[x][y] == grayBrush)
		{
			break;
		}

		if (destNode->mX == x && destNode->mY == y)
		{
			destNode->prev = parentNode;

			return destNode;
		}

		retOpenNodeFlag = CheckLeftDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LU, x, y, randBrush);
		if (retOpenNodeFlag)
		{
			break;
		}

		retOpenNodeFlag = CheckUpDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LU, x, y, randBrush);
		if (retOpenNodeFlag)
		{
			break;
		}
	}
	return nullptr;
}


//======================================================
// ���� �Ʒ� �밢�� �Լ��Դϴ�. 
//======================================================
JumpPointSearch::NODE* JumpPointSearch::CheckLeftDown(NODE* parentNode, NODE* destNode, int x, int y, HBRUSH randBrush, bool firstCall)
{
	bool retOpenNodeFlag = false;
	
	if (firstCall == true)
	{
		//===================================================================
		// ���� ���ǿ� ������ 5�������� Ž���ϰ� �ȴ�.
		if (destNode == CheckLeftHorizontal(parentNode, destNode, x, y, randBrush))
		{
			return destNode;
		}

		if (destNode == CheckDownVertical(parentNode, destNode, x, y, randBrush))
		{
			return destNode;
		}
		//===================================================================
	}


	while (1)
	{
		x -= 1;
		y += 1;

		if (x < 0 || y >= MAX_HEIGHT)
		{
			break;
		}

		if (brushBlockList[x][y] == grayBrush)
		{
			break;
		}

		if (destNode->mX == x && destNode->mY == y)
		{
			destNode->prev = parentNode;

			return destNode;
		}

		retOpenNodeFlag = CheckLeftDiagonalHorizontal(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LD, x, y, randBrush);
		if (retOpenNodeFlag)
		{
			break;
		}

		retOpenNodeFlag = CheckDownDiagonalVertical(parentNode, destNode, NODE_DIRECTION::NODE_DIR_LD, x, y, randBrush);
		if (retOpenNodeFlag)
		{
			break;
		}
	}
}






//=======================================================
// �밢�� �Լ��� ���� ������ ���� 
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

		// ���� ����Ʈ ��� �Ǵ� Ŭ���� ����Ʈ ��忡�� ĥ���� �ʴ´�.
		if (brushBlockList[iCnt][y] != blueBrush && brushBlockList[iCnt][y] != yellowBrush)
		{
			brushBlockList[iCnt][y] = randBrush;
		}

		if (y > 0 && iCnt + 1 < MAX_WIDTH)
		{
			if (brushBlockList[iCnt][y - 1] == grayBrush && (brushBlockList[iCnt + 1][y - 1] == oldBrush || brushBlockList[iCnt + 1][y - 1] == redBrush))
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
			if (brushBlockList[iCnt][y + 1] == grayBrush && (brushBlockList[iCnt + 1][y + 1] == oldBrush || brushBlockList[iCnt + 1][y + 1] == redBrush))
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
// �밢�� �Լ��� ���� ���� ����
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

		// ���� ����Ʈ ��� �Ǵ� Ŭ���� ����Ʈ ��忡�� ĥ���� �ʴ´�.
		if (brushBlockList[iCnt][y] != blueBrush && brushBlockList[iCnt][y] != yellowBrush)
		{
			brushBlockList[iCnt][y] = randBrush;
		}

		
		if (y > 0 && iCnt > 0)
		{
			if (brushBlockList[iCnt][y - 1] == grayBrush && (brushBlockList[iCnt - 1][y - 1] == oldBrush || brushBlockList[iCnt - 1][y - 1] == redBrush))
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
			if(brushBlockList[iCnt][y + 1] == grayBrush && (brushBlockList[iCnt - 1][y + 1] == oldBrush || brushBlockList[iCnt - 1][y + 1] == redBrush))
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
// �밢�� �Լ��� ���� �� ����
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


		// ���� ����Ʈ ��� �Ǵ� Ŭ���� ����Ʈ ��忡�� ĥ���� �ʴ´�.
		if (brushBlockList[x][iCnt] != blueBrush && brushBlockList[x][iCnt] != yellowBrush)
		{
			brushBlockList[x][iCnt] = randBrush;
		}

		if (x > 0 && iCnt > 0)
		{
			if (brushBlockList[x - 1][iCnt] == grayBrush && (brushBlockList[x - 1][iCnt - 1] == oldBrush || brushBlockList[x - 1][iCnt - 1] == redBrush))
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
			if (brushBlockList[x + 1][iCnt] == grayBrush && (brushBlockList[x + 1][iCnt - 1] == oldBrush || brushBlockList[x + 1][iCnt - 1] == redBrush))
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
// �밢�� �Լ��� ���� �Ʒ� ����
//=======================================================
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

		// ���� ����Ʈ ��� �Ǵ� Ŭ���� ����Ʈ ��忡�� ĥ���� �ʴ´�.
		if (brushBlockList[x][iCnt] != blueBrush && brushBlockList[x][iCnt] != yellowBrush)
		{
			brushBlockList[x][iCnt] = randBrush;
		}

		if (x > 0 && iCnt + 1 < MAX_HEIGHT)
		{
			if (brushBlockList[x - 1][iCnt] == grayBrush && (brushBlockList[x - 1][iCnt + 1] == oldBrush || brushBlockList[x - 1][iCnt + 1] == redBrush))
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

			if (brushBlockList[x + 1][iCnt] == grayBrush && (brushBlockList[x + 1][iCnt + 1] == oldBrush || brushBlockList[x + 1][iCnt + 1] == redBrush))
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



//==================================================================
// �������� �������� ������ ����Ʈ�� ���� �ʱ�ȭ�ϰ� �ٽ� ���� ã�´�.
//==================================================================
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



//=============================================================
// ���������� ������������ List�� reset�Ѵ�.
//=============================================================
void JumpPointSearch::ResetRouteList()
{
	CList<JumpPointSearch::NODE*>::Iterator iterE = routeList.end();

	for (CList<JumpPointSearch::NODE*>::Iterator iter = routeList.begin(); iter != iterE;)
	{
		iter = routeList.erase(iter);
	}
}


//=============================================================
// �ڿ������� ���������� ������������ ��θ� reset�Ѵ�.
//=============================================================
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



//=====================================================================
// �������� ������ ��ֹ��� �����ϰ� �� ������ �����Ѵ�.
//======================================================================
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

//==================================================================
// ���������� ������������ ��θ� �ִ´�.
//==================================================================
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

//====================================================
// ����ȭ ���� �����ϴ� �Լ��Դϴ�.
//====================================================
void JumpPointSearch::PathOptimizing()
{

	int startX;
	int startY;

	int endX;
	int endY;

	int nextEndX;
	int nextEndY;

	// ���� ����� ���� ����Դϴ�.
	CList<JumpPointSearch::NODE*>::Iterator nextIter;

	// ���� ����� ���� ���� ����Դϴ�.
	CList<JumpPointSearch::NODE*>::Iterator nextNextIter;

	CList<JumpPointSearch::NODE*>::Iterator iterE = optimizeRouteList.end();

	CList<JumpPointSearch::NODE*>::Iterator iter = optimizeRouteList.begin();

	while(1)
	{
		startX = iter->mX;
		startY = iter->mY;

		nextIter = iter.NextIter();
		
		//========================================================================
		// iter�� ���� ��尡 ������ ������� break ������ �����Ѵ�.
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
			// nextNextIter ��尡 �� ������� ���� ��带 ���س�忡 �����Ѵ�.
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
			// ������ �̾����ٸ��� ���� ���� ������ ���� üũ�Ѵ�.
			//======================================================================
			if (BresenhamLine::MakeLine(startX, startY, nextEndX, nextEndY))
			{				
				(*nextIter)->deleteCheck = true;
			}
			else
			{
				//=====================================================================================
				// ��ֹ��� ���ؼ� �̾��� �� ���� ������� nextNextIter�� ���� ��带 �������� �ٲ۴�.
				//=====================================================================================
				--nextNextIter;

				iter = nextNextIter;

				break;
			}
		}


	}


	// ���� üũ�� ��带 �����մϴ�.
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