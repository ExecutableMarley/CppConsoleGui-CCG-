// CppConsoleGui.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include <iostream>

#include <iostream>
#include <string>
#include <vector>    
#include <Windows.h>
//#include <stdlib.h> 
#include <thread>

#include "TimeFuncs.h"

#include "ConsolePrinter.h"




using namespace std;

using namespace newConsolePrinter;








bool Boolean = true;
int Integer = 1;
float Float = 1;

const char *chars[] = { "Item 1", "Item 2", "Item 3" };
int index = 0;

void ptrTest(newConsolePrinter::ConsolePrinter* ptr)
{

}

void lamTest(std::function<int(int)> lambda)
{
	cout << lambda(1) << endl;
}

int test(int t)
{
	return 1;
}

void foo()
{
	std::function<int(int)> abc = test;

	lamTest(abc);

	std::function<int(int)> cbc = [](int p) { return 12; };

	lamTest(cbc);


	auto dbc = [](int p) { return 12; };



	lamTest([](int p) -> int { return 12; });
}

template<class ... Types>
void something(Types ... a)
{

}
/*
template<class ... Types>
string format(const char* _format, Types ... a)
{
	char buffer[128];

	sprintf_s(buffer, format, a);

	return buffer;
}*/

template<class Type>
void test1(Type t)
{

}/*


template<class ... Types>
string format(string _format, Types ... a)
{
	return format(_format.c_str(), a);
}*/

int main()
{
	//const char *jju = "Hallo %s    [%d]";

	//string abc = format("Hallo %s    [%d]", "Welt", 10);
	//cout << abc << endl;
#if 0
	ConsolePrinter con(450, 500);

	//con.addTextElement("Line 1");
	//con.EmptyLine();
	//con.addTextElement("Line 3");

	con.addTextElement("SOME TEXT");

	con.addBoolElement("Bool Thingy 1", &Boolean);
	con.addBoolElement("Bool Thingy 2", &Boolean);

	con.addTextElement("-------------------------------------------------");

	con.EmptyLine();
	con.addTextElement("SOME TEXT");
	con.EmptyLine();

	con.Int("Int Thingy 3", &Integer, 1);
	con.Float("Float Thingy 4", &Float, 0.2);

	con.Combo("Combo Thingy", &index, (char**)chars, 3);


	con.evenStringSize();

	con.print();

	while (!GetAsyncKeyState(VK_END))
	{
		con.navigation();

		Sleep(150);
	}

#else
	newConsolePrinter::ConsolePrinter con(53, 30);

	con.setConsoleTitle("Hello World");

	//Registering functions that will be executed in the con thread
	//con.registerFunction([](void) -> void { Beep(1000, 200); });

	//con.registerFunction([](newConsolePrinter::ConsolePrinter* ptr) -> void { Beep(1000, 200); });

	//con.registerFunction(foo);
	//Creates an page and auto sets it to the default page
	ConsolePage* defaultPage = con.createDefaultPage();

	defaultPage->Bool("Bool Thingy 1", &Boolean);
	defaultPage->Bool("Bool Thingy 2", &Boolean);

	IntElement *intElement = defaultPage->Int("Int Thingy 3", &Integer, 1);
	defaultPage->Float("Float Thingy 4", &Float, 0.2f);

	defaultPage->EmptyLine();

	defaultPage->Text("Some Text");

	defaultPage->Combo("Combo", &index, (char**)chars, 3);

	defaultPage->Text("Logs:");

	LogElement *logs = defaultPage->LogElem("", 3);

	PageElement* pageElement = defaultPage->PageElem("Go to some Page");


	ConsolePage* newPage = con.createPage();

	newPage->Text("Some other page", true);

	newPage->Bool("Some Other Bool", &Boolean);

	newPage->Int("Some Other Int", &Integer);

	PageElement* newPageElem = newPage->PageElem("Go To Default");

	newPage->ReturnElem("Go Back");

	defaultPage->evenStringSize();
	newPage->evenStringSize();

	pageElement->setTargetPage(newPage);

	newPageElem->setTargetPage(defaultPage);

	con.start();

	intElement->setEventHandler([logs](newConsolePrinter::IntElement* el) -> void
	{
		logs->addLog("Value changed to: " + std::to_string(el->getValue()));
	});

	Sleep(5000);

	logs->addLog("A");
	Sleep(2000);
	logs->addLog("B");
	Sleep(2000);
	logs->addLog("C");

	while (!GetAsyncKeyState(VK_END))
	{
		Sleep(150);
	}

	return 0;

	char *screen = new char[100 * 100];
	for (int i = 0; i < 100 * 100; i++) screen[i] = ' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	//Print text easily
	//sprintf(&screen[0], "   ");

	sprintf_s(&screen[0], 100 * 100, "   d  ");

	WriteConsoleOutputCharacterA(hConsole, screen, 100 * 100, { 0,0 }, &dwBytesWritten);

	_SMALL_RECT Rect;
	Rect.Top = 0;
	Rect.Left = 0;
	Rect.Bottom = 100 - 1;
	Rect.Right = 100 - 1;

	SetConsoleScreenBufferSize(hConsole, { 100, 100 });

	SetConsoleWindowInfo(hConsole, true, &Rect);
#endif
}
