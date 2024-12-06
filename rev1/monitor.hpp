#pragma once
#include <filesystem>
#include <memory>

namespace StashSaves::Component
{
inline namespace v1
{


class Index;

namespace fs = std::filesystem;

class Monitor
{

public:
	Monitor();

public:
	void start();

private:
	void init();

private:
	fs::path _saves;
	fs::path _backup;

	std::unique_ptr<Index> _index;

};


} // end of namespace StashSaves::Component::v1
} // end of namespace StashSaves::Component