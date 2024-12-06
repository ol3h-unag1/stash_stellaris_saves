#pragma once

#include <filesystem>
#include "util.hpp"

namespace StashSaves::Component
{
inline namespace v1
{

	namespace fs = std::filesystem;
	using Int = StashSaves::Util::Int;
	
	class Index
	{
	public:
		Index(fs::path dir_to_watch);

	public:
		Int get_index() const;

	private:
		void watch_dir(fs::path);
	};


} // end of namespace StashSaves::Component::v1
} // end of namespace StashSaves::Component