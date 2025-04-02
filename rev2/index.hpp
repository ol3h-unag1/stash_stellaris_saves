#pragma once

#include <filesystem>
#include <string>
#include <functional>
#include <sys/types.h> 

#include "util.hpp"

namespace StashSaves::Component
{

namespace Index_v1
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

namespace Index_v2
{
	namespace fs = std::filesystem;
	using Int = StashSaves::Util::Int;

	class Index final
	{
	private:
		using CallbackType = std::function<void(Int, const fs::path&)>;

	public:
		explicit Index(fs::path dir_to_watch, CallbackType callback);
		~Index();

	public:
		void watch_dir();

	private:
		fs::path _directory;
		CallbackType _callback;
	};


	namespace Index_Current_Version = Index_v2;

}

} // end of namespace StashSaves::Component