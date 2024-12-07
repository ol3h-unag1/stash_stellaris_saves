#include <iostream>
#include <exception>

#include "monitor.hpp"
#include "util.hpp"
#include "index.hpp"

namespace StashSaves::Component 
{

//const char* G_game_path = ".local/share/Paradox Interactive/Stellaris/save games/"; // real dir
const char* G_game_path = "Stellaris/Saves/rev1/"; // test dir
const char* G_user_backup_path = "stash_saver/Stellaris/save games/";

auto base_path(auto&& user_name, auto&& local_game_saves_path) {

    fs::path base_path = "/home" / user_name / local_game_saves_path;
    return base_path;
}

// c-tor definition
v1::Monitor::Monitor() {
	
	try
	{
		init();
	}
	catch(std::exception& e)
	{
		std::cout << "Monitor::init() failed " << e.what() << std::endl;
		throw;		
	}
}


void v1::Monitor::init() {

		// get game saves directory
			// get user directory
		    // checko for stellaris saves dir in it
		// create back up directory
		// spawn index into Empire's subfolders
		// back up something that's already over the limit of files
		// ..
	    //

	auto const current_user = Util::get_current_username();
	_saves = base_path(current_user, G_game_path);
	_backup = base_path(current_user, G_user_backup_path);

	if (not Util::is_directory_exists(_saves))
	{
	    throw std::runtime_error("Game saves directory does not exist: " + _saves.string());
	}

	if (Util::is_directory_exists(_backup))
	{
	    std::cout << "Backup directory is ready: " << _backup << std::endl;
	}
	else
	{
		try
		{
			fs::create_directories(_backup);
		}
		catch(std::exception& e)
		{
			std::cout << "Failed to create backup directory: " << _backup << std::endl;
			throw;
		}

	    std::cout << "Backup directory has been created: " << _backup << std::endl;;
	}
	
	//_index = std::make_unique<Index>(_saves);

    std::cout << "Monitor initialized: " << _saves << "#\n#" << _backup << " #\n";
    for (auto&& empires_save : Util::get_flat_subdirectories(_saves))
    {
    	auto index = std::make_unique<Index>(empires_save);
    }


}

void v1::Monitor::start() {

    std::cout << "Monitor started." << std::endl;
}


} // end namespace StashSaves::Component 

int main()
{
try
{
	using Monitor = StashSaves::Component::Monitor;
	Monitor mon;

	mon.start();

	std::cout << "monitor::main()" << std::endl;
	return 0;
}
catch (std::exception& e)
{
	std::cout << e.what();
}
catch(...)
{
	std::cout << "monitor.cpp::main() unknown exception" << std::endl;
}
}