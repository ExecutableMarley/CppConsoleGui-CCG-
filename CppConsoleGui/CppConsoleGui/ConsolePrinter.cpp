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
-multiple page support                         //Done
-color support                                 //Text elem supported
-page inside element support (enter key and back key required) //Done
-event handling on elements                    //Done
-register funtions on console thread           //Done
-return pointer when elements are created      //Done
-log multi line element                        //Done but maybe refactor a bit
-multi line select element                     //Somewhat done
*/

namespace newConsolePrinter
{
	using namespace std;

	//Until compiler supports c++20 string format we use this
	template<typename ... Args>
	std::string string_format(const std::string& format, Args ... args)
	{
		//https://stackoverflow.com/a/26221725
		//From "iFreilicht" licensed under CC0 1.0
		int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
		if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
		size_t size = static_cast<size_t>(size_s);
		auto buf = std::make_unique<char[]>(size);
		std::snprintf(buf.get(), size, format.c_str(), args ...);
		return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
	}

	template<typename val>
	val clamp(val base, val min, val max)
	{
		if (base < min)
			return min;
		else if (base > max)
			return max;

		return base;
	}

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

		this->oldColor = color;

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

		this->oldColor = color;

		string curLine = stateArrow(isSelected) + name;
		
		//curLine.resize()
		
		curLine += string_format(" = % i", *value);

		curLine.resize(printer->iWidth - 1, ' ');

		cout << curLine << "\n";
	}

	void IntElement::changeValue(ConsolePrinter*printer, Action action)
	{
		switch (action)
		{
		case Increase:
			setValue(clamp(*value + printer->modifier(iStep), min, max)); break;
		case Decrease:
			setValue(clamp(*value - printer->modifier(iStep), min, max)); break;
		case Enter: 
			if (onAction) 
				onAction(this);
		}
	}

	//[FloatElement]

	void FloatElement::print(ConsolePrinter *printer, bool isSelected)
	{
		this->isSelected = isSelected;

		this->oldValue = *value;

		this->oldColor = color;

		string curLine = stateArrow(isSelected) + name;

		//curLine.resize()

		curLine += string_format(" = % 3.2f", *value);

		curLine.resize(printer->iWidth - 1, ' ');

		cout << curLine << "\n";
	}

	void FloatElement::changeValue(ConsolePrinter*printer, Action action)
	{
		switch (action)
		{
		case Increase: 
			setValue(clamp(*value + printer->modifier(fStep), min, max)); break;
		case Decrease: 
			setValue(clamp(*value - printer->modifier(fStep), min, max)); break;
		case Enter: 
			if (onAction)
				onAction(this);
		}

		if (abs(*value) < 0.0001)
			*value = 0;
	}

	//[ComboElement]

	void ComboElement::print(ConsolePrinter *printer, bool isSelected)
	{
		this->isSelected = isSelected;

		this->oldColor = this->color;

		this->oldValue = *value;



		string curLine = stateArrow(isSelected) + name;

		//curLine.resize()

		curLine += string_format(" =  %s", items[*value]);

		curLine.resize(printer->iWidth - 1, ' ');

		cout << curLine << "\n";
	}

	//[Text]

	void TextElement::print(ConsolePrinter *printer, bool isSelected)
	{
		this->oldValue = name;
		this->oldColor = color;

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

		printer->setConsoleColor((CColor)color, BLACK);

		cout << curLine;

		printer->setConsoleColor(WHITE, BLACK);
	}

	//[LogElement]

	void LogElement::print(ConsolePrinter* printer, bool isSelected)
	{
		this->oldLog = curLog;

		this->oldColor = color;

		string curLine = stateArrow(false) + curLog;

		curLine.resize(printer->iWidth - 1, ' ');

		curLine.append("\n");

		cout << curLine;
	}

	//[SelectElement]

	void SelectElement::print(ConsolePrinter *printer, bool isSelected)
	{
		this->isSelected = isSelected;

		this->oldColor = color;

		wasActiveElement = isActiveElement;

		string curLine = stateArrow(isSelected) + name + (isActiveElement ? "   [X]" : "      ");
		
		curLine.resize(printer->iWidth - 1, ' ');

		curLine.append("\n");

		cout << curLine;
	}



	//[PageElement]

	void PageElement::print(ConsolePrinter *printer, bool isSelected)
	{
		this->isSelected = isSelected;

		this->oldColor = color;

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
		printer->setCursorPosition(0, 0);
	}

}

//Key code values seems to differ on different operating systems
enum KeyMap
{
	
};

//We also probably need an enum for colors since these values will likely differ too



