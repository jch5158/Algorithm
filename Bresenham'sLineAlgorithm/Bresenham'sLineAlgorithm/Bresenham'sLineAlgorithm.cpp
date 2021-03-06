﻿// Bresenham'sLineAlgorithm.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "framework.h"
#include "BresenhamLine.h"
#include "Bresenham'sLineAlgorithm.h"


#define MAX_LOADSTRING 100

using namespace BresenhamLine;

// 전역 변수:
HINSTANCE           hInst;                                // 현재 인스턴스입니다.
WCHAR               szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR               szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HWND                hWnd;
HDC                 hdc;

extern HBRUSH       oldBrush;

// 출발지 브러쉬
extern HBRUSH       greenBrush;

// 목적지 브러쉬
extern HBRUSH       redBrush;


extern HBRUSH       blackBrush;
                                                
// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);




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
    LoadStringW(hInstance, IDC_BRESENHAMSLINEALGORITHM, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BRESENHAMSLINEALGORITHM));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BRESENHAMSLINEALGORITHM));
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

    static int mouseX;
    static int mouseY;

    static int endMouseX;
    static int endMouseY;

    static bool moveCheckFlag = false;

    static HPEN redPen;
    static HPEN oldPen;

    switch (message)
    {
    case WM_CREATE:

        greenBrush = CreateSolidBrush(RGB(0, 255, 0));

        redBrush = CreateSolidBrush(RGB(255, 0, 0));
        
        blackBrush = CreateSolidBrush(RGB(0, 0, 0));

        redPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));

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

    case WM_LBUTTONDOWN:
        
        mouseY = HIWORD(lParam);
        mouseX = LOWORD(lParam);

        brushBlockList[mouseX / PERMETER_OF_SQUARE][mouseY / PERMETER_OF_SQUARE] = blackBrush;

        moveCheckFlag = true;

        InvalidateRect(hWnd, nullptr, false);

        break;

    case WM_MOUSEMOVE:

        if (moveCheckFlag)
        {
            for (int iCnt1 = 0; iCnt1 < MAX_HEIGHT; iCnt1++)
            {
                for (int iCnt2 = 0; iCnt2 < MAX_WIDTH; iCnt2++)
                {
                    if (brushBlockList[iCnt2][iCnt1] == blackBrush)
                    {
                        brushBlockList[iCnt2][iCnt1] = oldBrush;
                    }
                }
            }

            endMouseY = HIWORD(lParam);
            endMouseX = LOWORD(lParam);

            MakeLine(mouseX / PERMETER_OF_SQUARE, mouseY / PERMETER_OF_SQUARE, endMouseX / PERMETER_OF_SQUARE, endMouseY / PERMETER_OF_SQUARE);
           
            InvalidateRect(hWnd, nullptr, false);
        }
        break;

    case WM_LBUTTONUP:

        moveCheckFlag = false;

        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            hdc = BeginPaint(hWnd, &ps);
            
            for (int iCnt1 = 0; iCnt1 < MAX_HEIGHT; iCnt1++)
            {
                for (int iCnt2 = 0; iCnt2 < MAX_WIDTH; iCnt2++)
                {
                    oldBrush = (HBRUSH)SelectObject(hdc, brushBlockList[iCnt2][iCnt1]);

                    Rectangle(hdc, PERMETER_OF_SQUARE * iCnt2, PERMETER_OF_SQUARE * iCnt1, PERMETER_OF_SQUARE * (iCnt2 + 1), PERMETER_OF_SQUARE * (iCnt1 + 1));

                    SelectObject(hdc, oldBrush);
                }
            }
            
            oldPen = (HPEN)SelectObject(hdc, redPen);

            MoveToEx(hdc, mouseX, mouseY, nullptr);

            LineTo(hdc, endMouseX, endMouseY);

            SelectObject(hdc, oldPen);
            
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
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
