#include <iostream>

#include "monitor.hpp"
#include "util.hpp"

namespace StashSaves::Component 
{

v1::Monitor::Monitor() {

		init();
}

void v1::Monitor::init() {

	fs::path username = Util::get_current_username();


		// get game saves directory
			// get user directory
		    // checko for stellaris saves dir in it
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
 	std::cout << std::filesystem::current_path() << std::endl;


	return 0;
}