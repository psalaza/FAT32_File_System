# OpSys3
Priject 3: Implementing a FAT32 File System

Group: Project3 20
Group Members: Charles Harris, Pedro Salazar, Spencer Lewis

File Listing:<br/>
README.md - this file; includes file descriptions, compilation instructions, division of labor, and known bugs<br/>

Directory - SystemCalls contains:<br/>
  Makefile - Used to compile, declare new system calls for when kernel is compiled<br/>
  sys_start_elevator.c, sys_issue_request.c, sys_stop_elevator.c - Each file<br/>
  contains the syscall pointer and wrapper functions used to compile the added<br/>
  system calls
  
Directory - SyscallModule contains:<br/>
  Makefile - Used to compile kernel module and create proc/<br/>
  elevator.c - Contains majority of project code which includes:<br/>
     Kernel module elevator implementation <br/>
     Syscall STUBS and functions they call <br/>
     /Proc printing functions <br/>
    
Makefile Description:<br/>
In order to add module:<br/>
under /usr/src/SyscallModule run:<br/>
  sudo make<br/>
  insmod elevator.ko<br/><br/>
  
Then navigate to the directory that contains<br/>
the provided code (producer and consumer) and run:<br/>
  make<br/><br/>
  
Then, to produce passengers run:<br/>
  ./produce.x<br/><br/>
  
To start the elevator:<br/>
  ./consumer --start<br/><br/>
  
To view the visual representation of the elevator:<br/>
  cat /proc/elevator<br/><br/>
  
If you want to stop the elevator:<br/>
  ./consumer --stop<br/><br/>
  
To remove the module, navigate back to<br/>
/usr/src/SyscallModule and run:<br/>
  sudo rmmod elevator<br/><br/>

Division of Labor:<br/><br/>

  Charles Harris:<br/>
  System Call Tracing (Part 1)<br/>
  Elevator Scheduler (Step 1)<br/>
  Commenting, Testing, and Reviewing other sections of project (Steps 2 and 3)<br/><br/>
  
  Pedro Salazar:<br/>
  Kernel Module (Part 2)<br/>
  Elevator Scheduler (Step 3)<br/>
  Commenting, Testing, and Reviewing other sections of project<br/><br/>
  
  Spencer Lewis:<br/>
  Elevator Scheduler (Step 2)<br/>
  README.md and Documentation<br/>
  Commenting, Testing, and Reviewing other sections of project<br/><br/>
  
Known Bugs:<br/>
  1) If you rapidly and repeatedly call the proc/ for a few minutes after<br/>
  the elevator has stopped, there is a small chance for a segmentation fault.<br/>
  We do not know if we fixed this or not but it went away.
