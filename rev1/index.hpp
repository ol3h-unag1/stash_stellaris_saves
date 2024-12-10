#pragma once

#include <filesystem>
#include <string>
#include <sys/types.h> 

#include "util.hpp"

namespace StashSaves::Component
{
inline namespace v1
{

	namespace fs = std::filesystem;
	using Int = StashSaves::Util::Int;

	class Index final
	{
	public:
		explicit Index(fs::path dir_to_watch, std::string socket);
		~Index();

	public:
		void watch_dir();

	private:
		void watch_dir_impl();

	private:
		fs::path _directory;
		std::string _socket;
		pid_t _child_pid;  
	};


} // end of namespace StashSaves::Component::v1
} // end of namespace StashSaves::Component