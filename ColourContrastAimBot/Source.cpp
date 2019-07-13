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
	POINT targetPos; // centered at top left corner of capture zone

	double radius, angle;
	int x, y, index; // centered at center of screen
	//int xp, yp;

	int red, green, blue;
	//int redPrev, greenPrev, bluePrev;
	//int sampleR, sampleG, sampleB;
	bool sampled = false;
	int brightest = 0;

	while (true) {
		angle = 2 * 3.141592654 / 8;
		radius = 2;
		if ((GetKeyState(VK_RBUTTON) & 0x100) != 0 && !(GetKeyState(VK_SHIFT) & 0x8000)) { // while rmb pressed
			pixels = capture(a, b);
			sampled = false;

			for (int i = 0; i < 40000; i++) {
				

				x = (int)(radius * cos(angle) + 200);
				y = (int)(radius * sin(angle) + 200);
				radius = radius + 0.1;
				angle += 1 / radius;
				index = y * 400 + x;

				if (x < 0 || x > 399 || y < 0 || y > 399) {
					//cout << "OUT OF BOUNDS  " << x << "  " << y << endl;
					break;
				}
				index = y * 400 + x;

				red = (int)pixels[index].rgbRed;
				green = (int)pixels[index].rgbGreen;
				blue = (int)pixels[index].rgbBlue;
				
				if ((abs(red - 245) < 40 && abs(green - 160) < 40 && abs(blue - 70) < 40) ) {
					brightest = red + green + blue;
					targetPos.x = index % 400;
					targetPos.y = index / 400;
					sampled = true;
				}

				//if (i == 0) {
				//	redPrev = red;
				//	greenPrev = green;
				//	bluePrev = blue;
				//}
				
				//cout << angle / 2 / 3.1415 << " " << radius << endl;
				//cout << red << " " << green << " " << blue << endl;
				//cout << abs(red - sampleR) << " " << abs(green - sampleG) << " " << abs(blue - sampleB) << endl;
				//cout << " " << endl;

				if (i%16 == 0 && sampled) { // bright areas
					//targetPos.x = index % 400;
					//targetPos.y = index / 400;
					mouse_event(MOUSEEVENTF_MOVE, (targetPos.x - 200), (targetPos.y - 199), 0, 0); // x and y are deltas, not abs coordinates
					//cout << "BREAK" << endl;
					break;
				}
				
				//redPrev = red;
				//greenPrev = green;
				//bluePrev = blue;
				//xp = x;
				//yp = y;
				
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
