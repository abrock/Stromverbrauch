#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <vector>
#include <map>
#include <stdlib.h>

using namespace std;

#include "getWeek.h"

void plotDay(int year, int month, int day, double price, int& daysum, ostream& plot, fstream& dayout) {
}

void plotMonth(int year, int month, double price, int& monthsum, ostream& plot, fstream& monthout) {
}

void execute() {
	fstream in;
	in.open("log", fstream::in);

	// Price per Wh
	const double price = 0.215/1000;

	// Time of the log entry and time of the log entry before.
	time_t time, lasttime;

	// counter reading of the log entry and of the log entry before.
	int counter, lastcounter;

	int week, lastweek = -16;

	int day, lastday = -16;

	int month, lastmonth = -16;

	int year, lastyear = -1000;

	int daysum = 0, weeksum = 0, monthsum = 0, yearsum = 0;

	double ex_day = false, ex_week = false, ex_month = false, ex_year = false;

	in >> lasttime;
	in >> lastcounter;

	fstream dayout, weekout, monthout, yearout, plot;
	plot.open("plots.sh", fstream::out);
	plot << "#!/bin/bash" << endl;

	stringstream ss_lastday;

	while (!in.eof()) {
		in >> time;
		in >> counter;

		if (time == lasttime) {
			continue;
		}

		double difference;
		if (counter >= lastcounter) {
			difference = counter - lastcounter;
		}
		else {
			difference = counter;
		}

		const double power = difference*3.6/(time-lasttime);

		daysum += difference;
		weeksum += difference;
		monthsum += difference;
		yearsum += difference;

		const struct tm * timeinfo;
		timeinfo = localtime(&time);
	
		week = getWeek(time);
		day = timeinfo->tm_mday;
		month = timeinfo->tm_mon+1;
		year = timeinfo->tm_year + 1900;
	
		dayout << time << "\t" << power << "\t" << daysum << endl;
		weekout << time << "\t" << power << "\t" << weeksum << endl;
		monthout << time << "\t" << power << "\t" << monthsum << endl;
		yearout << time << "\t" << power << "\t" << yearsum << endl;

		
		if (day != lastday) {
			dayout.close();
			if (ex_day) {
				stringstream data;
				data << "logs/" << lastyear << "-" << lastmonth << "-" << lastday << ".data";
				plot << "gnuplot -e \"set term svg enhanced; set output 'leistung-" << lastyear << "-" << lastmonth << "-" << lastday << ".svg'; set title '" << lastday << "." << lastmonth << "." << lastyear << " Gesamtverbrauch: " << (double)daysum/1000.0 << "kWh Kosten: " << daysum*price << " €'; set xlabel 'Uhrzeit'; set ylabel 'Leistung in kW'; set xdata time; set timefmt '%s'; set format x '%H'; plot '" << data.str() << "' using 1:2 with steps title '' \"" << endl;
				plot << "gnuplot -e \"set term svg enhanced; set output 'energie-" << lastyear << "-" << lastmonth << "-" << lastday << ".svg'; set title '" << lastday << "." << lastmonth << "." << lastyear << " Gesamtverbrauch: " << (double)daysum/1000.0 << "kWh Kosten: " << daysum*price << " €'; set xlabel 'Uhrzeit'; set ylabel 'Energie in kWh'; set xdata time; set timefmt '%s'; set format x '%H'; plot '" << data.str() << "' using 1:(\\$3/1000) with lines title '' \"" << endl;
			}
			stringstream data;
			data << "logs/" << year << "-" << month << "-" << day << ".data";
			daysum = 0;
			dayout.open(data.str().c_str(), fstream::out);
			ex_day = true;
		}

		if (month != lastmonth) {
			monthout.close();
			if (ex_month) {
				stringstream data;
				data << "logs/" << lastyear << "-" << lastmonth << ".data";
				monthout.open(data.str().c_str(), fstream::out);
				plot << "gnuplot -e \"set term svg; set output 'leistung-" << lastyear << "-" << lastmonth << ".svg'; set title 'Monat " << lastmonth << " " << lastyear << " " << "Verbrauch: " << monthsum/1000.0 << "kWh Kosten: " << monthsum*price << "€'; set xlabel 'Tag'; set ylabel 'Leistung in kW'; set xdata time; set timefmt '%s'; set format x '%d'; plot '" << data.str() << "' using 1:2 with steps title '' \"" << endl;
				plot << "gnuplot -e \"set term svg; set output 'energie-" << lastyear << "-" << lastmonth << ".svg'; set title 'Monat " << lastmonth << " " << lastyear << " " << "Verbrauch: " << monthsum/1000.0 << "kWh Kosten: " << monthsum*price << "€'; set xlabel 'Tag'; set ylabel 'Energie in kWh'; set xdata time; set timefmt '%s'; set format x '%d'; plot '" << data.str() << "' using 1:(\\$3/1000) with lines title '' \"" << endl;
			}

			stringstream data;
			data << "logs/" << year << "-" << month << ".data";
			monthout.open(data.str().c_str(), fstream::out);
			ex_month = true;
			monthsum = 0;
		}

		if (week != lastweek) {}

		if (year != lastyear) {}

		lasttime = time;
		lastcounter = counter;
		lastweek = week;
		lastday = day;
		lastmonth = month;
		lastyear = year;
	}
	
	{
		stringstream data;
		data << "logs/" << lastyear << "-" << lastmonth << "-" << lastday << ".data";
		plot << "gnuplot -e \"set term svg enhanced; set output 'leistung-" << lastyear << "-" << lastmonth << "-" << lastday << ".svg'; set title '" << lastday << "." << lastmonth << "." << lastyear << " Gesamtverbrauch: " << (double)daysum/1000.0 << "kWh Kosten: " << daysum*price << " €'; set xlabel 'Uhrzeit'; set ylabel 'Leistung in kW'; set xdata time; set timefmt '%s'; set format x '%H'; plot '" << data.str() << "' using 1:2 with steps title '' \"" << endl;
		plot << "gnuplot -e \"set term svg enhanced; set output 'energie-" << lastyear << "-" << lastmonth << "-" << lastday << ".svg'; set title '" << lastday << "." << lastmonth << "." << lastyear << " Gesamtverbrauch: " << (double)daysum/1000.0 << "kWh Kosten: " << daysum*price << " €'; set xlabel 'Uhrzeit'; set ylabel 'Energie in kWh'; set xdata time; set timefmt '%s'; set format x '%H'; plot '" << data.str() << "' using 1:(\\$3/1000) with lines title '' \"" << endl;
	}

	{
		stringstream data;
		data << "logs/" << lastyear << "-" << lastmonth << ".data";
		plot << "gnuplot -e \"set term svg; set output 'leistung-" << lastyear << "-" << lastmonth << ".svg'; set title 'Monat " << lastmonth << " " << lastyear << " " << "Verbrauch: " << monthsum/1000.0 << "kWh Kosten: " << monthsum*price << "€'; set xlabel 'Tag'; set ylabel 'Leistung in kW'; set xdata time; set timefmt '%s'; set format x '%d'; plot '" << data.str() << "' using 1:2 with steps title '' \"" << endl;
		plot << "gnuplot -e \"set term svg; set output 'energie-" << lastyear << "-" << lastmonth << ".svg'; set title 'Monat " << lastmonth << " " << lastyear << " " << "Verbrauch: " << monthsum/1000.0 << "kWh Kosten: " << monthsum*price << "€'; set xlabel 'Tag'; set ylabel 'Energie in kWh'; set xdata time; set timefmt '%s'; set format x '%d'; plot '" << data.str() << "' using 1:(\\$3/1000) with lines title '' \"" << endl;
	}
}

int main(void) {

	execute();	

	return 0;
}

