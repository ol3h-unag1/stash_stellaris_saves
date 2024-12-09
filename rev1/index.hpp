#pragma once

#include <filesystem>
#include <string>

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
		explicit Index(fs::path dir_to_watch, std::string socket);

	public:
		void watch_dir();

	private:
		void watch_dir_impl();

	private:
		fs::path _directory;
		std::string _socket;
	};


} // end of namespace StashSaves::Component::v1
} // end of namespace StashSaves::Component