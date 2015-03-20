------------------------------------
Using DPCUTIL SDK
------------------------------------


To use SDK in Visual Studio:

1.  Make sure the Adept Suite is installed.

2.  Open the Visual Studio project that will be using the dpcutil API calls.

3.  In the project settings (under the Link tab), add dpcutil.lib to the object/library modules list.

4.  Open Tools->Options and click the Directories tab.  Add the directory containing dpcutil.h and dpcdefs.h under "Include files".  Add the directory containing dpcutil.lib under "Library files".

The dpcutil.dll is placed in the Windows/System32 directory when Adept Suite is installed.



------------------------------------
DPCDEMO building and running
------------------------------------


DPCDEMO is is a command-line windows application compiled using the Microsoft C++ compiler in Visual Studio 6.

To compile DPCDEMO:

1.  Open Visual Studio and create a new Win32 Console Application project.

2.  Add main.cpp and gendefs.h (found in the dpcdemo source directory) to the project

3.  Complete steps 3 and 4 in the sequence above (under "Using DPCUTIL SDK").

4.  Compile and run DPCDEMO


To use DPCDEMO:

Open a command prompt and get to the directory containing dpcdemo.exe.  Open dpcdemo using the following parameters:

Get register byte:	dpcdemo -g <register> 		
Put register byte:	dpcdemo -p <register> <data byte>
Load stream of bytes:	dpcdemo -l <register> <filename> <# bytes>
Save stream of bytes: 	dpcdemo -s <register> <filename> <# bytes>