#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> 
#include <deque>
#include <fstream>
#include <algorithm> // replace
using namespace std;

// Chromium executable.
// TODO: Fix the PATH to the executable
string chromium= "../src/out/Debug/chrome ";

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

// Displays which protocol was used.
int display_protocol(int http2, int is_secure);

// Sets options
string set_options(int set_incognito, int set_no_extensions,
	int set_ignore_certificate_errors, int set_disable_cache,
	int http2, int is_secure);

// Log function
void LOG(const char* s, int verbose);

string protocol_in_use(int http2, int is_secure);

int usage(char* argv[]);

string get_url(int is_secure, string ip_addr_used);

// Computing average loading times
int average_loading_time(string log2_file, int times_to_reach,
	int http2, int is_secure, string name);

// Grep log_file and outputs load times in log2_file
int grep_load_times(string log_file, string log1_file,
	string log2_file);

