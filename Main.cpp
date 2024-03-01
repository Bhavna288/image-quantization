#include "Image.h"
#include <iostream>

#define MAX_LOADSTRING 100

// Global Variables:
MyImage			inImage;						// image objects
HINSTANCE		hInst;							// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// The title bar text
float globalZoomSpeed = 1.0f;					// Default zoom speed
float globalRotationSpeed = 0.0f;				// Default rotation speed
int globalFPS = 30;								// Default frames per second
int frameCounter = 1;							// Frame counter for animation

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);


// Main entry point for a windows application
int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// Create a separate console window to display output to stdout
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	char imagePath[MAX_PATH];

	int numArgsParsed = sscanf_s(lpCmdLine, "%s %f %f %d", imagePath, (unsigned)_countof(imagePath), &globalZoomSpeed, &globalRotationSpeed, &globalFPS);
	if (numArgsParsed != 4) {
		wprintf(L"Invalid command line arguments.\n");
		wprintf(L"Usage: YourProgram.exe imagePath zoomFactor rotationAngle framesPerSecond\n");
		return -1; // Exit if the command line arguments are not valid
	}

	if (globalZoomSpeed < 0.50f || globalZoomSpeed > 2.00f) {
		wprintf(L"Invalid zoom factor. It must be between 0.50 and 2.00.\n");
		return -1;
	}

	if (globalRotationSpeed < -180.00f || globalRotationSpeed > 180.00f) {
		wprintf(L"Invalid rotation speed. It must be between -180.00 and 180.00 degrees per second.\n");
		return -1;
	}

	if (globalFPS < 1 || globalFPS > 30) {
		wprintf(L"Invalid frames per second. It must be between 1 and 30.\n");
		return -1;
	}
	printf("Image Path: %s\n", imagePath);
	printf("Zoom Factor: %.2f\n", globalZoomSpeed);
	printf("Rotation Angle: %.2f\n", globalRotationSpeed);
	printf("Frames Per Second: %d\n", globalFPS);
	// Set up the images
	// Modify the height and width values here to read and display an image with
	// different dimensions. 
	int w = 512;
	int h = 512;
	inImage.setWidth(w);
	inImage.setHeight(h);

	inImage.setImagePath(imagePath);
	inImage.ReadImage();

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IMAGE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_IMAGE);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_IMAGE);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = (LPCSTR)IDC_IMAGE;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}


//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}


//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// TO DO: part useful to render video frames, may place your own code here in this function
	// You are free to change the following code in any way in order to display the video

	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
	RECT rt;
	GetClientRect(hWnd, &rt);

	switch (message)
	{
	case WM_CREATE:
		// Set a timer to trigger WM_TIMER message to update frames
		SetTimer(hWnd, 1, 1000 / globalFPS, NULL);
		break;

	case WM_TIMER:
		// On timer, modify and redraw image
		switch (wParam) {
		case 1:
			// Update the image based on zoom and rotation
			// printf("\nFrame counter: %d", frameCounter);
			inImage.Modify(globalZoomSpeed, globalRotationSpeed, globalFPS, frameCounter);
			InvalidateRect(hWnd, NULL, TRUE); // Redraw window
			++frameCounter; // Increment the frame counter for the next call
			break;
		}
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case ID_MODIFY_IMAGE:
			InvalidateRect(hWnd, &rt, false);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT: {
		hdc = BeginPaint(hWnd, &ps);

		// The BITMAPINFO structure defines the dimensions and color format of a DIB
		BITMAPINFO bmi = { 0 };
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = inImage.getWidth();
		bmi.bmiHeader.biHeight = -inImage.getHeight();		// Negative so the origin is the upper-left corner
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 24;						// Assuming 24-bit color depth
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = 0;						// 0 for BI_RGB
		bmi.bmiHeader.biXPelsPerMeter = 0;					// Resolution not important
		bmi.bmiHeader.biYPelsPerMeter = 0;					// Resolution not important
		bmi.bmiHeader.biClrUsed = 0;						// Use maximum color table size
		bmi.bmiHeader.biClrImportant = 0;					// All colors are important

		// Calculate the size of the image data
		DWORD dwBmpSize = ((inImage.getWidth() * bmi.bmiHeader.biBitCount + 31) / 32) * 4 * inImage.getHeight();

		// Create a buffer to hold the image data
		HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
		char* lpbitmap = (char*)GlobalLock(hDIB);

		// Copy the image data into the buffer. Assuming that getImageData() returns the modified image data.
		memcpy(lpbitmap, inImage.getImageData(), dwBmpSize);

		// Set the DIBits to the device context's current bitmap
		SetDIBitsToDevice(hdc,
			0, 0, inImage.getWidth(), inImage.getHeight(),	// Destination rectangle
			0, 0, 0, inImage.getHeight(),					// Source rectangle
			lpbitmap, &bmi, DIB_RGB_COLORS);

		// Release and free the buffer
		GlobalUnlock(hDIB);
		GlobalFree(hDIB);

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		KillTimer(hWnd, 1); // Remove the timer
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}




// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}