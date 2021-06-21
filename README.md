# counterd

A proof of concept for a program that daemonizes itself and counts to 10 while
logging to the system log.

## Build

```bash
$ make

gcc -Wall -Wextra -Wpedantic -O3 src/counterd.c -o bin/counterd
```

## Run

```bash
$ bin/counterd

# No output expected.
```

## See Logs

```bash
$ journalctl --identifier=bin/counterd

Jun 21 00:24:02 fsociety bin/counterd[24803]: Starting count
Jun 21 00:24:02 fsociety bin/counterd[24803]: count: 1
Jun 21 00:24:03 fsociety bin/counterd[24803]: count: 2
Jun 21 00:24:04 fsociety bin/counterd[24803]: count: 3
Jun 21 00:24:05 fsociety bin/counterd[24803]: count: 4
Jun 21 00:24:06 fsociety bin/counterd[24803]: count: 5
Jun 21 00:24:07 fsociety bin/counterd[24803]: count: 6
Jun 21 00:24:08 fsociety bin/counterd[24803]: count: 7
Jun 21 00:24:09 fsociety bin/counterd[24803]: count: 8
Jun 21 00:24:10 fsociety bin/counterd[24803]: count: 9
Jun 21 00:24:11 fsociety bin/counterd[24803]: count: 10
Jun 21 00:24:12 fsociety bin/counterd[24803]: Ending count
```
