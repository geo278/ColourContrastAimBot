// SmartAimBot.cpp : Defines the entry point for the application.

//#include "scan.h"
#include "iostream"
#include "Windows.h"
#include "vector"

using namespace std;

POINT a, b; // top left and bottom right corners
int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);
const int width = 400;
const int height = 200;
HWND hWndTarget = NULL;

//bool bloodHoundUltActive = false;

RGBQUAD * capture(POINT a, POINT b) {

	if (hWndTarget == NULL) {
		cout << "FindWindow failed";
	}

	// copy screen to bitmap
	HDC     hScreen = GetDC(hWndTarget);

	if (hScreen == NULL) {
		cout << "GetDC failed";
	}

	HDC     hDC = CreateCompatibleDC(hScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, abs(b.x - a.x), abs(b.y - a.y));
	HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
	BOOL    bRet = BitBlt(hDC, 0, 0, abs(b.x - a.x), abs(b.y - a.y), hScreen, a.x, a.y, SRCCOPY); // BitBlt does the copying



	// // save bitmap to clipboard
	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, hBitmap);
	CloseClipboard();


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

// void shoot() {
// 	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0); // start left click
// 	Sleep(10);
// 	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0); // finish Left click
// 	Sleep(10);
// }

bool checkColour(RGBQUAD sample, vector<RGBQUAD> targets) {
	bool result = false;
	int tolerance = 18;
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
			abs(sampleBlue - targetBlue) < tolerance ) {
			result = true;
			break;
		}
	}
	return result;
}
bool checkColour0(RGBQUAD sample) {
	bool result = false;
	int tolerance = 18;
	int sampleRed = (int)sample.rgbRed;
	int sampleGreen = (int)sample.rgbGreen;
	int sampleBlue = (int)sample.rgbBlue;

	if (sampleRed > 220 &&
		sampleGreen > 200 &&
		sampleBlue > 150 && sampleBlue < 210) {
		result = true;
		cout << "Hello World! asdf";
	}
	return result;
}

void Aim() {
	RGBQUAD * pixels;
	POINT targetPos; // centered at top left corner of capture zone

	vector<RGBQUAD> targets = {};
	RGBQUAD target;
	target.rgbRed = 161, target.rgbGreen = 22, target.rgbBlue = 42;
	targets.push_back(target);
	target.rgbRed = 165, target.rgbGreen = 19, target.rgbBlue = 19;
	targets.push_back(target);
	target.rgbRed = 144, target.rgbGreen = 35, target.rgbBlue = 39;
	targets.push_back(target);
	target.rgbRed = 122, target.rgbGreen = 35, target.rgbBlue = 32;
	targets.push_back(target);

	// R6 Glaz
	//target.rgbRed = 145, target.rgbGreen = 162, target.rgbBlue = 47;
	//targets.push_back(target);
	//target.rgbRed = 150, target.rgbGreen = 155, target.rgbBlue = 60;
	//targets.push_back(target);
	//target.rgbRed = 173, target.rgbGreen = 177, target.rgbBlue = 60;
	//targets.push_back(target);
	//target.rgbRed = 189, target.rgbGreen = 198,	target.rgbBlue = 74;
	//targets.push_back(target);
	//target.rgbRed = 197, target.rgbGreen = 203, target.rgbBlue = 161;
	//targets.push_back(target);
	//target.rgbRed = 194, target.rgbGreen = 193, target.rgbBlue = 141;
	//targets.push_back(target);
	//target.rgbRed = 181, target.rgbGreen = 189, target.rgbBlue = 123;
	//targets.push_back(target);
	//target.rgbRed = 149, target.rgbGreen = 149, target.rgbBlue = 52;
	//targets.push_back(target);
	//target.rgbRed = 191, target.rgbGreen = 206, target.rgbBlue = 46;
	//targets.push_back(target);
	//target.rgbRed = 153, target.rgbGreen = 170, target.rgbBlue = 88;
	//targets.push_back(target);
	//target.rgbRed = 180, target.rgbGreen = 197, target.rgbBlue = 101;
	//targets.push_back(target);


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
	// int red, green, blue;

	bool targetAcquired = false;
	//bool evadeCrosshairColour = false;
	// int brightest = 0;
	while (true) {
		// angle = 2 * 3.141592654 / 8;
		angle = 2 * 3.141592654 * 3 / 4;
		radius = 1;
		// if ((GetKeyState(VK_CONTROL) & 0x100) != 0 && !(GetKeyState(VK_CAPITAL) & 0x8000)) { // while CTRL pressed, shift not pressed
		if ((GetKeyState(VK_RBUTTON) & 0x100) != 0 ) { // while rmb pressed

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

				if (checkColour0(pixels[index])) {
					targetPos.x = index % width;
					targetPos.y = index / width;
					targetAcquired = true;
				}

				if (i % sampleCount == 0 && targetAcquired) { // if ring is complete and targetAcquired
					xAdjust = (targetPos.x - width / 2);
					yAdjust = (targetPos.y - height / 2);
					mouse_event(MOUSEEVENTF_MOVE, xAdjust, yAdjust, 0, 0); // x and y are deltas, not abs coordinates

					// if ((GetKeyState(VK_CONTROL) & 0x100) != 0 && xAdjust < 3 && yAdjust < 3) {
					// 	CreateThread(0, 0, (LPTHREAD_START_ROUTINE) shoot, 0, 0, 0);
					// }
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

// void passiveStrafe() {
// 	int strafeWidth = 140;
// 	INPUT _W_keyDown;
// 	_W_keyDown.type = INPUT_KEYBOARD;
// 	_W_keyDown.ki.wScan = MapVirtualKey(0x57, MAPVK_VK_TO_VSC); // hardware scan code
// 	_W_keyDown.ki.time = 0;
// 	_W_keyDown.ki.wVk = 0x57; // virtual-key code
// 	_W_keyDown.ki.dwExtraInfo = 0;
// 	_W_keyDown.ki.dwFlags = 0; // 0 for key down
// 	INPUT _W_keyUp = _W_keyDown;
// 	_W_keyUp.ki.dwFlags = KEYEVENTF_KEYUP;

// 	INPUT _S_keyDown = _W_keyDown;
// 	_S_keyDown.ki.wScan = MapVirtualKey(0x53, MAPVK_VK_TO_VSC); // hardware scan code
// 	_S_keyDown.ki.wVk = 0x53; // virtual-key code
// 	INPUT _S_keyUp = _S_keyDown;
// 	_S_keyUp.ki.dwFlags = KEYEVENTF_KEYUP;
// 	while (1) {
// 		if ((GetKeyState(VK_RBUTTON) & 0x100) != 0 && (GetKeyState(VK_CONTROL) & 0x100) == 0) {
// 			for (int i = 0; i < 5; i++) {
// 				mouse_event(MOUSEEVENTF_MOVE, -500, 0, 0, 0); // x and y are deltas, not abs coordinates
// 				Sleep(8);
// 				mouse_event(MOUSEEVENTF_MOVE, 1000, 0, 0, 0); // x and y are deltas, not abs coordinates
// 				Sleep(8);
// 				mouse_event(MOUSEEVENTF_MOVE, -500, 0, 0, 0); // x and y are deltas, not abs coordinates
// 			}
// 			while ((GetKeyState(VK_RBUTTON) & 0x100) != 0 && (GetKeyState(VK_CONTROL) & 0x100) == 0) {
// 				Sleep(300);
// 			}
// 		}
// 	}
// }

// void trackBloodHoundUltActive() {
// 	while (true) {
// 		if ((GetKeyState(0x5A) & 0x100) != 0) {
// 			bloodHoundUltActive = true;
// 			Sleep(35000);
// 			bloodHoundUltActive = false;
// 		}
// 		Sleep(5);
// 	}
// }

// Callback function. It will be called by EnumWindows function as many times as there are windows
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	if (IsWindowVisible(hwnd)) // check whether window is visible
	{
		char wnd_title[256];
		GetWindowText(hwnd, wnd_title, sizeof(wnd_title));
		cout << wnd_title << endl;

		string str(wnd_title);
		string substr("Visual Studio Code");
		if (str.find(substr) != std::string::npos) {
			hWndTarget = hwnd;
			cout << "CHECK VALUE: " << wnd_title << endl;
		}
	}
	return true; // function must return true if you want to continue enumeration
}

int main() {
	EnumWindows(EnumWindowsProc, 0);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE) updateResolution, 0, 0, 0);
	//CreateThread(0, 0, (LPTHREAD_START_ROUTINE) passiveStrafe, 0, 0, 0);
	Aim();
	return 0;
}





