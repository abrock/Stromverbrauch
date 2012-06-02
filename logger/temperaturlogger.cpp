#include <iostream>
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

using namespace std;
using namespace LibSerial;

#include "LibSerialHelper.h"

template<class T>
class Logger {
public:

Logger() {
	lineend = false;
	last_time = get_time();
	sensor_names["10b257290208008c"] = "Temperatur_aussen";
	sensor_names["287f085102000084"] = "Temperatur_Heizung_Ruecklauf";
	sensor_names["28d9075102000005"] = "Temperatur_Heizung_Vorlauf";
	sensor_names["101c0c2902080026"] = "Temperatur_Raum_Beamerplattform";
	sensor_names["101a712902080015"] = "Temperatur_Raum_Tafel";
	sensor_names["108e29ce010800f8"] = "Temperatur_Getraenkelager";
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

void put_line(stringstream& line) {
	if (line.str().size() > 10) {
		cerr << '#';
		string sensorid;
		T temperature;
		line >> sensorid;
		line >> temperature;
		push_temperature(sensorid, temperature);
	}
}

void push_temperature(const string & sensorid, T temperature ) {
	if (!isfinite(temperature)) {
		cerr << "Value from sensor " << sensorid << " is not finite!" << endl;
		return;
	}
	temp_sum[sensorid] += temperature;
	temp_count[sensorid]++;
}

void check_for_timediff() {
	string current_time = get_time();
	if (current_time == last_time) {
		return;
	}
	last_time = current_time;
	cout << "yay" << endl;
	
	fstream json;
	json.open("pachube.json", fstream::out);

	json << "{\"version\":\"1.0.0\",\"datastreams\":[" << endl;
	
	const time_t tim = time(NULL);
	bool firstval = true;
	for (typename map<string, T>::iterator it=temp_sum.begin() ; it != temp_sum.end(); it++) {
		if (temp_count[it->first] > 0) {
			write_log(it->first.c_str(), it->second/temp_count[it->first], tim);
		}
		if (firstval) {
			firstval = false;
		}
		else {
			json << ',';
		}
		if ((sensor_names.find(it->first) != sensor_names.end()) && isfinite(it->second) && temp_count[it->first] > 0) {
			json << "{\"id\":\"" << sensor_names[it->first] << "\", \"current_value\":\"" << it->second/temp_count[it->first] << "\"}" << endl;
		}
		it->second = 0;
		temp_count[it->first] = 0;
	}
	json << endl << "]}" << endl;
	json.flush();
	json.close();
	system("bash pachube.sh &");
}

void write_log(const char* filename, T temperature, const time_t tim) {
	stringstream name;
	name << "logs/" << filename;
	fstream log;
	log.open(name.str().c_str(), fstream::app | fstream::out);
	if (log.fail()) {
		cerr << "logfile fail!" << endl;
	}
	if (log.bad()) {
		cerr << "logfile bad!" << endl;
	}
	if (log.is_open()) {
		log << endl << tim << "\t" << temperature;
		if (log.fail()) {
			cerr << "logfile fail after writing!" << endl;
		}
		if (log.bad()) {
			cerr << "logfile bad after writing!" << endl;
		}
		log.flush();
		if (log.fail()) {
			cerr << "logfile fail after flushing!" << endl;
		}
		if (log.bad()) {
			cerr << "logfile bad after flushing!" << endl;
		}
	}
	else {
		cerr << "Couldn't open logfile for writing" << endl;
	}
log.close();
}

string get_time() {
	time_t tim=time(NULL);
	tm *now=localtime(&tim);
	stringstream time;
	time << now->tm_min;
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
map<string, T> temp_sum;
map<string, int> temp_count;
map<string, string> sensor_names;
string last_time;
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
