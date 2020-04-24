# OpSys3
Project 3: Implementing a FAT32 File System

Group: Project3 20
Group Members: Charles Harris, Pedro Salazar, Spencer Lewis

File Listing:<br/>
Directory - OpSys3 contains:<br/>
  README.md - this file: includes file descriptions, compilation instructions, division of labor, and known bugs<br/>
  Makefile - Used to compile executable and remove executable when finished<br/>
  main.c - <br/>
  fat32.img - FAT32 file system image file used for demo<br/>
  commitLog.pdf - screenshot of commits made to git repository<br/>
    
Makefile Description:<br/>
In order to compile the executable, run:<br/>
make<br/>
In the directory<br/><br/>
Then, to run the executable, enter:<br/>
./a.out fat32.img<br/><br/>
To remove the executable run:<br/>
make clean<br/><br/>

Division of Labor:<br/>

  Charles Harris:<br/>
  info, size, ls, cd commands<br/>
  creat command<br/>
  mkdir command<br/>
  mv command<br/>
  read/write commands<br/>
  Commenting, Testing, and Reviewing other sections of project (Steps 2 and 3)<br/><br/>
  
  Pedro Salazar:<br/>
  exit command<br/>
  Makefile<br/>
  Additional work on creat and mkdir commands<br/>
  Commenting, Testing, and Reviewing other sections of project<br/><br/>
  
  Spencer Lewis:<br/>
  open command<br/>
  close command<br/>
  Additional work on read and write commands<br/>
  README file<br/>
  Commenting, Testing, and Reviewing other sections of project<br/><br/>
  
No Known Bugs at this time.
