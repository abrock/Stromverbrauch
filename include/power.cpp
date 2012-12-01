using namespace std;
using namespace LibSerial;

#include "LibSerialHelper.h"

#include "api-key.txt"

template<class T>
class Logger {
public:

Logger() {
	lineend = false;
	last_time = get_time();
	first_counter = true;
}

/*!
 * Open a tty for reading the data sent by the microcontroler
 */
bool open_device(const char * device, int baudrate) {
	LibSerialHelper helper;
  helper.Open(usb0, "/dev/ttyUSB0", baudrate);
	return true;
}

/*!
 * Analyze a given raw logfile
 */
void analyze(const char* file_source, const char* file_dest) {
	fstream source, dest, dest_invalid;
	source.open(file_source, fstream::in);
	dest.open(file_dest, fstream::out);
	dest_invalid.open(string(string(file_dest)+string("_invalid")).c_str(), fstream::out);
	bool has_previous = false, has_previous_power = false;
	uint64_t last_pulse_counter = 0, last_timestamp = 0, last_overflow_counter = 0, last_timer_state = 0;
	T last_uc_time = 0;
	T previous_power = 0;
	vector<T> powers, power_changes;
	while (!source.eof() && source) {
		const size_t buf_size = 1024*1024;
		char buf[buf_size];
		source.getline(buf, buf_size);
		stringstream line(buf);
		if (line.str().size() < 10) {
			continue;
		}
		uint64_t timestamp, overflow_counter, timer_state, pulse_counter;
		T uc_time;
		line >> timestamp;
		line >> uc_time;
		line >> overflow_counter;
		line >> timer_state;
		line >> pulse_counter;
		if (has_previous) {
			T power = 3.6*((T)pulse_counter-(T)last_pulse_counter)/(uc_time - last_uc_time);
			powers.push_back(power);
			if (!has_previous_power) {
				previous_power = power;
				has_previous_power = true;
			}
			T power_change = (power-previous_power)/(uc_time - last_uc_time);
			power_changes.push_back(power_change);
			
			// Check for invalid power and write to separate log
			if (fabs(3.6*((T)pulse_counter-(T)last_pulse_counter)) > fabs((uc_time-last_uc_time)*230.0*35.0*3.0)) {
				dest_invalid << timestamp << "\t" << uc_time << "\t" << overflow_counter << "\t" << timer_state << "\t" << pulse_counter << "\t" << power << "\t" << power_change << endl;
			}
			dest << timestamp << "\t" << uc_time << "\t" << overflow_counter << "\t" << timer_state << "\t" << pulse_counter << "\t" << power << "\t" << power_change << endl;
			previous_power = power;
		}
		else {
			has_previous = true;
		}
		last_timestamp = timestamp;
		last_uc_time = uc_time;
		last_overflow_counter = overflow_counter;
		last_timer_state = timer_state;
		last_pulse_counter = pulse_counter;

	}

	distribution_output(powers, file_dest, "_powers");
	distribution_output(power_changes, file_dest, "_power_changes");
}

void distribution_output(vector<T>& values, const char* prefix, const char* name) {
	if (values.size() == 0) {
		return;
	}
	sort(values.begin(), values.end());
	fstream out;
	out.open(string(string(prefix)+string(name)).c_str(), fstream::out);
	for (size_t i = 0; i < values.size(); i++) {
		out << values[i] << "\t" << (double)i/(double)values.size() << endl;
	}
	out.close();
}

void put_char(char c) {
	if (c == '\n' || c == '\r') {
		if (!lineend) {
			lineend = true;
			put_line(line);
			line.str(string());
			line.clear();
		}
	}
	else {
		lineend = false;
		line << c;
	}
}

bool is_hex(string s) {
	return (s.find_first_not_of("0123456789abcdefABCDEF") == string::npos);		
}

uint64_t hex2uint(char hex) {
	switch(hex) {
		case '1': return 1;
		case '2': return 2;
		case '3': return 3;
		case '4': return 4;
		case '5': return 5;
		case '6': return 6;
		case '7': return 7;
		case '8': return 8;
		case '9': return 9;
		case 'a': return 10;
		case 'b': return 11;
		case 'c': return 12;
		case 'd': return 13;
		case 'e': return 14;
		case 'f': return 15;
		case 'A': return 10;
		case 'B': return 11;
		case 'C': return 12;
		case 'D': return 13;
		case 'E': return 14;
		case 'F': return 15;
	}
	return 0;
}

uint64_t hex2uint64(string hex) {
	if (hex.length() == 0) {
		return 0;
	}
	uint64_t result = 0;
	uint64_t factor = 1;
	for (size_t i = hex.size()-1; i < hex.size(); i--) {
		result += factor*hex2uint(hex[i]);
		factor *= 16;
	}
	return result;
}

void put_line(stringstream& line) {
	// Check if line has correct length
	if (line.str().size() != 16+16+4+2) {
		return;
	}
	string _pulse_counter = line.str().substr(0,16);
	string _overflow_counter = line.str().substr(17,16);
	string _timer_state = line.str().substr(34,4);

	if (!is_hex(_pulse_counter) || !is_hex(_overflow_counter) || !is_hex(_timer_state)) {
		return;
	}

	pulse_counter = hex2uint64(_pulse_counter);
	overflow_counter = hex2uint64(_overflow_counter);
	timer_state = hex2uint64(_timer_state);


	if (first_counter) {
		last_pulse_counter = pulse_counter;
		last_overflow_counter = overflow_counter;
		last_timer_state = timer_state;
		first_counter = false;
	}
	fstream out;
	out.open("/var/www/strom/data/newlog", fstream::out | fstream::app);

	time_t tim = time(NULL);

	out << setprecision(20) << tim << "\t" << uC_time2double(overflow_counter, timer_state) << "\t" << overflow_counter << "\t" << timer_state << "\t" <<  pulse_counter << endl;
	out.close();

	cerr << '#' << endl;
}

double uC_time2double(uint64_t overflow_c, uint64_t timer_state) {
	return ((double)overflow_c + ((double)timer_state)/256.0/256.0)*(1024.0*256.0*256.0/16000000.0);
}

void check_for_timediff() {
	string current_time = get_time();
	if (current_time == last_time || first_counter) {
		return;
	}
	last_time = current_time;
	
	if (last_pulse_counter == pulse_counter) {
		return;
	}
	double last_uC_time = uC_time2double(last_overflow_counter, last_timer_state);
	double current_uC_time = uC_time2double(overflow_counter, timer_state);
	double timediff = current_uC_time - last_uC_time;
	
	if (last_pulse_counter > pulse_counter) {
		cerr << "uC-restart detected" << endl;
		first_counter = true;
		last_overflow_counter = overflow_counter;
		last_timer_state = timer_state;
		last_pulse_counter = pulse_counter;
		return;
	}


	
	
	stringstream command1, command2;
	time_t tim = time(NULL);
	command2 << "echo \"put electricity.consumption " << tim << " " << pulse_counter << " location=RZL \" | nc -w 5 -q 0 labs.in.zekjur.net 4242";
	cout << "sending data to server...";


	// Simple check for plausability
	if (timediff > 0) {
		double power = 3600.0*(double)(pulse_counter-last_pulse_counter)/timediff;
		if (power < 230*35*3) {
			command1 << "echo \"put electricity.power " << tim << " " << power << " location=RZL \"  | nc -w 5 -q 0 labs.in.zekjur.net 4242";
			//system(command1.str().c_str());
			push_cosm(power, pulse_counter);
		}
	}
	//system(command2.str().c_str());
	cout << "done." << endl;
	cout << command1.str() << endl << command2.str() << endl;

	
	last_overflow_counter = overflow_counter;
	last_timer_state = timer_state;
	last_pulse_counter = pulse_counter;
}

void push_cosm(T power, uint64_t pulse_counter)  {
	stringstream command;
	fstream out;
	out.open("datafile.txt", fstream::out);
	out << setprecision(14) << "Strom_Gesamtverbrauch," << round((double)pulse_counter/10.0)/100.0 << endl
			<< "Strom_Leistung," << (int32_t)round(power) << endl;
	out.close();
	command << "curl --request PUT --data-binary @datafile.txt --header \"X-ApiKey: " << API_KEY << "\" " << API_URL;
	system(command.str().c_str());
	cout << command.str() << endl;
}

string get_time() {
	time_t tim=time(NULL);
	tm *now=localtime(&tim);
	stringstream time;
	time << now->tm_min << "\t" << ((int)(now->tm_sec))/10;
	return time.str();
}

int logging() {
	while(true) {
		while(usb0.rdbuf()->in_avail() > 0){
			char c;
			usb0.get(c);
			put_char(c);
/*
			if (c == '\n' || c == '\r') {
				if (!lineend) {
					gettimeofday(&timestamp, NULL);
					lineend = true;
					cout << timestamp.tv_sec << ".";
					int usec = timestamp.tv_usec;
					for (int i = 100*1000; i > 0; i /= 10) {
						cout << usec/i;
						usec %= i;
					}
					cout << "\t" << line.str() << endl;
					line.str(string());
					line.clear();
				}
			}
			else {
				line << c;
				lineend = false;
			}
*/
		}
		check_for_timediff();
		usleep(10000);
	}
	return 0;
}

private:
SerialStream usb0;
bool lineend;
stringstream line;
string last_time;
bool first_counter;
uint64_t pulse_counter, last_pulse_counter;
uint64_t overflow_counter, last_overflow_counter;
uint64_t timer_state, last_timer_state;
};

