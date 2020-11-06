sudo kill `sudo lsof | grep tun | awk '{print $2}'`
