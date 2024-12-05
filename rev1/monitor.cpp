#include <iostream>
#include <exception>

#include "monitor.hpp"
#include "util.hpp"

namespace StashSaves::Component 
{

const char* G_game_path = ".local/share/Paradox Interactive/Stellaris/save games/";

// will build a path where keeps its saves ;)
auto base_path(auto&& user_name, auto&& local_game_saves_path) {

    fs::path base_path = "/home" / user_name / local_game_saves_path;
    return base_path;
}

v1::Monitor::Monitor() {

		init();
}

void v1::Monitor::init() {

	_saves = base_path(Util::get_current_username(), G_game_path);

		// get game saves directory
			// get user directory
		    // checko for stellaris saves dir in it
		// create back up directory
		// spawn index

	if (not Util::is_directory_exists(_saves))
	{
	    throw std::runtime_error("Directory does not exist: " + _saves.string());
	}
	
    std::cout << "Monitor initialized: " << _saves << std::endl;
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