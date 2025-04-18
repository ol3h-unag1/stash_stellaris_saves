#include <iostream>
#include <format>
#include <exception>
#include <thread>


#include "monitor.hpp"
#include "index.hpp"
#include "util.hpp"

#include "system/platform_identity.hpp"

namespace StashSaves::Component 
{

using Int = StashSaves::Util::Int;

namespace v1
{

// c-tor definition
Monitor::Monitor(std::size_t file_limit)
try
: _portion_size(file_limit/2) // half this shit; just why not
{
	std::cout << std::format("Monitor::Monitor() - Body Begin. file limit to dir:{} portion size:{}", file_limit, _portion_size ) << std::endl;
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

void Monitor::init()
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

void Monitor::init_impl() {

	// auto plat_id{ StashSaves::PlatformIdentity::instance() };
	auto plat_id{ StashSaves::PlatformIdentity::mock_instance() };

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
	
	for (auto&& empire : Util::get_flat_subdirectories(_saves))
    {
    	std::cout << "empire: " << empire << std::endl;

		_empire_to_saves_list[empire] = Util::get_files_in_directory(empire);
		std::cout << "empire saves: " << _empire_to_saves_list[empire].size() << std::endl;
		if (_empire_to_saves_list[empire].size() >= _portion_size)
		{
			backup_saves(empire);
		}	

		auto callback = [this, empire](fs::path save) {

			index_callback(empire, save);
		};

		_indexes.emplace_back(std::make_unique<Index>(std::move(empire), std::move(callback)));
    }

	for (auto&& index : _indexes)
    {
    	index->watch_dir();
    }
}

void Monitor::start() {
	std::cout << std::format("Monitor::start() - Monitor started at {}:{}", __func__, __LINE__) << std::endl;

	std::size_t poll_number = 0u;

	while(true)
	{
		sleep(1);
		std::cout << std::format("Monitor::start() - Polling... {} at {}:{}", poll_number++, __func__, __LINE__) << std::endl;
	
	}
}

void Monitor::backup_saves(fs::path empire) {

	auto& saves_list = _empire_to_saves_list[empire];
    std::cout << std::format("TID: Monitor::backup() - Empire: {} | SavesList: {} at {}:{}", std::this_thread::get_id(),
                            empire.string(), saves_list.size(), __func__, __LINE__) << std::endl;

    // Determine the number of elements to process (up to _portion_size)
    size_t portion = std::min(_portion_size, saves_list.size());

    for (size_t i = 0; i < portion; ++i) {
        // Use a reference to avoid copying the path
        const auto& save = saves_list[i];

        try {
            // Create backup directory structure
            auto backup_path = _backup / empire.filename() / save.filename();
            fs::create_directories(backup_path.parent_path());

            std::cout << std::format("Monitor::backup() - Copying {} to {} at {}:{}",
                                    save.string(), backup_path.string(), __func__, __LINE__) << std::endl;

            // Copy the file with overwrite
            fs::copy_file(save, backup_path, fs::copy_options::overwrite_existing);

            // Construct the original file path using _saves data member
            fs::path original_path = _saves / empire.filename() / save.filename();

            // Delete the original file
            if (fs::remove(original_path)) {
                std::cout << std::format("Deleted original file: {} at {}:{}", 
                                        original_path.string(), __func__, __LINE__) << std::endl;
            } else {
                std::cout << std::format("Original file {} not found at {}:{}", 
                                        original_path.string(), __func__, __LINE__) << std::endl;
            }
        }
        catch(const fs::filesystem_error& e) {
            std::cout << std::format("Failed to process {}: {} at {}:{}",
                                   save.string(), e.what(), __func__, __LINE__) << std::endl;
        }
    }

    // Remove the first 'portion' elements from the saves_list
    saves_list.erase(saves_list.begin(), saves_list.begin() + portion);
	std::cout << std::format("Monitor::backup() - Remaining saves: {} at {}:{}", 
							saves_list.size(), __func__, __LINE__) << std::endl;
}

void Monitor::index_callback(const fs::path& empire, const fs::path& save) {
	
	std::cout << "MONITOR's INDEX CALLBACK | TID: " << std::this_thread::get_id() << std::endl;
	std::cout << std::format("Monitor::index_callback() - Empire: {} | Save: {} at {}:{}", empire.string(), save.string(), __func__, __LINE__) << std::endl;

	auto empire_it = _empire_to_saves_list.find(empire);
	if (empire_it != _empire_to_saves_list.end())
	{
		auto& saves = empire_it->second;
		saves.push_back(save);
		if (saves.size() >= _portion_size)
		{
			std::cout << "Calling for backup from TID: " << std::this_thread::get_id() 
				<< std::format(" | Empire: {} | SavesList: {} at {}:{}", empire.string(), saves.size(), __func__, __LINE__) << std::endl;
			
			backup_saves(empire);
		}
	}
	else
	{
		std::cout << std::format("Monitor::index_callback() - Empire not found: {} at {}:{}", empire.string(), __func__, __LINE__) << std::endl;
	}	
}

} // end namespace v1

} // end namespace StashSaves::Component 

int main()
{

#define RUN

#ifdef RUN
try
{
	std::size_t const G_Stellaris_Saves_Limit = 200;
	using Monitor = StashSaves::Component::Monitor;
	Monitor mon(G_Stellaris_Saves_Limit);

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