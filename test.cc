#ifndef TEST_HH
	#include "test.hh"
	#define TEST_HH
#endif

// TODO: Split this function in several parts: make it modular
// TODO: Handle the fact that you have to be root to change delay/latency
// TODO: Change the way we compute the page loading time, use the onload event
//		 fired by the browser
int main(int argc, char* argv[]) {
	
	clean_logs();

	deal_with_arguments(argc, argv);	
	set_delay();

	char d[4];
	sprintf(d, "%d", delay);
	string d_string = d;

	// Method std::to_string() requires --std=c++0x compilation flag
	string sleep_cmd = "sleep " + to_string(sleep_time) + " ";

	string path_to_logs = d_string+"/"+network+"/"+ip_addr_used;
	execute("mkdir -p "+path_to_logs);

	
	for (deque<string>::const_iterator it = urls.begin();it != urls.end(); ++it) {
		
		// *it = google/index.html and website = google.index.html
		string website = *it;
		replace(website.begin(), website.end(), '/', '.');

		string name_path = path_to_logs+"/"+website;

		string name = name_path;
		replace(name.begin(), name.end(), '/', '.');

		for (int proto = http; proto <= http2s; ++proto) {

			clean_cache();
			string log_file = name + "_" + stringFromProtocol(proto) + ".log";
			string log_file_1 = name + "_" + stringFromProtocol(proto) + "_1.log";
			string log_file_2 = name + "_" + stringFromProtocol(proto) + "_2.log";
			
			// Log stderr to log_file, and everytime the command is run, we erase
			// the content of the log_file (use of the '>' redirection).
			string command = chromium + set_options(proto) + get_url(proto, ip_addr_used) 
				+ *it + " > " + log_file + " 2>&1 " + "& " + sleep_cmd + "&& "
				+ kill_last_bg_process;

			for (int i = 0; i < times_to_reach; ++i) {
				execute(command);
				grep_load_times(log_file, log_file_1, log_file_2);
			}
			
			average_loading_time(log_file_2, times_to_reach, proto, name_path);

		}
	}

	concat_all_files();
	unset_delay();
	clean_logs();
	return 0;
}

int concat_all_files() {
	char d[4];
	sprintf(d, "%d", delay);
	string d_string = d;
	string path = d_string+"/"+network+"/"+ip_addr_used+".txt";
	ofstream outfile;
	outfile.open(path.c_str(), ios_base::app);
	outfile << "Website-Protocol http https h2c h2" << endl;

	for (deque<string>::const_iterator it = urls.begin(); 
			it != urls.end(); ++it) {
		string website = *it;
		replace(website.begin(), website.end(), '/', '.');
		string final_path = d_string+"/"+network+"/"+ip_addr_used+"/"+website;
		string delimiter = ".";
		string token = website.substr(0, website.find(delimiter));
		outfile << token+" ";
		
		ifstream myfile(final_path);
		if (myfile.is_open()) {
			string line;	
			for (int i = 0; i < 4; ++i) {
				getline(myfile, line);
				double loading_time = strtod(line.c_str(), NULL);
				outfile << loading_time << " ";
			}
		}
		outfile << endl;
		myfile.close();
	}
	outfile.close();

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
	printf("Usage: %s [-v] --ip <IP address> -s <time to wait> -t <times to reach> \
		-d <delay in ms> <interface> -C <target directory -r <webpage1.html \
		webpage2.html ...>\n", argv[0]);
	return 1;
}

string get_url(int proto, string ip_addr_used) {
	string result = "";
	if (proto == 1 or proto == 3) {
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

int average_loading_time(string log_file_2, int times_to_reach,
	int proto, string name_path) {

	string line;	
	ifstream myfile(log_file_2);

	if (myfile.is_open()) {
		ofstream outfile;
		outfile.open(name_path.c_str(), ios_base::app);

		double loading_time = 0;

		for (int i = 0; i < times_to_reach; ++i) {
			getline(myfile, line);
			double start = strtod(line.c_str(), NULL);
			getline(myfile, line);
			double end = strtod(line.c_str(), NULL);
			loading_time += end-start;
		}
		myfile.close();
		outfile << loading_time/times_to_reach << endl;
		outfile.close();
	}
	else {
		printf("Unable to open file\n");
		return 1;
	}
	return 0;
}

int grep_load_times(string log_file, string log_file_1,
	string log_file_2) {
	// Stores first occurence of setNavigationStart.
	// I have never seen more than one. Notice that we are
	// using '>' character, not the append '>>'.
	execute("cat " + log_file + " | grep -m 1 \
			^setNavigationStart > "	+ log_file_1);

	// Stores first occurent of markLoadEventEnd starting
	// from the end of the file (that's the purpose of the
	// 'tac' command!). Notice that we are using append
	// redirection '>>'; indeed we do not want to erase the
	// first timing which came from setNavigationStart.
	execute("tac " + log_file + " | grep -m 1 \
			^markLoadEventEnd  >> "	+ log_file_1);
	
	// Stores the third column of the log_file_1 into 
	// log_file_2: there should be numerous lines in log_file_2
	// with a single number (double) on each line.
	execute("cat " + log_file_1 + " | awk '{print $3}' >> " +
			log_file_2);
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
		delay = atoi(argv[i+1]);
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
	for (int i = 1; i < argc; ++i) {
		if ((result=check_arg(argc, argv, i)) == -1) {
			return 1;
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
	execute("rm -rf *.log");
}

void clean_cache() {
	execute("rm -rf ~/.cache/chromium");
}

void set_delay() {
	char d[4];
	sprintf(d, "%d", delay);
	if (delay!=0) {
		execute("sudo tc qdisc add dev "+interface+
				 " root netem delay "+d+"ms");
	}
}

void unset_delay() {
	if (delay!=0) {
		execute("sudo tc qdisc del root dev "+interface);
	}
}
