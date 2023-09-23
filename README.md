# Universal TCP server

# Compile and Install

Obtain source code from github

```shell
git clone https://github.com/username-nullptr/gts3.git
```

Compile the source code with cmake:

```shell
cd gts3
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=<dir>  # -DENABLE_SSL=y -DOpenSSL_DIR=<dir>
make install
```

# Usages

## Start server

```shell
./gts start
```

An ini file is used to set the running parameters of the server, and site configuration is done through a json file. 

* The default path of the file is the directory where the executable program resides.
* Detailed configuration instructions are in doc in the source directory.

## Built-in command line arguments

```shell
# ./gts --help

start, -start, --start :
    Start the server.

stop, -stop, --stop :
    Stop the server.

restart, -restart, --restart :
    Restart the server.

-d, --daemon :
    Start as a daemon process.

-i, --instace :
    Specify the server instance name to start multiple instances. (default is 'gts')

-f, --file :
    Specify the configuration file (default is './config.ini').

status, -status, --status :
    Viewing server status.

-v, --version:
    Viewing server version.

-h, --help:
    Viewing help.
```

## TCP server extension

```c++
// plugin.cpp
#include <gts/registration.h>

using namespace gts;

// Parameters are optional.
GTS_PLUGIN_INIT(/*const std::string &config_file*/)
{
    // do something
}

// If the initial process is time-consuming and the result of the process does not
// affect the subsequent process, you can use xxx to perform it asynchronously.
GTS_PLUGIN_ASYNC_INIT(/*const std::string &config_file*/)
{
    return make_future_ptr(std::async(std::launch::async, []
    {
        // do something
    }));
}

GTS_PLUGIN_EXIT()
{
    // do something
}

// gts::tcp_socket_ptr &socket.
// Perform special processing on a port.
GTS_PLUGIN_NEW_CONNECTION(socket /*, 8888*/)
{
    // do something
}

// The return value type must be 'std::string'.
GTS_PLUGIN_VIEW_STATUS()
{
    // do something
    return "status information";
}
```

## Web extension

- The loading of the plug-in requires a json file to configure. (Detailed configuration instructions are in doc in the source directory.)

```c++
// plugin0.cpp
#include <gts/web.h>

using namespace gts;
using namespace gts::http;

// Parameters are optional.
GTS_PLUGIN_INIT(/*const std::string &config_file*/)
{
    // do something
}

// If the initial process is time-consuming and the result of the process does not
// affect the subsequent process, you can use xxx to perform it asynchronously.
GTS_PLUGIN_ASYNC_INIT(/*const std::string &config_file*/)
{
    return make_future_ptr(std::async(std::launch::async, []
    {
        // do something
    }));
}

GTS_PLUGIN_EXIT()
{
    // do something
}

// The return value type must be 'std::string'.
GTS_PLUGIN_VIEW_STATUS()
{
    // do something
    return "status information";
}

// The parameter must be 'gts::http::request'.
// Paths that have a parent-child relationship are executed from the parent path to the child path.
GTS_PLUGIN_HTTP_REQUEST_FILTER(http::response &req /*, http::&response, const environments&*/)
{
    // A return of true indicates that the request has been processed and no more actual processing is performed.
    // return true;
    return false;
}

// The parameter must be 'gts::http::response'.
GTS_PLUGIN_HTTP_REQUEST_HANDLE("path", GET|PUT, http::response &resp /*, http::request&, const environments&*/)
{
    // req.cookie();
    // req.read_body();
    // req.save_file();
    // req.session();
    // resp.set_cookie("hello", http::cookie);
    // resp.redirect("https://github.com");
    // resp.write_body("hello world");
    // resp.write_file("path/hello.html");
    resp.write("hello world");
}

class controller
{
public:
    // future_ptr async_init(/*const std::string &config_file*/) {}
    
    void init(/*const std::string &config_file*/)
    {
        // do something
    }
    
    void exit()
    {
	    // do something
    }
    
    // The parameter must be 'gts::http::request'.
    // Paths that have a parent-child relationship are executed from the parent path to the child path.
    void request_filter(http::response &req /*, http::&response, const environments&*/)
    {
        // A return of true indicates that the request has been processed and no more actual processing is performed.
        // return true;
        return false;
    }
    
    // The parameter must be 'gts::http::response'.
    void request(http::response &resp /*, http::request&, const environments&*/)
    {
        // req.cookie();
        // req.read_body();
        // req.save_file();
        // req.session();
        // resp.set_cookie("hello", http::cookie);
        // resp.redirect("https://github.com");
        // resp.write_body("hello world");
        // resp.write_file("path/hello.html");
        resp.write("hello world");
    }
    
    // The return value type must be 'std::string'.
    std::string view_status()
    {
        // do something
        return "status information";
    }
};

GTS_PLUGIN_REGISTRATION
{
    using namespace gts::web;
    registration::class_<controller>("path")
        .init_method(controller::init)
        .exit_method(controller::exit)
        .view_status_method(controller::view_status)
        .filter_method("path", controller::request_filter)
        .request_handle_method<GET,POST>("path", controller::request);
}
```

## Command line extension

Place a directory named 'startup_extensions' in the directory where the executable program resides, and place the extension in it.

```c++
// plugin.cpp
#include <gts/registration.h>

GTS_PLUGIN_CMD_INIT()
{
	// do something
}

GTS_PLUGIN_CMD_EXIT()
{
	// do something
}

// If a parameter other than the preset parameter appears, it is processed in this function.
// Return true if the command arguments is processed, false otherwise.
// If false is returned, the command will throw an error.
GTS_PLUGIN_CMD_ARGS_PARSING(int argc, const char *argv[] /*const string_list &args*/)
{
	// do something
	return true;
}

GTS_PLUGIN_CMD_VIEW_VERSION()
{
	return "extension version info.\n";
}

GTS_PLUGIN_CMD_VIEW_HELP()
{
	return "extension help info.\n";
}
```

