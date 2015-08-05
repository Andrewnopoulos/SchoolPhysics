#include "PhysXTutorial.h"

#include "PhysicsTute.h"

#include <iostream>

int main()
{
	std::cout << "1 for custom engine, 2 for PhysX" << std::endl;

	int input;

	std::cin >> input;

	if (input == 1)
	{
		auto tute = new PhysicsTute();

		tute->Run();

		delete tute;
		return 0;
	}
	else if (input == 2)
	{
		auto tute = new PhysXTutorial();

		tute->Run();

		delete tute;
		return 0;
	}
}