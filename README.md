# Embedded System Software

**2014 Sogang University Embedded System Software**

This embedded system software is made for embedded linux system on ARM architecture in Ubuntu 13.10 system on x64 architecture. ACHRO 4210 Exynos (Huins board) is used as target board.

For more information, please contact to mail below:
<center>[dangercloz@gmail.com](mailto:dangercloz@gmail.com)</center>

## 1st Assignment
###Goal
By using device drivers and IPCs, implement stop-watch, text editor and custom mode

###Progress
**'14. 04. 08.**
- created basic structure of ICP between input-main-output processes using shared memory
- working on stop-watch using FND driver

**'14. 04. 10.**
- modified some of define variables (FND for mmap)
- basic stop watch function is working now
- renewed code for better readability

**'14. 04. 11.**
- when changed to mode2, input process self is held by mode 2
- found out /dev/input/event1 is blocking input device

**'14. 04. 12.**
- divided input process in two (event key process, input process)
- renewed code again for better modularity
- stop-watch mode (mode 1) is working properly
- still working on text editor mode (mode 2)
	- typing count display is not working properly (default value is set to 150. why?)
	- fpga_push_switch is working little awkwardly
	- need to implement clear lcd function and typing functions (alphabet & numeric)

**'14. 04. 13.**
- typing count works properly
- text editor mode (mode 2) works properly now
	- found issue : initial count is set to 3000. if button pressed, count works properly
- thinking about what to make for mode 3
