# Embedded System Software

**Sogang University 2014 Embedded System Software**

This embedded system software is made for embedded linux system on ARM architecture in Ubuntu 13.10 system on x64 architecture. ACHRO 4210 Exynos (Huins board) is used as target board.

For more information, please contact to mail below:
<center>[dangercloz@gmail.com](mailto:dangercloz@gmail.com)</center>

## 1st Assignment
###Goal
By using device drivers and IPCs, implement stop-watch, text editor and custom mode

###Progress
**'14. 04. 08.**
- created basic structure of IPC between input-main-output processes using shared memory
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

**'14. 04. 15.**
- replace original main file to re-factorized file
- better response of text typing (improvement of algoritm)
- free all shared memory after program terminates
- better performance overall with better refresh
- trying basic stuff on mode 3

**'14. 04. 16.**
- made custom mode with basic stuff (tired)
- some modification of memory allocations with devices closing

###Result
Completed all the requirements includeing custom mode with documentation


## 2nd Assignment
### Goal
Implement program with system call programming, module programming and device driver implementation

### Implementation
1. Create a module including device drivers and timer module
2. Create system call to return a variable with input of multiple parameters
3. Create application to print by using created module and system call

### Progress
**'14. 05. 07.**
- analyze requirements and plan for project

**'14. 05. 08.**
- analyze given examples of device driver and it's application

**'14. 05. 20.**
- creating basic frame work of module
- implemented system call (create a value from multiple parameters)
	- basic framework of application done

**'14. 05. 21.**
- application completed (not yet tested)
- system call for kernel completed
- module is still work is progress
	- finding hard to unite all device drivers

**'14. 05. 22.**
- most of module implementation is complete
	- start to working on text lcd and dot drivers
- application test complete
- system call test complete
- all module device integration complete and works properly
	- need to modify characters printed on the device

### Work in progress...
