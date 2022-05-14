# OS-Pgm1
This is a program I made for CS 4348 Operating System Concepts.

Instructions: 
1. An Ubuntu 20.04 LTS installation is required for all programming assignments.
    If necessary, download and install VirtualBox (https://www.virtualbox.org/wiki/Downloads).
    Download the ISO for Ubuntu 20.04 LTS (https://ubuntu.com/download/desktop).
    Create an environment using the Ubuntu ISO
2. Write a multi-process (using fork(): https://linux.die.net/man/2/fork),
    C/C++ client/server program which uses message queues (https://linux.die.net/man/7/mq_overview) to transfer requests and results.
    The client (child) process makes requests and prints results
    The server (parent) process must then execute the proper function and return the results to the client.
    Functions available:
      getdomainname,
      gethostname, and
      uname
3. In the report, tie the output of the program to the Ubuntu installation in use.
4. Upload report and code here.


Feedback to Learner 2/27/22 10:52 PM
{Total: 87; Build: 10; Code: 34, Standard prototype for main not present. {int main( int argc, char *argv[] ); Argument names may differ.} Does not clean up (reap) child processes.; Comments: 16, Inline comments easier to read than single line block comments above each line. Insufficient inline comments.; Report: 27, Lists do not make good reports. Paragraphs with less than three sentences.}
