// TestUSBLib.cpp : Defines the entry point for the console application.
//

#include <iostream>

#include <string.h>

#include "CObjectContainer.h"

#define THIS_FILE			"testelog.cpp"

int main( int argc, char *argv[])
{
	std::cout << "---inicio---" << std::endl;
	CObjectContainer::initialize();

	CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "main", TRACE, "Initializing - compile date: [%s]", __DATE__);

	return 0;
}

