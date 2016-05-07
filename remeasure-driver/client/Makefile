client-lib:
	gcc -Wall -c remeasure-client.c -I/usr/include/libnl3 -o remeasure-client.o
client-test: client-lib
	gcc -Wall -lnl-3 -lnl-genl-3 remeasure-client-test.c remeasure-client.o -o remeasure-client-test
switch-poller: client-lib
	gcc -Wall -lnl-3 -lnl-genl-3 remeasure-switch-poller.c remeasure-client.o -o remeasure-switch-poller
install:
	install -D -t "$(PREFIX)/etc/systemd/system/" remeasure-switch-poller.service && \
	install -D -t "$(PREFIX)/etc/remeasure/" handler.sh switch-poller.conf && \
	install -D -t "$(PREFIX)/usr/bin/" remeasure-switch-poller{,-runner}
