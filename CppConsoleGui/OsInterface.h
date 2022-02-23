#pragma once

#include <string>

/**
 * @author Marley Arns
 * @version 1.0
 * @github: github.com/ExecutableMarley
 */

//Currently not used and hardcoded to run on windows


class ConsoleOsTemplate
{
public:
	//Resize window to x y
	virtual bool resizeWindow(int width, int height) = 0;

	//Moves the topleft corner to x y
	virtual bool moveWindow(int x, int y) = 0;

	//Shows window on true and hides window on false
	virtual bool showWindow(bool state) = 0;

	//Sets the console window title
	virtual bool setConsoleTitle(std::string text) = 0;

	//Moves the cursor to x y
	virtual void setCursorPosition(int x, int y) = 0;

	//Sets the console text and background color
	virtual void setConsoleColor(int textColor, int backgroundColor) = 0;

	//True shows console cursor, false hides console cursor
	virtual void setCursorVisibility(bool b) = 0;

	//Returns if the specific key is pressed
	virtual bool checkHotkey(int hotkey) = 0;

	//Returns if our window is in the foreground 
	virtual bool windowInForeground() = 0;

	//Returns if our window is visible
	virtual bool isWindowVisible() = 0;
};

/*
Everything os dependent inside here
*/

//Windows implemtation
#if defined(_WIN32) || defined(_WIN64) //Checks for windows 32 and 64 bit

#include <Windows.h>

class ConsoleOsInterface : public ConsoleOsTemplate
{
private:
	//Handle to console
	HWND consoleHwnd;
	//Handle to console window
	HANDLE consoleHandle;

public:

	//Creates an console window which we will use
	ConsoleOsInterface()
	{
		//Try to get console window handle if one exists
		consoleHwnd = GetConsoleWindow();

		//Check if thread has console
		if (!consoleHwnd)
		{   //Try to create console
			if (!AllocConsole())
			{
				//Failed to create console
			}
			consoleHwnd = GetConsoleWindow();
		}

		consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		// Redirect the CRT standard input, output, and error handles to the console
		/*freopen("CONIN$" , "r", stdin);
		freopen("CONOUT$", "w", stderr);
		freopen("CONOUT$", "w", stdout);*/
	}

	//Resize window to x y
	bool resizeWindow(int width, int height)
	{
		RECT r;
		GetWindowRect(consoleHwnd, &r);

		return MoveWindow(consoleHwnd, r.left, r.top, width, height, TRUE);
	}

	//Moves the topleft corner to x y
	bool moveWindow(int x, int y)
	{
		RECT r;
		GetWindowRect(consoleHwnd, &r);

		return MoveWindow(consoleHwnd, x, y, r.right - r.left, r.bottom - r.top, TRUE);
	}

	//Shows window on true and hides window on false
	bool showWindow(bool state)
	{
		if (state)
			return ShowWindow(consoleHwnd, SW_SHOW);
		else
			return ShowWindow(consoleHwnd, SW_HIDE);
	}

	//Sets the console window title
	bool setConsoleTitle(std::string text)
	{
		return SetConsoleTitleA(text.c_str());
	}

	//Moves the cursor to x y
	void setCursorPosition(int x, int y)
	{
		COORD pos = { x, y };
		SetConsoleCursorPosition(consoleHandle, pos);
	}

	//Sets the console text and background color
	void setConsoleColor(int textColor, int backgroundColor)
	{
		SetConsoleTextAttribute(consoleHandle, (textColor + (backgroundColor * 16)));
	}

	//True shows console cursor, false hides console cursor
	void setCursorVisibility(bool b)
	{
		CONSOLE_CURSOR_INFO     cursorInfo;
		GetConsoleCursorInfo(consoleHandle, &cursorInfo);
		cursorInfo.bVisible = b; // set the cursor visibility
		SetConsoleCursorInfo(consoleHandle, &cursorInfo);
	}

	//Returns if the specific key is pressed
	bool checkHotkey(int hotkey)
	{
		if (hotkey)
			return GetAsyncKeyState(hotkey);
		return false;
	}

	//Returns if our window is in the foreground 
	bool windowInForeground()
	{
		return GetForegroundWindow() == consoleHwnd;
	}

	//Returns if our window is visible
	bool isWindowVisible()
	{
		return IsWindowVisible(consoleHwnd);
	}


	void getFontSize(int &iFontWidth, int &iFontHeight)
	{
		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(cfi);

		if (GetCurrentConsoleFontEx(consoleHandle, false, &cfi))
		{
			iFontWidth = cfi.dwFontSize.X;
			iFontHeight = cfi.dwFontSize.Y;
		}
		else
		{
			//Default console font size
			iFontWidth = 8;
			iFontHeight = 16;
		}
	}

	void getBorderSize(int &borderWidth, int &borderHeight)
	{
		RECT rcClient, rcWind;

		if (GetClientRect(consoleHwnd, &rcClient) &&
			GetWindowRect(consoleHwnd, &rcWind))
		{
			borderWidth = (rcWind.right - rcWind.left) - rcClient.right;
			borderHeight = (rcWind.bottom - rcWind.top) - rcClient.bottom;
		}
		else
		{
			//Default border size
			borderWidth = 33;
			borderHeight = 39;
		}
	}
};
#endif


#if defined (__APPLE__) || defined (__MACH__)
#error "IOS IS NOT SUPPORTED"
#endif

//Linux Implementation
#if defined (unix) || defined (__unix__) || defined (__unix)

class ConsoleOsInterface : public ConsoleOsTemplate
{
private:
	//Private Members

public:
	//Creates an console window which we will use
	ConsoleOsInterface()
	{
		//Dont forget to grab handles or whatever
	}

	//Resize window to x y
	bool resizeWindow(int width, int height)
	{

	}

	//Moves the topleft corner to x y
	bool moveWindow(int x, int y)
	{

	}

	//Shows window on true and hides window on false
	bool showWindow(bool state)
	{

	}

	//Sets the console window title
	bool setConsoleTitle(std::string text)
	{

	}

	//Moves the cursor to x y
	void setCursorPosition(int x, int y)
	{

	}

	//Sets the console text and background color
	void setConsoleColor(int textColor, int backgroundColor)
	{

	}

	//True shows console cursor, false hides console cursor
	void setCursorVisibility(bool b)
	{

	}

	//Returns if the specific key is pressed
	bool checkHotkey(int hotkey)
	{

	}

	//Returns if our window is in the foreground 
	bool windowInForeground()
	{

	}

	//Returns if our window is visible
	bool isWindowVisible()
	{

	}
};
#endif