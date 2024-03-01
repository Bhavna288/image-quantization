#ifndef M_PI
#define M_PI 3.14159265358979323846
#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "Image.h"
#include <cstring>
#include <math.h>
#include <utility>
#include <cmath>
#include <iostream>
#include <cstdio>

// Constructor and Desctructors
MyImage::MyImage()
{
	Data = NULL;
	OriginalData = NULL;
	Width = -1;
	Height = -1;
	ImagePath[0] = 0;
}

MyImage::~MyImage()
{
	if (Data)
		delete Data;
	if (OriginalData)
		delete OriginalData;
}


// Copy constructor
MyImage::MyImage(MyImage* otherImage)
{
	Height = otherImage->Height;
	Width = otherImage->Width;
	Data = new char[Width * Height * 3];
	OriginalData = new unsigned char[Width * Height * 3];
	strcpy_s(ImagePath, sizeof(ImagePath), otherImage->ImagePath); // Using strcpy_s

	for (int i = 0; i < (Height * Width * 3); i++)
	{
		Data[i] = otherImage->Data[i];
	}


}



// = operator overload
MyImage& MyImage::operator= (const MyImage& otherImage)
{
	Height = otherImage.Height;
	Width = otherImage.Width;
	Data = new char[Width * Height * 3];
	strcpy((char*)otherImage.ImagePath, ImagePath);

	for (int i = 0; i < (Height * Width * 3); i++)
	{
		Data[i] = otherImage.Data[i];
	}

	return *this;

}


// MyImage::ReadImage
// Function to read the image given a path
bool MyImage::ReadImage()
{

	// Verify ImagePath
	if (ImagePath[0] == 0 || Width < 0 || Height < 0)
	{
		fprintf(stderr, "Image or Image properties not defined");
		fprintf(stderr, "Usage is `Image.exe Imagefile w h`");
		return false;
	}

	// Create a valid output file pointer
	FILE* IN_FILE;
	errno_t err;
	err = fopen_s(&IN_FILE, ImagePath, "rb"); // Using fopen_s
	if (err != 0) {
		fprintf(stderr, "Error Opening File for Reading");
		return false;
	}

	// Create and populate RGB buffers
	int i;
	char* Rbuf = new char[Height * Width];
	char* Gbuf = new char[Height * Width];
	char* Bbuf = new char[Height * Width];

	for (i = 0; i < Width * Height; i++)
	{
		Rbuf[i] = fgetc(IN_FILE);
	}
	for (i = 0; i < Width * Height; i++)
	{
		Gbuf[i] = fgetc(IN_FILE);
	}
	for (i = 0; i < Width * Height; i++)
	{
		Bbuf[i] = fgetc(IN_FILE);
	}

	// Allocate Data structure and copy
	Data = new char[Width * Height * 3];
	OriginalData = new unsigned char[Width * Height * 3];
	for (i = 0; i < Height * Width; i++)
	{
		Data[3 * i] = Bbuf[i];
		Data[3 * i + 1] = Gbuf[i];
		Data[3 * i + 2] = Rbuf[i];
	}
	std::memcpy(OriginalData, Data, Width * Height * 3);
	// Clean up and return
	delete Rbuf;
	delete Gbuf;
	delete Bbuf;
	fclose(IN_FILE);

	return true;

}


// MyImage functions defined here
bool MyImage::WriteImage()
{
	// Verify ImagePath
	// Verify ImagePath
	if (ImagePath[0] == 0 || Width < 0 || Height < 0)
	{
		fprintf(stderr, "Image or Image properties not defined");
		return false;
	}

	// Create a valid output file pointer
	FILE* OUT_FILE;
	errno_t err;
	err = fopen_s(&OUT_FILE, ImagePath, "wb"); // Using fopen_s
	if (err != 0) {
		fprintf(stderr, "Error Opening File for Writing");
		return false;
	}

	// Create and populate RGB buffers
	int i;
	char* Rbuf = new char[Height * Width];
	char* Gbuf = new char[Height * Width];
	char* Bbuf = new char[Height * Width];

	for (i = 0; i < Height * Width; i++)
	{
		Bbuf[i] = Data[3 * i];
		Gbuf[i] = Data[3 * i + 1];
		Rbuf[i] = Data[3 * i + 2];
	}


	// Write data to file
	for (i = 0; i < Width * Height; i++)
	{
		fputc(Rbuf[i], OUT_FILE);
	}
	for (i = 0; i < Width * Height; i++)
	{
		fputc(Gbuf[i], OUT_FILE);
	}
	for (i = 0; i < Width * Height; i++)
	{
		fputc(Bbuf[i], OUT_FILE);
	}

	// Clean up and return
	delete Rbuf;
	delete Gbuf;
	delete Bbuf;
	fclose(OUT_FILE);

	return true;

}
struct RGBColor {
	unsigned char R;
	unsigned char G;
	unsigned char B;
};

// Helper function to clamp values
int Clamp(int value, int min, int max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

float DegToRad(float degrees) {
	return degrees * static_cast<float>(M_PI) / 180.0f;
}

RGBColor SamplePixel(unsigned char* data, float x, float y, int width, int height) {
	// Initialize the color to black (you may change this to white or any other color)
	RGBColor color = { 0, 0, 0 };

	// Calculate the base indices of the 3x3 grid
	int xBase = static_cast<int>(floor(x));
	int yBase = static_cast<int>(floor(y));

	// Initialize sums and count for averaging
	int sumR = 0, sumG = 0, sumB = 0, count = 0;

	// Sum the colors of the pixels in the 3x3 grid surrounding the floating-point coordinates
	for (int dy = -1; dy <= 1; dy++) {
		for (int dx = -1; dx <= 1; dx++) {
			// Calculate the exact position to sample
			int sampleX = Clamp(xBase + dx, 0, width - 1);
			int sampleY = Clamp(yBase + dy, 0, height - 1);

			// Calculate the index for the sample position
			int sampleIndex = (sampleY * width + sampleX) * 3;

			// Sum the color values
			sumB += static_cast<unsigned char>(data[sampleIndex]);
			sumG += static_cast<unsigned char>(data[sampleIndex + 1]);
			sumR += static_cast<unsigned char>(data[sampleIndex + 2]);

			// Increment the count
			count++;
		}
	}

	// Calculate the average color values
	if (count > 0) {
		color.B = sumB / count;
		color.G = sumG / count;
		color.R = sumR / count;
	}

	return color;
}


// Here is where you would place your code to modify an image
// eg Filtering, Transformation, Cropping, etc.
// This function should be called for each frame to apply the cumulative zoom and rotation.
bool MyImage::Modify(float zoomSpeed, float rotationSpeed, int fps, int currentFrame)
{
	// Calculate the total rotation in radians
	float theta = -DegToRad(rotationSpeed) * (currentFrame / static_cast<float>(fps));

	// Calculate the cumulative zoom factor for the current frame
	float cumulativeZoom = pow(1/zoomSpeed, static_cast<float>(currentFrame) / fps);

	// Initialize the center of the image
	float centerX = Width / 2.0f;
	float centerY = Height / 2.0f;

	// Temporary buffer for storing the modified image
	char* tempData = new char[Width * Height * 3];

	// Loop through each pixel in the output image
	for (int y = 0; y < Height; y++) {
		for (int x = 0; x < Width; x++) {
			// Translate coordinates to the center of the image
			float xOrigin = x - centerX;
			float yOrigin = y - centerY;

			// Apply the zoom and rotation transformations
			float xTransformed = (cos(theta) * xOrigin - sin(theta) * yOrigin) * cumulativeZoom;
			float yTransformed = (sin(theta) * xOrigin + cos(theta) * yOrigin) * cumulativeZoom;

			// Translate back from the image center
			float newX = xTransformed + centerX;
			float newY = yTransformed + centerY;

			// Sample the pixel from the original image
			RGBColor sampledColor = { 0, 0, 0 }; // Default to black

			// Check if the new coordinates are within the valid range
			if (newX >= 0 && newX < Width && newY >= 0 && newY < Height) {
				sampledColor = SamplePixel(OriginalData, newX, newY, Width, Height);
			}

			// Set the pixel in the temporary buffer
			int pixelIndex = (y * Width + x) * 3;
			tempData[pixelIndex] = sampledColor.B;
			tempData[pixelIndex + 1] = sampledColor.G;
			tempData[pixelIndex + 2] = sampledColor.R;
		}
	}

	// Copy the modified data back to the image data
	memcpy(Data, tempData, Width * Height * 3);

	// Clean up
	delete[] tempData;

	return true;
}
