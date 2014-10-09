#ifndef TEST_HH
	#include "test.hh"
	#define TEST_HH
#endif


// TODO: Split this function in several parts: make it modular

// TODO: Change the display mechanism to HTTP, HTTPS, HTTP/2 
//		 instead of spdy4 = {0, 1} and is_secure = {0, 1}.

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
	int sleep_time = 20;

	// Number of times you want to reach the webpage. The higher, the more
	// accurate will be page loading times.
	int times_to_reach = 1;

	int http2 = 0;
	int is_secure = 0; // 1 implies TLS, 0 implies cleartext TCP

	for (http2 = 0; http2 < 2; ++http2) {
		for (is_secure = 0; is_secure < 2; ++is_secure) {

			printf("Executing: rm -rf *log\n");
			system("rm -rf *log");

			printf("Executing: rm -rf ~/.cache/chromium\n");
			system("rm -rf ~/.cache/chromium");


			// Urls of the website on which we wish to test page loading times.
			string scheme_used;
			string port_used;
			string ip_addr_localhost = "127.0.0.1";
			string ip_addr_orange_server = "161.106.2.57";
			string ip_addr_vps = "198.50.151.105";
			string ip_addr_used = ip_addr_localhost;
			deque<string> urls;
			//urls.push_back("leopard.html");
			//urls.push_back("waves.html");
			urls.push_back("index.html");

			// Method std::to_string() requires --std=c++0x compilation flag
			string sleep_cmd = "sleep " + to_string(sleep_time) + " ";

			// Kills last launched background process.
			// TERM option used to "friendly" close an application.
			string kill_last_bg_process = "kill -TERM $! ";

			// Command line to execute.
			string command = chromium;
			
			if (set_incognito)
				command += incognito;
			if (set_no_extensions)
				command += no_extensions;
			if (set_ignore_certificate_errors)
				command += ignore_certificate_errors;
			if (http2) {
				command += enable_spdy4;
				if (is_secure) {
					command += h2;
					scheme_used = scheme_https;
					port_used = port_https;
				}
				else {
					command += h2c;
					scheme_used = scheme_http;
					port_used = port_http;
				}
			}
			else  {
				if (is_secure) {
					scheme_used = scheme_https;
					port_used = port_https;
				}
				else {
					scheme_used = scheme_http;
					port_used = port_http;
				}
			}

			command += scheme_used + ip_addr_used + port_used;

			for (deque<string>::const_iterator it = urls.begin(); it != urls.end();
				 ++it) {
				string new_command = command;
				new_command += *it;
				string log_file = *it + ".log";
				string log1_file = *it + "1.log";
				string log2_file = *it + "2.log";

				// Log stderr to file
				new_command += " >> " + log_file + " 2>&1 ";
				
				// Add a timer (sleep) until the page loads
				new_command += "& " + sleep_cmd;

				// Kills the last launched background process (that is chromium!).
				new_command += "&& " + kill_last_bg_process;

				// Run the command |times_to_reach| times
				for (int i = 0; i < times_to_reach; ++i) {
					if (verbose)
						printf("%s\n", new_command.c_str());
					system(new_command.c_str());
				}

				system(("egrep '(^setNavigationStart|^markLoadEventEnd)' " +
						log_file + " > " + log1_file).c_str());
				system(("cat " + log1_file + " | awk '{print $3}' > " +
						log2_file).c_str());
				
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
					printf("For spdy4 = %d and is_secure = %d:\n", http2, 
						   is_secure);
					printf("\tAverage loading time for %s = %f\n", 
						   log_file.c_str(), loading_time/times_to_reach);
				}
				else {
					cout << "Unable to open file";
				}
			}
		}
	}
	return 0;
}
