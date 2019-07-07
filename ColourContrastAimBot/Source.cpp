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

int getIndex(int x, int y) {
	int index = y * 400 + x;
	return index;
}

bool Aim() {
	POINT a, b;
	a.x = 760;
	a.y = 340;
	b.x = 1160;
	b.y = 740;
	RGBQUAD * pixels = new RGBQUAD[160000];
	POINT targetPos;

	int blue = 0, bluePrev;
	int tolerance = 20;
	double radius, angle, incFactor;
	incFactor = 30;
	int x, y, index; // scan coordinates

	int targetHeight, targetLeft, targetRight;
	
	while (true) {
		angle = 2 * 3.1415 / incFactor;
		radius = 3;
		if ((GetKeyState(VK_RBUTTON) & 0x100) != 0) { // while rmb pressed

			pixels = capture(a, b);
			for (int i = 0; i < 160000; i++) {
				x = radius * cos(angle) + 200;
				y = radius * sin(angle) + 200;
				angle += 2 * 3.1415 / incFactor;
				radius += 1 / incFactor;

				if (x < 46 || x > 353 || y < 46 || y > 353) {
					//cout << "OUT OF BOUNDS  " << x << "  " << y << endl;
					break;
				}
				index = getIndex(x, y);
				targetHeight = -1;
				targetLeft = -1;
				targetRight = -1;
				bluePrev = (int)pixels[getIndex(x, y + 9)].rgbBlue;
				for (int j = y + 10; j < 30; j+=2) { // top scan
					blue = (int)pixels[getIndex(x, j)].rgbBlue;
					if (abs(blue - bluePrev) > tolerance) {
						targetHeight = j - y;
						break;
					}
					bluePrev = blue;
				}
				//cout << blue << " bluePrev: " << bluePrev << " h: " << targetHeight << endl;

				bluePrev = (int)pixels[getIndex(x - 9, y)].rgbBlue;
				for (int k = x - 10; k > -30; k-=2) { // top scan
					blue = (int)pixels[getIndex(k, y)].rgbBlue;
					if (abs(blue - bluePrev) > tolerance) {
						targetLeft = x - k;
						break;
					}
					bluePrev = blue;
				}
				//cout << blue << " bluePrev: " << bluePrev << " l: " << targetLeft << endl;

				bluePrev = (int)pixels[getIndex(x + 9, y)].rgbBlue;
				for (int l = y + 10; l < 30; l+=2) { // top scan
					blue = (int)pixels[getIndex(l, y)].rgbBlue;
					if (abs(blue - bluePrev) > tolerance) {
						targetRight = l - x;
						break;
					}
					bluePrev = blue;
				}
				//cout << blue << " bluePrev: " << bluePrev << " r: " << targetRight << endl;

				if (abs((targetRight - targetLeft) - targetHeight) < 5 && targetHeight != -1) {
					targetPos.x = index % 400;
					targetPos.y = index / 400;
					mouse_event(MOUSEEVENTF_MOVE, targetPos.x - 200, targetPos.y - 200, 0, 0); // x and y are deltas, not abs coordinates
					break;
				}

				// scan distance will be 40
				// top scan first, then right - left must equal top - center +/- 10
			}
			delete[] pixels;
		}
		Sleep(50); // extra buffer time
	}
	return true;
}

int main() {
	Aim();
	return 0;
}
