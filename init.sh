#!/bin/bash

apt-get update

mkdir /var/pebble-dev/
cd /var/pebble-dev/

wget -q http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/sdk2/PebbleSDK-3.2.1.tar.gz
tar -zxf PebbleSDK-3.2.1.tar.gz

echo 'export PATH=/var/pebble-dev/PebbleSDK-3.2.1/bin:$PATH' >> ~/.bash_profile
. ~/.bash_profile

wget -q http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/sdk/arm-cs-tools-ubuntu-universal.tar.gz

cd /var/pebble-dev/PebbleSDK-3.2.1
tar -zxf /var/pebble-dev/arm-cs-tools-ubuntu-universal.tar.gz

sudo apt-get install python-pip python2.7-dev -y
sudo pip install virtualenv

cd /var/pebble-dev/PebbleSDK-3.2.1
virtualenv --no-site-packages .env
source .env/bin/activate
pip install -r requirements.txt
deactivate

sudo apt-get install libsdl1.2debian libfdt1 libpixman-1-0 -y