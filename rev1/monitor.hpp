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

using PlatformId = StashSaves::PlatformIdentity::PlatformId;
using PlatformIdPtr = std::shared_ptr<PlatformId>;

// end of forward declarations of PlatformIdentity types

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

	PlatformIdPtr _platform;

	std::vector<std::unique_ptr<Index>> _indexes;

#define DEBUG_COMPONENT_MONITOR
#ifdef DEBUG_COMPONENT_MONITOR
public:
	auto platform() const { return _platform; }

#endif
};
// end of Module Component Monitor


} // end of namespace StashSaves::Component::v1
} // end of namespace StashSaves::Component