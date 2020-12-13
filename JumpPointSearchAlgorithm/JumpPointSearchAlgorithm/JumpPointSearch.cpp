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



void JumpPointSearch::SettingMapAttribute(int posX, int posY)
{

	mJspMap[posY][posX] = (char)NODE_ATTRIBUTE::NODE_BLOCK;

}


void JumpPointSearch::setNodeGHF(NODE* pParentNode, int x, int y, float* pG, float* pH, float* pF)
{		
	if (pParentNode == nullptr)
	{
		*pG = 0.0f;
	}
	else
	{
		// subX �� subY�� ���ÿ� 1 �̻� ���̳� ��� �밢���̴�.
		if (abs(pParentNode->mX - x) >= 1 && abs(pParentNode->mY - y) >= 1)
		{
			*pG += 1.5f;
		}
		else
		{
			*pG += 1.0f;
		}
	}
	
	*pH = (float)(abs(mDestinationNode->mX - x) + abs(mDestinationNode->mY - y));

	*pF = *pG + *pH;

	return;
}




bool JumpPointSearch::PathFind(int startX, int startY, int destinationX, int destinationY, RouteNode *routeNodeArray, int routeNodeArraySize, RouteNode* optimizeNodeArray, int optimizeNodeArraySize)
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

		// ���۳�� ����
		setNodeGHF(nullptr, startX, startY, &mStartNode->G, &mStartNode->H, &mStartNode->F);

		// ���۳�� ��ǥ����
		mStartNode->mX = startX;
		mStartNode->mY = startY;

		mStartNode->mNodeDir = (BYTE)NODE_DIRECTION::NODE_DIR_ALL;

		mStartNode->prev = nullptr;
		
		// ���� ����Ʈ�� �߰�
		mOpenList.PushBack(mStartNode);

		// �� ���� ���� ���õǵ��� ����
		mFuncSetFlag = false;
	}



	curNode = selectOpenListNode();
	if (curNode->mX == mDestinationNode->mX && curNode->mY == mDestinationNode->mY)
	{
		insertRoute(curNode);

		pathOptimizing();

		settingRouteArray(routeNodeArray, routeNodeArraySize);

		settingOptimizeArray(optimizeNodeArray, optimizeNodeArraySize);

		functionFlag = false;

		return true;
	}
	
	// �� ã�� ���� ����
	insertOpenNode(curNode);

	// openList F�� ����
	openListBubbleSort();

	return false;
}

//======================================================
// ��ǥ�� openList�� ã�Ƽ� return 
//======================================================
JumpPointSearch::NODE* JumpPointSearch::findOpenList(int openX, int openY)
{

	CList<NODE*>::Iterator iterE = mOpenList.end();

	for (CList<NODE*>::Iterator iter = mOpenList.begin(); iter != iterE; ++iter)
	{
		if ((*iter)->mX == openX && (*iter)->mY == openY)
		{
			return *iter;
		}
	}

	return nullptr;
}

//======================================================
// �ش� ��ǥ�� closeList�� �ִٸ��� true�� �����Ѵ�.
//======================================================
bool JumpPointSearch::findCloseList(int closeX, int closeY)
{
	CList<NODE*>::Iterator iterE = mCloseList.end();

	for (CList<NODE*>::Iterator iter = mCloseList.begin(); iter != iterE; ++iter)
	{
		if ((*iter)->mX == closeX && (*iter)->mY == closeY)
		{
			return true;
		}
	}

	return false;
}


void JumpPointSearch::insertOpenNode(JumpPointSearch::NODE* node)
{
	if (node == nullptr)
	{
		return;
	}

	int posX = node->mX;
	int posY = node->mY;

	checkDirection(node, posX, posY);

	return;
}


void JumpPointSearch::checkDirection(NODE* node, int x, int y)
{
	NODE* retNode;
	
	HBRUSH randBrush;

	for (;;)
	{
		randBrush = CreateSolidBrush(RGB(rand() % 130 + 126, rand() % 130 + 126, rand() % 130 + 126));

		if (randBrush != grayBrush && randBrush != routeBrush)
		{
			break;
		}
	}

	// ��� ���⿡ �´� Ž���Լ��� ȣ���Ѵ�.
	switch ((NODE_DIRECTION)node->mNodeDir)
	{
	case NODE_DIRECTION::NODE_DIR_RR:

		checkRightHorizontal(node, x, y, randBrush);	

		break;
	case NODE_DIRECTION::NODE_DIR_RD:

		checkRightDown(node, x, y, randBrush);
		
		break;
	case NODE_DIRECTION::NODE_DIR_DD:
		
		checkDownVertical(node, x, y, randBrush);
		
		break;
	case NODE_DIRECTION::NODE_DIR_LD:

		checkLeftDown(node, x, y, randBrush);
		
		break;
	case NODE_DIRECTION::NODE_DIR_LL:
	
		checkLeftHorizontal(node, x, y, randBrush);
		
		break;
	case NODE_DIRECTION::NODE_DIR_LU:

		checkLeftUp(node, x, y, randBrush);
		
		break;
	case NODE_DIRECTION::NODE_DIR_UU:
		
		checkUpVertical(node, x, y, randBrush);
		
		break;
	case NODE_DIRECTION::NODE_DIR_RU:

		checkRightUp(node, x, y,randBrush);	

		break;
	case NODE_DIRECTION::NODE_DIR_ALL:

		checkRightUp(node, x, y, randBrush, false);
	
		checkRightHorizontal(node, x, y, randBrush);
		
		checkRightDown(node, x, y, randBrush ,false);
		
		checkDownVertical(node, x, y, randBrush);
		
		checkLeftDown(node, x, y, randBrush ,false);
		
		checkLeftHorizontal(node, x, y, randBrush);
		
		checkLeftUp(node, x, y, randBrush, false);
		
		checkUpVertical(node, x, y, randBrush);
		
		break;

	default:
		return;
	}

	return;
}


void JumpPointSearch::setCornerNode(NODE* parentNode,  NODE_DIRECTION nodeDir, int x, int y)
{
	NODE* newNode = nullptr;

	NODE* retOpenNode = nullptr;
	
	
	if (findCloseList(x, y) == false)
	{
		float tempG = 0;
		float tempH = 0;
		float tempF = 0;

		setNodeGHF(parentNode, x, y, &tempG, &tempH, &tempF);

		// ���¸���Ʈ�� ���� ������� ����
		retOpenNode = findOpenList(x, y);
		if (retOpenNode == nullptr)
		{
			newNode = (NODE*)malloc(sizeof(NODE));

			newNode->mX = x;

			newNode->mY = y;			

			newNode->G = tempG;

			newNode->H = tempH;

			newNode->F = tempF;

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
			// ������ ���¸���Ʈ�� �־��� ��� �θ��� G ���� parentNode�� G�� ���� ũ�ٸ�
			// parentNode�� �θ�μ� ���� �̾��ִ� ������ �Ѵ�.
			if (parentNode->G < retOpenNode->prev->G)
			{				
				retOpenNode->G = tempG;

				retOpenNode->H = tempH;

				retOpenNode->F = tempF;

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
void JumpPointSearch::checkRightHorizontal(NODE* parentNode, int x, int y, HBRUSH randBrush)
{	
	//=================================================================================
	// x ��ǥ�� x + 1 < mMapWidth �� ��쿡�� ������ �����մϴ�. 
	//=================================================================================
	if (x + 1 < mMapWidth)
	{
		//=================================================================================
		// ù Ž�� �� ������ �� ���� Ž�� ������ Ȯ���Ѵ�.
		//=================================================================================
		if (y > 0)
		{
			if (mJspMap[y - 1][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				checkRightUp(parentNode, x, y, randBrush, false);
			}
		}

		//=================================================================================
		// ù Ž�� �� ������ �Ʒ� ���� Ž�� ������ Ȯ���Ѵ�.
		//=================================================================================
		if (y + 1 < mMapHeight)
		{
			if (mJspMap[y + 1][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				checkRightDown(parentNode, x, y, randBrush, false);
			}
		}
	}	
	
	//===============================================================================
	// xIndex = x + 1 �� �ؼ� ���� ��ġ���� Ž���� �Ѵ�. 
	//===============================================================================
	for (int xIndex = x + 1; xIndex < mMapWidth; ++xIndex)
	{
		// ���� ������ ��� �ƹ��͵� ���� �ʰ� return 
		if (mJspMap[y][xIndex] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return;
		}

		// �������� ���� ��� �ٷ� �̾��ְ� return �Ѵ�. 
		if (xIndex == mDestinationNode->mX && y == mDestinationNode->mY)
		{
			setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_RR, xIndex, y);

			return;
		}


		// ���� ����Ʈ ��� �Ǵ� Ŭ���� ����Ʈ ��忡�� ĥ���� �ʴ´�.
		if (brushBlockList[xIndex][y] != blueBrush && brushBlockList[xIndex][y] != yellowBrush)
		{
			brushBlockList[xIndex][y] = randBrush;
		}


		//=============================================================================
		// x ���� mMapWidth ���� ���� ��쿡�� �����÷ο� ���� x+1 ��ġ�� Ȯ���� �� �ִ�.
		//=============================================================================
		if (xIndex + 1 < mMapWidth)
		{		
			//=============================================================================
			// y ���� 1 �̻��� ��쿡�� ���� ���� Ȯ���� �� �ִ�.
			//=============================================================================
			if (y > 0)
			{
				if (mJspMap[y - 1][xIndex] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][xIndex + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					// �ش� ��ġ�� ��� �����
					setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_RR, xIndex, y);

					return;
				}
			}

			//============================================================================
			// y ���� y + 1 < mMapHeight �� ��쿡�� ���� ���� Ȯ���� �� �ִ�.	
			//============================================================================
			if (y + 1 < mMapHeight)
			{
				if (mJspMap[y + 1][xIndex] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][xIndex + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_RR, xIndex, y);

					return;
				}
			}
		}
	}

	return;
}


//===============================================
// ���� ���� Ž�� �Լ��Դϴ�.
//===============================================
void JumpPointSearch::checkLeftHorizontal(NODE* parentNode, int x, int y, HBRUSH randBrush)
{

	if (x > 0)
	{
		//=================================================================================
		// ù Ž�� �� ���� �� ���� Ž�� ������ Ȯ���մϴ�.
		//=================================================================================
		if (y > 0)
		{
			if (mJspMap[y - 1][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				checkLeftUp(parentNode, x, y, randBrush, false);
			}
		}

		//=================================================================================
		// ù Ž�� �� ���� �Ʒ� ���� Ž�� ������ Ȯ���մϴ�.
		//=================================================================================
		if (y + 1 < mMapHeight)
		{
			if (mJspMap[y + 1][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				checkLeftDown(parentNode, x, y, randBrush, false);
			}
		}
	}	


	for (int xIndex = x - 1; xIndex >= 0; --xIndex)
	{
		// �ش� ��ǥ�� ��ֹ��� ������ �ٷ� return �Ѵ�.
		if (mJspMap[y][xIndex] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return;
		}


		// �������� ������ �̾��ְ� �ٷ� return �Ѵ�.
		if (xIndex == mDestinationNode->mX && y == mDestinationNode->mY)
		{
			setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_LL, xIndex, y);

			return;
		}

		// ���� ����Ʈ ��� �Ǵ� Ŭ���� ����Ʈ ��忡�� ĥ���� �ʴ´�.
		if (brushBlockList[xIndex][y] != blueBrush && brushBlockList[xIndex][y] != yellowBrush)
		{
			brushBlockList[xIndex][y] = randBrush;
		}


		if (xIndex > 0)
		{
			if (y > 0)
			{
				if (mJspMap[y - 1][xIndex] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][xIndex - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_LL, xIndex, y);

					return;
				}
			}

			if (y + 1 < mMapHeight)
			{
				if (mJspMap[y + 1][xIndex] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][xIndex - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_LL, xIndex, y);

					return;
				}
			}
		}	
	}

	return;
}


//===============================================
// ���� �� ���� Ž�� �Լ��Դϴ�.
//===============================================
void JumpPointSearch::checkUpVertical(NODE* parentNode, int x, int y, HBRUSH randBrush)
{		
	if (y > 0)
	{
		//=================================================================================
		// ù Ž�� �� ���� �� ���� Ž�� ������ Ȯ���մϴ�.
		//=================================================================================
		if (x > 0)
		{
			if (mJspMap[y][x - 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				checkLeftUp(parentNode, x, y, randBrush, false);
			}
		}

		//=================================================================================
		// ù Ž�� �� ������ �� ���� Ž�� ������ Ȯ���մϴ�.
		//=================================================================================
		if (x + 1 < mMapWidth)
		{
			if (mJspMap[y][x + 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				checkRightUp(parentNode, x, y, randBrush, false);
			}
		}
	}
	

	for (int yIndex = y - 1; yIndex >= 0; --yIndex)
	{
		//=============================================
		// �ش� ��ġ�� ��ֹ��� ���� ��� �ٷ� return
		//=============================================
		if (mJspMap[yIndex][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return;
		}


		//===================================================
		// �ش� ��ġ�� �������� ���� ��� ��� ���� �� return
		//===================================================
		if (x == mDestinationNode->mX && yIndex == mDestinationNode->mY)
		{
			setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_UU, x, yIndex);

			return;
		}

		// ���� ����Ʈ ��� �Ǵ� Ŭ���� ����Ʈ ��忡�� ĥ���� �ʴ´�.
		if (brushBlockList[x][yIndex] != blueBrush && brushBlockList[x][yIndex] != yellowBrush)
		{
			brushBlockList[x][yIndex] = randBrush;
		}


		if (yIndex > 0)
		{
			if (x > 0)
			{
				if (mJspMap[yIndex][x - 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[yIndex - 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_UU, x, yIndex);

					return;
				}
			}


			if (x + 1 < mMapWidth)
			{
				if (mJspMap[yIndex][x + 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[yIndex - 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_UU, x, yIndex);

					return;
				}
			}
		}
	}

	return;
}


//===============================================
// ���� �Ʒ� ���� Ž�� �Լ��Դϴ�.
//===============================================
void JumpPointSearch::checkDownVertical(NODE* parentNode, int x, int y, HBRUSH randBrush)
{

	if(y + 1 < mMapHeight)
	{
		//=================================================================================
		// ù Ž�� �� ���� �Ʒ� ���� Ž�� ������ Ȯ���մϴ�.
		//=================================================================================	
		if (x > 0)
		{
			if (mJspMap[y][x - 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				checkLeftDown(parentNode, x, y, randBrush, false);

			}
		}

		//=================================================================================
		// ù Ž�� �� ������ �Ʒ� ���� Ž�� ������ Ȯ���մϴ�.
		//=================================================================================
		if (x + 1 < mMapWidth)
		{
			if (mJspMap[y][x + 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				checkRightDown(parentNode, x, y, randBrush, false);

			}
		}
	}	


	for (int yIndex = y + 1; yIndex < mMapHeight; ++yIndex)
	{
		// ��ֹ��� ����ĥ ��� return �Ѵ�.
		if (mJspMap[yIndex][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return;
		}


		// �������� ������ ��� �������� �̾��ְ� return �Ѵ�.
		if (x == mDestinationNode->mX && yIndex == mDestinationNode->mY)
		{
			setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_DD, x, yIndex);

			return;
		}

		// ���� ����Ʈ ��� �Ǵ� Ŭ���� ����Ʈ ��忡�� ĥ���� �ʴ´�.
		if (brushBlockList[x][yIndex] != blueBrush && brushBlockList[x][yIndex] != yellowBrush)
		{
			brushBlockList[x][yIndex] = randBrush;
		}

		if (x > 0 && yIndex + 1 < mMapHeight )
		{
			if (mJspMap[yIndex][x - 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[yIndex + 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_DD, x, yIndex);

				return;
			}
		}

		if (x + 1 < mMapWidth && yIndex + 1 < mMapHeight)
		{
			if (mJspMap[yIndex][x + 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[yIndex + 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
			{
				setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_DD, x, yIndex);

				return;
			}
		}		
	}

	return;
}



//======================================================
// ������ �� �밢�� �Լ��Դϴ�. 
//======================================================
void JumpPointSearch::checkRightUp(NODE* parentNode, int x, int y, HBRUSH randBrush ,bool firstCall)
{		
	//===============================================
	// �밢�� �Լ� ȣ�� �� ���� �Լ� ȣ�� ����
	//===============================================
	if (firstCall)
	{	
		//===================================================================
		// ���� ���ǿ� ������ 5�������� Ž���ϰ� �ȴ�.
		checkRightHorizontal(parentNode, x, y, randBrush);

		checkUpVertical(parentNode, x, y, randBrush);		
		//===================================================================
	}

	for(;;)
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
			setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_RU, x, y);

			return;
		}


		// TODO : �ؿ� �� ���� �Լ� �ϳ��� ����� 
		if (checkRightDiagonalHorizontal(parentNode, NODE_DIRECTION::NODE_DIR_RU, x, y, randBrush) == true)
		{
			return;
		}

		if (checkUpDiagonalVertical(parentNode, NODE_DIRECTION::NODE_DIR_RU, x, y, randBrush) == true)
		{
			return;
		}
	}

	return;
}


//======================================================
// ������ �Ʒ� �밢�� �Լ��Դϴ�. 
//======================================================
void JumpPointSearch::checkRightDown(NODE* parentNode, int x, int y, HBRUSH randBrush, bool firstCall)
{		
	if (firstCall)
	{
		//===================================================================
		// ���� ���ǿ� ������ 5�������� Ž���ϰ� �ȴ�.
		checkRightHorizontal(parentNode, x, y, randBrush);
		
		checkDownVertical(parentNode, x, y, randBrush);	
		//===================================================================
	}

	for(;;)
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
			setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_RD, x, y);

			return;
		}

		if (checkRightDiagonalHorizontal(parentNode, NODE_DIRECTION::NODE_DIR_RD, x, y, randBrush) == true)
		{
			return;
		}

		if (checkDownDiagonalVertical(parentNode, NODE_DIRECTION::NODE_DIR_RD, x, y, randBrush) == true)
		{
			return;
		}
	}

	return;
}


//======================================================
// ���� �� �밢�� �Լ��Դϴ�. 
//======================================================
void JumpPointSearch::checkLeftUp(NODE* parentNode, int x, int y, HBRUSH randBrush ,bool firstCall)
{
	if (firstCall)
	{
		//===================================================================
		// ���� ���ǿ� ������ 5�������� Ž���ϰ� �ȴ�.
		checkLeftHorizontal(parentNode, x, y, randBrush);	

		checkUpVertical(parentNode, x, y, randBrush);		
		//===================================================================
	}

	for(;;)
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
			
			setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_LU, x, y);

			return;
		}

		if (checkLeftDiagonalHorizontal(parentNode, NODE_DIRECTION::NODE_DIR_LU, x, y, randBrush) == true)
		{
			return;
		}

		if (checkUpDiagonalVertical(parentNode, NODE_DIRECTION::NODE_DIR_LU, x, y, randBrush) == true)
		{
			return;
		}
	}

	return;
}


//======================================================
// ���� �Ʒ� �밢�� �Լ��Դϴ�. 
//======================================================
void JumpPointSearch::checkLeftDown(NODE* parentNode, int x, int y, HBRUSH randBrush, bool firstCall)
{
	if (firstCall == true)
	{
		//===================================================================
		// ���� ���ǿ� ������ 5�������� Ž���ϰ� �ȴ�.
		checkLeftHorizontal(parentNode, x, y, randBrush);
		

		checkDownVertical(parentNode,  x, y, randBrush);		
		//===================================================================
	}


	for(;;)
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
			setCornerNode(parentNode, NODE_DIRECTION::NODE_DIR_LD, x, y);

			return;
		}

		if (checkLeftDiagonalHorizontal(parentNode, NODE_DIRECTION::NODE_DIR_LD, x, y, randBrush) == true)
		{
			return;
		}

		if (checkDownDiagonalVertical(parentNode, NODE_DIRECTION::NODE_DIR_LD, x, y, randBrush) == true)
		{
			return;
		}
	}
}



//=======================================================
// �밢�� �Լ��� ���� ������ ���� 
//=======================================================
bool JumpPointSearch::checkRightDiagonalHorizontal(NODE* parentNode, NODE_DIRECTION nodeDir,int x, int y, HBRUSH randBrush)
{

	for (int xIndex = x; xIndex < mMapWidth; ++xIndex)
	{
		if (mJspMap[y][xIndex] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return false;
		}
	
		// ������ ��带 ������ ��쿡�� �� �ڸ��� ��� ������ return 
		if (xIndex == mDestinationNode->mX && y == mDestinationNode->mY)
		{
			setCornerNode(parentNode, nodeDir, x, y);
			
			return true;
		}

		// ���� ����Ʈ ��� �Ǵ� Ŭ���� ����Ʈ ��忡�� ĥ���� �ʴ´�.
		if (brushBlockList[xIndex][y] != blueBrush && brushBlockList[xIndex][y] != yellowBrush)
		{
			brushBlockList[xIndex][y] = randBrush;
		}

		if (xIndex + 1 < mMapWidth)
		{
			if (y > 0)
			{
				if (mJspMap[y - 1][xIndex] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][xIndex + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, nodeDir, x, y);

					return true;
				}
			}

			if (y + 1 < mMapHeight)
			{
				if (mJspMap[y + 1][xIndex] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][xIndex + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, nodeDir, x, y);

					return true;
				}
			}
		}	
	}

	return false;
}


//=======================================================
// �밢�� �Լ��� ���� ���� ����
//=======================================================
bool JumpPointSearch::checkLeftDiagonalHorizontal(NODE* parentNode, NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush)
{

	for (int xIndex = x; xIndex >= 0; --xIndex)
	{
		if (mJspMap[y][xIndex] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return false;
		}		

		//=============================================
		// �������� ������ ��� return
		//=============================================
		if (xIndex == mDestinationNode->mX && y == mDestinationNode->mY)
		{
			setCornerNode(parentNode, nodeDir, x, y);

			return true;
		}

		// ���� ����Ʈ ��� �Ǵ� Ŭ���� ����Ʈ ��忡�� ĥ���� �ʴ´�.
		if (brushBlockList[xIndex][y] != blueBrush && brushBlockList[xIndex][y] != yellowBrush)
		{
			brushBlockList[xIndex][y] = randBrush;
		}

		
		if (xIndex > 0)
		{
			if (y > 0)
			{
				if (mJspMap[y - 1][xIndex] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y - 1][xIndex - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, nodeDir, x, y);

					return true;
				}
			}

			if (y + 1 < mMapHeight)
			{
				if (mJspMap[y + 1][xIndex] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[y + 1][xIndex - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, nodeDir, x, y);

					return true;
				}
			}
		}
	}

	return false;
}


//=======================================================
// �밢�� �Լ��� ���� �� ����
//=======================================================
bool JumpPointSearch::checkUpDiagonalVertical(NODE* parentNode, NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush)
{
	
	for (int yIndex = y; yIndex >= 0; --yIndex)
	{
		// ��ֹ� ������ ��� return
		if (mJspMap[yIndex][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return false;
		}
		
		//=================================================
		// ������ ������ ��� ��� ���� �� return
		//=================================================
		if (x == mDestinationNode->mX && yIndex == mDestinationNode->mY)
		{
			setCornerNode(parentNode, nodeDir, x, y);

			return true;
		}


		// ���� ����Ʈ ��� �Ǵ� Ŭ���� ����Ʈ ��忡�� ĥ���� �ʴ´�.
		if (brushBlockList[x][yIndex] != blueBrush && brushBlockList[x][yIndex] != yellowBrush)
		{
			brushBlockList[x][yIndex] = randBrush;
		}

		if (yIndex > 0)
		{
			if (x > 0)
			{
				if (mJspMap[yIndex][x - 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[yIndex - 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, nodeDir, x, y);

					return true;
				}
			}

			if (x + 1 < mMapWidth)
			{
				if (mJspMap[yIndex][x + 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[yIndex - 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, nodeDir, x, y);

					return true;
				}
			}
		}
	}

	return false;
}


//=======================================================
// �밢�� �Լ��� ���� �Ʒ� ����
//=======================================================
bool JumpPointSearch::checkDownDiagonalVertical(NODE* parentNode, NODE_DIRECTION nodeDir, int x, int y, HBRUSH randBrush)
{

	for (int yIndex = y; yIndex < mMapHeight; yIndex++)
	{
		if (mJspMap[yIndex][x] == (char)NODE_ATTRIBUTE::NODE_BLOCK)
		{
			return false;
		}	

		if (x == mDestinationNode->mX && yIndex == mDestinationNode->mY)
		{
			setCornerNode(parentNode, nodeDir, x, y);

			return true;
		}

		// ���� ����Ʈ ��� �Ǵ� Ŭ���� ����Ʈ ��忡�� ĥ���� �ʴ´�.
		if (brushBlockList[x][yIndex] != blueBrush && brushBlockList[x][yIndex] != yellowBrush)
		{
			brushBlockList[x][yIndex] = randBrush;
		}

		if (yIndex + 1 < mMapHeight)
		{
			if (x > 0)
			{
				if (mJspMap[yIndex][x - 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[yIndex + 1][x - 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, nodeDir, x, y);

					return true;
				}
			}
		
			if (x + 1 < mMapWidth)
			{
				if (mJspMap[yIndex][x + 1] == (char)NODE_ATTRIBUTE::NODE_BLOCK && mJspMap[yIndex + 1][x + 1] == (char)NODE_ATTRIBUTE::NODE_UNBLOCK)
				{
					setCornerNode(parentNode, nodeDir, x, y);

					return true;
				}
			}
		}		
	}

	return false;
}




//================================================================
// ���� ����Ʈ���� ���ĵǾ� �ִ� F�� ��带 �̴´�.
//================================================================
JumpPointSearch::NODE* JumpPointSearch::selectOpenListNode()
{
	// F���� ���� ������ �����س��� ������ begin���� ������ ù ��带 ���� �� �ִ�.
	CList<JumpPointSearch::NODE*>::Iterator iter = mOpenList.begin();

	if (*iter == nullptr)
	{
		return nullptr;
	}

	// ����� ���� �����Ѵ�.
	NODE* node = *iter;

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
void JumpPointSearch::ResetAll(RouteNode* routeNodeArray, int routeNodeArraySize, RouteNode* optimizeNodeArray, int optimizeNodeArraySize)
 {	
	if (mFuncSetFlag == false)
	{
		free(mStartNode);

 		free(mDestinationNode);
	}

	functionFlag = false;

	mFuncSetFlag = true;

	resetOpenList();

	resetCloseList();

	resetRouteList();

	resetRouteNodeArray(routeNodeArray, routeNodeArraySize);

	resetOptimizeArray(optimizeNodeArray, optimizeNodeArraySize);

	resetOptimizeRoute();

	resetJspMap();

	resetBlock();
}



//==================================================================
// �������� �������� ������ ����Ʈ�� ���� �ʱ�ȭ�ϰ� �ٽ� ���� ã�´�.
//==================================================================
void JumpPointSearch::ReStart(RouteNode* routeNodeArray, int routeNodeArraySize, RouteNode* optimizeNodeArray, int optimizeNodeArraySize)
{
	if (mFuncSetFlag == false)
	{
		free(mStartNode);

		free(mDestinationNode);
	}

	functionFlag = true;

	mFuncSetFlag = true;

	resetOpenList();

	resetCloseList();

	resetRouteList();

	resetJspMap();

	resetRoute();

	resetRouteNodeArray(routeNodeArray, routeNodeArraySize);

	resetOptimizeArray(optimizeNodeArray, optimizeNodeArraySize);

	resetOptimizeRoute();
}


//=============================================================
// ���¸���Ʈ ��带 �����Ѵ�.
//=============================================================
void JumpPointSearch::resetOpenList()
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
void JumpPointSearch::resetCloseList()
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
void JumpPointSearch::resetRouteList()
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
void JumpPointSearch::resetOptimizeRoute()
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
void JumpPointSearch::resetBlock()
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
void JumpPointSearch::resetRoute()
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
void JumpPointSearch::insertRoute(NODE* node)
{
	for(;;)
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
void JumpPointSearch::pathOptimizing()
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

	for(;;)
	{
		startX = (*iter)->mX;
		startY = (*iter)->mY;

		nextIter = iter.GetNextIter();
		//nextIter = ++iter;
		
		//========================================================================
		// iter�� ���� ��尡 ������ ������� break ������ �����Ѵ�.
		//=========================================================================
		if (nextIter == iterE)
		{
			break;
		}
	
		nextNextIter = nextIter;

		for(;;)
		{
			nextNextIter = nextNextIter.GetNextIter();
			//nextNextIter = ++nextNextIter;


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
				//nextIter.deleteCheck = true;

				nextIter.SetDeleteFlag();
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
		if (iter.GetDeleteFlag())
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

	CList<NODE*>::Iterator iterE = mRouteList.end();

	for (CList<NODE*>::Iterator iter = mRouteList.begin(); iter != iterE; ++iter)
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


void JumpPointSearch::settingOptimizeArray(RouteNode* optimizeNodeArray, int optimizeNodeArraySize)
{
	int index = 0;

	CList<NODE*>::Iterator iterE = mOptimizeRouteList.end();

	for (CList<NODE*>::Iterator iter = mOptimizeRouteList.begin(); iter != iterE; ++iter)
	{
		optimizeNodeArray[index].mPosX = iter->mX;

		optimizeNodeArray[index].mPosY = iter->mY;

		optimizeNodeArray[index].mRouteFlag = true;

		index += 1;

		if (index >= optimizeNodeArraySize)
		{
			return;
		}

	}
}


void JumpPointSearch::resetOptimizeArray(RouteNode* optimizeNodeArray, int optimizeNodeArraySize)
{
	for (int iCnt = 0; iCnt < optimizeNodeArraySize; ++iCnt)
	{
		optimizeNodeArray[iCnt].mRouteFlag = false;
	}
}


//=============================================================
// F�� ���� ������ �����Ѵ�.
//=============================================================
void JumpPointSearch::openListBubbleSort() {

	CList<JumpPointSearch::NODE*>::Iterator iterE = mOpenList.end();

	--iterE;

	int listSize = mOpenList.GetUseSize();

	for (int iCnt = 0; iCnt < listSize; iCnt++)
	{
		for (CList<JumpPointSearch::NODE*>::Iterator iter = mOpenList.begin(); iter != iterE; ++iter)
		{
			CList<JumpPointSearch::NODE*>::Iterator iterN = iter.GetNextIter();

			if (iter->F > iterN->F)
			{
				mOpenList.DataSwap(iter, iterN);
			}
		}

		--iterE;
	}
}