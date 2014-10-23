#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> 
#include <deque>
#include <fstream>
#include <algorithm> // replace
#include <string.h> // strcmp

using namespace std;

// Chromium executable.
string chromium_path = getenv("CHROMIUM_PATH");
string chromium= chromium_path + "src/out/Debug/chrome ";

// Incognito mode: no caching and no automatic connection to gstatic.com
// and google.com.
int set_incognito = 1;
string incognito = "--incognito ";

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

// Machine IP address on which we run our test
string ip_addr_localhost = "127.0.0.1";
string ip_addr_orange_server = "161.106.2.57";
string ip_addr_vps = "198.50.151.105";
string ip_addr_yiping = "172.20.36.138";

// Kills last launched background process. TERM option to close "friendly".
string kill_last_bg_process = "kill -TERM $! ";

// Sets options
string set_options(int proto);

// Log function
void LOG(const char* s);

int usage(char* argv[]);

string get_url(int proto, string ip_addr_used);

// Computing average loading times
int average_loading_time(string log2_file, int times_to_reach,
	int proto, string name);

// Grep log_file and outputs load times in log2_file
int grep_load_times(string log_file, string log1_file,
	string log2_file);

// Parses the arguments
int deal_with_arguments(int argc, char* argv[]);
int check_arg(int argc, char* argv[], int i);

// Execute command line 
int execute(string s);

// Global variables for arguments
int verbose = 0;
int sleep_time = 20;
int times_to_reach = 1;
string ip_addr_used = ip_addr_localhost;
string network = "default";
deque<string> urls;
int delay = 0;
string interface = "lo";

// Create a global file containing all the results
int concat_all_files();

// Protocols to use
enum Protocol {
	http = 0,
	https = 1,
	http2 = 2,
	http2s = 3
};

static inline const char *stringFromProtocol(int proto) {
	static const char *strings[] = {"http", "https", "http2", "http2s"};
	return strings[proto];
}

// Clean up functions
void clean_cache();
void clean_logs();

void set_delay();
void unset_delay();
