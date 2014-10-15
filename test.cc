#ifndef TEST_HH
	#include "test.hh"
	#define TEST_HH
#endif

// TODO: Split this function in several parts: make it modular

int main(int argc, char* argv[]) {
	int verbose = 0;
	
	if (argc > 2)
		return usage(argv);

	if (argc == 2) {
		string argv1 = argv[1];
		if (argv1 == "-v")
			verbose = 1;
		else
			return usage(argv);
	}

	// Time to wait for the webpage to load in seconds.
	// Method std::to_string() requires --std=c++0x compilation flag
	int sleep_time = 20;
	string sleep_cmd = "sleep " + to_string(sleep_time) + " ";

	// Number of times you want to reach the webpage.
	int times_to_reach = 1;

	string ip_addr_used = ip_addr_yiping;

	// List of files (websites) to test
	deque<string> urls;
	//urls.push_back("hahaha.html");
	urls.push_back("leopard.html");
	//urls.push_back("waves.html");
	//urls.push_back("laposte/index.html");

	// Test all urls.
	for (deque<string>::const_iterator it = urls.begin(); 
			it != urls.end(); ++it) {
		string name = *it;
		replace(name.begin(), name.end(), '/', '.');
		printf("%s\n", (*it).c_str());
	
		// And test for each protocol
		for (int http2 = 0; http2 < 2; ++http2) {
			for (int is_secure = 0; is_secure < 2; ++is_secure) {

				// Cleaning up the cache
				LOG("rm -rf ~/.cache/chromium\n", verbose);
				system("rm -rf ~/.cache/chromium");
				
				string executable = chromium;
				string options = set_options(set_incognito,
					set_no_extensions, set_ignore_certificate_errors,
					set_disable_cache, http2, is_secure);

				string url = get_url(is_secure, ip_addr_used)+*it;

				string command = executable+options+url;

				string protocol = protocol_in_use(http2, is_secure);
				string log_file = protocol + "_" + name + ".log";
				string log1_file = protocol + "_" + name + "1.log";
				string log2_file = protocol + "_" + name + "2.log";

				// Log stderr to log_file, and everytime the 
				// command is run, we erase the content of the 
				// log_file (use of the '>' redirection).
				command += " > " + log_file + " 2>&1 ";
				command += "& " + sleep_cmd;
				command += "&& " + kill_last_bg_process;

				for (int i = 0; i < times_to_reach; ++i) {
					if (verbose)
						printf("%s\n", command.c_str());
					system(command.c_str());
					grep_load_times(log_file, log1_file, log2_file);
				}
				
				average_loading_time(log2_file, times_to_reach, http2,
					is_secure);

			}
		}
	}
	return 0;
}

string protocol_in_use(int http2, int is_secure) {
	if (http2) {
		if (is_secure)
			return "h2";
		else
			return "h2c";
	}
	else {
		if (is_secure)
			return "https";
		else
			return "http";
	}
	return "unknown";
}

string set_options(int set_incognito, int set_no_extensions,
	int set_ignore_certificate_errors, int set_disable_cache,
	int http2, int is_secure) {
	string result = "";
	if (set_incognito)
		result += incognito;
	if (set_no_extensions)
		result += no_extensions;
	if (set_ignore_certificate_errors)
		result += ignore_certificate_errors;
	if (set_disable_cache)
		result += disable_cache;
	if (http2) {
		result += enable_spdy4;
		if (is_secure)
			result += h2;
		else 
			result += h2c;
	}
	return result;
}

void LOG(const char* s, int verbose) {
	if (verbose)
		printf(s);
}

int usage(char* argv[]) {
	printf("Usage: %s [-v]\n", argv[0]);
	return 1;
}

string get_url(int is_secure, string ip_addr_used) {
	string result = "";
	if (is_secure) {
		result += scheme_https;
		result += ip_addr_used;
		result += port_https;
	}
	else {
		result += scheme_http;
		result += ip_addr_used;
		result += port_http;
	}
	return result;
}

int average_loading_time(string log2_file, int times_to_reach,
	int http2, int is_secure) {

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
		printf("\t%s = ", protocol_in_use(http2, is_secure).c_str());
		printf("%f\n", loading_time/times_to_reach);
	}
	else {
		printf("Unable to open file\n");
		return 1;
	}
	return 0;
}

int grep_load_times(string log_file, string log1_file,
	string log2_file) {
	// Stores first occurence of setNavigationStart.
	// I have never seen more than one. Notice that we are
	// using '>' character, not the append '>>'.
	system(("cat " + log_file + " | grep -m 1 \
			^setNavigationStart > "	+ log1_file).c_str());

	// Stores first occurent of markLoadEventEnd starting
	// from the end of the file (that's the purpose of the
	// 'tac' command!). Notice that we are using append
	// redirection '>>'; indeed we do not want to erase the
	// first timing which came from setNavigationStart.
	system(("tac " + log_file + " | grep -m 1 \
			^markLoadEventEnd  >> "	+ log1_file).c_str());
	
	// Stores the third column of the log1_file into 
	// log2_file: there should be numerous lines in log2_file
	// with a single number (double) on each line.
	system(("cat " + log1_file + " | awk '{print $3}' >> " +
			log2_file).c_str());
	return 0;
}


