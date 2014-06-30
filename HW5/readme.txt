2014 Sogang Univ. Embedded System Software Assignment #5
Due date. 2014. 06. 30. (Mon)
Made by Lee Junho 20091648 (dangercloz@gmail.com)
=========================================================
Device driver used
- fnd_driver.ko			(major number : 241)
- led_dirver.ko			(major number : 240)
- fpga_fnd_driver.ko		(major number : 261)
- fpga_text_lcd_driver.ko	(major number : 263)
- fpga_led_driver.ko		(major number : 260)
- fpga_dot_driver.ko		(major number : 262)
- fpga_push_switch_driver.ko	(major number : 265)

By using insdev.sh file included, in minicom mode
sh insdev will insert all drivers automatically

=========================================================
Figure Switch Mode
- Refresh CPU usage every 1 second
- FPGA PUSH SWITCH 9 works as 0 on application

=========================================================
Puzzle Mode Score Calculation
- 30 second is given to solve puzzle
- 10 score for proper place for each button
- ex) 3 X 3 puzzle
	1 2 3
	4 6 5
	7 - 9	== (1+1+1+1+1) * 10 = 50

=========================================================
Custom Mode Introduction
- Press switch button on board then application will respond
- Or press button on application is fine
- Modify button shows how many buttons had pressed
