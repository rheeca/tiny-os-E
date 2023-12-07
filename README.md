# Project E
By Rheeca Guion and Chetana Musunuru

This project adds the following new features to the OS:
- Scheduler: Allows multiple processes to run at the same time.
- Shell commands:
    - Created `kill`
    - Modified `exec`
    - Created `execb`
- New kernel interrupts: `killProcess()`, `waitOnProcess()`

#### What We Did
1. Created the `handleTimerInterrupt()` function. Every time the timer goes off, this function is called and the scheduler runs. The scheduler looks for an active process in the process table and runs it.
2. Updated the `executeProgram()` function. This now looks for an inactive process in the process table and stores the program in the free segment. Then it sets the process to be active so that the scheduler will run it eventually.
3. Created the `kill` shell command, which ends the specified process by setting it to inactive in the process table. 
4. Modified the `exec` shell command, which executes a program and blocks the shell until the program has finished. This is done through the `waitForProcess()` interrupt.
5. Created the `execb` shell command, which executes a program as a background process, enabling multiple processes, including the shell, to run at the same time.
4. Created the `waitForProcess()` interrupt, which sets the current process to waiting on the specified process.

#### How To Verify
1. Execute `compileOS.sh` to compile the files and create `diskc.img`.
2. Run the simulator and load `diskc.img`.
3. This will start the shell which appears on the screen with `A:>`.
4. Run `execb number`. This should run the number program as a background process. Two processes should now be in green at the top of the screen.
5. Run `execb number` again. Three processes should now be in green at the top of the screen.
6. Run `exec letter`. This should run the letter program and cause the shell to wait until it is finished. You should not be able to type in the shell until all letters have been printed. The shell process (0) will now be red. Processes 1 and 2 (number) and 3 (letter) would be green.
7. After the letter program ends, its process number should also disappear from the top of the screen. The shell should also start running again, and process 0 should now be green.
8. Run `kill 1`. This should kill process 1, if it hasn't already ended.
9. Run `kill 2`. This should kill process 2, if it hasn't already ended.

