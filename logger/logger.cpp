#include <iostream>
#include <iomanip>
#include <fstream>
#include <SerialStream.h>
#include <sstream>
#include <stdio.h>
#include <sys/time.h>
#include <cmath>
#include <vector>
#include <map>
#include <algorithm>
#include <cstdlib>
#include <stdint.h>

using namespace std;
using namespace LibSerial;

#include "LibSerialHelper.h"

template<class T>
class Logger {
public:

Logger() {
	lineend = false;
	last_time = get_time();
	first_counter = true;
}

bool open_device(const char * device, int baudrate) {
	LibSerialHelper helper;
  helper.Open(usb0, "/dev/ttyUSB0", baudrate);
	return true;
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

uint16_t hex2uint16(char hex) {
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
		result += factor*hex2uint16(hex[i]);
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

	cerr << '#' << endl;
}

double uC_time2double(uint64_t overflow_c, uint16_t timer_state) {
	return ((double)overflow_c + ((double)timer_state)/256/256)*(1024.0*256.0*256.0/16000000.0);
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
		if (power < 230*63*3) {
			command1 << "echo \"put electricity.power " << tim << " " << power << " location=RZL \"  | nc -w 5 -q 0 labs.in.zekjur.net 4242";
			system(command1.str().c_str());
		}
	}
	system(command2.str().c_str());
	cout << "done." << endl;
	cout << command1.str() << endl << command2.str() << endl;
	
	last_overflow_counter = overflow_counter;
	last_timer_state = timer_state;
	last_pulse_counter = pulse_counter;
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
uint16_t timer_state, last_timer_state;
};

int main(int argc, char* argv[]) {

	Logger<float> my_log;

	if (argc < 3) {
		cerr << "Usage: " << argv[0] << " device baudrate" << endl;
		return 0;
	}

	int baudrate = 9600;
	{
		stringstream tmp(argv[2]);
		tmp >> baudrate;
	}

	my_log.open_device(argv[1], baudrate);
	my_log.logging();


	return 0;
}
