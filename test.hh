#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> 
#include <deque>
#include <fstream>
#include <algorithm> // replace
#include <string.h> // strcmp

using namespace std;

// Machine IP address on which we run our test
string ip_addr_localhost = "127.0.0.1";
string ip_addr_orange_server = "161.106.2.57";
string ip_addr_vps = "198.50.151.105";
string ip_addr_yiping = "172.20.36.138";

// Default values for arguments (see usage() to change these)
int verbose = 0;							// No logging 
int sleep_time = 20;						// Requests last 20 seconds
int times_to_reach = 1;						// Only 1 request
string ip_addr_used = ip_addr_localhost;	// Local tests
string network = "default";					// Logging to "default" directory
deque<string> urls;							// No URLs to reach
string delay = "0";							// Oms delay
string losses = "0";						// O% losses
string interface = "lo";					// Loopback is default interface for delay

// Protocols to use
enum Protocol {
	http = 0,
	https = 1,
	http2 = 2,
	http2s = 3
};

// Gives the name of the protocol with the corresponding int
static inline const char *stringFromProtocol(int proto) {
	static const char *strings[] = {"http", "https", "http2", "http2s"};
	return strings[proto];
}

// Chromium executable.
string chromium_path = getenv("CHROMIUM_PATH");
string chromium= chromium_path + "src/out/Debug/chrome ";

// Incognito mode: no caching and no automatic connection to gstatic.com
// and google.com + execute as root!
int set_incognito = 1;
string incognito = "--incognito --user-data-dir $HOME ";

// No loading of chrome://extensions/.../background_page.html.
int set_no_extensions = 1;
string no_extensions = "--disable-extensions \
	--disable-component-extensions-with-background-pages ";

// No user prompt when there is a certificate error.
// The user could also put the certificate in the browser to avoid the
// error.
int set_ignore_certificate_errors = 1;
string ignore_certificate_errors = "--ignore-certificate-errors ";

// Disable all sorts of caching.
int set_disable_cache = 1;
string disable_cache = "--disable-cache --disable-gpu-program-cache \
	--disable-gpu-shader-disk-cache --disable-offline-load-stale-cache \
	--disk-cache-size=0 --gpu-program-cache-size-kb=0 --media-cache-size=0 \
	--disable-application-cache ";

// Enables HTTP/2 (aka SPDY4)
string enable_spdy4 = "--enable-spdy4 ";

// Always use HTTP/2, and use it over TLS
string h2 = "--use-spdy=ssl ";

// Always use HTTP/2, and use it over TCP
string h2c = "--use-spdy=no-ssl ";

// Usual scheme and ports for http and https
string scheme_http = "http://";
string scheme_https = "https://";
string port_http = ":80/";
string port_https = ":443/";

// Kills last launched background process. TERM option to close "friendly".
string kill_last_bg_process = "kill -TERM $! ";

// Sets options
string set_options(int proto);

// Log function
void LOG(const char* s);

int usage(char* argv[]);

string get_url(int proto);

// Grep log_file and outputs load times in log2_file
int grep_load_times(int proto, string name);

// Parses the arguments
int deal_with_arguments(int argc, char* argv[]);
int check_arg(int argc, char* argv[], int i);

// Execute command line 
int execute(string s);

// Create a global file containing all the results
int concat_all_files();

// Clean up functions
void clean_cache();
void clean_logs();

// Traffic Controller (TC) functions
void set_delay_and_losses();
void unset_delay_and_losses();

// Transform all slashes in input string into dots
string slash_to_dot(string input);
