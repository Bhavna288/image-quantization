#ifndef IMAGE_DISPLAY
#define IMAGE_DISPLAY
#define _CRT_SECURE_NO_WARNINGS

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "afxwin.h"

// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// Class structure of Image 
// Use to encapsulate an RGB image
class MyImage
{

private:
	int		Width;					// Width of Image
	int		Height;					// Height of Image
	char	ImagePath[_MAX_PATH];	// Image location
	char* Data;						// RGB data of the image
	unsigned char* OriginalData;    // Original Data for preserving

public:
	// Constructor
	MyImage();
	// Copy Constructor
	MyImage(MyImage* otherImage);
	// Destructor
	~MyImage();

	// operator overload
	MyImage& operator= (const MyImage& otherImage);

	// Reader & Writer functions
	void	setWidth(const int w) { Width = w; };
	void	setHeight(const int h) { Height = h; };
	void	setImageData(const char* img) { Data = (char*)img; };
	void	setImagePath(const char* path) { strcpy_s(ImagePath, path); }
	int		getWidth() { return Width; };
	int		getHeight() { return Height; };
	char* getImageData() { return Data; };
	char* getImagePath() { return ImagePath; }

	// Input Output operations
	bool	ReadImage();
	bool	WriteImage();

	// Modifications
	bool Modify(float zoomSpeed, float rotationSpeed, int fps, int currentFrame);

};

#endif //IMAGE_DISPLAY