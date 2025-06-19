# Communicator

# Client-Server Messenger Project developed during computer networks classes

## Launch Instructions

**Required libraries:**
+ `nlohmann/json`
+ `wxWidgets` (at least version 3.2.4) – for the client, available at: https://www.wxwidgets.org/downloads/
+ `sqlite3`
+ other libraries that the above depend on

**Launching after downloading:**

### Client:
+ `client/build/`
+ delete `CMakeCache.txt`
+ `cmake ..`
+ `make`
+ `./bin/Client`

### Server:
+ `server/build/`
+ delete `CMakeCache.txt`
+ `cmake ..`
+ `make`
+ `./bin/Server`

**Databases are available in** `build/resource/resource`


