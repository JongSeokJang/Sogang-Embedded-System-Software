2014 Sogang Univ. Embedded System Software Assignment #3
Due date. 2014. 06. 09. (Mon)
Made by Lee Junho 20091648 (dangercloz@gmail.com)
=========================================================
Execution Order

On target board

1. insmod stopwatch.ko
2. mknod /dev/stopwatch
3. ./app
4. when you want to remove module
   rm /dev/stopwatch
   rmmod stopwatch.ko

=========================================================
Driver Name : /dev/stopwatch
Major Number : 245
Minor Number : 0
