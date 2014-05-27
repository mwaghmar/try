Description
============
	
    Facade class MailHeaderParser provides following two methods for parsing Received Headers-

	static void parseReceivedHeader(const char* headerFile, ReceivedHeaderList& rhList);
	static void parseReceivedHeader(const std::string& headerString, ReceivedHeaderList& rhList);

    Upon return rhList parameter holds parsed Received Headers.
    rhList is of type std::vector<ReceivedHeader>.
    After parsing this vector can be used to get the results of parsing.

    No cleanup method is required as I am using STL containers to get the result of parsing.  

=========
TO Build 
=========
just issue make command as - 

# make

make creates an executable file named readheaders

if make does NOT work for you then issue following command to build it manually 

# g++ -I. main.cpp ReceivedHeader.cpp MailHeaderParser.cpp -o readheaders

=========
To Test
=========
TestInputFiles directory contains sample header files that I used for testing purpose. 
You can use those files for testing.

Execute readheaders as 

# ./readheaders [<header_files>] 

Command line argument <header_files> is optional.
You can pass in one or more header files as command line arguments to the program.

OR

If you don't pass any argument then program will wait for the input.
You can either enter the header string or redirect input to the program.

# ./readheaders < header.txt



+++++++++++++++++++++++++++++++++++++++++++++++++++
Following are two sample runs of readheaders
+++++++++++++++++++++++++++++++++++++++++++++++++++

+++++++++++++
SAMPLE RUN 1
+++++++++++++

[root@localhost Submission]# ./readheaders < ./TestInputFiles/sample_email_headers.txt 
Enter your header String terminated by EOF i.e. ^D: 
##### Received Header #####

from=[68.230.240.27]
by=zest.cage.cloudmark.com
date=Fri, 08 Jan 2010 00:02:42 -0800

##### Received Header #####

from=eastrmimpi05.cox.net
by=eastrmmtai108.cox.net
date=Fri, 8 Jan 2010 03:02:42 -0500

##### Received Header #####

from=mail.filtermx.com
by=eastrmimpi05.cox.net
date=Fri, 08 Jan 2010 03:02:41 -0500


+++++++++++++
SAMPLE RUN 2
+++++++++++++

[root@localhost Submission]# ./readheaders ./TestInputFiles/*.txt

======================================
Parsing File: ./TestInputFiles/BadDate.txt
======================================
##### Received Header #####

from=eastrmimpi05.cox.net
by=eastrmmtai108.cox.net
date=Fri,


======================================
Parsing File: ./TestInputFiles/EmptyFile.txt
======================================

======================================
Parsing File: ./TestInputFiles/EmptyRcvdHeader.txt
======================================

======================================
Parsing File: ./TestInputFiles/GarbageFields.txt
======================================
##### Received Header #####

from=asdfasdfasdfasfa341243124123415432652356463465364563465364563463653634634634634563464366346345634635
by=zsfasfasfsaE232323232@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@
date=Fri, sdfasfasdfasfdasdfasdfa%%%%%%%%%%%%%%^^&&&&&&&**********+++++++++


======================================
Parsing File: ./TestInputFiles/Garbage.txt
======================================

======================================
Parsing File: ./TestInputFiles/NoRcvdHeaders.txt
======================================

======================================
Parsing File: ./TestInputFiles/OnlyBy.txt
======================================
##### Received Header #####

from=
by=eastrmmtai108.cox.net
date=


======================================
Parsing File: ./TestInputFiles/OnlyDate.txt
======================================
##### Received Header #####

from=
by=
date=Fri, 8 Jan 2010 03:02:42 -0500


======================================
Parsing File: ./TestInputFiles/OnlyFrom.txt
======================================
##### Received Header #####

from=mail.filtermx.com
by=
date=


======================================
Parsing File: ./TestInputFiles/sample_email_headers.txt
======================================
##### Received Header #####

from=[68.230.240.27]
by=zest.cage.cloudmark.com
date=Fri, 08 Jan 2010 00:02:42 -0800

##### Received Header #####

from=eastrmimpi05.cox.net
by=eastrmmtai108.cox.net
date=Fri, 8 Jan 2010 03:02:42 -0500

##### Received Header #####

from=mail.filtermx.com
by=eastrmimpi05.cox.net
date=Fri, 08 Jan 2010 03:02:41 -0500

