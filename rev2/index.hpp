#pragma once

#include <filesystem>
#include <string>
#include <functional>
#include <thread>

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


} // end of namespace StashSaves::Component::Index_v1

namespace Index_v2
{
	namespace fs = std::filesystem;
	using Int = StashSaves::Util::Int;

	class Index final
	{
	public:
		using CallbackType = std::function<void(fs::path)>;

	public:
		explicit Index(fs::path dir_to_watch, CallbackType callback);
		~Index();

	public:
		void watch_dir();

	public:
		void watch_dir_impl();

	private:
		fs::path _directory;
		CallbackType _callback;

		std::thread _thread;
	};

} // end of namespace StashSaves::Component::Index_v2

} // end of namespace StashSaves::Component