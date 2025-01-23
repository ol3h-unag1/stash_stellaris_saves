#pragma once
#include <filesystem>
#include <memory>
#include <optional>
#include <vector>

// Forward declarations of PlatformIdentity types
namespace StashSaves::PlatformIdentity
{
	struct PlatformId;
}

namespace StashSaves::Component
{

inline namespace v1
{

// Forward declarations of Component types
class Index;
// end of forward declarations of Component types

namespace fs = std::filesystem;

// Module Component Monitor
class Monitor
{
	using PlatformId = StashSaves::PlatformIdentity::PlatformId;

public:
	Monitor();

public:
	void start();

private:
	void init();

private:
	fs::path _saves;
	fs::path _backup;

	std::unique_ptr<PlatformId> _platform;
	std::vector<std::unique_ptr<Index>> _indexes;
};
// end of Module Component Monitor


} // end of namespace StashSaves::Component::v1
} // end of namespace StashSaves::Component