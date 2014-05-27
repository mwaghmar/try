#include <unistd.h>
#include<string>
#include <vector>
#include <iostream>
#include <string.h>



typedef std::vector<std::string> CmdLineArgs;

std::string getCommandOutPut(const std::string& cmd, CmdLineArgs& cmdArgs, 
								unsigned char* stdinput, const unsigned stdinputlength)
{
	int outfd[2];
	int infd[2];
	int oldstdin, oldstdout;

	pipe(outfd); // Where the parent is going to write to
	pipe(infd); // From where parent is going to read

	oldstdin = dup(0); // Save current stdin
	oldstdout = dup(1); // Save stdout

	close(0);
	close(1);

	dup2(outfd[0], 0); // Make the read end of outfd pipe as stdin
	dup2(infd[1], 1); // Make the write end of infd as stdout

	if(!fork()) {

		//std::cerr<< "No. of cmd args : " << cmdArgs.size() << std::endl;
		char **argv = new char*[2 + cmdArgs.size()];
		argv[0] =  (char*) cmd.c_str();
		int i = 1;
		for(CmdLineArgs::iterator itr = cmdArgs.begin(); itr != cmdArgs.end(); ++itr) { 
			argv[i++] = (char*) itr->c_str();
			std::cerr << "Arg : \n"  << itr->c_str() << "\n";
		}
		argv[i] = 0;

		close(outfd[0]); // Not required for the child
		close(outfd[1]);
		close(infd[0]);
		close(infd[1]);

		execv(argv[0],argv);
	}
	else {
		close(0); close(1);
		dup2(oldstdin, 0);
		dup2(oldstdout, 1);

		close(outfd[0]); // These are being used by the child
		close(infd[1]);

		if( stdinput && stdinputlength ) {
			write(outfd[1],stdinput, stdinputlength ); // Write to childâs stdin
			close(outfd[1]);
		}
		
		char input[1024] = { 0 };
		int bytesRead = 0;
		std::string retVal;
		while( bytesRead = read(infd[0], input, 1024) ) {
			retVal += std::string(input);
			memset(input, 0, sizeof(input));
		}

		//char input[1024] = { 0 };
		//input[read(infd[0], input, 1024)] = 0; // Read from childâs stdout
		//std::string retVal(input);
		
		return retVal;
	}
}

char* pk = "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCvC+m32GusrDvN5tQyAlr7SEcg\n"
"cyNTfLSHG14bgX2CQzz6z5saoYGZWI3blurN4M+yTLlYEQGcI5bqqoFy40V7b2UF\n"
"+UtU7hlXF+0041qDgN6iQGca19mizpBsdHycYgR4NaXcEt1P3JNOczX9HDTqdXSP\n"
"1wECQ1TmlDRLCFoqSwIDAQAB\n";

char* msg = "axgBjaGSkgJDFUkGkxLHlacLpcjaPeri";

char* res = "rLZKIXTVQHkE4arW1VV46kqKtHkLMIp954UiiTczoZJFV5WjrXjS3QD+yPStXvLk\n"
"24+qGhZCPyzKwIg7GYoalk2VbjGItmD8ICRWdiekiznDPuO1dBkJfs26Qk+DjV6h\n"
"LZvCm/oEgUOHe98qpJ24vsCuVaIFTj+ytjz80Xvcr9I=\n";

int main()
{
		CmdLineArgs javaArgs;
		javaArgs.push_back("checkSignature");
		javaArgs.push_back(std::string(pk));
		javaArgs.push_back(std::string(msg));
		javaArgs.push_back(std::string(res));

		std::string signVerificationMsg = 
				getCommandOutPut("/usr/bin/java", javaArgs, NULL, 0);

		std::cout << signVerificationMsg << std::endl;
		return 0;
}
