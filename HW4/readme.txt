2014 Sogang Univ. Embedded System Software Assignment #4
Due date. 2014. 06. 09. (Mon)
Made by Lee Junho 20091648 (dangercloz@gmail.com)
=========================================================
Calculation of CPU usage

[root@dangerCloz /root]# cat /proc/stat shows
CPU num		user mode	n-user mode	system mode	idle state
--------------------------------------------------------------------------
cpu		1714278		9666		631901		135528477
cpu0		842765		5302		372331		67721763
cpu1		871513		4364		259570		67806714
...

This represents used jiffies size since booting.

In order to calculate CPU usage, subtract idle time from total jiffies

Idle jiffies usage = (idle jiffies / (idle jiffies + user jiffies + nuser jiffies + system jiffies)) * 100

CPU usage = 100 - idle jiffies usage
=========================================================
Parsing start time interval : 1s
