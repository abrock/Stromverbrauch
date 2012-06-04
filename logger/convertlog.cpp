#include <iostream>

using namespace std;

void trim(string& s) {
	size_t pos = 0;
	while (s.size() > pos && (s[pos] == ' ' || s[pos] == '\r' || s[pos] == '\n' || s[pos] == '\t')) {
		pos++;
	}
	s = s.erase(0, pos);
	if (s.size() < 1) {
		return;
	}
	pos = s.size() -1;
	while (pos > 0 && (s[pos] == ' ' || s[pos] == '\r' || s[pos] == '\n' || s[pos] == '\t')) {
		pos--;
	}
	s = s.substr(0,pos);
}

int main(void) {
	const size_t buf_size = 1024*1024;
	char buf[buf_size];
	while (!cin.eof()) {
		cin.getline(buf, buf_size);
		string line(buf);
		trim(line);
		if (line.length() > 0) {
			cout << "put electricity.consumption " << line << " location=RZL" << endl;
		}
	}
}
