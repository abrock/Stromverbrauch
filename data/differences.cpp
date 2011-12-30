#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

void calculate(const char* str_in, const char* str_out) {
	fstream in, out;
	in.open(str_in, fstream::in);
	out.open(str_out, fstream::out);

    
	string date, lastdate("");
	in >> date;

	int counter, lastcounter;
	
	in >> counter;
	lastcounter = counter;

	while(!in.eof()) {
		in >> date;
		in >> counter;
		if (date.compare(lastdate) != 0) {
			out << date << " " << counter-lastcounter << endl;
			lastcounter = counter;
		}
		lastdate = date;
	}

	in.close();
	out.close();
}

int main(void) {

	calculate("log", "log_diff");
	calculate("log10", "log10_diff");
	calculate("log30", "log30_diff");
	calculate("log60", "log60_diff");

	return 0;
}

