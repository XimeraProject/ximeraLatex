

This readme applies to the tools used in the Xronos project, specifically: xronosfmt, xronosbatch, and xronosraw.

In order to build these tools you will need to install gcc and make.  On Ubuntu or other debian based Linux releases you can use apt-get to install these items.  Specifically "apt-get install gcc"  and "apt-get install make".

To build the tools change directory to where the source is located.  Then run "make".  A makefile has been provided as part of the source package.  To install the tools run "sudo make install".

The file ProblemConverter.sh has been provided to run the tools in the correct order.  The tools have many command line switches that modify behavior.  You may want to run each tool without any arguments to see the list of supported switches then modify ProblemConverter.sh as necessary.


