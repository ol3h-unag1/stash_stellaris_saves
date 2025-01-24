#include <iostream>
#include <format>
#include <exception>

#include "monitor.hpp"
#include "plat_id.hpp"
#include "util.hpp"
#include "index.hpp"

namespace StashSaves::Component 
{

// c-tor definition
v1::Monitor::Monitor()
try
	: _platform(std::make_unique<PlatformId>(StashSaves::PlatformIdentity::get_platform().value()))
{
	std::cout << std::format("Monitor::Monitor() - Body Begin") << std::endl;
	init();
	std::cout << std::format("Monitor::Monitor() - Body End") << std::endl;
}
catch(std::exception& e)
{
	std::cout << "Monitor::Monitor() - Exception: " <<  e.what() << std::endl;
}
catch(...)
{
	std::cout << std::format("Monitor::Monitor() - Unknown exception. ") << std::endl;
}

void v1::Monitor::init()
{
	try
	{
		init_impl();
	}
	catch(std::exception& e)
	{
		std::cout << "Monitor::init() - Exception: " <<  e.what() << std::endl;
	}
	catch(...)
	{
		std::cout << std::format("Monitor::init() - Unknown exception. ") << std::endl;
	}
}
void v1::Monitor::init_impl() {

	//std::cout << "Monitor::init() - Socket temporary path: " << G_socket_temp_path << std::endl;
	//std::cout << "Monitor::init() - Game saves path: " << G_game_path << std::endl;
	//std::cout << "Monitor::init() - User backup path: " << G_user_backup_path << std::endl;
	
		// get game saves directory
			// get user directory
		    // checko for stellaris saves dir in it
		// create back up directory
		// spawn index into Empire's subfolders
		// back up something that's already over the limit of files
		// ..
	    //

	auto const current_user = Util::get_current_username(_platform);
	_saves = Util::get_save_games_path(_platform);
	_backup = Util::get_backup_path(_platform);

	if (not Util::is_directory_exists(_saves))
	{
		throw std::runtime_error(std::string("Saves directory does not exist: ") + _saves.string());
	}
	else
	{
		std::cout << std::format("Saves directory is ready: {} at {}:{}", _saves.string(), __func__, __LINE__) << std::endl;
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
	
	auto socket_base_path = Util::get_socket_path(_platform);
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
	while(true)
	{
		sleep(3);
		std::cout << "Monitor::start() wakee wakee! " << std::endl;
	}
}


} // end namespace StashSaves::Component 

int main()
{

#define RUN

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