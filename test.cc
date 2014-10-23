#ifndef TEST_HH
	#include "test.hh"
	#define TEST_HH
#endif

// TODO: Split this function in several parts: make it modular
// TODO: Handle the fact that you have to be root to change delay/latency
// TODO: Change the way we compute the page loading time, use the onload event
//		 fired by the browser
// TODO: Remove bash calls to the minimum (that is chromium calls)
//		 There is still one call to parse the logs
int main(int argc, char* argv[]) {
	
	clean_logs();
	deal_with_arguments(argc, argv);
	set_delay();

	for (deque<string>::const_iterator it = urls.begin();it != urls.end(); ++it) {
		
		string name = delay+"."+network+"."+ip_addr_used+"."+slash_to_dot(*it);

		for (int i = 0; i < times_to_reach; ++i) {
			for (int proto = http; proto <= http2s; ++proto) {
				clean_cache();
				string log_file = name + "." + stringFromProtocol(proto) + ".log";
				
				// Launches Chromium
				string command = chromium + set_options(proto) + get_url(proto) 
					+ *it + " > " + log_file + " 2>&1 " + "& sleep " + to_string(sleep_time)
					+ " && " + kill_last_bg_process;
				execute(command);
				grep_load_times(proto, name);
			}
		}
	}

	concat_all_files();
	unset_delay();
	clean_logs();
	return 0;
}

int concat_all_files() {

	ofstream of;
	of.open(delay+"."+network+"."+ip_addr_used+".txt", ios_base::app);
	of << "Website\\Protocol http https http2 http2s\n";

	for (deque<string>::const_iterator it = urls.begin(); it != urls.end(); ++it) {

		// Remove the .html at the end
		of << slash_to_dot(*it).substr(0, slash_to_dot(*it).find("."));
		
		string final_path = delay+"."+network+"."+ip_addr_used+"."+slash_to_dot(*it);
		ifstream myfile(final_path);
		if (myfile.is_open()) {
			string line;	
			for (int i = 0; i < 4; ++i) { // 4 lines for 4 protocols
				getline(myfile, line);
				of << " " << strtod(line.c_str(), NULL);
			}
		}
		of << endl;
		myfile.close();
	}
	of.close();

	return 0;
}

string set_options(int proto) {
	string result = "";
	if (set_incognito)
		result += incognito;
	if (set_no_extensions)
		result += no_extensions;
	if (set_ignore_certificate_errors)
		result += ignore_certificate_errors;
	if (set_disable_cache)
		result += disable_cache;

	switch (proto) {
		case http2s:
			result += enable_spdy4;
			result += h2;
			break;
		case http2:
			result += enable_spdy4;
			result += h2c;
			break;
		default:
			break;
	}
	
	return result;
}

void LOG(const char* s) {
	if (verbose)
		fprintf(stdout, "%s\n", s);
}

int usage(char* argv[]) {
		printf("Usage: %s \t[-v] \
		\n\t\t--ip <IP address> \
		\n\t\t-s <time to wait> \
		\n\t\t-t <times to reach> \
		\n\t\t-d <delay in ms> <interface> \
		\n\t\t-C <target directory \
		\n\t\t-r <webpage1.html webpage2.html ...>\n", argv[0]);
	return 1;
}

string get_url(int proto) {
	if (proto%2==1) // case it's a secure protocol
		return scheme_https+ip_addr_used+port_https;
	return scheme_http+ip_addr_used+port_http;
}

int grep_load_times(int proto, string name) {

	string log_file = name + "." + stringFromProtocol(proto) + ".log";
	string log_file_2 = "2_"+log_file;
	string line;	

	// Stores first occurence of setNavigationStart.
	// I have never seen more than one. Notice that we are
	// using '>' character, not the append '>>'.
	// Stores first occurent of markLoadEventEnd starting
	// from the end of the file (that's the purpose of the
	// 'tac' command!). Notice that we are using append
	// redirection '>>'; indeed we do not want to erase the
	// first timing which came from setNavigationStart.
	execute("cat " + log_file + " | grep -m 1 \
			^setNavigationStart | awk '{print $3}' > " + log_file_2
			+" && " + "tac " + log_file + " | grep -m 1 \
			^markLoadEventEnd | awk '{print $3}' >> "	+ log_file_2);

	ifstream myfile(log_file_2);

	if (myfile.is_open()) {
		ofstream outfile;
		outfile.open(name.c_str(), ios_base::app);

		double mean_loading_time = 0;

		for (int i = 0; i < times_to_reach; ++i) {
			getline(myfile, line);
			double start = strtod(line.c_str(), NULL);
			getline(myfile, line);
			double end = strtod(line.c_str(), NULL);
			mean_loading_time += end-start;
		}
		myfile.close();
		outfile << mean_loading_time/times_to_reach << endl;
		outfile.close();
	}
	else {
		printf("Unable to open file\n");
		return 1;
	}
	
	return 0;
}

int check_arg(int argc, char* argv[], int i) {
	if (strcmp(argv[i], "-v") == 0) {
		verbose=1;
		return 0;
	}
	else if (strcmp(argv[i], "--ip") == 0) {
		ip_addr_used = argv[i+1];
		return 1;
	}
	else if (strcmp(argv[i], "-s") == 0) {
		sleep_time = atoi(argv[i+1]);
		return 1;
	}
	else if (strcmp(argv[i], "-t") == 0) {
		times_to_reach = atoi(argv[i+1]);
		return 1;
	}
	else if (strcmp(argv[i], "-C") == 0) {
		network = argv[i+1];
		return 1;
	}
	else if (strcmp(argv[i], "-d") == 0) {
		delay = argv[i+1];
		interface = argv[i+2];
		return 2;
	}
	else if (strcmp(argv[i], "-r") == 0) {
		int number_of_urls = 0;
		while (++i < argc) {
			number_of_urls++;
			urls.push_back(argv[i]);
		}
		return number_of_urls;
	}
	else {
		return -1;
	}
	return 0;
}

int deal_with_arguments(int argc, char* argv[]) {
	int result;
	if (argc == 1) {
		usage(argv);
		exit(1);
	}
	for (int i = 1; i < argc; ++i) {
		if ((result=check_arg(argc, argv, i)) == -1) {
			usage(argv);
			exit(1);
		}
		i += result;
	}
	return 0;
}

int execute(string s) {
	LOG(s.c_str());
	return system(s.c_str());
}

void clean_logs() {
	execute("rm -rf *.log *.html");
}

void clean_cache() {
	execute("rm -rf ~/.cache/chromium");
}

void set_delay() {
	if (atoi(delay.c_str())!=0) {
		execute("sudo tc qdisc add dev "+interface+
				 " root netem delay "+delay+"ms");
	}
}

void unset_delay() {
	if (atoi(delay.c_str())!=0) {
		execute("sudo tc qdisc del root dev "+interface);
	}
}

string slash_to_dot(string input) {
	string output = input;
	replace(output.begin(), output.end(), '/', '.');
	return output;
}

