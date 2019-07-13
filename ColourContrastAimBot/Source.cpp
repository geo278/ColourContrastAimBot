// SmartAimBot.cpp : Defines the entry point for the application.

//#include "scan.h"
#include "iostream"
#include "Windows.h"

using namespace std;

int width = 200;
int height = 200;

RGBQUAD * capture(POINT a, POINT b) {
	// copy screen to bitmap
	HDC     hScreen = GetDC(NULL);
	HDC     hDC = CreateCompatibleDC(hScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, abs(b.x - a.x), abs(b.y - a.y));
	HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
	BOOL    bRet = BitBlt(hDC, 0, 0, abs(b.x - a.x), abs(b.y - a.y), hScreen, a.x, a.y, SRCCOPY); // BitBlt does the copying

	/*
	// save bitmap to clipboard
	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, hBitmap);
	CloseClipboard();
	*/

	// Array conversion:
	RGBQUAD* pixels = new RGBQUAD[width * height];

	BITMAPINFOHEADER bmi = { 0 };
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biPlanes = 1;
	bmi.biBitCount = 32;
	bmi.biWidth = width;
	bmi.biHeight = -height;
	bmi.biCompression = BI_RGB;
	bmi.biSizeImage = 0;// 3 * ScreenX * ScreenY;
	
	GetDIBits(hDC, hBitmap, 0, height, pixels, (BITMAPINFO*)& bmi, DIB_RGB_COLORS);


	// clean up
	SelectObject(hDC, old_obj);
	DeleteDC(hDC);
	ReleaseDC(NULL, hScreen);
	DeleteObject(hBitmap);
	return pixels;
}

bool Aim() {
	POINT a, b;
	a.x = 1920 / 2 - width / 2;
	a.y = 1080 / 2 - height / 2;
	b.x = 1920 / 2 + width / 2;
	b.y = 1080 / 2 + height / 2;

	RGBQUAD * pixels;
	POINT targetPos; // centered at top left corner of capture zone

	double radius, angle;
	int x, y, index; // centered at center of screen
	int red, green, blue;
	int sampleR = 245, sampleG = 160, sampleB = 70; // nc infravision
	// int sampleR = 180, sampleG = 160, sampleB = 130; // corona ir values
	// int sampleR = 255, sampleG = 145, sampleB = 57; // vs infravision
	// int sampleR = 145, sampleG = 145, sampleB = 145; // tr irnv values

	bool sampled = false;
	int brightest = 0;

	while (true) {
		angle = 2 * 3.141592654 / 8;
		radius = 2;
		//0x39
		if ((GetKeyState(0x39) & 0x100) != 0) { // while key 9 pressed
			// sample colors
			// cout << "reached" << endl;
		}
		if ((GetKeyState(VK_RBUTTON) & 0x100) != 0 && !(GetKeyState(VK_SHIFT) & 0x8000)) { // while rmb pressed
			pixels = capture(a, b);
			sampled = false;

			for (int i = 0; i < 40000; i++) {
				x = (int)(radius * cos(angle) + width / 2);
				y = (int)(radius * sin(angle) + height / 2);
				radius = radius + 0.1;
				angle += 1 / radius;
				index = y * width + x;

				if (x < 0 || x > 399 || y < 0 || y > 399) {
					//cout << "OUT OF BOUNDS  " << x << "  " << y << endl;
					break;
				}

				red = (int)pixels[index].rgbRed;
				green = (int)pixels[index].rgbGreen;
				blue = (int)pixels[index].rgbBlue;
				
				if ((abs(red - sampleR) < 35 && abs(green - sampleG) < 35 && abs(blue - sampleB) < 35) ) {
					brightest = red + green + blue;
					targetPos.x = index % width;
					targetPos.y = index / width;
					sampled = true;
				}

				//cout << angle / 2 / 3.1415 << " " << radius << endl;
				//cout << red << " " << green << " " << blue << endl;
				//cout << abs(red - sampleR) << " " << abs(green - sampleG) << " " << abs(blue - sampleB) << endl;
				//cout << " " << endl;

				if (i%16 == 0 && sampled) { // bright areas
					mouse_event(MOUSEEVENTF_MOVE, (targetPos.x - width / 2), (targetPos.y - height / 2), 0, 0); // x and y are deltas, not abs coordinates
					//cout << "BREAK" << endl;
					break;
				}
				
			}
			delete[] pixels;
		}
		Sleep(1);
	}
	return true;
}

int main() {
	Aim();
	return 0;
}
