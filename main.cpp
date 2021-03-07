#include <iostream>
#include <windows.h>
#include <chrono>

HWND hwnd = GetConsoleWindow();
RECT consoleRect;
POINT cursorPos;

bool flag = false;

void wait_for_click() {
	while (true) {
		if (GetForegroundWindow() != hwnd) // Make sure we have this window selected
			continue;

		GetCursorPos(&cursorPos); // Retrieves the position of the mouse cursor, in screen coordinates.
		ScreenToClient(hwnd, &cursorPos); // converts the screen coordinates of a specified point on the screen to client-area coordinates.
		GetWindowRect(hwnd, &consoleRect); // Retrieves the dimensions of the bounding rectangle of the specified window.
		int width = (consoleRect.right - consoleRect.left) - 50; // It's kind of outside the window so we shrink it by 50 pixels
		int height = (consoleRect.bottom - consoleRect.top) - 50;
		if (cursorPos.x >= width || cursorPos.x <= 0 || cursorPos.y >= height || cursorPos.y <= 0) // Skip if the cursor is outside the bounds
			continue;

		// "flag" is so we can't hold the mouse button down
		if (GetAsyncKeyState(VK_LBUTTON) < 0 && !flag) {
			flag = true;
			return;
		}
		else if (GetAsyncKeyState(VK_LBUTTON) == 0)
			flag = false;
	}
} //wait_for_click

int main()
{
#pragma region Window Stuff
	// Change console font size https://stackoverflow.com/a/35383318/9286324
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = 0;                   // Width of each character in the font
	cfi.dwFontSize.Y = 22;                  // Height
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	wcscpy_s(cfi.FaceName, L"Consolas"); // Choose your font
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);

	// Set console size to 1024x768 https://www.daniweb.com/posts/jump/1321383
	GetWindowRect(hwnd, &consoleRect);
	MoveWindow(hwnd, consoleRect.left, consoleRect.top, 1024, 768, TRUE);

	// Disable cursor https://stackoverflow.com/a/18028927/9286324
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO     cursorInfo;
	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(out, &cursorInfo);

	// Disable resizing https://stackoverflow.com/a/47359526/9286324
	SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);

	// Disable "Quick Edit" mode https://stackoverflow.com/a/55498324/9286324
	HANDLE hInput;
	DWORD prev_mode;
	hInput = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleMode(hInput, &prev_mode);
	SetConsoleMode(hInput, prev_mode & ENABLE_EXTENDED_FLAGS | (prev_mode & ~ENABLE_QUICK_EDIT_MODE));
#pragma endregion Window Stuff

	while (true) { // main loop
		std::cout << "Welcome to the reaction time test!\nLeft click to begin, and left click again fast as you can when the console flashes green." << std::endl;

		wait_for_click();

		system("cls");

		int sum = 0, count[5];
		for (int i = 0; i < 5; i++) { // benchmark loop
			system("color 47"); // set red background

			Sleep(rand() % 2000 + 3000); // Sleep between 2 to 5 seconds

			system("color 27"); // set green background

			std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

			wait_for_click();

			int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count();

			system("color 07"); // set default background
			system("cls");

			count[i] = elapsed;

			if (i != 4)
				std::cout << elapsed << "ms\n" << i + 1 << "/5 completed. (Left click when ready)" << std::endl;
			else {
				std::cout << "Your times were:" << std::endl;
				for (int j = 0; j < 5; j++) {
					sum += count[j];
					std::cout << '[' << j + 1 << "] " << count[j] << "ms" << std::endl;
				}
				std::cout << "Average time: " << sum / 5 << "ms\nLeft click to go again!" << std::endl;
			}

			wait_for_click();

			system("cls");
		} // benchmark loop

	} // main loop
	return 0;
}