# Chat application

Simple client-server chat application using Boost.Asio. Currently, only handles one chat room.

**Dependencies**:

- Boost.Asio for organizing the networking;
- Boost.Program_options for handling console parameters.

### Compilation info

Compilation is done using CMake. Since I only have access to Windows and MacOS platforms, Linux **is not** officially supported. That said, feel free to modify [CMakeLists.txt](/CMakeLists.txt) to add Linux support (most likely, _APPLE_ config would work just fine).

To compile, you need to install Conan package manager first. After that, just use CMake to generate the appropriate build files. Simple compilation setup would look something like this:

```
pip install conan
cd build
cmake ..
cmake --build .
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
