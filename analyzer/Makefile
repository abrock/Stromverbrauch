default:
	g++ -O2 -I../logger -I. -I../include -Wall -pedantic -g -o ana analyzer.cpp -lserial

plot:
	gnuplot power-changes.gpl
	gnuplot power-powerchange.gpl
	gnuplot powers.gpl
	gnuplot power-time.gpl
	gnuplot timerstate-power.gpl 
