#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <vector>

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
public:
	Monitor();

public:
	void start();

private:
	void init();
	void init_impl();

private:
	fs::path _saves;
	fs::path _backup;

	std::vector<std::unique_ptr<Index>> _indexes;
};
// end of Module Component Monitor


} // end of namespace StashSaves::Component::v1
} // end of namespace StashSaves::Component