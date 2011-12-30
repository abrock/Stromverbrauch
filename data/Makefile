bb:
	scp Makefile job.sh getWeek.h plots plots.cpp  root@blackbox.raumzeitlabor.de:/var/www/strom/data

getlog:
	scp root@blackbox.raumzeitlabor.de:/var/www/strom/data/log .

plot:
	g++ -Wall -pedantic -O2 -o plots plots.cpp

clean:
	rm *.svg plots differences
