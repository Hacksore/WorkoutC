#!/bin/bash

apt-get update

mkdir /var/pebble-dev/
cd /var/pebble-dev/

wget -q http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/sdk2/PebbleSDK-3.2.1.tar.gz
tar -zxf PebbleSDK-3.2.1.tar.gz

wget -q http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/sdk/arm-cs-tools-ubuntu-universal.tar.gz

echo 'PSDK="/var/pebble-dev/PebbleSDK-3.2.1/bin/pebble"' >> /etc/environment

cd /var/pebble-dev/PebbleSDK-3.2.1
tar -zxf /var/pebble-dev/arm-cs-tools-ubuntu-universal.tar.gz

apt-get install python-pip python2.7-dev -y
pip install virtualenv

cd /var/pebble-dev/PebbleSDK-3.2.1
virtualenv --no-site-packages .env
source .env/bin/activate
pip install -r requirements.txt
deactivate

apt-get install libsdl1.2debian libfdt1 libpixman-1-0 -y

chmod a+x /home/vagrant/pebble-dev/build.sh
