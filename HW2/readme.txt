2014 Sogang Univ. Embedded System Software Assignment #2
Due date. 2014. 05. 24. (Sat)
Made by Lee Junho 20091648 (dangercloz@gmail.com)
=========================================================
Execution Order

On target board

1. insmod dev_driver.ko
2. mknod /dev/dev_driver
3. ./app [1-100] [1-100] [0001-8000]
4. when you want to remove module
   rm /dev/dev_driver
   rmmod dev_driver

=========================================================
Driver Name : /dev/dev_driver
Major Number : 242
Minor Number : 0
