#include <iostream>
#include <format>
#include <exception>

#include "monitor.hpp"
#include "util.hpp"
#include "index.hpp"

namespace StashSaves::Component 
{

//const char* G_game_path = ".local/share/Paradox Interactive/Stellaris/save games/"; // real dir
const char* G_game_path = "Stellaris/Saves/rev1"; // mock dir
const char* G_user_backup_path = "stash_saver/Stellaris/save games";
const char* G_socket_temp_path = "stash_saver/Stellaris/.sockets";

auto base_path(auto&& user_name, auto&& path) {

    fs::path base_path = "/home" / user_name / path; // NRVO? Maybe. Maybe not. Probably.
    return base_path;
}

// c-tor definition
v1::Monitor::Monitor() {
	
	std::cout << std::format("Monitor::Monitor() - Constructor Begin") << std::endl;
	try
	{
		init();
		std::cout << std::format("Monitor::Monitor() - Initialized") << std::endl;
	}
	catch(std::exception& e)
	{
		std::cout << "Monitor::Monitor() - Exception: " <<  e.what() << std::endl;
	}
	catch(...)
	{
		std::cout << std::format("Monitor::Monitor() - Unknown exception") << std::endl;
	}
	std::cout << std::format("Monitor::Monitor() - Constructor End") << std::endl;
}

void v1::Monitor::init() {


	// std::cout << "Monitor::init() - Socket temporary path: " << G_socket_temp_path << std::endl;
	// std::cout << "Monitor::init() - Game saves path: " << G_game_path << std::endl;
	// std::cout << "Monitor::init() - User backup path: " << G_user_backup_path << std::endl;
	
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
		throw std::runtime_error(std::string("Saves directory does not exist: ") + _saves.string());
	}

	if (Util::is_directory_exists(_backup))
	{
		std::cout << std::format("Backup directory is ready: {} at {}:{}", _backup.string(), __func__, __LINE__) << std::endl;
	}
	else
	{
		std::cout << std::format("Creating backup directory: {} at {}:{}", _backup.string(), __func__, __LINE__) << std::endl;
		try
		{
			fs::create_directories(_backup);
		}
		catch(std::exception& e)
		{
			std::cout << "Failed to create backup directory: " << _backup << " - Exception: " << e.what() << " at " << __func__ << ":" << __LINE__ << std::endl;
		}
		catch(...)
		{
			std::cout << "Failed to create backup directory: " << _backup << " - Unknown exception at " << __func__ << ":" << __LINE__ << std::endl;
		}

		std::cout << std::format("Backup directory has been created: {} at {}:{}", _backup.string(), __func__, __LINE__) << std::endl;
	}
	
	auto socket_base_path = base_path(current_user, G_socket_temp_path);

	std::cout << std::format("Monitor initialized: {} {} at {}:{}", _saves.string(), _backup.string(), __FILE__, __LINE__) << std::endl;

    for (auto&& empires_save : Util::get_flat_subdirectories(_saves))
    {
    	auto empire_name_socket = empires_save.filename().replace_extension(".sock");
    	auto socket = socket_base_path / empire_name_socket;
    	std::cout << empires_save << " | socket: " << socket << std::endl;
    	_indexes.emplace_back(std::make_unique<Index>(empires_save, socket));
    }

    for (auto&& index : _indexes)
    {
    	index->watch_dir();
    }
}

void v1::Monitor::start() {

	std::cout << std::format("Monitor::start() - Monitor started at {}:{}", __func__, __LINE__) << std::endl;
}


} // end namespace StashSaves::Component 

int main()
{

#define RUN


#ifndef RUN
std::cout << std::format("Current username: {}", StashSaves::Util::get_current_username().string()) << std::endl;
#endif


#ifdef RUN
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
	std::cout << std::format("monitor.cpp::main()  exception {} at {}:{}", e.what(), __func__, __LINE__) << std::endl;
}
catch(...)
{
	std::cout << std::format("monitor.cpp::main() unknown exception at {}:{}", __func__, __LINE__) << std::endl;
}
#endif

	return 1;
}