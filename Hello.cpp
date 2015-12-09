// Hello.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Hello.h"
#include "Parser.h"
#include <atlstr.h>
#include <commdlg.h>

#define MAX_LOADSTRING 100


// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

HDC memDc;
HBITMAP memBmp;


//MouseMove时得到横纵坐标
int initMovePosX = 0;
int initMovePosY = 0;
int firstMoveFlag = 0;
int MovePosX = 0;
int MovePosY = 0;
//MouseWheel时方向以及横纵坐标
int zDelta = 0;
int WheelPosX;
int WheelPosY;
//memDc
int memDcX = -1;
int memDcY = -1;
//记录窗口的大小
int oldWidth = 0;
int oldHeight = 0;
//记录滚轮的方向和次数
int wheelDir = 0;

//记录五个输入框输入函数的长度，默认为1，表示为空、
int dwNum1 = 1, dwNum2 = 1, dwNum3 = 1, dwNum4 = 1, dwNum5 = 1;
//记录五个输入框的表达式
string expr1, expr2, expr3, expr4, expr5;

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


//color
COLORREF ShowColorDialog(HWND);  // COLORREF 就是个DWORD // typedef DWORD COLORREF;  

COLORREF gColor1 = RGB(0, 139, 251);
COLORREF gColor2 = RGB(252, 52, 54);
COLORREF gColor3 = RGB(54, 99, 6);
COLORREF gColor4 = RGB(251, 156, 50);
COLORREF gColor5 = RGB(150, 5, 184);
//color

//读文件
WCHAR filePath[100];
int fpNum = 1;//读文件的标记位
vector<float> fileData;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_HELLO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HELLO));

    MSG msg;

    // 主消息循环: 
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
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HELLO));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_HELLO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, L"PlotTool", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   //HWND hWnd = CreateWindowW(szWindowClass, TEXT("Plot"), WS_OVERLAPPEDWINDOW,
	 //  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
string GetExpr(WCHAR var[100],int & dwNum){
	wchar_t * wText = var;
	//DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);// WideCharToMultiByte的运用
	dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);// WideCharToMultiByte的运用,其中dwNum=1时表示输入为空
	char *psText;   // psText为char*的临时数组，作为赋值给std::string的中间变量
	psText = new char[dwNum];
	WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);// WideCharToMultiByte的再次运用,将宽字符串转换为多字节字符串
	string expr = psText;// std::string赋值
	delete[]psText;// psText的清除
	return expr;
}

void PlotFunc(int pixels,int xNum,int yNum,string expr,COLORREF col) {
	HPEN hFuncPen, hFunPenOld;
	hFuncPen = CreatePen(PS_SOLID, 1, col);
	hFunPenOld = (HPEN)SelectObject(memDc, hFuncPen);
	double yRelVal;  //记录函数y轴的相对值
	int xPixels;
	int yPixels;
	int xLastPixels;
	int yLastPixels;
	xLastPixels = 680 - xNum*pixels;
	yLastPixels = 335 - Compute(expr, -xNum)*pixels;
	for (int i = 0; i < 2 * xNum * 50 + 1; i++) {
		xPixels = 680 + (0.02*i - xNum)*pixels;
		yRelVal = Compute(expr, 0.02*i - xNum);
		yPixels = 335 - yRelVal*pixels;
		if (yRelVal < yNum && yRelVal > -yNum && (335 - yLastPixels) / pixels < yNum && (335 - yLastPixels) / pixels > -yNum) {
			MoveToEx(memDc, xLastPixels, yLastPixels, NULL);
			LineTo(memDc, xPixels, yPixels);
		}
		xLastPixels = xPixels;
		yLastPixels = yPixels;
	}
	//MoveToEx(memDc, 680, 335, NULL);
	//LineTo(memDc, 800, 100);
	SelectObject(memDc, hFunPenOld);
	DeleteObject(hFuncPen);
}


//把HBITMAP保存为bmp图片 
BOOL  SaveBmp(HBITMAP     hBitmap, CString     FileName)
{
	HDC     hDC;
	//当前分辨率下每象素所占字节数       
	int     iBits;
	//位图中每象素所占字节数       
	WORD     wBitCount;
	//定义调色板大小，     位图中像素字节大小     ，位图文件大小     ，     写入文件字节数           
	DWORD     dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
	//位图属性结构           
	BITMAP     Bitmap;
	//位图文件头结构       
	BITMAPFILEHEADER     bmfHdr;
	//位图信息头结构           
	BITMAPINFOHEADER     bi;
	//指向位图信息头结构               
	LPBITMAPINFOHEADER     lpbi;
	//定义文件，分配内存句柄，调色板句柄           
	HANDLE     fh, hDib, hPal, hOldPal = NULL;

	//计算位图文件每个像素所占字节数           
	hDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL)     *     GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else  if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else
		wBitCount = 24;

	GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed = 0;

	dwBmBitsSize = ((Bitmap.bmWidth *wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

	//为位图内容分配内存           
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	//     处理调色板               
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}

	//     获取该调色板下新的像素值           
	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight,
		(LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize,
		(BITMAPINFO *)lpbi, DIB_RGB_COLORS);

	//恢复调色板               
	if (hOldPal)
	{
		::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	//创建位图文件               
	fh = CreateFile(FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)         return     FALSE;

	//     设置位图文件头           
	bmfHdr.bfType = 0x4D42;     //     "BM"           
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;
	//     写入位图文件头           
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	//     写入位图文件其余内容           
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
	//清除               
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);

	return     TRUE;
}



//color
static HWND hwndPanel;
//color

HWND hEdit1, hEdit2, hEdit3, hEdit4, hEdit5, hButton1, hButton2;
HWND hButtonColor1,hButtonColor2, hButtonColor3, hButtonColor4, hButtonColor5;
HWND hEdit6,hButton4;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		{
			hEdit1 = CreateWindowW(L"EDIT", TEXT(""),WS_CHILD|WS_VISIBLE|WS_BORDER,
				980, 100, 250, 30, hWnd, (HMENU)11, hInst, NULL);
			hEdit2 = CreateWindowW(L"EDIT", TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
				980, 150, 250, 30, hWnd, (HMENU)22, hInst, NULL);
			hEdit3 = CreateWindowW(L"EDIT", TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
				980, 200, 250, 30, hWnd, (HMENU)33, hInst, NULL);
			hEdit4 = CreateWindowW(L"EDIT", TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
				980, 250, 250, 30, hWnd, (HMENU)44, hInst, NULL);
			hEdit5 = CreateWindowW(L"EDIT", TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
				980, 300, 250, 30, hWnd, (HMENU)55, hInst, NULL);
			hButton1 = CreateWindowW(L"BUTTON", TEXT("Plot"), WS_CHILD | WS_VISIBLE | WS_BORDER,
				1000, 350, 100, 40, hWnd, (HMENU)111, hInst, NULL);
			hButton2 = CreateWindowW(L"BUTTON", TEXT("Reset"), WS_CHILD | WS_VISIBLE | WS_BORDER,
				1120, 350, 100, 40, hWnd, (HMENU)222, hInst, NULL);

			hButtonColor1 = CreateWindowW(L"BUTTON", TEXT("Color"), WS_CHILD | WS_VISIBLE | WS_BORDER,
				1240, 100, 60, 30, hWnd, (HMENU)333, hInst, NULL);
			hButtonColor2 = CreateWindowW(L"BUTTON", TEXT("Color"), WS_CHILD | WS_VISIBLE | WS_BORDER,
				1240, 150, 60, 30, hWnd, (HMENU)444, hInst, NULL);
			hButtonColor3 = CreateWindowW(L"BUTTON", TEXT("Color"), WS_CHILD | WS_VISIBLE | WS_BORDER,
				1240, 200, 60, 30, hWnd, (HMENU)555, hInst, NULL);
			hButtonColor4 = CreateWindowW(L"BUTTON", TEXT("Color"), WS_CHILD | WS_VISIBLE | WS_BORDER,
				1240, 250, 60, 30, hWnd, (HMENU)666, hInst, NULL);
			hButtonColor5 = CreateWindowW(L"BUTTON", TEXT("Color"), WS_CHILD | WS_VISIBLE | WS_BORDER,
				1240, 300, 60, 30, hWnd, (HMENU)777, hInst, NULL);
			    
			hEdit6 = CreateWindowW(L"EDIT", TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
				900, 500, 400, 30, hWnd, (HMENU)66, hInst, NULL);
			hButton4 = CreateWindowW(L"BUTTON", TEXT("GetFile"), WS_CHILD | WS_VISIBLE | WS_BORDER,
				900, 550, 100, 30, hWnd, (HMENU)110, hInst, NULL);

			break;
			
		}
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
            switch (wmId)
            {
			//Plot
			case 111:
			{
				/*int var1, var2;
				TCHAR buf[256];
				::SendMessage(hEdit1,WM_GETTEXT,255, (LPARAM)buf);
				var1 = ::wcstol(buf, NULL, 10); //将字符串转换成长整数值
				::SendMessage(hEdit2, WM_GETTEXT, 255, (LPARAM)buf);
				var2 = ::wcstol(buf, NULL, 10);
				int result = var1 + var2;
				::wsprintf(buf, L"%d", result);
				SendMessage(hEdit3, WM_SETTEXT, 0,(LPARAM)buf);*/
				WCHAR var1[100], var2[100], var3[100], var4[100], var5[100];
				::SendMessage(hEdit1, WM_GETTEXT, 255, (LPARAM)var1);
				::SendMessage(hEdit2, WM_GETTEXT, 255, (LPARAM)var2);
				::SendMessage(hEdit3, WM_GETTEXT, 255, (LPARAM)var3);
				::SendMessage(hEdit4, WM_GETTEXT, 255, (LPARAM)var4);
				::SendMessage(hEdit5, WM_GETTEXT, 255, (LPARAM)var5);

				

				/*wchar_t * wText = var1;
				//DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);// WideCharToMultiByte的运用
				dwNum1 = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);// WideCharToMultiByte的运用,其中dwNum=1时表示输入为空
				char *psText;   // psText为char*的临时数组，作为赋值给std::string的中间变量
				psText = new char[dwNum1];
				WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum1, NULL, FALSE);// WideCharToMultiByte的再次运用,将宽字符串转换为多字节字符串
				expr1 = psText;// std::string赋值
				delete[]psText;// psText的清除*/
				
				expr1 = GetExpr(var1,dwNum1);
				expr2 = GetExpr(var2,dwNum2);
				expr3 = GetExpr(var3,dwNum3);
				expr4 = GetExpr(var4,dwNum4);
				expr5 = GetExpr(var5,dwNum5);

				InvalidateRect(hWnd, NULL, FALSE);
				UpdateWindow(hWnd);


				//if (Compute(expr, 0.2) == 3.2) 
					//::SendMessage(hEdit1, WM_SETTEXT, 0, (LPARAM)var5);
				/*::SendMessage(hEdit1, WM_SETTEXT, 0, (LPARAM)var5);
				::SendMessage(hEdit2, WM_SETTEXT, 0, (LPARAM)var4);
				::SendMessage(hEdit3, WM_SETTEXT, 0, (LPARAM)var3);
				::SendMessage(hEdit4, WM_SETTEXT, 0, (LPARAM)var2);
				::SendMessage(hEdit5, WM_SETTEXT, 0, (LPARAM)var1);*/
				
				
				
				
				
				break;

			}
			//Reset
			case 222:
			{	
				::SendMessage(hEdit1, WM_SETTEXT, 0, NULL);
				::SendMessage(hEdit2, WM_SETTEXT, 0, NULL);
				::SendMessage(hEdit3, WM_SETTEXT, 0, NULL);
				::SendMessage(hEdit4, WM_SETTEXT, 0, NULL);
				::SendMessage(hEdit5, WM_SETTEXT, 0, NULL);

				//删除函数图象
				dwNum1 = 1;
				dwNum2 = 1;
				dwNum3 = 1;
				dwNum4 = 1;
				dwNum5 = 1;

				InvalidateRect(hWnd, NULL, FALSE);
				UpdateWindow(hWnd);


				break;

			}
			case 333:
			{
				//color
				gColor1 = ShowColorDialog(hWnd);  // 调用调色板对话框  
				InvalidateRect(hwndPanel, NULL, TRUE);
				//color
				break;
			}
			case 444:
			{
				//color
				gColor2 = ShowColorDialog(hWnd);  // 调用调色板对话框  
				InvalidateRect(hwndPanel, NULL, TRUE);
				//color
				break;
			}
			case 555:
			{
				//color
				gColor3 = ShowColorDialog(hWnd);  // 调用调色板对话框  
				InvalidateRect(hwndPanel, NULL, TRUE);
				//color
				break;
			}
			case 666:
			{
				//color
				gColor4 = ShowColorDialog(hWnd);  // 调用调色板对话框  
				InvalidateRect(hwndPanel, NULL, TRUE);
				//color
				break;
			}
			case 777:
			{
				//color
				gColor5 = ShowColorDialog(hWnd);  // 调用调色板对话框  
				InvalidateRect(hwndPanel, NULL, TRUE);
				//color
				break;
			}

			case 110:
			{
				::SendMessage(hEdit6, WM_GETTEXT, 255, (LPARAM)filePath);
				wchar_t * wTempText = filePath;
				fpNum = WideCharToMultiByte(CP_OEMCP, NULL, wTempText, -1, NULL, 0, NULL, FALSE);
				char *psText;   // psText为char*的临时数组，作为赋值给std::string的中间变量
				psText = new char[fpNum];
				WideCharToMultiByte(CP_OEMCP, NULL, wTempText, -1, psText, fpNum, NULL, FALSE);// WideCharToMultiByte的再次运用,将宽字符串转换为多字节字符串
				string fp = psText;// std::string赋值
				delete[]psText;// psText的清除

				fileData  = readFile(fp);

				InvalidateRect(hWnd, NULL, FALSE);
				UpdateWindow(hWnd);

				break;
			}



            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case IDR_MENU1:
				SaveBmp(memBmp, "abcde");
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_MOUSEMOVE:
		{
			if (wParam & 0x1 && firstMoveFlag == 0) {
				firstMoveFlag = 1;
				initMovePosX = LOWORD(lParam);
				initMovePosY = HIWORD(lParam);
				MovePosX = LOWORD(lParam);
				MovePosY = HIWORD(lParam);
			}
			else if (wParam & 0x1 && firstMoveFlag > 0) {
				initMovePosX = MovePosX;
				initMovePosY = MovePosY;
				MovePosX = LOWORD(lParam);
				MovePosY = HIWORD(lParam);

				WCHAR var1[100], var2[100], var3[100], var4[100], var5[100];
				::SendMessage(hEdit1, WM_GETTEXT, 255, (LPARAM)var1);
				::SendMessage(hEdit2, WM_GETTEXT, 255, (LPARAM)var2);
				::SendMessage(hEdit3, WM_GETTEXT, 255, (LPARAM)var3);
				::SendMessage(hEdit4, WM_GETTEXT, 255, (LPARAM)var4);
				::SendMessage(hEdit5, WM_GETTEXT, 255, (LPARAM)var5);



				expr1 = GetExpr(var1, dwNum1);
				expr2 = GetExpr(var2, dwNum2);
				expr3 = GetExpr(var3, dwNum3);
				expr4 = GetExpr(var4, dwNum4);
				expr5 = GetExpr(var5, dwNum5);

				

				InvalidateRect(hWnd, NULL, FALSE);
				UpdateWindow(hWnd);
			}
		}
		break;
	case WM_LBUTTONUP:
		{
			if (firstMoveFlag > 0)
				firstMoveFlag = 0;
		}
		break;
	case WM_MOUSEWHEEL:
		{
			zDelta = (short)HIWORD(wParam);
			WheelPosX = (short)LOWORD(lParam);
			WheelPosY = (short)HIWORD(lParam);
			if (zDelta > 0 )
				wheelDir++;
			else if (zDelta < 0)
				wheelDir--;

			WCHAR var1[100], var2[100], var3[100], var4[100], var5[100];
			::SendMessage(hEdit1, WM_GETTEXT, 255, (LPARAM)var1);
			::SendMessage(hEdit2, WM_GETTEXT, 255, (LPARAM)var2);
			::SendMessage(hEdit3, WM_GETTEXT, 255, (LPARAM)var3);
			::SendMessage(hEdit4, WM_GETTEXT, 255, (LPARAM)var4);
			::SendMessage(hEdit5, WM_GETTEXT, 255, (LPARAM)var5);



			expr1 = GetExpr(var1, dwNum1);
			expr2 = GetExpr(var2, dwNum2);
			expr3 = GetExpr(var3, dwNum3);
			expr4 = GetExpr(var4, dwNum4);
			expr5 = GetExpr(var5, dwNum5);


			InvalidateRect(hWnd,NULL,FALSE);
			UpdateWindow(hWnd);
		}
		break;
		
    case WM_PAINT:
        {
			// TODO: 在此处添加使用 hdc 的任何绘图代码...
			
			TCHAR buf[256];

			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			RECT rc;
			GetClientRect(hWnd, &rc);

			/*rc.left += 125;
			rc.right -= 225;
			rc.top += 50;
			rc.bottom -= 75;*/

			int nWidth = rc.right - rc.left;
			int nHeight = rc.bottom - rc.top;

			if (oldWidth != nWidth || oldHeight != nHeight) {
				memDcX = nWidth / 4;
				memDcY = nHeight / 4;
				oldWidth = nWidth;
				oldHeight = nHeight;
			}

			//wchar_t strText[] = TEXT("这是一个Gdi要显示的字符串");

			//HDC hdc = GetDC(hWnd);
			SIZE sz;
			memDc = CreateCompatibleDC(hdc);

			memBmp = CreateCompatibleBitmap(hdc, nWidth, nHeight);
			HBITMAP oldBmp = (HBITMAP)SelectObject(memDc, memBmp);

			RECT fillRc;
			fillRc.left = 0;
			fillRc.top = 0;
			fillRc.bottom = nHeight;
			fillRc.right = nWidth;

			LOGBRUSH lb;
			lb.lbColor = RGB(255, 255, 255);//memDC背景色
			lb.lbStyle = BS_SOLID;
			lb.lbHatch = 0;
			HBRUSH brush = CreateBrushIndirect(&lb);

			FillRect(memDc, &fillRc, brush);


				

			//绘制坐标系
			int pixels = 66 + 10 * wheelDir;  //单位像素点
			int xNum = 660 / pixels;  //x轴最大刻度
			int yNum = 330 / pixels;

			HPEN hpenXY, hpenXYOld;
			hpenXY = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
			hpenXYOld = (HPEN)SelectObject(memDc, hpenXY);
			//绘制X轴
			MoveToEx(memDc, 680, 335, NULL);
			LineTo(memDc, 1360, 335);
			LineTo(memDc, 0, 335);
			//绘制Y轴
			MoveToEx(memDc, 680, 335, NULL);
			LineTo(memDc, 680, 0);
			LineTo(memDc, 680, 1360);

			//TextOut(hdc, 275, 355, L"0", 1);
			SelectObject(memDc, hpenXYOld);
			DeleteObject(hpenXY);


			

				//绘制坐标系中的虚线和刻度值
			TCHAR tickNumbers[256];  //刻度值
			int totalBytes;
			HPEN hpen, hpenOld;
			hpen = CreatePen(PS_DASH, 1, RGB(216, 216, 216));
			hpenOld = (HPEN)SelectObject(memDc, hpen);
			for (int i = 0; i < 2 * xNum + 1; i++) {
				if (i != xNum) {
					MoveToEx(memDc, 680 - xNum*pixels + i * pixels, 0, NULL);
					LineTo(memDc, 680 - xNum*pixels + i * pixels, 670);
					::wsprintf(tickNumbers, L"%d", i - xNum);
					totalBytes = WideCharToMultiByte(CP_OEMCP, NULL, tickNumbers, -1, NULL, 0, NULL, FALSE);
					TextOut(memDc, 680 - xNum*pixels + i * pixels + 5, 340, tickNumbers, totalBytes - 1);
				}
			}
			for (int i = 0; i < 2 * yNum + 1; i++) {
				if (i != yNum) {
					MoveToEx(memDc, 10, 335 + yNum*pixels - pixels * i, NULL);
					LineTo(memDc, 1330, 335 + yNum*pixels - pixels * i);
					::wsprintf(tickNumbers, L"%d", i - yNum);
					totalBytes = WideCharToMultiByte(CP_OEMCP, NULL, tickNumbers, -1, NULL, 0, NULL, FALSE);
					TextOut(memDc, 685, 335 + yNum*pixels - pixels * i + 5, tickNumbers, totalBytes - 1);
				}
			}

			SelectObject(memDc, hpenOld);
			DeleteObject(hpen);

			if (dwNum1 != 1) {
				PlotFunc(pixels, xNum, yNum, expr1, gColor1);
			}
			dwNum1 = 1;
			if (dwNum2 != 1) {
				PlotFunc(pixels, xNum, yNum, expr2, gColor2);
			}
			dwNum2 = 1;
			if (dwNum3 != 1) {
				PlotFunc(pixels, xNum, yNum, expr3, gColor3);
			}
			dwNum3 = 1;
			if (dwNum4 != 1) {
				PlotFunc(pixels, xNum, yNum, expr4, gColor4);
			}
			dwNum4 = 1;
			if (dwNum5 != 1) {
				PlotFunc(pixels, xNum, yNum, expr5, gColor5);
			}
			dwNum5 = 1;

			//绘制从文件中得到的数据的图像
			if (fpNum != 1) {
				HPEN hFuncPen, hFunPenOld;
				hFuncPen = CreatePen(PS_SOLID, 1, RGB(25, 25, 25));
				hFunPenOld = (HPEN)SelectObject(memDc, hFuncPen);
				int xLastPixels = 680 + fileData[0] * pixels;
				int yLastPixels = 335 - fileData[1] * pixels;
				int xPixels;
				int yPixels;
				for (int i = 2; i < fileData.size(); i++) {

					xPixels = 680 + fileData[i] * pixels;
					i++;
					yPixels = 335 - fileData[i] * pixels;
					if ((335 - yPixels) / pixels  < yNum && (335 - yPixels) / pixels  > -yNum && (335 - yLastPixels) / pixels < yNum && (335 - yLastPixels) / pixels > -yNum) {
						MoveToEx(memDc, xLastPixels, yLastPixels, NULL);
						LineTo(memDc, xPixels, yPixels);
					}
					xLastPixels = xPixels;
					yLastPixels = yPixels;
				}

				//MoveToEx(memDc, 680, 335, NULL);
				//LineTo(memDc, 800, 100);
				SelectObject(memDc, hFunPenOld);
				DeleteObject(hFuncPen);
			}
			fpNum = 1;
				

			BitBlt(hdc, rc.left + 100, rc.top + 100, nWidth / 2, nHeight / 2, memDc, memDcX + initMovePosX - MovePosX, memDcY + initMovePosY - MovePosY, SRCCOPY);
			memDcX = memDcX + initMovePosX - MovePosX;
			memDcY = memDcY + initMovePosY - MovePosY;

			SelectObject(memDc, oldBmp);
			//SelectObject(memDc, oldPen);
			//SelectObject(memDc, oldFont);
			DeleteObject(brush);
			//DeleteObject(memDc);
			DeleteDC(memDc);
			//::DrawText(memDc, strText, wcslen(strText), &fillRc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);   // 居中显示

			/*HPEN hpenRim, hpenRimOld;
			hpenRim = CreatePen(PS_SOLID, 3, RGB(120, 109, 109));
			hpenRimOld = (HPEN)SelectObject(hdc, hpenRim);
			MoveToEx(hdc, rc.left + 100, rc.top + 100, NULL);
			LineTo(hdc, rc.left + 100+ nWidth / 2, rc.top + 100);
			MoveToEx(hdc, rc.left + 100, rc.top + 100, NULL);
			LineTo(hdc, rc.left + 100 , rc.top + 100+ nHeight / 2);
			MoveToEx(hdc, rc.left + 100 + nWidth / 2, rc.top + 100, NULL);
			LineTo(hdc, rc.left + 100 + nWidth / 2, rc.top + 100 + nHeight / 2);
			MoveToEx(hdc, rc.left + 100, rc.top + 100 + nHeight / 2, NULL);
			LineTo(hdc, rc.left + 100 + nWidth / 2, rc.top + 100 + nHeight / 2);

			SelectObject(hdc, hpenRimOld);
			DeleteObject(hpenRim);*/

			//绘制控制台
			RECT rect;
			HBRUSH hBrush = (HBRUSH)GetStockObject(DC_BRUSH);
			rect.left = 900;   //向下竖排
			rect.right = 930;

			SetDCBrushColor(hdc, RGB(0, 139, 251));
			rect.bottom = 130;
			rect.top = 100;
			FillRect(hdc, &rect, hBrush);

			SetDCBrushColor(hdc, RGB(252, 52, 54));
			rect.bottom = 180;
			rect.top = 150;
			FillRect(hdc, &rect, hBrush);

			SetDCBrushColor(hdc, RGB(54, 99, 6));
			rect.bottom = 230;
			rect.top = 200;
			FillRect(hdc, &rect, hBrush);

			SetDCBrushColor(hdc, RGB(251, 156, 50));
			rect.bottom = 280;
			rect.top = 250;
			FillRect(hdc, &rect, hBrush);

			SetDCBrushColor(hdc, RGB(150, 5, 184));
			rect.bottom = 330;
			rect.top = 300;
			FillRect(hdc, &rect, hBrush);

			WCHAR fx[10] = L"f(x)=";

			for (int i = 0; i < 5; i++) {
				::wsprintf(buf, L"%d", i + 1);
				buf[1] = '.';
				TextOut(hdc, 870, 105 + i * 50, buf, 2);
				TextOut(hdc, 950, 105 + i * 50, fx, 5);
			}
			::wsprintf(buf, L"%s", TEXT("Read from csv file："));
			TextOut(hdc, 900, 470, buf, 19);


			ReleaseDC(hWnd, hdc);
			//DeleteDC(hdc);
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

// “关于”框的消息处理程序。
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




COLORREF ShowColorDialog(HWND hWnd) {


	CHOOSECOLOR cc;
	static COLORREF crCustClr[16];


	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hWnd;
	cc.lpCustColors = (LPDWORD)crCustClr;
	cc.rgbResult = RGB(0, 255, 0);
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	ChooseColor(&cc);


	return cc.rgbResult;
}


