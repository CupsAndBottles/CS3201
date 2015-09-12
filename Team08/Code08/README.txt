README.txt
==================================================================

Team08
Static Program Analysis Tool (Prototype 1)
CS3201/2

Daren Tan       A0111855J
Cheong Yan Qin  A0101728M
Leon Duan       A0108453J
Su Gi Chandran  A0101642J
Tricia Tan      A0116733J
Lim Wei Cheng   A0101871N

==================================================================

This folder contains a Microsoft Visual Studio solution of a static 
program analyzer prototype,(SPA) for short. It is able to analyze 
programs written in a source language SIMPLE. This solution is 
currently integrated with Autotester to automate the input of source
and queries text files to get an xml result list.

The current release version of AutoTester.exe and SPA.lib is found in 
the Release folder that also contains a total of 8 queries and source 
texts (inclusive of the sample-queries and sample-source).

To initiate the current program, open a new CMD shell and direct to 
the Release folder. Then, initialize the AutoTester.exe in this form:

AutoTester Simple01-Source.txt Simple01-Queries.txt out.xml
