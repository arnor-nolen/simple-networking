# Chat application

Simple client-server chat application using Boost.Asio. Currently, only handles one chat room.

**Dependencies**:

- Boost.Asio for organizing the networking;
- Boost.Program_options for handling console parameters.

### Compilation info

Compilation is done using CMake. Since I only have access to Windows and MacOS platforms, Linux **is not** officially supported. That said, feel free to modify [CMakeLists.txt](/CMakeLists.txt) to add Linux support (most likely, _APPLE_ config would work just fine). You can find an example of my VS Code compilation setup in [here](/.vscode/tasks.json).

In short, you need to pass paths to required include and lib folders to CMake (listed in dependencies), so compilation process would look something like this:

```
cd build
cmake .. -DCMAKE_LIBRARY_PATH='$lib_folder' -DCMAKE_INCLUDE_PATH='$include_folder'
cmake --build . --config Release
```

Now, enjoy your freshly minted binaries inside [bin](/bin) folder!

### Launching the app

Use `--help` flag for getting the info about console parameters.

To launch the server, use:
`./server --port $port`

To launch the client:
`./client --name $your_nickname --host $host --port $port`

### Tests

You can test the 1000 clients load on Windows using [this Powershell script](/test/loadtest.ps1).
