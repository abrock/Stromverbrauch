#include <iostream>
#include <cstdlib>

using namespace std;

int main(void) {
	cerr << "Start" << endl;
	system("bash sleep.sh &");
	cerr << "Stop" << endl;
}
