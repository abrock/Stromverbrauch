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

	Logger<float> my_log;

	if (argc < 3) {
		cerr << "Usage: " << argv[0] << " source dest" << endl;
		return 0;
	}

	my_log.analyze(argv[1], argv[2]);

	return 0;
}
