#pragma once

/**
 * @author Marley Arns
 * @version 1.0
 * @github: github.com/ExecutableMarley
 */

#include <thread>

#include "TimeFuncs.h"

namespace newConsolePrinter
{
	using namespace std;

	enum Action
	{
		Increase,
		Decrease,
		Enter,
	};

	enum CColor
	{
		BLACK = 0,
		DARK_BLUE = 1,
		DARK_GREEN = 2,
		DARK_CYAN = 3,
		DARK_RED = 4,
		DARK_PURPLE = 5, dark_pink = 5, dark_magenta = 5,
		dark_yellow = 6,
		dark_white = 7,
		gray = 8,
		blue = 9,
		green = 10,
		aqua = 11, cyan = 11,
		red = 12,
		purple = 13, pink = 13, magenta = 13,
		yellow = 14,
		white = 15
	};

	struct Settings
	{
		int navInputDelay = 200;
		int conRefreshDelay = 200;
	};

	struct Hotkeys
	{
		int up = 0x26;
		int down = 0x28;
		int left = 0x25;
		int right = 0x27;
		int enter = 0x65;   //Numpad 5
		int lastPage = VK_NUMPAD0;
		int nextPage = 0;   //Unused
		int prevPage = 0;   //Unused
		int modifier5  = VK_CONTROL;
		int modifier10 = VK_MENU;
		int toggleVisible = 0x2D;  //VK_INSERT
		int exitApp = VK_END;
	};

	

	class ConsolePrinter;
	class ConsolePage;

	class Element
	{
		friend class ConsolePrinter;
		friend class ConsolePage;

	protected:
		Element *next;
		Element *prev;

		string name;

		int lineNumber;

		int color;

		const char* stateArrow(bool state)
		{
			return state ? "->" : "  ";
		}

		Element(Element* prev)
		{
			//Save the previous element 
			this->prev = prev;
			//Add this as the next to the last element
			if (prev)
				prev->addNext(this);
		}

		//Abstract print method
		void virtual print(ConsolePrinter*printer, bool isSelected) = 0;
		//
		bool virtual hasChanged(bool isSelected) = 0;
		//
		void virtual changeValue(ConsolePrinter*printer, Action action) = 0;

		//onValueChange()
		
		bool hasNext()
		{
			return next != NULL;
		}
		bool hasPrev()
		{
			return prev != NULL;
		}

		Element *getNext()
		{
			return next;
		}
		Element *getPrev()
		{
			return prev;
		}

		//Add next pointer
		void addNext(Element *next)
		{
			this->next = next;
		}
	};

	class ButtonElement : public Element
	{
		bool isSelected;

		std::function<void(ButtonElement*)> onAction;

	public:
		ButtonElement(string name, Element* previous) : Element(previous)
		{
			this->name = name;
		}

	protected:
		//Print function
		void print(ConsolePrinter *printer, bool isSelected);
		//
		void changeValue(ConsolePrinter*printer, Action action)
		{
			switch (action)
			{
			//case Increase:
			//case Decrease:setValue(!*value); break;
			case Enter: if (onAction) onAction(this);
			}
		}

		//Returns true if the value has changed since the last draw
		bool hasChanged(bool isSelected)
		{
			if (this->isSelected != isSelected)
				return true;

			return false;
		}
	public:
		void setOnAction(std::function<void(ButtonElement*)> function)
		{
			this->onAction = function;
		}

		void activate()
		{
			if (onAction)
				onAction(this);
		}
	};


	class BoolElement : public Element
	{
		const char* stateChar(bool state)
		{
			return state ? "[ON] " : "[OFF]";
		}

		//Used to check if the value has changed since the last draw
		bool oldValue;
		//Pointer to the value
		bool *value;

		bool isSelected = false;

		//Event Handling
		std::function<void(BoolElement*)> handler;

		std::function<void(BoolElement*)> onAction;

	public:
		//Constructor
		BoolElement(string name, bool *value, Element* previous) : Element(previous)
		{
			this->name = name;

			this->value = value;

			oldValue = *value;
		}

	protected:
		//Print function
		void print(ConsolePrinter *printer, bool isSelected);
		//Returns true if the value has changed since the last draw
		bool hasChanged(bool isSelected)
		{
			if (this->isSelected != isSelected)
				return true;

			return oldValue != *value;
		}
		//
		void changeValue(ConsolePrinter*printer, Action action)
		{
			switch (action)
			{
			case Increase: 
			case Decrease:setValue(!*value); break;
			case Enter: if (onAction) onAction(this);
			}
		}

	public:
		void setValue(bool value)
		{
			*this->value = value;

			if (handler)
				handler(this);
		}

		bool getValue()
		{
			return *this->value;
		}

		void setEventHandler(std::function<void(BoolElement*)> function)
		{
			this->handler = function;
		}
		void setOnAction(std::function<void(BoolElement*)> function)
		{
			this->onAction = function;
		}
	};


	class IntElement : public Element
	{
		int oldValue;

		int *value;

		int iStep;

		bool isSelected = false;

		int min = INT_MIN; 
		int max = INT_MAX;

		//Event Handling
		std::function<void(IntElement*)> handler;

		std::function<void(IntElement*)> onAction;

	public:
		//Constructor
		IntElement(string name, int *value, int iStep, Element* previous) : Element(previous)
		{
			this->name = name;

			this->value = value;

			this->iStep = iStep;

			oldValue = *value;
		}

	protected:
		//Print function
		void print(ConsolePrinter *printer, bool isSelected);
		//Returns true if the value has changed since the last draw
		bool hasChanged(bool isSelected)
		{
			if (this->isSelected != isSelected)
				return true;

			return oldValue != *value;
		}
		//
		void changeValue(ConsolePrinter*printer, Action action);

	public:
		void setValue(int value)
		{
			*this->value = value;

			if (handler)
				handler(this);
		}

		int getValue()
		{
			return *this->value;
		}

		void setEventHandler(std::function<void(IntElement*)> function)
		{
			this->handler = function;
		}
		void setOnAction(std::function<void(IntElement*)> function)
		{
			this->onAction = function;
		}
	};

	class FloatElement : public Element
	{
		float oldValue;

		float *value;

		float fStep;

		bool isSelected = false;

		//Event Handling
		std::function<void(FloatElement*)> onValueChange;

		std::function<void(FloatElement*)> onAction;

	public:
		//Constructor
		FloatElement(string name, float *value, float fStep, Element* previous) : Element(previous)
		{
			this->name = name;

			this->value = value;

			this->fStep = fStep;

			this->oldValue = oldValue;
		}

	protected:
		//Print function
		void print(ConsolePrinter *printer, bool isSelected);
		//Returns true if the value has changed since the last draw
		bool hasChanged(bool isSelected)
		{
			if (this->isSelected != isSelected)
				return true;

			return oldValue != *value;
		}
		//
		void changeValue(ConsolePrinter*printer, Action action)
		{
			switch (action)
			{
			case Increase: setValue(*value + fStep); break;
			case Decrease: setValue(*value - fStep); break;
			case Enter: if (onAction) onAction(this);
			}

			if (abs(*value) < 0.0001)
				*value = 0;
		}

	public:
		void setValue(float value)
		{
			*this->value = value;

			if (onValueChange)
				onValueChange(this);
		}

		float getValue()
		{
			return *this->value;
		}

		void setOnValueChange(std::function<void(FloatElement*)> function)
		{
			this->onValueChange = function;
		}
		void setOnAction(std::function<void(FloatElement*)> function)
		{
			this->onAction = function;
		}
	};

	class ComboElement : public Element
	{
		int oldValue;

		int *value;

		char** items;

		int max;

		bool isSelected = false;

		//Event Handling
		std::function<void(ComboElement*)> onValueChange;

		std::function<void(ComboElement*)> onAction;

	public:
		ComboElement(string name, int *value, char **items, int max, Element *previous) : Element(previous)
		{
			this->name = name;

			this->value = value;

			this->items = items;

			this->max = max;

			this->oldValue = *value;
		}

	protected:
		//Print function
		void print(ConsolePrinter *printer, bool isSelected);
		//Returns true if the value has changed since the last draw
		bool hasChanged(bool isSelected)
		{
			if (this->isSelected != isSelected)
				return true;

			return oldValue != *value;
		}
		//
		void changeValue(ConsolePrinter*printer, Action action)
		{
			switch (action)
			{
			case Increase: if (*value < max - 1) setValue(*value + 1); break;
			case Decrease: if (*value > 0) setValue(*value - 1); break;
			case Enter: if (onAction) onAction(this);
			}
		}
	public:
		void setValue(int value)
		{
			*this->value = value;

			if (onValueChange)
				onValueChange(this);
		}

		int getValue()
		{
			return *this->value;
		}

		string getItem()
		{
			return string(items[*value]);
		}

		void setOnValueChange(std::function<void(ComboElement*)> function)
		{
			this->onValueChange = function;
		}
		void setOnAction(std::function<void(ComboElement*)> function)
		{
			this->onAction = function;
		}
	};

	class TextElement : public Element
	{
		bool centralize;

	public:
		//Constructor
		TextElement(string name, bool centralize) : Element(NULL)
		{
			this->name = name;

			this->centralize = centralize;
		}

	protected:
		//Print function
		void print(ConsolePrinter *printer, bool isSelected);
		//
		bool hasChanged(bool isSelected)
		{
			return false;
		}
		//
		void changeValue(ConsolePrinter*printer, Action action)
		{
		}
	public:
		void setText(string text)
		{
			this->name = text;
		}
	};

	class LogElement : public Element
	{
		string curLog;

		bool hasChangedb = false;

		LogElement *nextLogElem = NULL;

	public:
		LogElement(LogElement *previous) : Element(NULL)
		{
			if (previous)
				previous->setNextLogElement(this);
		}

	protected:
		// Print function
		void print(ConsolePrinter *printer, bool isSelected);
		//Returns true if the value has changed since the last draw
		bool hasChanged(bool isSelected)
		{
			return hasChangedb;
		}
		//
		void changeValue(ConsolePrinter*printer, Action action)
		{

		}

		void setNextLogElement(LogElement* next)
		{
			this->nextLogElem = next;
		}

	public:
		//Removes all logs
		void clearLogs()
		{
			curLog = "";

			if (nextLogElem)
				nextLogElem->clearLogs();

			hasChangedb = true;
		}
		//Add a log, pushes other logs down
		void addLog(string newLog)
		{
			if (nextLogElem && !curLog.empty())
			{
				nextLogElem->addLog(curLog);
			}

			curLog = newLog;

			hasChangedb = true;
		}
	};

	//[WIP]
	class SelectElement : public Element
	{
		SelectElement *next;
		SelectElement *previous;

		bool isActiveElement;

	public:
		SelectElement(string name, Element *previous, SelectElement *prevSelectElement)
			: Element(previous)
		{
			this->name = name;

			this->previous = prevSelectElement;

			if (prevSelectElement)
				prevSelectElement->setNextSelectElement(this);
		}

		void setNextSelectElement(SelectElement *next)
		{
			this->next = next;
		}

	protected:
		// Print function
		void print(bool isSelected)
		{
			

			cout << "  "  << "               \n";
		}
		//Returns true if the value has changed since the last draw
		bool hasChanged(bool isSelected)
		{
			return false;
		}
		//
		void changeValue(Action action)
		{
			if (action == Enter)
			{
				for (SelectElement *e = next; e; e = e->next)
				{
					e->isActiveElement = false;
				}

				for (SelectElement *e = previous; e; e = e->previous)
				{
					e->isActiveElement = false;
				}

				this->isActiveElement = true;
			}
		}
	};


	//typedef void(*voidFunction)();

	class ConsolePrinter;

	class CallbackFunction
	{
		std::function<void(void)> voidFunc;
		std::function<void(ConsolePrinter*)> ptrFunc;
		Intervall intervall;
	public:
		CallbackFunction(std::function<void(void)> func, int millis = 0)
		{
			intervall = Intervall(millis);

			voidFunc = func;
		}
		CallbackFunction(std::function<void(ConsolePrinter*)> func, int millis = 0)
		{
			intervall = Intervall(millis);

			ptrFunc = func;
		}

		void runFunction(ConsolePrinter *conPtr)
		{
			if (voidFunc)
			{
				voidFunc();
			}

			if (ptrFunc)
			{
				ptrFunc(conPtr);
			}
		}

		bool check()
		{
			return intervall.check();
		}
	};

	


	class PageElement : public Element
	{
		ConsolePage* targetPage;

		bool isSelected = false;

		bool isReturnElem;

	public:
		PageElement(string name, Element* previous, bool isReturnElem, ConsolePage* targetPage = NULL) : Element(previous)
		{
			this->name = name;
			this->isReturnElem = isReturnElem;
			this->targetPage = targetPage;
		}

	protected:
		//
		void print(ConsolePrinter *printer, bool isSelected);
		//
		bool hasChanged(bool isSelected)
		{
			if (this->isSelected != isSelected)
				return true;

			return false;
		}
		//
		void changeValue(ConsolePrinter *printer, Action action)
		{
			switch (action)
			{
			case Enter:
				activate(printer);

				break;
			}
		}
	public:
		void setTargetPage(ConsolePage* targetPage)
		{
			this->targetPage = targetPage;
		}

		void activate(ConsolePrinter *printer);
	};




	class ConsolePage
	{
		//Pointer to our printer
		ConsolePrinter* printer = NULL;
		//Pointer to the parent page
		ConsolePage* lastPage = NULL;


		std::vector<Element*> elementList;
		//The current selected element
		Element* currentElement;
		//Last added control element
		Element* lastInteractiveElement = NULL;

	public:
		ConsolePage(ConsolePrinter* printer)
		{
			this->printer = printer;
		}
		~ConsolePage()
		{
			for (Element *e : elementList)
			{
				delete e;
			}
			elementList.clear();
		}

		//
		void printAll();
		//
		void printChangedElements();

		//Selects the first element on the page
		void gotoFirst()
		{
			for (Element* e : elementList)
			{
				if (e->hasNext())
				{
					currentElement = e;
					return;
				}
			}
		}
		//Selects the last element on the page
		void gotoLast()
		{
			for (Element* e : elementList)
			{
				if (!e->hasNext())
				{
					currentElement = e;
					return;
				}
			}
		}

		void up()
		{
			if (!currentElement)
				return;

			if (currentElement->hasPrev())
				currentElement = currentElement->getPrev();
		}
		void down()
		{
			if (!currentElement)
				return;

			if (currentElement->hasNext())
				currentElement = currentElement->getNext();
		}

		void changeValue(Action action)
		{
			if (!currentElement)
				return;

			currentElement->changeValue(printer,action);
		}

		void setParentPage(ConsolePage* last)
		{
			this->lastPage = last;
		}

		ConsolePage* getParentPage()
		{
			return lastPage;
		}

		ButtonElement* Button(string name)
		{
			ButtonElement* newButton = new ButtonElement(name, lastInteractiveElement);

			lastInteractiveElement = elementList.back();

			return newButton;
		}

		BoolElement* Bool(string name, bool *value)
		{
			BoolElement *newElement = new BoolElement(name, value, lastInteractiveElement);

			elementList.push_back(newElement);

			lastInteractiveElement = elementList.back();

			return newElement;
		}

		IntElement* Int(string name, int *value, int steps = 1)
		{
			IntElement *newElement = new IntElement(name, value, steps, lastInteractiveElement);

			elementList.push_back(newElement);

			lastInteractiveElement = elementList.back();

			return newElement;
		}

		FloatElement* Float(string name, float* value, float steps = 1)
		{
			FloatElement *newElement = new FloatElement(name, value, steps, lastInteractiveElement);

			elementList.push_back(newElement);

			lastInteractiveElement = elementList.back();

			return newElement;
		}

		TextElement* Text(string name, bool centralize = false)
		{
			TextElement *newElement = new TextElement(name, centralize);

			elementList.push_back(newElement);

			return newElement;
		}

		void EmptyLine()
		{
			elementList.push_back(new TextElement("", false));
		}
		//Add std::string support
		ComboElement* Combo(string name, int* value, char** items, int max)
		{
			ComboElement *newElement = new ComboElement(name, value, items, max, lastInteractiveElement);

			elementList.push_back(newElement);

			lastInteractiveElement = elementList.back();

			return newElement;
		}

		LogElement* LogElem(string name, size_t size)
		{
			LogElement* newLog = new LogElement(NULL);

			elementList.push_back(newLog);

			LogElement* temp = newLog;

			for (int i = 0; i < size; i++)
			{
				temp = new LogElement(temp);

				elementList.push_back(temp);
			}

			return newLog;
		}

		PageElement* PageElem(string name)
		{
			PageElement* newElement = new PageElement(name, lastInteractiveElement, false);

			elementList.push_back(newElement);

			lastInteractiveElement = elementList.back();

			return newElement;
		}

		PageElement* ReturnElem(string name)
		{
			PageElement* newElement = new PageElement(name, lastInteractiveElement, true);

			elementList.push_back(newElement);

			lastInteractiveElement = elementList.back();

			return newElement;
		}

		void evenStringSize()
		{
			int maxWidth = 0;
			for (Element *e : elementList)
			{
				if (maxWidth < e->name.length() && (e->hasNext() || e->hasPrev()))
					maxWidth = e->name.length();
			}

			for (Element *e : elementList)
			{
				if (e->hasNext() || e->hasPrev())
					e->name.append(string(maxWidth - e->name.length(), ' '));
			}
		}

		void load(bool resetSelction = true)
		{
			//Go to first control element if possible
			if (resetSelction)
				gotoFirst();

			//Print page
			printAll();
		}
	};


	




	class ConsolePrinter
	{
	public:
		//std::vector<Element*> elementList;
		//The current selected element
		//Element* currentElement;
		//Last added control element
		//Element* lastInteractiveElement = NULL;

		HWND consoleHwnd;
		HANDLE consoleHandle;

		Settings settings;

		Intervall printIntervall;
		Intervall navInputIntervall;

		Hotkeys key;
		//If printer has its own thread
		thread curThread;

		std::vector<CallbackFunction> callbackFunctions;

		std::vector<ConsolePage*> pageList;

		ConsolePage* currentPage;

		//Width in characters
		int iWidth;
		//Height in characters
		int iHeight;

		//Window width
		float fWidth;
		//Window height
		float fHeight;

		ConsolePrinter(int width, int height, Hotkeys newKeys = Hotkeys())
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

			//border y = 30 x = 18

			this->iWidth = width;
			this->iHeight = height;

			this->fWidth  = (width + 1) * 8 + 26;
			this->fHeight = height * 16 + 30;

			resizeWindow(fWidth, fHeight);

			//cout << "123456789 123456789 123456789 123456789 123456789 \n";

			key = newKeys;

			setCursorVisibility(false);

			clearConsole();
		}
		~ConsolePrinter()
		{
			if (curThread.joinable())
				curThread.join();

			for (ConsolePage *e : pageList)
			{
				delete e;
			}
			pageList.clear();
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

			return MoveWindow(consoleHwnd, x, y, r.right - r.left, r.bottom - r.top, TRUE);
		}
		bool showWindow(bool state)
		{
			if (state)
				return ShowWindow(consoleHwnd, SW_SHOW);
			else
				return ShowWindow(consoleHwnd, SW_HIDE);
		}
		bool setConsoleTitle(string text)
		{
			return SetConsoleTitleA(text.c_str());
		}

		void setCursorPosition(int x, int y)
		{
			COORD pos = { x, y };
			SetConsoleCursorPosition(consoleHandle, pos);
		}

		void setConsoleColor(CColor textColor, CColor backgroundColor)
		{
			SetConsoleTextAttribute(consoleHandle, (textColor + (backgroundColor * 16)));
		}

		void setCursorVisibility(bool b)
		{
			CONSOLE_CURSOR_INFO     cursorInfo;
			GetConsoleCursorInfo(consoleHandle, &cursorInfo);
			cursorInfo.bVisible = b; // set the cursor visibility
			SetConsoleCursorInfo(consoleHandle, &cursorInfo);
		}

		void clearConsole()
		{
			setCursorPosition(0, 0);

			string emptyLine(iWidth - 1, ' ');

			for (int i = 0; i < iHeight; i++)
			{
				cout << emptyLine << "\n";
			}
		}

		void printAll()
		{
			if (!currentPage)
				return;

			setCursorPosition(0, 0);

			currentPage->printAll();
		}


		ConsolePage* createPage()
		{
			ConsolePage* page = new ConsolePage(this);

			pageList.push_back(page);

			return page;
		}

		void setDefaultPage(ConsolePage* defaultPage)
		{
			this->currentPage = defaultPage;
		}

		ConsolePage* createDefaultPage()
		{
			if (currentPage)
				return NULL;

			currentPage = createPage();

			return currentPage;
		}
		

		

		void printChanhgedElements()
		{
			if (currentPage)
			currentPage->printChangedElements();
			/*for (Element *e : elementList)
			{
				if (e->hasChanged(currentElement == e))
				{
					setCursorPosition(0, line);
					e->print(currentElement == e);
				}
				line++;
			}*/
		}

		bool checkHotkey(int hotkey)
		{
			if (hotkey)
				return GetAsyncKeyState(hotkey);
			return false;
		}

		void navigation()
		{
			//Hide Unhide window
			if (checkHotkey(key.toggleVisible))
			{
				//WINDOWPLACEMENT wnd;

				//GetWindowPlacement(consoleHwnd, &wnd);

				if (IsWindowVisible(consoleHwnd))
					showWindow(false);
				else
					showWindow(true);
			}

			//Check if window is the foreground window
			if (GetForegroundWindow() != consoleHwnd)
			{
				return;
			}

			if (checkHotkey(key.up))
			{
				/*if (currentElement->hasPrev())
				{
					currentElement = currentElement->getPrev();
				}*/
				currentPage->up();
			}
			if (checkHotkey(key.down))
			{
				/*if (currentElement->hasNext())
				{
					currentElement = currentElement->getNext();
				}*/
				currentPage->down();
			}

			if (checkHotkey(key.left))
			{
				currentPage->changeValue(Decrease);
				//currentElement->changeValue(false);
			}
			if (checkHotkey(key.right))
			{
				currentPage->changeValue(Increase);
				//currentElement->changeValue(true);
			}
			if (checkHotkey(key.enter))
			{
				currentPage->changeValue(Enter);
			}
			
			//left, right, enter, modifier 1, modifier 2

			if (checkHotkey(key.lastPage))
			{
				backPage();
			}
		}

		template <class T>
		T modifier(T value)
		{
			value = checkHotkey(key.modifier5 ) ? value * 5  : value;
			value = checkHotkey(key.modifier10) ? value * 10 : value;

			return value;
		}

		void run()
		{
			currentPage->load();

			while (!checkHotkey(key.exitApp))
			{
				if (navInputIntervall.check(settings.navInputDelay))
					navigation();

				if (printIntervall.check(settings.conRefreshDelay))
					printChanhgedElements();

				executeRegisteredFunctions();

				Sleep(1);
			}
		}

		thread *start()
		{
			curThread = thread(&ConsolePrinter::run, this);

			return &curThread;
		}

		/*void registerFunction(CallbackFunction cb)
		{
			callbackFunctions.push_back(cb);
		}*/

		void registerFunction(std::function<void(ConsolePrinter*)> func, int millis = 0)
		{
			callbackFunctions.push_back(CallbackFunction(func, millis));
		}

		void registerFunction(std::function<void(void)> func, int millis = 0)
		{
			callbackFunctions.push_back(CallbackFunction(func, millis));
		}

		void clearRegisteredFunctions()
		{
			callbackFunctions.clear();
		}

		void executeRegisteredFunctions()
		{
			for (CallbackFunction cb : callbackFunctions)
			{
				if (cb.check())
					cb.runFunction(this);
			}
		}



		void loadPage(ConsolePage* page)
		{
			//Clears the console
			clearConsole();

			//Set parent page of the new element
			page->setParentPage(currentPage);

			page->load();

			currentPage = page;
		}
		void backPage()
		{
			//Check if we have an parent page
			if (currentPage->getParentPage())
			{
				//Clears the console
				clearConsole();


				currentPage->getParentPage()->load(false);
				//Set page to current
				ConsolePage* parentPage = currentPage->getParentPage();

				//currentPage->setParentPage(NULL);

				currentPage = parentPage;
			}
		}
	};
}