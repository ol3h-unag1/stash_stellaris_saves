# stash_stellaris_saves


Requirements for Vlad:

Non-funcitonal:
Read example of inotify API is in index.cpp. 
Build index.cpp, run. Add, delete, move, copy-in-out files in directory. 
See output of index.cpp object.
Build with:
g++ -std=c++23 -o index_service index.cpp -Ofast

Use ChatGPT extensively.

Functional:
I need you to write index service that will notify client about file beeing added to directory:

	1) Service should run indefinetly.
		1.a) Until killed with some kill -N pid from parent process.
	2) Service should read a parameter, that's a path to target directory where service will monitor changes in filesystem (index).
		2.a) Be read as passed argument
	3) When target's directory index changed, service should notify client.
		3.a) Read client's executable path as passed argument, call it passing path to newly created dile as an argument.

Example of usage:
	./index_service '/home/user451/Paradox Interactive/Stellaris/save games/vladoid_cosmosa20240112/' '/home/user451/stashss/stash_exe'
	
Example of call to client:		
	/home/holep/mone666/stash_exe '2253.08.21.sav'


Tips.
There's execute_terminal_command in repository. It's easier way to call the client back.
build_run.sh from repository have examples of how to generate files, ask GPT to do something for yourself to create files by timer.
If you need something to test you component agains - talk to me.

Good luck!