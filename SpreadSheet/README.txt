Build Instruction
=================

To build just fire make command -

# make 

Make would create an executable by name kontagent_ss

Test Instruction
=================

$ ./kontagent_ss <CSV_FILE>> <target_file>

Sample test files are located in ./test/ directory.

Sample Run with Output
=======================

#./kontagent_ss test/inp2.csv  inp2.csv.out

Input Sheet: 
1,  2,  3, 
=A1,  =B1,  6, 
=B3,  8,  =C2, 

Evaluated Sheet: 
1, 2, 3, 
1, 2, 6, 
8, 8, 6, 

Written the result to file: inp2.csv.out
