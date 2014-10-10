#ifndef TEST_HH
	#include "test.hh"
	#define TEST_HH
#endif


// TODO: Split this function in several parts: make it modular

// TODO: Handle several types of url no matter what the format.
//		 Might have to change the name of the log files.

int main(int argc, char* argv[]) {
	int verbose = 0;
	
	// Handles command line arguments
	if (argc > 2) {
		printf("Usage: %s [-v]\n", argv[0]);
		return 1;
	}
	if (argc == 2) {
		string argv1 = argv[1];
		if (argv1 == "-v") {
			verbose = 1;
		}
		else {
			printf("Wrong argument\n");
			return 1;
		}
	}

	// Time to wait for the webpage to load in seconds.
	// Method std::to_string() requires --std=c++0x compilation flag
	int sleep_time = 20;
	string sleep_cmd = "sleep " + to_string(sleep_time) + " ";

	// Number of times you want to reach the webpage. The higher, the more
	// accurate will be page loading times.
	int times_to_reach = 1;

	int http2 = 0;
	int is_secure = 0; // 1 implies TLS, 0 implies cleartext TCP

	// List of files (websites) to test
	deque<string> urls;
	// urls.push_back("waves.html");
	// urls.push_back("leopard.html");
	urls.push_back("index.html");

	for (deque<string>::const_iterator it = urls.begin(); 
			it != urls.end(); ++it) {

		printf("%s\n", (*it).c_str());

		for (http2 = 0; http2 < 2; ++http2) {
			for (is_secure = 0; is_secure < 2; ++is_secure) {

				// Cleaning up log files
				if (verbose)
					printf("Executing: rm -rf *log\n");
				system("rm -rf *log");
				// Cleaning up the cache
				if (verbose)
					printf("Executing: rm -rf ~/.cache/chromium\n");
				system("rm -rf ~/.cache/chromium");

				// Command line to execute.
				string command = chromium;

				// Define url to reach 
				string scheme_used;
				string port_used;
				string ip_addr_used = ip_addr_localhost;

				// Setting options
				if (set_incognito)
					command += incognito;
				if (set_no_extensions)
					command += no_extensions;
				if (set_ignore_certificate_errors)
					command += ignore_certificate_errors;
				if (set_disable_cache)
					command += disable_cache;
				if (http2) {
					command += enable_spdy4;
					if (is_secure)
						command += h2;
					else 
						command += h2c;
				}

				// Setting the url
				if (is_secure) {
					scheme_used = scheme_https;
					port_used = port_https;
				}
				else {
					scheme_used = scheme_http;
					port_used = port_http;
				}

				command += scheme_used + ip_addr_used + port_used;
				


				string new_command = command;
				new_command += *it;
				string log_file = *it + ".log";
				string log1_file = *it + "1.log";
				string log2_file = *it + "2.log";

				// Log stderr to log_file
				new_command += " >> " + log_file + " 2>&1 ";
				
				new_command += "& " + sleep_cmd;

				new_command += "&& " + kill_last_bg_process;

				for (int i = 0; i < times_to_reach; ++i) {
					if (verbose)
						printf("%s\n", new_command.c_str());
					system(new_command.c_str());
				}

				system(("egrep '(^setNavigationStart|^markLoadEventEnd)' " +
						log_file + " > " + log1_file).c_str());
				system(("cat " + log1_file + " | awk '{print $3}' > " +
						log2_file).c_str());
			
				// Computing average loading times
				string line;	
				ifstream myfile(log2_file);
				if (myfile.is_open()) {
					double loading_time = 0;

					for (int i = 0; i < times_to_reach; ++i) {
						getline(myfile, line);
						double start = strtod(line.c_str(), NULL);
						getline(myfile, line);
						double end = strtod(line.c_str(), NULL);
						loading_time += end-start;
					}
					myfile.close();
					display_protocol(http2, is_secure);
					printf("%f\n", loading_time/times_to_reach);
				}
				else {
					printf("Unable to open file\n");
				}
			}
		}
	}
	return 0;
}

int display_protocol(int http2, int is_secure) {
	if (http2) {
		if (is_secure)
			printf("\tHTTP/2 over TLS: ");
		else
			printf("\tHTTP/2 over TCP: ");
	}
	else {
		if (is_secure)
			printf("\tHTTPS: ");
		else
			printf("\tHTTP: ");
	}
	return 0;
}

