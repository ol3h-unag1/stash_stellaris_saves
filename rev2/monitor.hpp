#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <vector>

namespace StashSaves::Component
{

namespace v1
{

// Forward declarations of Component types
class Index;
// end of forward declarations of Component types

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