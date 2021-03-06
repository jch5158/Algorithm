#include "stdafx.h"
#include "Bresenham'sLineAlgorithm.h"
#include "BresenhamLine.h"


HBRUSH		 oldBrush;

// 출발지 브러쉬
HBRUSH       greenBrush;

// 목적지 브러쉬
HBRUSH       redBrush;


HBRUSH       blackBrush;


HBRUSH BresenhamLine::brushBlockList[MAX_WIDTH][MAX_HEIGHT] = { oldBrush };


void BresenhamLine::MakeLine(int startX, int startY, int endX, int endY)
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

			brushBlockList[indexX][indexY] = blackBrush;

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

			brushBlockList[indexX][indexY] = blackBrush;

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



}
