#!/bin/bash

echo `date +%s` `tail -n 1 /var/www/strom/data/screenlog.0` >> /var/www/strom/data/log
