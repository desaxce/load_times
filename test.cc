#ifndef TEST_HH
	#include "test.hh"
	#define TEST_HH
#endif

// TODO: Split this function in several parts: make it modular
// TODO: Handle the fact that you have to be root to change delay/latency

int main(int argc, char* argv[]) {
	
	// Parses command line arguments.
	deal_with_arguments(argc, argv);	

	// If there is some delay on the link
	if (strcmp(delay.c_str(), "0ms")!=0) {
		execute(("sudo tc qdisc add dev "+interface+
				 " root netem delay "+delay).c_str());
	}

	// Method std::to_string() requires --std=c++0x compilation flag
	string sleep_cmd = "sleep " + to_string(sleep_time) + " ";

	clean_logs();

	execute(("mkdir -p "+delay+"/"+network+"/"+ip_addr_used).c_str());


	// Test all given urls.
	for (deque<string>::const_iterator it = urls.begin();it != urls.end(); ++it) {
		
		// *it = google/index.html and website = google.index.html
		string website = *it;
		replace(website.begin(), website.end(), '/', '.');

		string name_path = delay+"/"+network+"/"+ip_addr_used+"/"+website;

		string name = name_path;
		replace(name.begin(), name.end(), '/', '.');

		// And test for each protocol
		for (int proto = http; proto <= http2s; ++proto) {

			clean_cache();
			
			string options = set_options(proto);

			string url = get_url(proto, ip_addr_used)+*it;

			string command = chromium+options+url;

			string protocol_in_use =  stringFromProtocol(proto);
			string log_file = name + "_" + protocol_in_use + ".log";
			string log1_file = name + "_" + protocol_in_use + "_1.log";
			string log2_file = name + "_" + protocol_in_use + "_2.log";

			// Log stderr to log_file, and everytime the 
			// command is run, we erase the content of the 
			// log_file (use of the '>' redirection).
			command += " > " + log_file + " 2>&1 ";
			command += "& " + sleep_cmd;
			command += "&& " + kill_last_bg_process;

			for (int i = 0; i < times_to_reach; ++i) {
				execute(command.c_str());
				grep_load_times(log_file, log1_file, log2_file);
			}
			
			average_loading_time(log2_file, times_to_reach, proto, name_path);

		}
	}
	concat_all_files();
	
	if (strcmp(delay.c_str(), "0ms")!=0) {
		execute(("sudo tc qdisc del root dev "+interface).c_str());
	}

	clean_logs();
	return 0;
}

int concat_all_files() {
	string path = delay+"/"+network+"/"+ip_addr_used+".txt";
	ofstream outfile;
	outfile.open(path.c_str(), ios_base::app);
	outfile << "Website-Protocol http https h2c h2" << endl;

	for (deque<string>::const_iterator it = urls.begin(); 
			it != urls.end(); ++it) {
		string website = *it;
		replace(website.begin(), website.end(), '/', '.');
		string final_path = delay+"/"+network+"/"+ip_addr_used+"/"+website;
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
	printf("Usage: %s [-v] <IP address> <time to wait>\n", argv[0]);
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

int average_loading_time(string log2_file, int times_to_reach,
	int proto, string name_path) {

	string line;	
	ifstream myfile(log2_file);
//	string results = log2_file + ".res";

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

int grep_load_times(string log_file, string log1_file,
	string log2_file) {
	// Stores first occurence of setNavigationStart.
	// I have never seen more than one. Notice that we are
	// using '>' character, not the append '>>'.
	execute(("cat " + log_file + " | grep -m 1 \
			^setNavigationStart > "	+ log1_file).c_str());

	// Stores first occurent of markLoadEventEnd starting
	// from the end of the file (that's the purpose of the
	// 'tac' command!). Notice that we are using append
	// redirection '>>'; indeed we do not want to erase the
	// first timing which came from setNavigationStart.
	execute(("tac " + log_file + " | grep -m 1 \
			^markLoadEventEnd  >> "	+ log1_file).c_str());
	
	// Stores the third column of the log1_file into 
	// log2_file: there should be numerous lines in log2_file
	// with a single number (double) on each line.
	execute(("cat " + log1_file + " | awk '{print $3}' >> " +
			log2_file).c_str());
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
	for (int i = 1; i < argc; ++i) {
		if ((result=check_arg(argc, argv, i)) == -1) {
			return 1;
		}
		i += result;
	}
	return 0;
}

int execute(const char* s) {
	LOG(s);
	return system(s);
}

void clean_logs() {
	execute("rm -rf *.log");
}

void clean_cache() {
	execute("rm -rf ~/.cache/chromium");
}

