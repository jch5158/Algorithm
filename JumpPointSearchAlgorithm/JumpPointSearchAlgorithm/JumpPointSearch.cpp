#include "stdafx.h"
#include "CList.h"
#include "JumpPointSearchAlgorithm.h"
#include "BresenhamLine.h"
#include "JumpPointSearch.h"



//===================================================================
// Ÿ�̸ӷ� �ڵ� �Լ� ȣ�� �� ���� ������ ���� �ʵ��� �ϴ� Flag
//===================================================================
bool functionFlag = false;



JumpPointSearch::JumpPointSearch(int mapWidth, int mapHeight)
{
	mMapWidth = mapWidth;
	mMapHeight = mapHeight;
	
	mJspMap = (char**)malloc(sizeof(char*) * mMapHeight);

	for (int iCnt = 0; iCnt < mMapHeight; ++iCnt)
	{
		mJspMap[iCnt] = (char*)malloc(sizeof(char*) * mMapWidth);
	}

	mFuncSetFlag = true;	

	for (int iCntY = 0; iCntY < mMapHeight; ++iCntY)
	{
		for (int iCntX = 0; iCntX < mMapWidth; ++iCntX)
		{
			mJspMap[iCntY][iCntX] = (char)NODE_ATTRIBUTE::NODE_UNBLOCK;
		}
	}
}


JumpPointSearch::~JumpPointSearch()
{
	for (int iCntY = 0; iCntY < mMapHeight; ++iCntY)
	{
		free(mJspMap[iCntY]);
	}

	free(mJspMap);
}



void JumpPointSearch::SettingMapAttrivute(int posX, int posY)
{

	mJspMap[posY][posX] = (char)NODE_ATTRIBUTE::NODE_BLOCK;

}



bool JumpPointSearch::PathFind(int startX, int startY, int destinationX, int destinationY, RouteNode *routeNodeArray, int routeNodeArraySize)
{
	
	// ���� ���
	static JumpPointSearch::NODE* curNode = nullptr;	 


	// ���͸� �Է��ϱ� ������ �ش� �Լ��� ȣ�� �� �ٷ� ���ϵ˴ϴ�.
	if (functionFlag == false)
	{
		return false;
	}

	if (mFuncSetFlag == true)
	{
		// ������ ��� �����Ҵ�
		mDestinationNode = (NODE*)malloc(sizeof(NODE));

		// ������ ����� ��ǥ ����
		mDestinationNode->mX = destinationX;
		mDestinationNode->mY = destinationY;

		// ���� ��� ����
		mStartNode = (NODE*)malloc(sizeof(NODE));

		// ���۳�� ��ǥ����
		mStartNode->mX = startX;
		mStartNode->mY = startY;

		mStartNode->mNodeDir = (BYTE)NODE_DIRECTION::NODE_DIR_ALL;

		mStartNode->prev = nullptr;

		// ���۳�� G ����
		mStartNode->G = 0;

		// ������������ ���밪	
		mStartNode->H += (float)(abs(destinationX - startX) + abs(destinationY - startY));

		mStartNode->F = mStartNode->G + mStartNode->H;

		// Ŭ���� ����Ʈ�� �߰�
		mOpenList.PushBack(mStartNode);

		// �ѹ��� ���� ���õǵ��� ����
		mFuncSetFlag = false;
	}



	curNode = SelectOpenListNode();
	if (curNode->mX == mDestinationNode->mX && curNode->mY == mDestinationNode->mY)
	{
		InsertRoute(curNode);

		PathOptimizing();

		settingRouteArray(routeNodeArray, routeNodeArraySize);

		functionFlag = false;

		return true;
	}
	
	InsertOpenNode(curNode);

	// openList F�� ����
	openListBubbleSort();

	return false;
}

//======================================================
// ��ǥ�� openList�� ã�Ƽ� return 
//======================================================
JumpPointSearch::NODE* JumpPointSearch::FindOpenList(int openX, int openY)
{

	CList<NODE*>::Iterator iterE = mOpenList.end();

	for (CList<NODE*>::Iterator iter = mOpenList.begin(); iter != iterE; ++iter)
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
	CList<NODE*>::Iterator iterE = mCloseList.end();

	for (CList<NODE*>::Iterator iter = mCloseList.begin(); iter != iterE; ++iter)
	{
		if (iter->mX == closeX && iter->mY == closeY)
		{
			return true;
		}
	}

	return false;
}


void JumpPointSearch::InsertOpenNode(JumpPointSearch::NODE* node)
{
	if (node == nullptr)
	{
		return;
	}

	int posX = node->mX;
	int posY = node->mY;

	int newNodePosX = 0;
	int newNodePosY = 0;

	CheckDirection(node, posX, posY);

	return;
}


void JumpPointSearch::CheckDirection(NODE* node, int x, int y)
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

		CheckRightHorizontal(node, x, y, randBrush);	

		break;
	case NODE_DIRECTION::NODE_DIR_RD:

		CheckRightDown(node, x, y, randBrush);
		
		break;
	case NODE_DIRECTION::NODE_DIR_DD:
		
		CheckDownVertical(node, x, y, randBrush);
		
		break;
	case NODE_DIRECTION::NODE_DIR_LD:

		CheckLeftDown(node, x, y, randBrush);
		
		break;
	case NODE_DIRECTION::NODE_DIR_LL:
	
		CheckLeftHorizontal(node, x, y, randBrush);
		
		break;
	case NODE_DIRECTION::NODE_DIR_LU:

		CheckLeftUp(node, x, y, randBrush);
		
		break;
	case NODE_DIRECTION::NODE_DIR_UU:
		
		CheckUpVertical(node, x, y, randBrush);
		
		break;
	case NODE_DIRECTION::NODE_DIR_RU:

		CheckRightUp(node, x, y,randBrush);	

		break;
	case NODE_DIRECTION::NODE_DIR_ALL:

		CheckRightUp(node, x, y, randBrush, false);
	
		CheckRightHorizontal(node, x, y, randBrush);
		
		CheckRightDown(node, x, y, randBrush ,false);
		
		CheckDownVertical(node, x, y, randBrush);
		
		CheckLeftDown(node, x, y, randBrush ,false);
		
		CheckLeftHorizontal(node, x, y, randBrush);
		
		CheckLeftUp(node, x, y, randBrush, false);
		
		CheckUpVertical(node, x, y, randBrush);
		
		break;

	default:
		return;
	}

	return;
}


void JumpPointSearch::SetCornerNode(NODE* parentNode,  NODE_DIRECTION nodeDir, int x, int y)
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

			newNode->H = (float)(abs(mDestinationNode->mX - newNode->mX) + abs(mDestinationNode->mY - newNode->mY));

			newNode->F = newNode->G + newNode->H;

			newNode->mNodeDir = (BYTE)nodeDir;

			newNode->prev = parentNode;

			// ���� ������ �ٲ��ش�.

			if (x != mDestinationNode->mX || y != mDestinationNode->mY)
			{
				brushBlockList[newNode->mX][newNode->mY] = blueBrush;
			}
		
			mOpenList.PushBack(newNode);
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

				retOpenNode->H = (float)(abs(mDestinationNode->mX - retOpenNode->mX) + abs(mDestinationNode->mY - retOpenNode->mY));

				retOpenNode->F = retOpenNode->G + retOpenNode->H;

				retOpenNode->mNodeDir = (BYTE)nodeDir;

				retOpenNode->prev = parentNode;
				
			}
		}
	}


	return;
}


//===============================================
// ������ ���� Ž�� �Լ��Դϴ�.
//===============================================
void JumpPointSearch::CheckRightHorizontal(NODE* parentNode, int x, int y, HBRUSH randBrush)
{	
	
	//=================================================================================
	// ������ �� ���� Ž�� ������ �����ϴ�
	//=================================================================================
	if (y > 0 && x + 1 < mMapWidth)
	{
		if (mJspMap[y - 1][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
		{	
			CheckRightUp(parentNode, x, y, randBrush, false);				
		}
	}

	//=================================================================================
	// ������ �Ʒ� ���� Ž�� ������ �����ϴ�
	//=================================================================================
	if (y + 1 < mMapHeight && x + 1 < mMapWidth)
	{
		if (mJspMap[y + 1][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK )
		{
			CheckRightDown(parentNode, x, y, randBrush, false);	
		}
	}
	
	//============================================================================
	// iCnt = x + 1 �� �ϴ� ������ �ش� ��ġ�� ���� ��尡 �ִ� ��ġ�̱� �����̴�. 
	// iCnt + 1 < mMapWidth �� ������ ���� �Ʒ��� ���� ���� ��� X + 1 ��ǥ�� 
	// ���� ������ Ȯ���ϱ� �������� �����÷ο찡 �߻����� �ʵ��� �ϱ� �����Դϴ�.
	//============================================================================
	for (int iCnt = x + 1; iCnt < mMapWidth; ++iCnt)
	{
		// ���� ������ ��� retur nullptr �� �Ѵ�.
		if (mJspMap[y][iCnt] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return;
		}

		// �������� ���� ��� �ٷ� �̾��ְ� return �Ѵ�. 
		if (iCnt == mDestinationNode->mX && y == mDestinationNode->mY)
		{
			SetCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_RR, iCnt, y);

			return;
		}


		// ���� ����Ʈ ��� �Ǵ� Ŭ���� ����Ʈ ��忡�� ĥ���� �ʴ´�.
		if (brushBlockList[iCnt][y] != blueBrush && brushBlockList[iCnt][y] != yellowBrush)
		{
			brushBlockList[iCnt][y] = randBrush;
		}


		//====================================================================
		// y ���� 0 �̻��� ��쿡�� ���� ���� Ȯ���� �� �ִ�.
		//====================================================================
		if (y > 0 && iCnt + 1 < mMapWidth )
		{
			if (mJspMap[y - 1][iCnt] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][iCnt + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				SetCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_RR, iCnt, y);
				
				return;
			}
		}


		//====================================================================
		// y ���� y + 1 < mMapHeight �� ��쿡�� ���� ���� Ȯ���� �� �ִ�.
		//====================================================================
		if (y + 1 < mMapHeight && iCnt + 1 < mMapWidth)
		{
			if (mJspMap[y + 1][iCnt] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][iCnt + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				SetCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_RR, iCnt, y);
			
				return;
			}
		}
	}

	return;
}


//===============================================
// ���� ���� Ž�� �Լ��Դϴ�.
//===============================================
void JumpPointSearch::CheckLeftHorizontal(NODE* parentNode, int x, int y, HBRUSH randBrush)
{

	//=================================================================================
	// ���� �� ���� Ž�� ������ �����ϴ�
	//=================================================================================
	if (y > 0 && x > 0)
	{
		if (mJspMap[y - 1][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
		{		
			CheckLeftUp(parentNode, x, y, randBrush, false);			
		}
	}

	//=================================================================================
	// ���� �Ʒ� ���� Ž�� ������ �����ϴ�
	//=================================================================================
	if (y + 1 < mMapHeight && x > 0)
	{
		if (mJspMap[y + 1][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK )
		{
			CheckLeftDown(parentNode, x, y, randBrush, false);		
		}
	}

	for (int iCnt = x - 1; iCnt >= 0; --iCnt)
	{
		// �ش� ��ǥ�� ��ֹ��� ������ �ٷ� return �Ѵ�.
		if (mJspMap[y][iCnt] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return;
		}


		// �������� ������ �̾��ְ� �ٷ� return �Ѵ�.
		if (iCnt == mDestinationNode->mX && y == mDestinationNode->mY)
		{
			SetCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_LL, iCnt, y);

			return;
		}

		// ���� ����Ʈ ��� �Ǵ� Ŭ���� ����Ʈ ��忡�� ĥ���� �ʴ´�.
		if (brushBlockList[iCnt][y] != blueBrush && brushBlockList[iCnt][y] != yellowBrush)
		{
			brushBlockList[iCnt][y] = randBrush;
		}

		if (y > 0 && iCnt - 1 >= 0)
		{
			if (mJspMap[y - 1][iCnt] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][iCnt - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				SetCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_LL, iCnt, y);

				return;
			}
		}


		if (y + 1 < mMapHeight && iCnt - 1 >= 0)
		{
			if (mJspMap[y + 1][iCnt] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][iCnt - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				SetCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_LL, iCnt, y);

				return;
			}
		}
	}

	return;
}


//===============================================
// ���� �� ���� Ž�� �Լ��Դϴ�.
//===============================================
void JumpPointSearch::CheckUpVertical(NODE* parentNode, int x, int y, HBRUSH randBrush)
{	
	
	//=================================================================================
	// ���� �� ���� Ž�� ������ �����ϴ�
	//=================================================================================
	if (x > 0 && y > 0)
	{
		if (mJspMap[y][x - 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
		{
			CheckLeftUp(parentNode, x, y, randBrush, false);	
		}
	}

	//=================================================================================
	// ������ �� ���� Ž�� ������ �����ϴ�
	//=================================================================================
	if (x + 1 < mMapWidth && y > 0)
	{
		if (mJspMap[y][x + 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
		{ 	
			CheckRightUp(parentNode, x, y, randBrush, false);	
		}
	}

	for (int iCnt = y - 1; iCnt >= 0; --iCnt)
	{
		//=============================================
		// �ش� ��ġ�� ��ֹ��� ���� ��� �ٷ� return
		//=============================================
		if (mJspMap[iCnt][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return;
		}


		//===================================================
		// �ش� ��ġ�� �������� ���� ��� ��� ���� �� return
		//===================================================
		if (x == mDestinationNode->mX && iCnt == mDestinationNode->mY)
		{
			SetCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_UU, x, iCnt);

			return;
		}

		// ���� ����Ʈ ��� �Ǵ� Ŭ���� ����Ʈ ��忡�� ĥ���� �ʴ´�.
		if (brushBlockList[x][iCnt] != blueBrush && brushBlockList[x][iCnt] != yellowBrush)
		{
			brushBlockList[x][iCnt] = randBrush;
		}


		if (x > 0 && iCnt - 1 >= 0)
		{
			if (mJspMap[iCnt][x - 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[iCnt - 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				SetCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_UU, x, iCnt);

				return;
			}
		}


		if (x + 1 < mMapWidth && iCnt - 1 >= 0)
		{
			if (mJspMap[iCnt][x + 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[iCnt - 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK )
			{
				SetCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_UU, x, iCnt);

				return;
			}
		}	
	}

	return;
}


//===============================================
// ���� �Ʒ� ���� Ž�� �Լ��Դϴ�.
//===============================================
void JumpPointSearch::CheckDownVertical(NODE* parentNode, int x, int y, HBRUSH randBrush)
{

	//=================================================================================
	// ���� �Ʒ� ���� Ž�� ������ �����ϴ�
	//=================================================================================
	if (x > 0 && y + 1 < mMapHeight)
	{
		if (mJspMap[y][x - 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK )
		{
			CheckLeftDown(parentNode, x, y, randBrush, false);
			
		}
	}

	//=================================================================================
	// ������ �Ʒ� ���� Ž�� ������ �����ϴ�
	//=================================================================================
	if (x + 1 < mMapWidth && y + 1 < mMapHeight)
	{
		if (mJspMap[y][x + 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
		{
			CheckRightDown(parentNode, x, y, randBrush, false);
			
		}
	}


	for (int iCnt = y + 1; iCnt < mMapHeight; ++iCnt)
	{
		// ��ֹ��� ����ĥ ��� return �Ѵ�.
		if (mJspMap[iCnt][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return;
		}


		// �������� ������ ��� �������� �̾��ְ� return �Ѵ�.
		if (x == mDestinationNode->mX && iCnt == mDestinationNode->mY)
		{
			SetCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_DD, x, iCnt);

			return;
		}

		// ���� ����Ʈ ��� �Ǵ� Ŭ���� ����Ʈ ��忡�� ĥ���� �ʴ´�.
		if (brushBlockList[x][iCnt] != blueBrush && brushBlockList[x][iCnt] != yellowBrush)
		{
			brushBlockList[x][iCnt] = randBrush;
		}

		if (x > 0 && iCnt + 1 < mMapHeight )
		{
			if (mJspMap[iCnt][x - 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[iCnt + 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				SetCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_DD, x, iCnt);

				return;
			}
		}

		if (x + 1 < mMapWidth && iCnt + 1 < mMapHeight)
		{
			if (mJspMap[iCnt][x + 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[iCnt + 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				SetCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_DD, x, iCnt);

				return;
			}
		}		
	}

	return;
}



//======================================================
// ������ �� �밢�� �Լ��Դϴ�. 
//======================================================
void JumpPointSearch::CheckRightUp(NODE* parentNode, int x, int y, HBRUSH randBrush ,bool firstCall)
{		
	bool setOpenNodeFlag;

	//===============================================
	// �밢�� �Լ� ȣ�� �� ���� �Լ� ȣ�� ����
	//===============================================
	if (firstCall)
	{	
		//===================================================================
		// ���� ���ǿ� ������ 5�������� Ž���ϰ� �ȴ�.
		CheckRightHorizontal(parentNode, x, y, randBrush);

		CheckUpVertical(parentNode, x, y, randBrush);		
		//===================================================================
	}

	while (1)
	{
		x += 1;

		y -= 1;

		// �迭 ������ ��� ��� ȣ�� ���� 
		if (x >= mMapWidth || y < 0)
		{
			break;
		}

		// ��ֹ��� ������ ��� ȣ�� ����
		if (mJspMap[y][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			break;
		}


		// �������� �������� ���������� �̾��ְ� return �Ѵ�.
		if (mDestinationNode->mX == x && mDestinationNode->mY == y)
		{			
			SetCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_RU, x, y);

			return;
		}

		setOpenNodeFlag = CheckRightDiagonalHorizontal(parentNode, NODE_DIRECTION::NODE_DIR_RU, x, y, randBrush);
		if (setOpenNodeFlag)
		{
			return;
		}


		setOpenNodeFlag = CheckUpDiagonalVertical(parentNode, NODE_DIRECTION::NODE_DIR_RU, x, y, randBrush);
		if (setOpenNodeFlag)
		{
			return;
		}
	}

	return;
}


//======================================================
// ������ �Ʒ� �밢�� �Լ��Դϴ�. 
//======================================================
void JumpPointSearch::CheckRightDown(NODE* parentNode, int x, int y, HBRUSH randBrush, bool firstCall)
{	
	bool setOpenNodeFlag;

	if (firstCall)
	{
		//===================================================================
		// ���� ���ǿ� ������ 5�������� Ž���ϰ� �ȴ�.
		CheckRightHorizontal(parentNode, x, y, randBrush);
		
		CheckDownVertical(parentNode, x, y, randBrush);	
		//===================================================================
	}

	while (1)
	{
		x += 1;

		y += 1;

		if (x >= mMapWidth || y >= mMapHeight)
		{
			break;
		}

		if (mJspMap[y][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			break;
		}

		if (mDestinationNode->mX == x && mDestinationNode->mY == y)
		{			
			SetCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_RD, x, y);

			return;
		}

		setOpenNodeFlag = CheckRightDiagonalHorizontal(parentNode, NODE_DIRECTION::NODE_DIR_RD, x, y, randBrush);
		if (setOpenNodeFlag)
		{
			return;
		}

		setOpenNodeFlag	= CheckDownDiagonalVertical(parentNode, NODE_DIRECTION::NODE_DIR_RD, x, y, randBrush);
		if (setOpenNodeFlag)
		{
			return;
		}
	}

	return;
}


//======================================================
// ���� �� �밢�� �Լ��Դϴ�. 
//======================================================
void JumpPointSearch::CheckLeftUp(NODE* parentNode, int x, int y, HBRUSH randBrush ,bool firstCall)
{
	bool setOpenNodeFlag;

	if (firstCall)
	{
		//===================================================================
		// ���� ���ǿ� ������ 5�������� Ž���ϰ� �ȴ�.
		CheckLeftHorizontal(parentNode, x, y, randBrush);	

		CheckUpVertical(parentNode, x, y, randBrush);		
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

		if (mJspMap[y][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			break;
		}

		if (mDestinationNode->mX == x && mDestinationNode->mY == y)
		{
			
			SetCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_LU, x, y);

			return;
		}

		setOpenNodeFlag = CheckLeftDiagonalHorizontal(parentNode, NODE_DIRECTION::NODE_DIR_LU, x, y, randBrush);
		if (setOpenNodeFlag)
		{
			return;
		}

		setOpenNodeFlag = CheckUpDiagonalVertical(parentNode, NODE_DIRECTION::NODE_DIR_LU, x, y, randBrush);
		if (setOpenNodeFlag)
		{
			return;
		}
	}

	return;
}


//======================================================
// ���� �Ʒ� �밢�� �Լ��Դϴ�. 
//======================================================
void JumpPointSearch::CheckLeftDown(NODE* parentNode, int x, int y, HBRUSH randBrush, bool firstCall)
{
	bool setOpenNodeFlag;

	if (firstCall == true)
	{
		//===================================================================
		// ���� ���ǿ� ������ 5�������� Ž���ϰ� �ȴ�.
		CheckLeftHorizontal(parentNode, x, y, randBrush);
		

		CheckDownVertical(parentNode,  x, y, randBrush);		
		//===================================================================
	}


	while (1)
	{
		x -= 1;
		y += 1;

		if (x < 0 || y >= mMapHeight)
		{
			break;
		}

		if (mJspMap[y][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			break;
		}

		if (mDestinationNode->mX == x && mDestinationNode->mY == y)
		{
			SetCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_LD, x, y);

			return;
		}

		setOpenNodeFlag = CheckLeftDiagonalHorizontal(parentNode, NODE_DIRECTION::NODE_DIR_LD, x, y, randBrush);
		if (setOpenNodeFlag)
		{
			return;
		}

		setOpenNodeFlag = CheckDownDiagonalVertical(parentNode, NODE_DIRECTION::NODE_DIR_LD, x, y, randBrush);
		if (setOpenNodeFlag)
		{
			return;
		}
	}
}



//=======================================================
// �밢�� �Լ��� ���� ������ ���� 
//=======================================================
bool JumpPointSearch::CheckRightDiagonalHorizontal(NODE* parentNode, NODE_DIRECTION nodeDir,int x, int y, HBRUSH randBrush)
{

	for (int iCnt = x; iCnt < mMapWidth; ++iCnt)
	{
		if (mJspMap[y][iCnt] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return false;
		}
	
		// ������ ��带 ������ ��쿡�� �� �ڸ��� ��� ������ return 
		if (iCnt == mDestinationNode->mX && y == mDestinationNode->mY)
		{
			SetCornerNode(parentNode, nodeDir, x, y);
			
			return true;
		}

		// ���� ����Ʈ ��� �Ǵ� Ŭ���� ����Ʈ ��忡�� ĥ���� �ʴ´�.
		if (brushBlockList[iCnt][y] != blueBrush && brushBlockList[iCnt][y] != yellowBrush)
		{
			brushBlockList[iCnt][y] = randBrush;
		}

		if (y > 0 && iCnt + 1 < mMapWidth)
		{
			if (mJspMap[y - 1][iCnt] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][iCnt + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				SetCornerNode(parentNode, nodeDir, x, y);

				return true;
			}
		}

		if(y + 1 < mMapHeight && iCnt + 1 < mMapWidth)
		{
			if (mJspMap[y + 1][iCnt] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][iCnt + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				SetCornerNode(parentNode, nodeDir, x, y);

				return true;
			}
		}	
	}

	return false;
}


//=======================================================
// �밢�� �Լ��� ���� ���� ����
//=======================================================
bool JumpPointSearch::CheckLeftDiagonalHorizontal(NODE* parentNode, NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush)
{

	for (int iCnt = x; iCnt >= 0; --iCnt)
	{
		if (mJspMap[y][iCnt] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return false;
		}		

		//=============================================
		// �������� ������ ��� return
		//=============================================
		if (iCnt == mDestinationNode->mX && y == mDestinationNode->mY)
		{
			SetCornerNode(parentNode, nodeDir, x, y);

			return true;
		}

		// ���� ����Ʈ ��� �Ǵ� Ŭ���� ����Ʈ ��忡�� ĥ���� �ʴ´�.
		if (brushBlockList[iCnt][y] != blueBrush && brushBlockList[iCnt][y] != yellowBrush)
		{
			brushBlockList[iCnt][y] = randBrush;
		}

		
		if (y > 0 && iCnt > 0)
		{
			if (mJspMap[y - 1][iCnt] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][iCnt - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
			    SetCornerNode(parentNode, nodeDir, x, y);

				return true;
			}
		}

		if (y + 1 < mMapHeight && iCnt > 0)
		{
			if(mJspMap[y + 1][iCnt] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][iCnt - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				SetCornerNode(parentNode, nodeDir, x, y);
				
				return true;
			}
		}	
	}

	return false;
}


//=======================================================
// �밢�� �Լ��� ���� �� ����
//=======================================================
bool JumpPointSearch::CheckUpDiagonalVertical(NODE* parentNode, NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush)
{
	
	for (int iCnt = y; iCnt >= 0; --iCnt)
	{
		// ��ֹ� ������ ��� return
		if (mJspMap[iCnt][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return false;
		}
		
		//=================================================
		// ������ ������ ��� ��� ���� �� return
		//=================================================
		if (x == mDestinationNode->mX && iCnt == mDestinationNode->mY)
		{
			SetCornerNode(parentNode, nodeDir, x, y);

			return true;
		}


		// ���� ����Ʈ ��� �Ǵ� Ŭ���� ����Ʈ ��忡�� ĥ���� �ʴ´�.
		if (brushBlockList[x][iCnt] != blueBrush && brushBlockList[x][iCnt] != yellowBrush)
		{
			brushBlockList[x][iCnt] = randBrush;
		}

		if (x > 0 && iCnt > 0)
		{
			if (mJspMap[iCnt][x - 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[iCnt - 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				SetCornerNode(parentNode, nodeDir, x, y);
				
				return true;
			}
		}

		if (x + 1 < mMapWidth && iCnt > 0)
		{
			if (mJspMap[iCnt][x + 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[iCnt - 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				SetCornerNode(parentNode, nodeDir, x, y);

				return true;
			}
		}
	}

	return false;
}


//=======================================================
// �밢�� �Լ��� ���� �Ʒ� ����
//=======================================================
bool JumpPointSearch::CheckDownDiagonalVertical(NODE* parentNode, NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush)
{

	for (int iCnt = y; iCnt < mMapHeight; iCnt++)
	{
		if (mJspMap[iCnt][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return false;
		}	

		if (x == mDestinationNode->mX && iCnt == mDestinationNode->mY)
		{
			SetCornerNode(parentNode, nodeDir, x, y);

			return true;
		}

		// ���� ����Ʈ ��� �Ǵ� Ŭ���� ����Ʈ ��忡�� ĥ���� �ʴ´�.
		if (brushBlockList[x][iCnt] != blueBrush && brushBlockList[x][iCnt] != yellowBrush)
		{
			brushBlockList[x][iCnt] = randBrush;
		}

		if (x > 0 && iCnt + 1 < mMapHeight)
		{
			if (mJspMap[iCnt][x - 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[iCnt + 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				SetCornerNode(parentNode, nodeDir, x, y);
				
				return true;
			}
		}

		if (x + 1 < mMapWidth && iCnt + 1 < mMapHeight)
		{

			if (mJspMap[iCnt][x + 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[iCnt + 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				SetCornerNode(parentNode, nodeDir, x, y);				

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
	CList<JumpPointSearch::NODE*>::Iterator iter = mOpenList.begin();

	if ((*iter)->data == nullptr)
	{
		return nullptr;
	}

	// ����� ���� �����Ѵ�.
	NODE* node = (*iter)->data;

	// ���� ����Ʈ���� �ش� ��带 �����.
	mOpenList.erase(iter);

	// closeList�� �־��ش�.
	mCloseList.PushFront(node);

	// Ŭ�����Ʈ�̴� ������ �ٲپ��ش�.

	if ((mStartNode->mX != node->mX || mStartNode->mY != node->mY) && (mDestinationNode->mX != node->mX || mDestinationNode->mY != node->mY) )
	{
		brushBlockList[node->mX][node->mY] = yellowBrush;
	}

	return node;
}


//=============================================================
// ��� ����Ʈ�� ���� �ʱ�ȭ ��Ų��.
//=============================================================
void JumpPointSearch::ResetAll(RouteNode* routeNodeArray, int routeNodeArraySize)
 {	
	if (mFuncSetFlag == false)
	{
		free(mStartNode);

 		free(mDestinationNode);
	}

	functionFlag = false;

	mFuncSetFlag = true;

	ResetOpenList();

	ResetCloseList();

	ResetRouteList();

	resetRouteNodeArray(routeNodeArray, routeNodeArraySize);

	OptimizeRouteReset();

	resetJspMap();

	ResetBlock();
}



//==================================================================
// �������� �������� ������ ����Ʈ�� ���� �ʱ�ȭ�ϰ� �ٽ� ���� ã�´�.
//==================================================================
void JumpPointSearch::ReStart(RouteNode* routeNodeArray, int routeNodeArraySize)
{
	if (mFuncSetFlag == false)
	{
		free(mStartNode);

		free(mDestinationNode);
	}

	functionFlag = true;

	mFuncSetFlag = true;

	ResetOpenList();

	ResetCloseList();

	ResetRouteList();

	resetJspMap();

	RouteReset();

	resetRouteNodeArray(routeNodeArray, routeNodeArraySize);

	OptimizeRouteReset();
}


//=============================================================
// ���¸���Ʈ ��带 �����Ѵ�.
//=============================================================
void JumpPointSearch::ResetOpenList()
{
	CList<JumpPointSearch::NODE*>::Iterator iterE = mOpenList.end();

	for (CList<JumpPointSearch::NODE*>::Iterator iter = mOpenList.begin(); iter != iterE;)
	{
		iter = mOpenList.erase(iter);
	}
}



//=============================================================
// Ŭ�����Ʈ ��带 �����Ѵ�.
//=============================================================
void JumpPointSearch::ResetCloseList()
{
	CList<JumpPointSearch::NODE*>::Iterator iterE = mCloseList.end();

	for (CList<JumpPointSearch::NODE*>::Iterator iter = mCloseList.begin(); iter != iterE;)
	{
		iter = mCloseList.erase(iter);
	}
}



//=============================================================
// ���������� ������������ List�� reset�Ѵ�.
//=============================================================
void JumpPointSearch::ResetRouteList()
{
	CList<JumpPointSearch::NODE*>::Iterator iterE = mRouteList.end();

	for (CList<JumpPointSearch::NODE*>::Iterator iter = mRouteList.begin(); iter != iterE;)
	{
		iter = mRouteList.erase(iter);
	}
}


//=============================================================
// �ڿ������� ���������� ������������ ��θ� reset�Ѵ�.
//=============================================================
void JumpPointSearch::OptimizeRouteReset()
{
	CList<JumpPointSearch::NODE*>::Iterator iterE = mOptimizeRouteList.end();

	for (CList<JumpPointSearch::NODE*>::Iterator iter = mOptimizeRouteList.begin(); iter != iterE;)
	{
		iter = mOptimizeRouteList.erase(iter);
	}
}



//=============================================================
// ������ �����Ѵ�.
//=============================================================
void JumpPointSearch::ResetBlock()
{
	for (int iCntY = 0; iCntY < mMapHeight; iCntY++)
	{
		for (int iCntX = 0; iCntX < mMapWidth; iCntX++)
		{
			brushBlockList[iCntX][iCntY] = oldBrush;
		}
	}
}



void JumpPointSearch::resetJspMap()
{
	for (int iCntY = 0; iCntY < mMapHeight; ++iCntY)
	{
		for (int iCntX = 0; iCntX < mMapWidth; ++iCntX)
		{
			mJspMap[iCntY][iCntX] = (char)NODE_ATTRIBUTE::NODE_UNBLOCK;
		}
	}
}


//=====================================================================
// �������� ������ ��ֹ��� �����ϰ� �� ������ �����Ѵ�.
//======================================================================
void JumpPointSearch::RouteReset()
{
	for (int iCntY = 0; iCntY < mMapHeight; iCntY++)
	{
		for (int iCntX = 0; iCntX < mMapWidth; iCntX++)
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
		mRouteList.PushFront(node);

		mOptimizeRouteList.PushFront(node);

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

	CList<JumpPointSearch::NODE*>::Iterator iterE = mOptimizeRouteList.end();

	CList<JumpPointSearch::NODE*>::Iterator iter = mOptimizeRouteList.begin();

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
	for (iter = mOptimizeRouteList.begin(); iter != iterE;)
	{
		if ((*iter)->deleteCheck)
		{
			iter = mOptimizeRouteList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}



void JumpPointSearch::settingRouteArray(RouteNode* routeNodeArray, int routeArraySize)
{
	int index = 0;

	CList<NODE*>::Iterator iterE = mOptimizeRouteList.end();

	for (CList<NODE*>::Iterator iter = mOptimizeRouteList.begin(); iter != iterE; ++iter)
	{
		routeNodeArray[index].mPosX = iter->mX;

		routeNodeArray[index].mPosY = iter->mY;

		routeNodeArray[index].mRouteFlag = true;

		index += 1;
		
		if (index >= routeArraySize)
		{
			return;
		}

	}
}



void JumpPointSearch::resetRouteNodeArray(RouteNode* routeNodeArray, int routeNodeArraySize)
{
	for (int iCnt = 0; iCnt < routeNodeArraySize; ++iCnt)
	{
		routeNodeArray[iCnt].mRouteFlag = false;
	}
}

//=============================================================
// F�� ���� ������ �����Ѵ�.
//=============================================================
void JumpPointSearch::openListBubbleSort() {

	CList<JumpPointSearch::NODE*>::Iterator iterE = mOpenList.end();

	--iterE;

	for (int iCnt = 0; iCnt < mOpenList.listLength; iCnt++)
	{
		for (CList<JumpPointSearch::NODE*>::Iterator iter = mOpenList.begin(); iter != iterE; ++iter)
		{
			CList<JumpPointSearch::NODE*>::Iterator iterN = iter.node->next;

			if (iter->F > iterN->F)
			{
				mOpenList.DataSwap(iter, iterN);
			}
		}

		--iterE;
	}

}