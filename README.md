In order to compile this project, the TivaWare software will need to be downloaded and installed.

This can be found on TI's server. At the time of this writing, it is the SW-TM4C package here:
http://www.ti.com/tool/sw-tm4c

While only a Windows installer is available, the software can be accessed by changing the extension to .zip and unzipping the .exe.

Please do the following to link TivaWare:
1. Right click on the project name
2. Click Properties
3. Under Build->Variables(tab), change TIVAWARE_INSTALL to where you installed your SW-TM4C package.

The DriverLib will also have to be included if you are going to use TivaWare packages
1. Right click on the project name
2. Click Import -> Import File system
3. Open the TIVAWARE_INSTALL/driverlib/ccs/Debug directory
4. Import driverlib.lib - this can be linked or copied

The uartstdio.c and uartstdio.h files will need to be re-linked
Because of the way CCS interprets these files, they will need to be linked rather than copied.
1. Right click on the project
2. Click import - under general click File System
3. Under your TivaWare install select utils
4. Select uartstdio.c and .h
5. Click advanced and check the box for create links in workspace before clicking finish.

Project options should be applied, but if they aren't:
* Build->Arm Linker->Basic Options: Heap size should be set (arbitrarily set to 1024)
* Build->Arm Compiler->Advanced Options->Predefined Symbols: Pre-define NAME should have a line: TARGET_IS_BLIZZARD_RB1
* Build->Arm Compiler->Include Options: Add dir to #include search... should include "${TIVAWARE_INSTALL}"
