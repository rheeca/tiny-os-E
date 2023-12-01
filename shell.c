//Rheeca Guion and Chetana Musunuru, 2023
#define MAX_BUFFER 13312

int isCommand(char*, char*);

main() {
	char input[512];
	char buffer[MAX_BUFFER];
	char dir[512];
	char filename[7];
	char txtInput[512];
	int sectorsRead,txtLength, i, j, k;
	int process, processID;
	enableInterrupts();

	while(1) {
		// clear variables
		for(i=0; i<512; i++) {
			input[i] = '\0';
		}
		for(i=0; i<MAX_BUFFER; i++) {
			buffer[i] = '\0';
		}
		sectorsRead = 0;

		// wait for input
		syscall(0, "A:>");
		syscall(1, input);

		if(isCommand(input, "type")) {
			// print file contents
			syscall(3, input+5, buffer, &sectorsRead);
			if(sectorsRead<=0) syscall(0, "error: file not found\r\n");
			else {
				syscall(0, buffer);
				syscall(0, "\r\n");
			}
		} else if(isCommand(input, "exec")) {
			// execute a program
			syscall(3, input+5, buffer, &sectorsRead);
			if(sectorsRead<=0) syscall(0, "error: file not found\r\n");
			else {
				syscall(4, input+5, &processID);
			}

			// block shell until program terminates
			syscall(10, processID);
		} else if(isCommand(input, "dir")) {
			// list files in the directory
			syscall(2, dir, 2);
			for (i=0; i<512; i+=32) {
				if (dir[i] == '\0') continue;
				for (j=0; j<6; j++) {
					filename[j] = dir[i+j];
				}
				filename[6] = '\0';
				syscall(0, filename);
				syscall(0, "\r\n");
			}
		} else if(isCommand(input, "del")) {
			// delete a file
			syscall(7, input+4);
		} else if(isCommand(input, "copy")) {
			// copy a file
			for(i=5; i<512; i++) {
				if(input[i] == ' ') {
					input[i] = 0;
					i++;
					break;
				}
			}
			syscall(3, input+5, buffer, &sectorsRead); // filename1
			if(sectorsRead<=0) syscall(0, "error: file not found\r\n");

			syscall(8, buffer, input+i, sectorsRead); // filename2
		} else if(isCommand(input, "create")) {
			// create a text file
			txtLength = 0;
			for(i=0; i<26; i++) { // max file length is 26 sectors
				// clear line input
				for(j=0; j<512; j++) {
					txtInput[j] = '\0';
				}

				// read line
				syscall(1, txtInput);
				if(txtInput[0]=='\0') break;
				
				// add carriage return and newline to end of line
				for(j=0; j<510; j++) { // cap at 510 bytes to leave room for a carriage return and a newline
					if(txtInput[j]=='\0') {
						break;
					}
				}
				txtInput[j++] = '\r';
				txtInput[j++] = '\n';	

				// add line to buffer
				for(k=0; k<j; k++) {
					buffer[txtLength+k] = txtInput[k];
				}
				txtLength += j;
			}
			syscall(8, buffer, input+7, i);
		} else if(isCommand(input, "kill")) {
			process = input[5] - '0'; // convert to int (only for single digit)
			syscall(9, process);
		} else {
			syscall(0, "error: invalid command\r\n");
		}
	}
}

int isCommand(char* input, char* command) {
	while (*input != ' ' && *command != ' ' && *input != '\0' && *command != '\0'){
		if (*input != *command) {
			return 0;
		}
		input++;
		command++;
	}

	return 1;
}

