#include <iostream>
#include <format>
#include <exception>

#include "monitor.hpp"
#include "index.hpp"
#include "util.hpp"

#include "system/platform_identity.hpp"
#include "icc/socket.hpp"

namespace StashSaves::Component 
{

// c-tor definition
v1::Monitor::Monitor()
try
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

	auto plat_id{ StashSaves::PlatformIdentity::instance() };
	auto const current_user = plat_id->get_current_username();
	_saves = plat_id->get_save_games_path();
	_backup = plat_id->get_backup_path();

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
	
	auto socket_base_path = plat_id->get_socket_path();
	for (auto&& empires_save : Util::get_flat_subdirectories(_saves))
    {
    	auto empire_name_socket = empires_save.filename().replace_extension(".sock");
    	auto socket = socket_base_path / empire_name_socket;
    	std::cout << empires_save << " | socket: " << socket << std::endl;
    	_indexes.emplace_back(std::make_unique<Index>(empires_save, socket));
		_sockets.emplace_back(std::move(socket));
    }

    for (auto&& index : _indexes)
    {
    	index->watch_dir();
    }
}

void v1::Monitor::start() {
	std::cout << std::format("Monitor::start() - Monitor started at {}:{}", __func__, __LINE__) << std::endl;

	std::size_t poll_number = 0u;

	while(true)
	{
		sleep(1);
		for(auto const& socket : _sockets)
		{
			try 
			{
				auto msg{ StashSaves::read_from_socket(socket) };
				if(not msg.empty())
				{
					std::cout << std::format("[{}] socket: {} with message: {} ", poll_number, socket, msg) << std::endl;
				}
				else
				{
					std::cout << std::format("[{}] NO MESSAGE FOR socket: {}", poll_number, socket) << std::endl;
				}
			}
			catch(std::exception& e)
			{
				std::cout << std::format("{} Error in {} - can't read from {} socket because {}", poll_number, __func__, socket, e.what()) << std::endl;
			}
			++poll_number;
		}
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