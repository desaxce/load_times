#ifndef TEST_HH
	#include "test.hh"
	#define TEST_HH
#endif

// TODO: Handle the fact that you have to be root to change delay/latency
// TODO: Change the way we compute the page loading time, use the onload event
//		 fired by the browser: however that would not work with real website.
// TODO: Remove bash calls to the minimum (that is chromium calls)
//		 There is still one call to parse the logs.
// TODO: Before any testing, there should be at least one or two calls to chromium
//		 to ensure that it's loading correctly and fast enough
int main(int argc, char* argv[]) {
	
	clean_logs();
	deal_with_arguments(argc, argv);
	set_delay_and_losses();

	// Launch Chromium to warm up...
	execute(chromium + "--no-sandbox --user-data-dir & sleep 60 && " + kill_last_bg_process);

	for (deque<string>::const_iterator it = urls.begin();it != urls.end(); ++it) {
		
		string name = delay+"."+losses+"."+platform+"."+ip_addr_used+"."+slash_to_dot(*it);

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
		for (int proto = http; proto <= http2s; ++proto) {
			recap(proto, name);
		}	
	}

	concat_all_files();
	unset_delay_and_losses();
	clean_logs();
	return 0;
}

int concat_all_files() {

	ofstream of;
	of.open(delay+"."+losses+"."+platform+"."+ip_addr_used+".txt", ios_base::app);
	//of << "Website\\Protocol http https http2 http2s\n";

	of << times_to_reach << endl;
	for (deque<string>::const_iterator it = urls.begin(); it != urls.end(); ++it) {


		string final_path = delay+"."+losses+"."+platform+"."+ip_addr_used+"."+slash_to_dot(*it);
		ifstream myfile(final_path);
		if (myfile.is_open()) {
			string line;
			// Remove the .html at the end
			of << slash_to_dot(*it).substr(0, slash_to_dot(*it).find(".")) << endl;
			while(getline(myfile, line)) {
				of << line << endl;
			}
		}

		myfile.close();
	}
	of.close();

	return 0;
}

string set_options(int proto) {
	string result = "";
	if (set_incognito)
		result += incognito;
	if (set_no_browser_check)
		result += no_browser_check;
	if (set_no_extensions)
		result += no_extensions;
	if (set_ignore_certificate_errors)
		result += ignore_certificate_errors;
	if (set_disable_cache)
		result += disable_cache;

	switch (proto) {
		case http2s:
			result += enable_spdy4;
			//Below line had to be removed because you cannot interact with real
			//websites with this (dunno why)
			//result += h2;
			break;
		case http2:
			result += enable_spdy4;
			result += h2c;
			break;
		case http:
		case https:
			result += "--use-spdy=off ";
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
		printf("Usage: %s \t[-v, --verbose] \
		\n\t\t--ip <IP address> \
		\n\t\t-s, --sleep <time to wait> \
		\n\t\t-t, --times-to-reach <times to reach> \
		\n\t\t-d, --delay <delay in ms> <interface> \
		\n\t\t-C <target directory \
		\n\t\t-r, --reach <webpage1.html webpage2.html ...>\n", argv[0]);
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

	// Stores first occurence of setNavigationStart.
	// I have never seen more than one. Notice that we are
	// using the append '>>' both times.
	// Stores first occurent of markLoadEventEnd starting
	// from the end of the file (that's the purpose of the
	// 'tac' command!). Notice that we are using append
	// redirection '>>'; indeed we do not want to erase the
	// first timing which came from setNavigationStart.
	execute("cat " + log_file + " | grep -m 1 \
			^setNavigationStart | awk '{print $3}' >> " + log_file_2
			+" && " + "tac " + log_file + " | grep -m 1 \
			^markLoadEventEnd | awk '{print $3}' >> "	+ log_file_2);
	
	return 0;
}

int recap(int proto, string name) {

	string log_file = name + "." + stringFromProtocol(proto) + ".log";
	string log_file_2 = "2_"+log_file;
	string line;	

	ifstream myfile(log_file_2);

	if (myfile.is_open()) {
		ofstream outfile;
		outfile.open(name.c_str(), ios_base::app);
		for (int i = 0; i < times_to_reach; ++i) {
			getline(myfile, line);
			double start = strtod(line.c_str(), NULL);
			getline(myfile, line);
			double end = strtod(line.c_str(), NULL);
			outfile << end-start << " ";
		}
		outfile << endl;
		myfile.close();
		outfile.close();
	}
	else {
		printf("Unable to open file\n");
		return 1;
	}

	return 0;
}

int check_arg(int argc, char* argv[], int i) {
	if (strcmp(argv[i], "-v") == 0 or strcmp(argv[i], "--verbose") == 0) {
		verbose=1;
		return 0;
	}
	else if (strcmp(argv[i], "--ip") == 0) {
		ip_addr_used = argv[i+1];
		return 1;
	}
	else if (strcmp(argv[i], "-s") == 0 or strcmp(argv[i], "--sleep") == 0) {
		sleep_time = atoi(argv[i+1]);
		return 1;
	}
	else if (strcmp(argv[i], "-t") == 0 or strcmp(argv[i], "--times-to-reach") == 0) {
		times_to_reach = atoi(argv[i+1]);
		return 1;
	}
	else if (strcmp(argv[i], "-C") == 0) {
		platform = argv[i+1];
		return 1;
	}
	else if (strcmp(argv[i], "-d") == 0 or strcmp(argv[i], "--delay") == 0) {
		delay = argv[i+1];
		return 1;
	}
	else if (strcmp(argv[i], "-i") == 0 or strcmp(argv[i], "--interface") == 0) {
		interface = argv[i+1];
		return 1;
	}
	else if (strcmp(argv[i], "-l") == 0 or strcmp(argv[i], "--losses") == 0) {
		losses = argv[i+1];
		return 1;
	}
	else if (strcmp(argv[i], "-r") == 0 or strcmp(argv[i], "--reach") == 0) {
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

void set_delay_and_losses() {
	execute("sudo tc qdisc add dev "+interface+
			 " root netem delay "+delay+"ms loss "
			 +losses+"%");
}

void unset_delay_and_losses() {
	execute("sudo tc qdisc del root dev "+interface);
}

string slash_to_dot(string input) {
	string output = input;
	replace(output.begin(), output.end(), '/', '.');
	return output;
}

