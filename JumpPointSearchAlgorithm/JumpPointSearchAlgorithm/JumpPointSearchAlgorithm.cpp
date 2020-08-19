﻿// JumpPointSearchAlgorithm.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//


#include "stdafx.h"
#include "framework.h"
#include "CList.h"
#include "JumpPointSearchAlgorithm.h"
#include "JumpPointSearch.h"

#define MAX_LOADSTRING 100

using namespace JumpPointSearch;


// 전역 변수:
HINSTANCE           hInst;                                // 현재 인스턴스입니다.
WCHAR               szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR               szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HWND                hWnd;
HDC                 hdc;

// red 색상이 칠해진 타일
int                redX;
int                redY;

// green 색상이 칠해진 타일
int                greenX;
int                greenY;

bool               returnFlag = false;

JumpPointSearch::NODE* retNode;


// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void CALLBACK       TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_JUMPPOINTSEARCHALGORITHM, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_JUMPPOINTSEARCHALGORITHM));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_JUMPPOINTSEARCHALGORITHM));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HANDLE hTimer;

    static HBRUSH oldBrush;

    // 출발지 체크 브러쉬
    static HBRUSH greenBrush;

    // 목적지 체크 브러쉬
    static HBRUSH redBrush;

    // 장애물 체크 브러쉬
    static HBRUSH grayBrush;

    // 오픈 리스트 체크 브러쉬
    static HBRUSH blueBrush;

    static HBRUSH yellowBrush;

    static HPEN oldPen;
    static HPEN redPen;


    // 마우스 좌표
    static DWORD mouseX;
    static DWORD mouseY;


    static bool wallFlag = false;
    static bool wallClearFlag = false;
    static bool loopFlag = false;

    switch (message)
    {
    case WM_CREATE:

        greenBrush = CreateSolidBrush(RGB(0, 255, 0));

        redBrush = CreateSolidBrush(RGB(255, 0, 0));

        blueBrush = CreateSolidBrush(RGB(0, 0, 255));

        yellowBrush = CreateSolidBrush(RGB(255, 255, 0));

        grayBrush = CreateSolidBrush(RGB(105, 105, 105));

        redPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));

        hTimer = (HANDLE)SetTimer(hWnd, 1, 50, (TIMERPROC)TimerProc);

        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_KEYDOWN:

        if (wParam == VK_RETURN)
        {
            returnFlag = true;
            functionFlag = true;
        }

        if (wParam == VK_SPACE)
        {
            functionFlag = false;
            retNode = nullptr;
            ResetAll();
        }

        InvalidateRect(hWnd, nullptr, false);

        break;
    case WM_MOUSEMOVE:

        if (wallFlag == true)
        {
            mouseY = HIWORD(lParam);
            mouseX = LOWORD(lParam);

            if (blockList[mouseX / PERMETER_OF_SQUARE][mouseY / PERMETER_OF_SQUARE] == (BYTE)BLOCK_COLOR::BASIC)
            {
                blockList[mouseX / PERMETER_OF_SQUARE][mouseY / PERMETER_OF_SQUARE] = (BYTE)BLOCK_COLOR::GRAY;

                InvalidateRect(hWnd, nullptr, false);
            }
        }
        else if (wallClearFlag == true)
        {
            mouseY = HIWORD(lParam);
            mouseX = LOWORD(lParam);
            if (blockList[mouseX / PERMETER_OF_SQUARE][mouseY / PERMETER_OF_SQUARE] == (BYTE)BLOCK_COLOR::GRAY)
            {
                blockList[mouseX / PERMETER_OF_SQUARE][mouseY / PERMETER_OF_SQUARE] = (BYTE)BLOCK_COLOR::BASIC;

                InvalidateRect(hWnd, nullptr, false);
            }
        }


        break;
    case WM_LBUTTONDOWN:

        mouseY = HIWORD(lParam);
        mouseX = LOWORD(lParam);


        if (MK_CONTROL & wParam)
        {
            if (blockList[mouseX / PERMETER_OF_SQUARE][mouseY / PERMETER_OF_SQUARE] == (BYTE)BLOCK_COLOR::GRAY)
            {
                blockList[mouseX / PERMETER_OF_SQUARE][mouseY / PERMETER_OF_SQUARE] = (BYTE)BLOCK_COLOR::BASIC;
            }

            wallClearFlag = true;
        }
        else if (MK_SHIFT & wParam)
        {

            if (blockList[mouseX / PERMETER_OF_SQUARE][mouseY / PERMETER_OF_SQUARE] == (BYTE)BLOCK_COLOR::BASIC)
            {
                blockList[mouseX / PERMETER_OF_SQUARE][mouseY / PERMETER_OF_SQUARE] = (BYTE)BLOCK_COLOR::GRAY;
            }


            wallFlag = true;
        }
        else
        {
            blockList[greenX][greenY] = (BYTE)BLOCK_COLOR::BASIC;

            greenX = mouseX / PERMETER_OF_SQUARE;
            greenY = mouseY / PERMETER_OF_SQUARE;

            blockList[greenX][greenY] = (BYTE)BLOCK_COLOR::GREEN;
        }


        InvalidateRect(hWnd, nullptr, false);

        break;

    case WM_RBUTTONDOWN:

        mouseY = HIWORD(lParam);
        mouseX = LOWORD(lParam);

        blockList[redX][redY] = (BYTE)BLOCK_COLOR::BASIC;

        redX = mouseX / PERMETER_OF_SQUARE;
        redY = mouseY / PERMETER_OF_SQUARE;

        blockList[redX][redY] = (BYTE)BLOCK_COLOR::RED;

        InvalidateRect(hWnd, nullptr, false);


        break;
    case WM_LBUTTONUP:

        wallFlag = false;
        wallClearFlag = false;

        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        hdc = BeginPaint(hWnd, &ps);

        for (int iCnt1 = 0; iCnt1 < MAX_HEIGHT; iCnt1++)
        {
            for (int iCnt2 = 0; iCnt2 < MAX_WIDTH; iCnt2++)
            {

                switch (blockList[iCnt2][iCnt1])
                {
                case (BYTE)BLOCK_COLOR::GREEN:

                    oldBrush = (HBRUSH)SelectObject(hdc, greenBrush);

                    break;
                case (BYTE)BLOCK_COLOR::RED:

                    oldBrush = (HBRUSH)SelectObject(hdc, redBrush);

                    break;
                case (BYTE)BLOCK_COLOR::GRAY:

                    oldBrush = (HBRUSH)SelectObject(hdc, grayBrush);

                    break;
                case (BYTE)BLOCK_COLOR::BLUE:

                    oldBrush = (HBRUSH)SelectObject(hdc, blueBrush);

                    break;
                case (BYTE)BLOCK_COLOR::YELLOW:

                    oldBrush = (HBRUSH)SelectObject(hdc, yellowBrush);

                    break;

                }

                Rectangle(hdc, PERMETER_OF_SQUARE * iCnt2, PERMETER_OF_SQUARE * iCnt1, PERMETER_OF_SQUARE * (iCnt2 + 1), PERMETER_OF_SQUARE * (iCnt1 + 1));

                SelectObject(hdc, oldBrush);
            }
        }

        JumpPointSearch::NODE* paintNode = retNode;

        oldPen = (HPEN)SelectObject(hdc, redPen);

        while (1)
        {
            if (paintNode == nullptr)
            {
                break;
            }


            MoveToEx(hdc, 10 + (paintNode->mX * PERMETER_OF_SQUARE), 10 + (paintNode->mY * PERMETER_OF_SQUARE), nullptr);

            paintNode = paintNode->prev;

            if (paintNode != nullptr)
            {
                LineTo(hdc, 10 + (paintNode->mX * PERMETER_OF_SQUARE), 10 + (paintNode->mY * PERMETER_OF_SQUARE));
            }
        }


        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:

        KillTimer(hWnd, 1);


        // PostQuitMessage 함수를 호출하여 WM_QUIT 메시지를 보낸다. WM_QUIT 메시지가 입력되면 
        // 메시지 루프의 GetMessage 함수 리턴값이 False가 되어 프로그램이 종료된다.
        PostQuitMessage(0);
        break;
    default:


        // 윈도우의 이동이나 크기변경 따위의 처리는 직접 해 줄 필요없이 DefWindowProc으로 넘겨주기만 하면 된다.
        // 또한 DefWindowProc 함수가 메시지를 처리했을 경우 이 함수가 리턴한 값을 WndProc 함수가 다시 리턴해 주어야 한다.
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{

    if (returnFlag == true)
    {
        retNode = PathFind(greenX, greenY, redX, redY);
        InvalidateRect(hWnd, nullptr, false);
        if (retNode != nullptr)
        {
            returnFlag = false;
        }
    }

}
// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}