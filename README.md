# Assembler-Project
Assembler project for C lab course.
This project gets an Assembly code file and provides the binary representation of the code.
The code is supported on 32-bit Ubuntu virtual machine and might not work on other environments.
To run the project first type *make* in the terminal to create the run file.
After it type *./assembler file.am* to start the process. ( An example file can be found in the repository ).
The program outputs 4 files: .as .ent .ext and .ob .
.as is the code after macro expansion.
.ent is a list of entry variables and their storage location in the database.
.ent is a list of external variables and their storage location in the database.
.ob is the binary representation of the code converted to 5 sets of 4 bits to make it easier to read.
