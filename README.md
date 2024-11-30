
# stash_stellaris_saves

## Overview

`stash_stellaris_saves` is a lightweight file indexing service designed to monitor a directory for file changes and notify a specified client when new files are added.

## Features

- Runs indefinitely to monitor a directory.
- Notifies a client executable with the name of newly added files.
- Flexible and easily extendable for various use cases.

## Requirements

- C++ compiler supporting C++23 (e.g., `g++`).
- Familiarity with building and running C++ applications.

## Getting Started

### Building and Running the Example

1. Test the inotify API using `index.cpp` (example provided in the repository).
2. Build and run the example:
   ```bash
   g++ -std=c++23 -o index_service index.cpp -Ofast
   ./index_service
   ```
3. Observe the output as you add, delete, or move files in the target directory.

### Building and Running the Service

1. Compile the service:
   ```bash
   g++ -std=c++23 -o index_service index_service.cpp -Ofast
   ```
2. Run the service, passing the path to the directory to monitor and the client executable:
   ```bash
   ./index_service "/path/to/watch/" "/path/to/client/"
   ```
   Example:
   ```bash
   ./index_service "/home/user/Stellaris/saves/" "/home/user/stashss/stash_exe"
   ```

### Client Notification

When a new file is added to the monitored directory, the service executes the client with the file name as an argument. Example call to the client:
```bash
/path/to/client/newly_added_file_name
```

## Tips

- Use `execute_terminal_command` (provided in the repository) for simpler client calls.
- Use `build_run.sh` for automated file generation and testing.
- Explore and experiment—there’s wide leeway for customization.

## Contributing

Contributions are welcome! Feel free to submit issues or pull requests.

---

**Good luck!**
