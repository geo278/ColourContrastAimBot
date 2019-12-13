// SmartAimBot.cpp : Defines the entry point for the application.

//#include "scan.h"
#include "iostream"
#include "Windows.h"
#include "vector"

using namespace std;

POINT a, b; // top left and bottom right corners
int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);
const int width = 280;
const int height = 280;

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

void shoot() {
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0); // start left click
	Sleep(10);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0); // finish Left click
	Sleep(10);
}

bool checkColour(RGBQUAD sample, vector<RGBQUAD> targets) {
	bool result = false;
	int tolerance = 12;
	int sampleRed = (int)sample.rgbRed;
	int sampleGreen = (int)sample.rgbGreen;
	int sampleBlue = (int)sample.rgbBlue;
	int targetRed, targetGreen, targetBlue;
	for (int i = 0; i < targets.size(); i++) {
		targetRed = targets[i].rgbRed;
		targetGreen = targets[i].rgbGreen;
		targetBlue = targets[i].rgbBlue;
		if (abs(sampleRed - targetRed) < tolerance && 
			abs(sampleGreen - targetGreen) < tolerance &&
			abs(sampleBlue - targetBlue) < tolerance * 2 / 3 ) {
			result = true;
			break;
		}
	}
	return result;
}

void Aim() {
	RGBQUAD * pixels;
	POINT targetPos; // centered at top left corner of capture zone

	vector<RGBQUAD> targets = {};
	RGBQUAD target;
	target.rgbRed = 145, target.rgbGreen = 162, target.rgbBlue = 47;
	targets.push_back(target);
	target.rgbRed = 150, target.rgbGreen = 155, target.rgbBlue = 60;
	targets.push_back(target);
	target.rgbRed = 173, target.rgbGreen = 177, target.rgbBlue = 60;
	targets.push_back(target);
	target.rgbRed = 189, target.rgbGreen = 198,	target.rgbBlue = 74;
	targets.push_back(target);
	target.rgbRed = 197, target.rgbGreen = 203, target.rgbBlue = 161;
	targets.push_back(target);
	target.rgbRed = 194, target.rgbGreen = 193, target.rgbBlue = 141;
	targets.push_back(target);
	target.rgbRed = 181, target.rgbGreen = 189, target.rgbBlue = 123;
	targets.push_back(target);

	// 173 167 130

	// bright colour thermals
// int sampleR = 255, sampleG = 170, sampleB = 80; // nc infravision
// int sampleR = 255, sampleG = 145, sampleB = 57; // vs infravision
// int sampleR = 180, sampleG = 160, sampleB = 130; // corona ir values
// int sampleR = 145, sampleG = 145, sampleB = 145; // tr irnv values
// int targetR = 250, targetG = 250, targetB = 250; // bf4 2x irnv
	
	const int sampleCount = 16, reboundMax = 3;
	double radius, angle;
	int reboundCount = 0;
	int x, y, index, xAdjust, yAdjust; // 0 indexed from top left
	int red, green, blue;

	bool targetAcquired = false;
	//bool evadeCrosshairColour = false;
	// int brightest = 0;
	while (true) {
		// angle = 2 * 3.141592654 / 8;
		angle = 2 * 3.141592654 * 3 / 4;
		radius = 1;
		// if ((GetKeyState(VK_CONTROL) & 0x100) != 0 && !(GetKeyState(VK_CAPITAL) & 0x8000)) { // while CTRL pressed, shift not pressed
		if ((GetKeyState(VK_RBUTTON) & 0x100) != 0 && !(GetKeyState(VK_CAPITAL) & 0x8000)) { // while rmb pressed, shift not pressed
			pixels = capture(a, b);
			targetAcquired = false;
			//evadeCrosshairColour = false;
			for (int i = 0; i < sampleCount * width; i++) {
				x = (int)(radius * cos(angle) + width / 2);
				y = (int)(radius * sin(angle) + height / 2);
				if (i % sampleCount == 0) { // if ring is complete
					radius++; // increment radius
				}
				angle += 2 * 3.141592654 / sampleCount; // increment angle per iteration
				if (x < 0 || x > width - 1 || y < 0 || y > height - 1) { // boundary check
					break;
				}
				//if (radius > 45 && (abs(width / 2 - x) <= 2 || abs(height / 2 - y) <= 2)) { 
				//	evadeCrosshairColour = true;
				//}
				index = y * width + x; // get 1d array index
				//red = (int)pixels[index].rgbRed;
				//green = (int)pixels[index].rgbGreen;
				//blue = (int)pixels[index].rgbBlue;

				if (checkColour(pixels[index], targets)) {
					// brightest = red + green + blue;
					targetPos.x = index % width;
					targetPos.y = index / width;
					targetAcquired = true;
				}

				if (i % sampleCount == 0 && targetAcquired) { // if ring is complete and targetAcquired
					xAdjust = (targetPos.x - width / 2);
					yAdjust = (targetPos.y - height / 2);
					mouse_event(MOUSEEVENTF_MOVE, xAdjust, yAdjust, 0, 0); // x and y are deltas, not abs coordinates

					if ((GetKeyState(VK_CONTROL) & 0x100) != 0 && xAdjust < 3 && yAdjust < 3) {
						CreateThread(0, 0, (LPTHREAD_START_ROUTINE) shoot, 0, 0, 0);
					}
					break;
				}
			}
			delete[] pixels;
		}
		Sleep(1);
	}

}
void updateResolution() {
	while (1) {
		screenWidth = GetSystemMetrics(SM_CXSCREEN);
		screenHeight = GetSystemMetrics(SM_CYSCREEN);
		a.x = screenWidth / 2 - width / 2;
		a.y = screenHeight / 2 - height / 2;
		b.x = screenWidth / 2 + width / 2;
		b.y = screenHeight / 2 + height / 2;
		Sleep(10000);
	}
}

void passiveStrafe() {
	int strafeWidth = 140;
	INPUT _W_keyDown;
	_W_keyDown.type = INPUT_KEYBOARD;
	_W_keyDown.ki.wScan = MapVirtualKey(0x57, MAPVK_VK_TO_VSC); // hardware scan code
	_W_keyDown.ki.time = 0;
	_W_keyDown.ki.wVk = 0x57; // virtual-key code
	_W_keyDown.ki.dwExtraInfo = 0;
	_W_keyDown.ki.dwFlags = 0; // 0 for key down
	INPUT _W_keyUp = _W_keyDown;
	_W_keyUp.ki.dwFlags = KEYEVENTF_KEYUP;

	INPUT _S_keyDown = _W_keyDown;
	_S_keyDown.ki.wScan = MapVirtualKey(0x53, MAPVK_VK_TO_VSC); // hardware scan code
	_S_keyDown.ki.wVk = 0x53; // virtual-key code
	INPUT _S_keyUp = _S_keyDown;
	_S_keyUp.ki.dwFlags = KEYEVENTF_KEYUP;
	while (1) {
		if ((GetKeyState(VK_RBUTTON) & 0x100) != 0 && (GetKeyState(VK_CAPITAL) & 0x100) == 0) {
			for (int i = 0; i < 5; i++) {
				mouse_event(MOUSEEVENTF_MOVE, -500, 0, 0, 0); // x and y are deltas, not abs coordinates
				Sleep(8);
				mouse_event(MOUSEEVENTF_MOVE, 1000, 0, 0, 0); // x and y are deltas, not abs coordinates
				Sleep(8);
				mouse_event(MOUSEEVENTF_MOVE, -500, 0, 0, 0); // x and y are deltas, not abs coordinates
			}
			while ((GetKeyState(VK_RBUTTON) & 0x100) != 0 && (GetKeyState(VK_CAPITAL) & 0x100) == 0) {
				Sleep(300);
			}
		}
	}
}

int main() {
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE) updateResolution, 0, 0, 0);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE) passiveStrafe, 0, 0, 0);
	Aim();
	return 0;
}
