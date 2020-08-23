#pragma once

//extern HBRUSH grayBrush;

//extern HBRUSH brushBlockList[MAX_WIDTH][MAX_HEIGHT];


namespace BresenhamLine
{
 
    bool MakeLine(int startX, int startY, int endX, int endY);

    bool CatchLine(int startX, int startY, int endX, int endY);

};
