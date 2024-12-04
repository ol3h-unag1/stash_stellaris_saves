#include <iostream>

#include "monitor.hpp"
#include "util.hpp"

namespace StashSaves::Component 
{

v1::Monitor::Monitor() {

	try 
	{
		init();
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}

void v1::Monitor::init() {

		// get game saves directory
		// create back up directory
		// spawn index

    std::cout << "Monitor initialized" << std::endl;
}

void v1::Monitor::start() {

    std::cout << "Monitor started." << std::endl;
}


} // end namespace StashSaves::Component 

int main()
{
	using Monitor = StashSaves::Component::Monitor;
	Monitor mon;

	mon.start();

	std::cout << "monitor::main()" << std::endl;

	return 0;
}