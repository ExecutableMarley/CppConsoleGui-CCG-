#include <iostream>
#include <string>
#include <vector>    

#include "TimeFuncs.h"
#include "ConsolePrinter.h"

using namespace std;
using namespace CCG;

bool Boolean = true;
int Integer = 1;
float Float = 1;

const char *comboElements[] = { "Item 1", "Item 2", "Item 3" };
int comboIndex = 0;

int main()
{
	Hotkeys hotkeys;

	hotkeys.down  = VK_NUMPAD2;
	hotkeys.up    = VK_NUMPAD8;
	hotkeys.left  = VK_NUMPAD4;
	hotkeys.right = VK_NUMPAD6;

	//width, height, optional custom hotkeys
	CCG::ConsolePrinter con(53, 30, hotkeys);

	con.setConsoleTitle("Hello World");

	//Register an function to add an text rotation effect to the console title
	con.registerFunction([](CCG::ConsolePrinter* ptr) -> void 
	{
		string str = "Hello World";
		static int i = 0;

		std::rotate(str.begin(), str.begin() + (++i % str.size()), str.end());
		ptr->setConsoleTitle(str);
	
	}, 1000);

	//Creates an page and auto sets it to the default page
	ConsolePage* defaultPage = con.createDefaultPage();

	
	defaultPage->Bool("Bool Thing 1", &Boolean);
	defaultPage->Bool("Bool Thing 2", &Boolean);

	
	IntElement *intElement = defaultPage->Int("Int Thingy 3", &Integer, 1);
	
	defaultPage->Float("Float Thingy 4", &Float, 0.2f, -1.f, 2.3);

	defaultPage->EmptyLine();

	defaultPage->Text("Some Text", false, RED);

	defaultPage->Combo("Combo", &comboIndex, (char**)comboElements, 3);

	defaultPage->Text("Logs:");

	LogElement *logs = defaultPage->LogElem(3);

	PageElement* pageElement = defaultPage->PageElem("Go to some Page");

	//Create a new page
	ConsolePage* newPage = con.createPage();

	newPage->Text("Some other page", true);

	newPage->Bool("Some Other Bool", &Boolean);

	newPage->Int("Some Other Int", &Integer);

	newPage->Selection({ "Option 1", "Option 2", "Something else" });


	newPage->ReturnElem("Go Back");

	defaultPage->evenStringSize();
	newPage->evenStringSize();

	pageElement->setTargetPage(newPage);

	//Start the console in a new thread
	con.start();

	//Add event listener to int element
	intElement->setOnValueChange([logs](CCG::IntElement* el) -> void
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
}