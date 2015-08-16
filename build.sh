#!/bin/bash -e
DIR="/home/vagrant/pebble-dev/app"
cd $DIR
sudo $PSDK build
sudo $PSDK install --phone 192.168.1.125