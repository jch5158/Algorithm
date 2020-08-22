#include "stdafx.h"
#include "CList.h"
#include "JumpPointSearch.h"
#include "JumpPointSearchAlgorithm.h"
#include "BresenhamLine.h"

extern HBRUSH grayBrush;

extern HBRUSH brushBlockList[MAX_WIDTH][MAX_HEIGHT];


bool BresenhamLine::MakeLine(int startX, int startY, int endX, int endY)
{
	int subX = abs(startX - endX);

	int subY = abs(startY - endY);

	int errorValue = 0;

	int lineX = 0;
	int lineY = 0;

	int indexX = 0;
	int indexY = 0;

	if (subX >= subY)
	{
		errorValue = subX;

		while (1)
		{
			if (startX <= endX)
			{
				indexX = startX + lineX;
			}
			else
			{
				indexX = startX - lineX;
			}

			if (startY <= endY)
			{
				indexY = startY + lineY;
			}
			else
			{
				indexY = startY - lineY;
			}

			if (brushBlockList[indexX][indexY] == grayBrush)
			{
				return false;
			}


			if (subX == lineX && subY == lineY)
			{
				break;
			}

			lineX += 1;

			errorValue += subY * 2;

			if (subX * 2 <= errorValue)
			{
				lineY += 1;
				errorValue -= subX * 2;
			}
		}
	}
	else
	{
		errorValue = subY;

		while (1)
		{

			if (startX <= endX)
			{
				indexX = startX + lineX;
			}
			else
			{
				indexX = startX - lineX;
			}

			if (startY <= endY)
			{
				indexY = startY + lineY;
			}
			else
			{
				indexY = startY - lineY;
			}

			if (brushBlockList[indexX][indexY] == grayBrush)
			{
				return false;
			}

			if (subX == lineX && subY == lineY)
			{
				break;
			}

			lineY += 1;

			errorValue += subX * 2;

			if (subY * 2 <= errorValue)
			{
				lineX += 1;
				errorValue -= subY * 2;
			}
		}
	}


	return true;
}
