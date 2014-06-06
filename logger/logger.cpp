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

#include "power.cpp"

int main(int argc, char* argv[]) {


	typedef double T;
	Logger<T> my_log;

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
