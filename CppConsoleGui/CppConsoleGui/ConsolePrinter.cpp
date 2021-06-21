/**
 * @author Marley Arns
 * @version 1.0
 * @github: github.com/ExecutableMarley
 */

#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>
//#include <stdlib.h>
#include <thread>   //Threads

#include "ConsolePrinter.h"

/*
Todo:
-multiple page support                         //Somewhat done but nothing here to actually switch it
-color support
-page inside element support (enter key and back key required) //Done
-event handling on elements                    //Done
-register funtions on console thread           //Done
-return pointer when elements are created      //Done
-log multi line element                        //Done but maybe refactor a bit
-multi line select element
*/

namespace newConsolePrinter
{
	using namespace std;


	//[ButtonElement]

	void ButtonElement::print(ConsolePrinter *printer, bool isSelected)
	{
		this->isSelected = isSelected;

		string curLine = stateArrow(isSelected) + name;

		/*while (curLine.length() < printer->iWidth)
			curLine.append(" ");*/

		curLine.resize(printer->iWidth - 1);

		cout << curLine << "\n";
	}



	//[BoolElement]

	void BoolElement::print(ConsolePrinter *printer, bool isSelected)
	{
		this->isSelected = isSelected;

		this->oldValue = *value;

		string curLine = stateArrow(isSelected) + name + " = " + stateChar(*value);

		/*while (curLine.length() < printer->iWidth - 1)
			curLine.append(" ");*/

		curLine.resize(printer->iWidth - 1);

		curLine.append("\n");

		cout << curLine;

		//cout << stateArrow(isSelected) << name << " = " << stateChar(*value) << "                          \n";
	}

	//[IntElement]

	void IntElement::print(ConsolePrinter *printer, bool isSelected)
	{
		this->isSelected = isSelected;

		this->oldValue = *value;


		

		char *formatedChars = new char[printer->iWidth - 1];

		std::memset(formatedChars, ' ', printer->iWidth - 1);

		/*for (int i = 0; i < printer->iWidth - 1; i++)
		{
			formatedChars[i] = ' ';
		}*/

		sprintf_s(formatedChars, printer->iWidth - 1, "%s%s = % 2i  ", stateArrow(isSelected), name.c_str(), *value);

		cout << formatedChars << "\n";

		delete formatedChars;
	}

	void IntElement::changeValue(ConsolePrinter*printer, Action action)
	{
		switch (action)
		{
		case Increase: setValue(*value + printer->modifier(iStep)); return;
		case Decrease: setValue(*value - printer->modifier(iStep)); return;
		case Enter: if (onAction) onAction(this);
		}
	}

	//[FloatElement]

	void FloatElement::print(ConsolePrinter *printer, bool isSelected)
	{
		this->isSelected = isSelected;

		this->oldValue = *value;

		

		char *formatedChars = new char[printer->iWidth - 1];

		std::memset(formatedChars, ' ', printer->iWidth - 1);

		/*for (int i = 0; i < printer->iWidth - 1; i++)
		{
			formatedChars[i] = ' ';
		}*/

		sprintf_s(formatedChars, printer->iWidth - 1,"%s%s = % 3.2f", stateArrow(isSelected), name.c_str(), *value);

		cout << formatedChars << "\n";

		delete formatedChars;
	}

	//[ComboElement]

	void ComboElement::print(ConsolePrinter *printer, bool isSelected)
	{
		this->isSelected = isSelected;

		this->oldValue = *value;



		char *formatedChars = new char[printer->iWidth - 1];

		std::memset(formatedChars, ' ', printer->iWidth - 1);
		/*for (int i = 0; i < printer->iWidth - 1; i++)
		{
			formatedChars[i] = ' ';
		}*/

		sprintf_s(formatedChars, printer->iWidth - 1, "%s%s = %s", stateArrow(isSelected), name.c_str(), items[*value]);

		cout << formatedChars << "\n";
		//cout << stateArrow(isSelected) << name << " = " << items[*value] << "               \n";
		delete formatedChars;
	}

	//[Text]

	void TextElement::print(ConsolePrinter *printer, bool isSelected)
	{
		int width = printer->iWidth;

		string curLine = stateArrow(false);

		if (centralize)
		{
			int something = name.length() / 2;
			int ssss = width / 2;

			ssss -= 2;

			//curLine.replace(ssss - something, 10, name);

			for (int i = 0; i < ssss - something; i++)
				curLine.append(" ");
		}

		//string text(100, ' ');

		curLine.append(name);

		/*while (curLine.length() < width - 1)
			curLine.append(" ");*/

		curLine.resize(printer->iWidth - 1);

		curLine.append("\n");

		cout << curLine;
	}

	//[LogElement]

	void LogElement::print(ConsolePrinter* printer, bool isSelected)
	{
		hasChangedb = false;

		string curLine = stateArrow(false) + curLog;

		/*while (curLine.length() < printer->iWidth - 1)
			curLine.append(" ");*/

		curLine.resize(printer->iWidth - 1);

		curLine.append("\n");

		cout << curLine;
	}


	//[PageElement]

	void PageElement::print(ConsolePrinter *printer, bool isSelected)
	{
		this->isSelected = isSelected;

		//cout << stateArrow(isSelected) << name << "            \n";

		string curLine(printer->iWidth - 1, ' ');

		curLine.replace(0, 2, stateArrow(isSelected));

		curLine.replace(2, name.length(), name);

		cout << curLine << "\n";
	}

	void PageElement::activate(ConsolePrinter *printer)
	{
		if (targetPage)
			printer->loadPage(targetPage);
		else if (isReturnElem)
			printer->backPage();
	}




	void ConsolePage::printAll()
	{
		printer->setCursorPosition(0, 0);

		for (Element *e : elementList)
		{
			e->print(printer, e == currentElement);
		}
	}

	void ConsolePage::printChangedElements()
	{
		int line = 0;
		for (Element *e : elementList)
		{
			if (e->hasChanged(e == currentElement))
			{
				printer->setCursorPosition(0, line);
				e->print(printer, e == currentElement);
			}
			line++;
		}
	}

	template<typename ... Args>
	std::string string_format(const std::string& format, Args ... args)
	{
		int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
		if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
		size_t size = static_cast<size_t>(size_s);
		auto buf = std::make_unique<char[]>(size);
		std::snprintf(buf.get(), size, format.c_str(), args ...);
		return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
	}
}




//Old version super professional commented out
/*
struct element
{
	enum types
	{
		BOOL,
		INT,
		FLOAT,
		CHAR,
		TEXT,
		EMPTY
	};

	const char* stateChar(bool state)
	{
		return state ? "[ON] " : "[OFF]";
	}

	const char* stateArrow(bool state)
	{
		return state ? "->" : "  ";
	}

	element(string newName, uintptr_t newPointer, types newType, int newLineIndex)
	{
		name = newName;
		ptr = newPointer;
		myType = newType;
		lineIndex = newLineIndex;
	}
	element(string newName, types newType, int newLineIndex)
	{
		name = newName;
		//ptr = newPointer;
		myType = newType;
		lineIndex = newLineIndex;
	}
	element(string newName, bool  *newPointer,int newLineIndex)
	{
		name = newName;
		ptr = (uintptr_t)newPointer;
		myType = BOOL;
		lineIndex = newLineIndex;
	}
	element(string newName, int   *newPointer,int newLineIndex, int step)
	{
		name = newName;
		ptr = (uintptr_t)newPointer;
		myType = INT;
		lineIndex = newLineIndex;
		iStep = step;
	}
	element(string newName, float *newPointer,int newLineIndex, float step)
	{
		name = newName;
		ptr = (uintptr_t)newPointer;
		myType = FLOAT;
		lineIndex = newLineIndex;
		fStep = step;
	}
	element(string newName, int   *newPointer,char **items, int newLineIndex, int max)
	{
		name = newName;
		ptr = (uintptr_t)newPointer;
		this->items = items;
		myType = CHAR;
		lineIndex = newLineIndex;
		iStep = max;
	}

	void print(int curIndex)
	{
		//std::cout << stateArrow(curIndex == index) << name << " = ";

		if (myType == EMPTY)
		{
			cout << "\n";
		}
		if (myType == TEXT)
		{
			cout << stateArrow(false) << name << "\n";
		}
		if (myType == INT)
		{
			cout << stateArrow(curIndex == lineIndex) << name << " = " << *((int*)ptr) << "             \n";
		}
		if (myType == FLOAT)
		{
			cout << stateArrow(curIndex == lineIndex) << name << " = " << *((float*)ptr) << "                    \n";
		}
		if (myType == BOOL)
		{
			cout << stateArrow(curIndex == lineIndex) << name << " = " << stateChar(*((bool*)ptr)) << "               \n";
		}
		if (myType == CHAR)
		{
			cout << stateArrow(curIndex == lineIndex) << name << " = " << (items)[*(int*)ptr]  << "               \n";
		}
	}
	void changeValue(bool positive)
	{
		if (myType == INT)
		{
			if (positive && *(int*)ptr < INT_MAX)
				*(int*)ptr += iStep;
			else if (!positive && *(int*)ptr > INT_MIN)
				*(int*)ptr -= iStep;
		}
		if (myType == FLOAT)
		{
			if (positive && *(float*)ptr < FLT_MAX)
				*(float*)ptr += fStep;
			else if (!positive && *(float*)ptr > -1000.f)
				*(float*)ptr -= fStep;

			if (*(float*)ptr < 0.001 && *(float*)ptr > -0.001)
			{
				*(float*)ptr = 0.f;
			}
		}
		if (myType == BOOL)
		{
			*(bool*)ptr = !*(bool*)ptr;
		}
		if (myType == CHAR)
		{
			if (positive && *(int*)ptr < iStep - 1)
				*(int*)ptr += 1;
			else if (!positive && *(int*)ptr > 0)
				*(int*)ptr -= 1;
		}
	}

	union
	{
		int iStep;
		float fStep;
	};

	types myType;
	string name;
	int lineIndex; //used to find the y position of the line
	uintptr_t ptr;

	char **items;

	int color;

	int next = -1;
	int prev = -1;
};


class ConsolePrinter
{
	int bgColor;
	int textColor;

	int lineMaxIndex = 0;
	int curLineIndex = 0;
	int lastLineIndex = 0;
	int lastElemenIndex = -1;

	//bool isWindowVisible;

	HWND consoleHwnd;
	HANDLE consoleHandle;

	std::vector<element> elemenList;

	Hotkeys key;

public:
	ConsolePrinter(int width, int height, Hotkeys newKeys = Hotkeys())
	{
		//Try to get console handle if one exists
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

		resizeWindow(width, height);

		key = newKeys;
	}

	bool resizeWindow(int width, int height)
	{
		RECT r;
		GetWindowRect(consoleHwnd, &r);

		return MoveWindow(consoleHwnd, r.left, r.top, width, height, TRUE);
	}
	bool moveWindow(int x, int y)
	{
		RECT r;
		GetWindowRect(consoleHwnd, &r);

		return MoveWindow(consoleHwnd,x, y, r.right - r.left, r.bottom - r.top, TRUE);
	}

	bool toggleWindow(bool state)
	{
		if (state)
			return ShowWindow(consoleHwnd, SW_SHOW);
		else
			return ShowWindow(consoleHwnd, SW_HIDE);
	}

	void evenStringSize()
	{
		int maxSize = 0;

		for (auto i = elemenList.begin(); i != elemenList.end();i++)
		{
			if (maxSize < (*i).name.size() && (*i).myType != element::types::TEXT)
				maxSize = (*i).name.size();
		}

		for (auto i = elemenList.begin(); i != elemenList.end();i++)
		{
			while (maxSize > (*i).name.size())
				(*i).name.append(" ");
		}
	}

	void addTextElement(string name)
	{
		element e(name, 0, element::types::TEXT,lineMaxIndex);
		elemenList.push_back(e);

		lineMaxIndex++;
	}
	void EmptyLine()
	{
		element e("\n", 0, element::types::EMPTY,lineMaxIndex);
		elemenList.push_back(e);

		lineMaxIndex++;
	}
	void addBoolElement(string name, bool *newBool)
	{
		element e(name, (uintptr_t)newBool, element::types::BOOL,lineMaxIndex);

		//set the next of the previous to current
		if (lastElemenIndex > -1) //But only if its not the first element
		elemenList[lastElemenIndex].next = lineMaxIndex;
		//set the pre of the current
		e.prev = lastElemenIndex;
		//update index
		lastElemenIndex = lineMaxIndex;

		elemenList.push_back(e);

		if (curLineIndex == 0)
			curLineIndex = lineMaxIndex;

		//lasAddedIndex++;
		lineMaxIndex++;
	}
	void Int(string name, int *newInt, int step)
	{
		element e(name, newInt, lineMaxIndex, step);

		//set the next of the previous to current
		if (lastElemenIndex > -1) //But only if its not the first element
		elemenList[lastElemenIndex].next = lineMaxIndex;
		//set the pre of the current
		e.prev = lastElemenIndex;
		//update index
		lastElemenIndex = lineMaxIndex;

		elemenList.push_back(e);

		if (curLineIndex == 0)
			curLineIndex = lineMaxIndex;

		//lasAddedIndex++;
		lineMaxIndex++;
	}
	void Float(string name, float *newFloat, float step)
	{
		element e(name, newFloat, lineMaxIndex, step);

		//set the next of the previous to current
		if (lastElemenIndex > -1) //But only if its not the first element
		elemenList[lastElemenIndex].next = lineMaxIndex;
		//set the pre of the current
		e.prev = lastElemenIndex;
		//update index
		lastElemenIndex = lineMaxIndex;

		elemenList.push_back(e);

		if (curLineIndex == 0)
			curLineIndex = lineMaxIndex;

		//lasAddedIndex++;
		lineMaxIndex++;
	}
	void Combo(string name, int *newInt, char** items, int step)
	{
		element e(name, newInt,items, lineMaxIndex, step);

		//set the next of the previous to current
		if (lastElemenIndex > -1) //But only if its not the first element
			elemenList[lastElemenIndex].next = lineMaxIndex;
		//set the pre of the current
		e.prev = lastElemenIndex;
		//update index
		lastElemenIndex = lineMaxIndex;

		elemenList.push_back(e);

		if (curLineIndex == 0)
			curLineIndex = lineMaxIndex;

		//lasAddedIndex++;
		lineMaxIndex++;
	}

	void setCursorPosition(int x, int y)
	{
		COORD pos = { x, y };
		SetConsoleCursorPosition(consoleHandle, pos);
	}

	void print()
	{
		setCursorPosition(0, 0);

		for (auto i = elemenList.begin(); i != elemenList.end();i++)
		{
			(*i).print(curLineIndex);
		}
	}
	bool checkHotkey(int hotkey)
	{
		if (hotkey)
			return GetAsyncKeyState(hotkey);
		return false;
	}

	void navigation()
	{

		if (checkHotkey(key.toggleVisible))
		{
			WINDOWPLACEMENT wnd;

			GetWindowPlacement(consoleHwnd, &wnd);

			if (IsWindowVisible(consoleHwnd))
				toggleWindow(false);
			else
				toggleWindow(true);
		}


		if (GetForegroundWindow() != consoleHwnd)
		{
			return;
		}

		if (GetAsyncKeyState(key.up))
		{
			if (elemenList[curLineIndex].prev != -1)
			{
				curLineIndex = elemenList[curLineIndex].prev;
			}

		}
		if (GetAsyncKeyState(key.down))
		{
			if (elemenList[curLineIndex].next != -1)
			{
				curLineIndex = elemenList[curLineIndex].next;
			}

		}
		if (GetAsyncKeyState(key.left))
		{
			elemenList[curLineIndex].changeValue(false);
			print();
		}
		if (GetAsyncKeyState(key.right))
		{
			elemenList[curLineIndex].changeValue(true);
			print();
		}

		if (curLineIndex != lastLineIndex)
		{
			print();

			lastLineIndex = curLineIndex;
		}
	}
};*/

