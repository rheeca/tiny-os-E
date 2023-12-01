//Rheeca Guion and Chetana Musunuru, 2023
void printString(char*);
void printChar(char);
void readString(char*);
void readSector(char*, int);
void readFile(char*, char*, int*);
void executeProgram(char*);
void terminate();
void writeSector(char*, int);
void deleteFile(char*);
void writeFile(char*, char*, int);
void killProcess(int);
void handleInterrupt21(int, int, int, int);
void handleTimerInterrupt(int, int);

// the process table
int processActive[8];
int processStackPointer[8];

int currentProcess;

void main() {
	char buffer[13312];
	int sectorsRead, i, dataseg;

	dataseg = setKernelDataSegment();
	for(i=0; i<8; i++) {
		processActive[i] = 0;
		processStackPointer[i] = 0xff00;
	}
	currentProcess = -1;
	restoreDataSegment(dataseg);

	makeInterrupt21();

	printString("\r\nStarting shell. Please wait...\r\n");
	interrupt(0x21, 4, "shell", 0, 0);
	makeTimerInterrupt();

	while(1);
}

void printString(char* chars) {
	while(*chars != '\0') {
		printChar(*chars);
		chars += 0x1;
	}
}

void printChar(char c) {
	int ah, al, ax;
	ah=0xe;
	al=c;
	ax=(ah*256)+al;
	interrupt(0x10, ax, 0, 0, 0);
}

void readString(char* chars) {
	char* start=chars;
	char c;
	while(1) {
		c = interrupt(0x16, 0, 0, 0, 0);
		if(c==0xd) break;
		if (c==0x8) {
			// handling for backspace key	
			if(chars!=start) {
				// erase character on screen
				printChar(0x8);
				printChar(0x20);
				printChar(0x8);
				// erase character in stored string
				chars -= 0x1;
				*chars = 0x20;
			}
		} else {
			printChar(c);
			*chars = c;
			chars += 0x1;
		}
	}
	// print newline
	printChar(c);
	printChar(0xa);

	// add null character to end of string
	*chars = 0x0;
}

void readSector(char* buffer, int sector) {
	int ah, al, bx, ch, cl, dh, dl, ax, cx, dx;
	ah = 2;
	al = 1;
	bx = buffer;
	ch = 0; 	// track number
	cl = sector+1;	// relative sector number
	dh = 0;		// head number
	dl = 0x80;

	ax = (ah*256)+al;
	cx = (ch*256)+cl;
	dx = (dh*256)+dl;
	buffer = interrupt(0x13, ax, bx, cx, dx);
}

void readFile(char* filename, buffer, int* sectorsRead) {
	char dir[512];
	int fileentry, i, fileFound;
	readSector(dir, 2);

	// look for filename in directory	
	for(fileentry=0; fileentry<512; fileentry+=32) {
		fileFound=1;
		for(i=0; i<6; i++) {
			if(!(filename[i]==dir[fileentry+i])) {
				fileFound=0;
				break;
			}
			if(filename[i]==0) break;
		}
		if(fileFound) {
			break;
		}
	}
	if(!fileFound) {
		*sectorsRead=0;
		return;
	}
	// load sectors
	for(i=0; i<26; i++) {
		if(dir[fileentry+6+i]==0) break;
		readSector(buffer, dir[fileentry+6+i]);
		*sectorsRead += 1;
		buffer += 512;
	}
}

void executeProgram(char* name) {
	char buffer[13312];
	int sectorsRead, i;
	int msgAddr = 0x0;
	int segment, dataseg, isActive, process;
	
	readFile(name, buffer, &sectorsRead);
	if(sectorsRead<=0) {
		printString("cannot find program to execute\r\n");
		return;
	}

	// look for free segment
	dataseg = setKernelDataSegment();
	for(process=0; process<8; process++) {
		isActive = processActive[process];
		if(!isActive) {
			break;
		}
	}
	restoreDataSegment(dataseg);
	segment = (process+2)*0x1000;
	
	// transfer to memory
	for(i=0; i<13312; i++) {
		putInMemory(segment, msgAddr, buffer[i]);
		msgAddr += 0x1;
	}

	initializeProgram(segment);
	dataseg = setKernelDataSegment();
	processActive[process] = 1;
	processStackPointer[process] = 0xff00;
	restoreDataSegment(dataseg);
}

void terminate() {
	int dataseg;
	dataseg = setKernelDataSegment();
	processActive[currentProcess] = 0;
	restoreDataSegment(dataseg);
	while(1);
}

void writeSector(char* buffer, int sector) {
	int ah, al, bx, ch, cl, dh, dl, ax, cx, dx;
	ah = 3;
	al = 1;
	bx = buffer;
	ch = 0; 	// track number
	cl = sector+1;	// relative sector number
	dh = 0;		// head number
	dl = 0x80;

	ax = (ah*256)+al;
	cx = (ch*256)+cl;
	dx = (dh*256)+dl;
	buffer = interrupt(0x13, ax, bx, cx, dx);
}

void deleteFile(char* filename) {
	char map[512];
	char dir[512];
	int fileentry, i, fileFound, sector;

	readSector(map, 1);
	readSector(dir, 2);

	for(fileentry=0; fileentry<512; fileentry+=32) {
		fileFound=1;
		for(i=0; i<6; i++) {
			if(!(filename[i]==dir[fileentry+i])) {
				fileFound=0;
				break;
			}
		}
		if(fileFound) {
			break;
		}
	}
	if(!fileFound) {
		return;
	}

	// remove filename from directory
	dir[fileentry]=0;
	writeSector(dir, 2);

	// free up sectors in the map
	for(i=0; i<26; i++) {
		sector = dir[fileentry+6+i];
		if(sector==0) break;
		map[sector] = 0;
	}
	writeSector(map, 1);
}

void writeFile(char* buffer, char* filename, int numberOfSectors) {
	char map[512];
	char dir[512];
	char bufSector[512];
	int fileentry, hasEmptyEntry, sector;
	int i, j, k;

	readSector(map, 1);
	readSector(dir, 2);

	// find empty entry in directory
	hasEmptyEntry = 0;
	for(fileentry=0; fileentry<512; fileentry+=32) {
		if(dir[fileentry]==0) {
			hasEmptyEntry=1;
			break;
		}
	}
	if(!hasEmptyEntry) {
		return;
	}

	// copy filename to directory
	for(i=0; i<6; i++) {
		if(filename[i]==0) {
			for(j=i; j<6; j++) {
				dir[fileentry+j] = 0;
			}
			break;
		}
		dir[fileentry+i] = filename[i];
	}

	// search through map for a free sector
	sector = 0;
	for(i=3; i<512; i++) {
		if(map[i] == 0) {
			// add sector to map and dir
			map[i] = 0xFF;
			dir[fileentry+6+sector]=i;
			
			// write 512 bytes from buffer into the sector
			for(j=0; j<512; j++) {
				bufSector[j] = buffer[(sector*512)+j];
			}
			writeSector(bufSector, i);

			sector++;
			if(sector >= numberOfSectors) break;
		}
	}

	// fill remaining bytes in directory entry to 0
	for(i=numberOfSectors; i<26; i++) {
		dir[fileentry+6+i] = 0;
	}

	writeSector(map, 1);
	writeSector(dir, 2);
}

void killProcess(int process) {
	int dataseg;
	dataseg = setKernelDataSegment();
	processActive[process] = 0;
	restoreDataSegment(dataseg);
}

void handleInterrupt21(int ax, int bx, int cx, int dx) {
	if(ax==0) {
		printString(bx);
	} else if(ax==1) {
		readString(bx);
	} else if(ax==2) {
		readSector(bx, cx);
	} else if(ax==3) {
		readFile(bx, cx, dx);
	} else if(ax==4) {
		executeProgram(bx);
	} else if(ax==5) {
		terminate();
	} else if(ax==6) {
		writeSector(bx, cx);
	} else if(ax==7) {
		deleteFile(bx);
	} else if(ax==8) {
		writeFile(bx, cx, dx);
	} else if(ax==9) {
		killProcess(bx);
	} else {
		printString("Invalid ax value.\r\n");
	}
}

void handleTimerInterrupt(int segment, int sp) {
	int dataseg, i;
	//printChar('T');
	//printChar('i');
	//printChar('c');

	dataseg = setKernelDataSegment();
	// draw active process numbers
	for(i=0; i<8; i++)
        {
                putInMemory(0xb800,60*2+i*4,i+0x30);
                if(processActive[i]==1)
                        putInMemory(0xb800,60*2+i*4+1,0x20);
                else
                        putInMemory(0xb800,60*2+i*4+1,0);
        }

	// scheduler
	if(currentProcess > -1) {
		processStackPointer[currentProcess] = sp;
	}
	while(1) {
		currentProcess++;
		if(currentProcess >= 8) currentProcess = 0;
		if(processActive[currentProcess]) break;
	}
	segment = (currentProcess+2)*0x1000;
	sp = processStackPointer[currentProcess];
	restoreDataSegment(dataseg);

	returnFromTimer(segment, sp);
}

