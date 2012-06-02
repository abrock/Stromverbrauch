default:
	g++ -Wall -pedantic -g -o log temperaturlogger.cpp -lserial

deploy:
	scp pachube.sh plot.gpl LibSerialHelper.h temperaturlogger.cpp Makefile dockstar:~/logger/

getlog:
	scp -r dockstar:~/logger/logs .

plot:
	gnuplot plot.gpl

pushbb:
	scp temp1.svg temp2.svg bb:/var/www/temperatur/
