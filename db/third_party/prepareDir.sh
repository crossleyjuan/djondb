#!/bin/sh

mkdir -p /var/djondb
chown cross /var/djondb
touch /etc/djondb.conf
chown cross /etc/djondb.conf
echo DATA_DIR=/var/djondb >> /etc/djondb.conf
