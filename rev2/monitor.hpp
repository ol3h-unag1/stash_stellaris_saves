#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <vector>
#include <unordered_map>

namespace StashSaves::Component
{

// Forward declarations of Component types

namespace Index_v2
{
	class Index;
} 

using Index = Index_v2::Index;

// end of forward declarations of Component types

inline namespace v1
{

namespace fs = std::filesystem;

// Module Component Monitor
class Monitor
{
private:
	using SavesList = std::vector<fs::path>;
	using EmpiresSaves = std::unordered_map<fs::path, SavesList>;

public:
	Monitor(std::size_t);

public:
	void start();

private:
	void init();
	void init_impl();

private:
	void backup_saves(fs::path empire);

private:
	void index_callback(const fs::path& empires, const fs::path& save);

private:
	fs::path _saves;
	fs::path _backup;

	std::vector<std::unique_ptr<Index>> _indexes;
	
	EmpiresSaves _empire_to_saves_list;

private:
	std::size_t const _portion_size = 0u;
};
// end of Module Component Monitor


} // end of namespace StashSaves::Component::v1
} // end of namespace StashSaves::Component