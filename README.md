By Rheeca Guion and Chetana Musunuru

# Project E (Current)

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

# Previous Versions

## Project D

This project adds the following new features to the OS:
- List directory
- Delete a file
- Copy a file
- Create a file

#### What We Did
1. Created the writeSector function which writes the contents of the buffer to a specific sector.
2. Created the shell command `dir`, which lists the files in the directory.
3. Created the shell command `del`, which deletes a file. 
    >A:>del filename

    - The filename is removed from the directory and its sectors are set to zero on the map.
4. Created the shell command `copy`, which copies a file.
    >A:>copy filename1 filename2

    - This command will copy the file contents of filename1 into a new file called filename2.
5. Created the shell command `create`, which creates a new file.
    >A:>create filename1

    - After running the command, the user can type any text into the terminal, which will be saved in the new file. To finish typing, enter an empty line.

#### How To Verify
1. Execute `compileOS.sh` to compile the files and create `diskc.img`.
2. Run the simulator and load `diskc.img`.
3. This will start the shell which appears on the screen with `A:>`.
4. Run `dir`. This should list the files currently on the disk.
5. Run `del tstpr1`. Verify that the file was deleted by running `dir` again. The deleted file should no longer be listed.
6. Run `copy messag mess2`. This should create a new file called `mess2`. Verify that the file was copied by running `type mess2`. It should print the same contents as the `messag` file.
    - Another test: Copy the shell file and execute the copy. This should take you to the shell prompt.
7. Run `create textfl`. To verify, type a few lines of text into the terminal. Once finished, enter an empty line to return to the shell. The `dir` command should list the new file and the `type` command should print out the contents that were typed in earlier.

## Project C

This project adds the following new features to the OS:
- Read a file
- Execute a program
- Terminate (call the shell)

#### What We Did
1. Created the readFile function which takes a filename and returns the contents of the file.
2. Created the executeProgram function which takes the name of the program, finds the file, loads it into memory and executes it.
3. Created the terminate function which calls the shell.
4. Updated the Interrupt 0x21 handler to be able to read a file, execute a program, and terminate (which calls the shell).
5. Created the shell program, which can receive two commands: 
    - `type` - prints the given file's contents
    - `exec` - executes the program

#### How To Verify
1. Execute `compileOS.sh` to compile the files and create `diskc.img`.
2. Run the simulator and load `diskc.img`.
3. This will start the shell which appears on the screen with `A:>`.
4. Run `type messag`. This should output `If this message prints out, then your readFile function is working correctly!` to indicate that it is working.
5. Run `exec tstpr2`. This should output `tstpr2 is working!` to indicate that it is working.
6. Error handling: Entering invalid commands outputs an error message. Entering filenames that cannot be found outputs an error message.

## Project B

This OS is able to print to screen, read input from the user, and read from the sector.

#### What We Did
1. Print text to the screen using Interrupt 0x10.
2. Read input from the user using Interrupt 0x16. This also supports erasing a character using backspace.
3. Read from the sector using Interrupt 0x13.
4. Created an Interrupt 0x21, which can be called.
5. Implemented the Interrupt 0x21 handler to be able to provide printString, readString and readSector based on an input parameter.

#### How To Verify
*Note: printString, readString and readSector are all called from Interrupt 0x21 in the following demo*
1. Run the simulator and load `diskc.img`.
2. The OS will print a greeting to the screen. This demonstrates the printString function.
3. The OS will prompt the user for a line of text. Backspace is supported. After entering a line, the OS will print it back to the screen.
4. The OS will then read the message from the sector and print it to the screen. You can also change the text in `message.txt` and rerun the `compileOS.sh` to see a different sector message.

## Project A

A small kernel that prints "Hello World".
