// SmartAimBot.cpp : Defines the entry point for the application.

//#include "scan.h"
#include "iostream"
#include "Windows.h"

using namespace std;

RGBQUAD * capture(POINT a, POINT b) {
	// copy screen to bitmap
	HDC     hScreen = GetDC(NULL);
	HDC     hDC = CreateCompatibleDC(hScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, abs(b.x - a.x), abs(b.y - a.y));
	HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
	BOOL    bRet = BitBlt(hDC, 0, 0, abs(b.x - a.x), abs(b.y - a.y), hScreen, a.x, a.y, SRCCOPY); // BitBlt does the copying

	// save bitmap to clipboard
	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, hBitmap);
	CloseClipboard();


	// Array conversion:
	RGBQUAD* pixels = new RGBQUAD[160000];

	BITMAPINFOHEADER bmi = { 0 };
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biPlanes = 1;
	bmi.biBitCount = 32;
	bmi.biWidth = 400;
	bmi.biHeight = -400;
	bmi.biCompression = BI_RGB;
	bmi.biSizeImage = 0;// 3 * ScreenX * ScreenY;
	
	GetDIBits(hDC, hBitmap, 0, 400, pixels, (BITMAPINFO*)& bmi, DIB_RGB_COLORS);


	// clean up
	SelectObject(hDC, old_obj);
	DeleteDC(hDC);
	ReleaseDC(NULL, hScreen);
	DeleteObject(hBitmap);
	return pixels;
}

bool Aim() {
	POINT a, b;
	a.x = 760;
	a.y = 340;
	b.x = 1160;
	b.y = 740;

	RGBQUAD * pixels = new RGBQUAD[160000];
	POINT targetPos;

	int red, redPrev;
	int green, greenPrev;
	int blue, bluePrev;

	double radius, angle, incFactor;
	incFactor = 80;
	int x, y, index; // scan coordinates
	
	while (true) {
		angle = 3.1415;
		radius = 3;
		if ((GetKeyState(VK_RBUTTON) & 0x100) != 0) { // while rmb pressed

			pixels = capture(a, b);
			for (int i = 0; i < 160000; i++) {
				x = radius * cos(angle) + 200;
				y = radius * sin(angle) + 200;
				angle += 2 * 3.1415 / incFactor;
				radius += 1 / incFactor;

				if (x < 0 || x > 399 || y < 0 || y > 399) {
					//cout << "OUT OF BOUNDS  " << x << "  " << y << endl;
					break;
				}
				index = y * 400 + x;

				red = (int)pixels[index].rgbRed;
				green = (int)pixels[index].rgbGreen;
				blue = (int)pixels[index].rgbBlue;

				if (i == 0) {
					redPrev = red;
					greenPrev = green;
					bluePrev = blue;
				}

				if (//abs(red - redPrev) > 90 && 
					//abs(green - greenPrev) > 90 && 
					abs(blue - bluePrev) > 90) { // bright areas
					targetPos.x = index % 400;
					targetPos.y = index / 400;
					mouse_event(MOUSEEVENTF_MOVE, targetPos.x - 200, targetPos.y - 200 + 1, 0, 0); // x and y are deltas, not abs coordinates
					break;
				}
				redPrev = red;
				greenPrev = green;
				bluePrev = blue;
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
