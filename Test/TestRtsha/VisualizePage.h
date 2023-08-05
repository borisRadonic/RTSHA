#pragma once
#include "MemoryPage.h"
#include <iostream>
#include <sstream>
#include <string>


using namespace rtsha;

class VisualizePage
{
public: 

	VisualizePage() = delete;

	VisualizePage(rtsha_page* page):_page(page)
	{

	}

	void print(std::stringstream& textStream);

	rtsha_page* _page;
};

