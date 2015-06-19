#include "PhysXTutorial.h"

#include "PhysicsTute.h"

int main()
{
	auto tute = new PhysicsTute();

	tute->Run();

	delete tute;
	return 0;
}