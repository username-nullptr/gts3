# Universal TCP server

# Compile and Install

Obtain source code from github

```shell
git clone https://github.com/username-nullptr/gts3.git
```

Compile the source code with cmake:

```shell
cd gts3
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=<dir>  # -DOpenSSL_DIR=<dir>
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
// The return value type can be any type that can be copied, but will not be processed.
GTS_DECL_EXPORT void plugin_init(/*const std::string &config_file*/)
{
	// do something
}

GTS_DECL_EXPORT void plugin_exit()
{
	// do something
}

// The parameter must be 'gts::tcp_socket_ptr'.
GTS_DECL_EXPORT void new_connection(tcp_socket_ptr socket)
{
	// do something
}

// The return value type must be 'std::string'.
GTS_DECL_EXPORT std::string view_status()
{
	// do something
    return "status information";
}

GTS_PLUGIN_REGISTRATION
{
	gts::registration(new_connection)
		.view_status_method(view_status)
		.init_method(plugin_init)
		.exit_method(plugin_exit);
    
    // Perform special processing on a port.
    // gts::registration(new_connection, 10000);
}
```

## Web extension

- The loading of the plug-in requires a json file to configure. (Detailed configuration instructions are in doc in the source directory.)

```c++
// plugin0.cpp
#include <gts/web.h>

using namespace gts;

// Parameters are optional.
GTS_DECL_EXPORT void plugin_init(/*const std::string &config_file*/)
{
	// do something
}

// If the initial process is time-consuming and the result of the process does not
// affect the subsequent process, you can use xxx to perform it asynchronously.
GTS_DECL_EXPORT future_ptr plugin_async_init(/*const std::string &config_file*/)
{
	return make_future_ptr(std::async(std::launch::async, []
	{
		// do something
	}));
}

GTS_DECL_EXPORT void plugin_exit()
{
	// do something
}

// The return value type must be 'std::string'.
GTS_DECL_EXPORT std::string view_status()
{
	// do something
    return "status information";
}

// The parameter must be 'gts::http::request'.
// Paths that have a parent-child relationship are executed from the parent path to the child path.
GTS_DECL_EXPORT void request_filter(http::response &request /*, http::&response, const environments&*/)
{
    // A return of true indicates that the request has been processed and no more actual processing is performed.
    // return true;
	return false;
}

// The parameter must be 'gts::http::response'.
GTS_DECL_EXPORT void global_request(http::response &response /*, http::request&, const environments&*/)
{
	response.write("hello world");
}

class GTS_DECL_EXPORT controller
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
    void request_filter(http::response &request /*, http::&response, const environments&*/)
    {
        // A return of true indicates that the request has been processed and no more actual processing is performed.
        // return true;
	    return false;
    }
    
    // The parameter must be 'gts::http::response'.
    void request(http::response &response /*, http::request&, const environments&*/)
    {
	    response.write("hello world");
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
	using namespace gts::http;
	using namespace gts::web;
    
	registration()
			.init_method(plugin_init)
			.exit_method(plugin_exit)
			.view_status_method(view_status)
             .filter_method("/", request_filter)
			.request_handle_method<GET,POST>("path", global_request);
        
	registration::class_<controller>("path")
			.init_method(controller::init)
			.exit_method(controller::exit)
			.view_status_method(controller::view_status)
             .filter_method("path", controller::request_filter)
			.request_handle_method<GET,POST>("path", controller::request);
}
```

## Command line extension

The plug-in library must be named 'gtsstartupextensions' and placed in the directory where the server's executables reside.

```c++
// plugin.cpp
#include <gts/registration.h>

GTS_DECL_EXPORT void extensions_init()
{
	// do something
}

GTS_DECL_EXPORT void extensions_exit()
{
	// do something
}

// If a parameter other than the preset parameter appears, it is processed in this function.
// Return true if the command arguments is processed, false otherwise.
// If false is returned, the command will throw an error.
GTS_DECL_EXPORT bool args_parsing(int argc, const char *argv[] /*const string_list &args*/)
{
	// do something
	return true;
}

GTS_DECL_EXPORT std::string view_version_info()
{
	return "extension version info.\n";
}

GTS_DECL_EXPORT std::string view_help()
{
	return "extension help info.\n";
}

GTS_PLUGIN_REGISTRATION
{
	gts::extension::registration()
			.init_method(extensions_init)
			.exit_method(extensions_exit)
			.args_parsing_method(args_parsing)
			.view_version_method(view_version_info)
			.view_help_method(view_help);
}
```

