#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> 
#include <deque>
#include <fstream>

using namespace std;

int main() {

	int set_incognito = 1;
	int set_no_extensions = 1;
	int set_ignore_certificate_errors = 1;
	int set_h2 = 0;
	int set_h2c = 1;
	
	// Time to wait for the webpage to load in seconds.
	int sleep_time = 10;

	// Number of times you want to reach the webpage. The higher, the more
	// accurate will be page loading times.
	int times_to_reach = 10;

	// Chromium executable.
	// TODO: Fix the PATH to the executable
	string chromium= "../src/out/Debug/chrome ";

	// Chromium does not automatically connect to certain sites like
	// google.fr; also chromium does not use caching.
	string incognito = "--incognito ";

	// Removes annoying connection to 
	// chrome://extensions/.../background_page.html.
	string no_extensions = "--disable-extensions \
		--disable-component-extensions-with-background-pages ";

	// Skips when there is a non-recognized certificat. This can be 
	// avoided if the server certificate is inserted in the browser
	string ignore_certificate_errors = "--ignore-certificate-errors ";

	// Says if we should use SPDY4 aka HTTP/2, and indicates whether we
	// use h2 (with TLS) or h2c (directly on TCP).
	string h2 = "--use-spdy=ssl ";
	string h2c = "--use-spdy=no-ssl ";

	// Urls of the website on which we wish to test page loading times.
	deque<string> urls;
	urls.push_back("leopard.html");
	urls.push_back("waves.html");

	// We use the method std::to_string(), so  we need to add --std=c++0x
	// in CXXFLAGS (see Makefile)
	string sleep_cmd = "sleep " + to_string(sleep_time) + " ";

	// Kills last launched background process.
	// TERM option used to "friendly" close an application.
	string kill_last_bg_process = "kill -TERM $! ";

	// Command line to execute.
	string command = chromium;
	
	if (set_incognito) {
		command += incognito;
	}
	if (set_no_extensions) {
		command += no_extensions;
	}
	if (set_ignore_certificate_errors) {
		command += ignore_certificate_errors;
	}
	if (set_h2) {
		command += h2;
	}
	else if (set_h2c) { // I put "else if" to avoid any conflict wrt h2.
		command += h2c;
	}

	command += "http://127.0.0.1:443/";
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
			printf("Average loading time for %s = %f\n", log_file.c_str(), 
				   loading_time/times_to_reach);
		}
		else {
			cout << "Unable to open file";
		}
	}
	return 0;
}
