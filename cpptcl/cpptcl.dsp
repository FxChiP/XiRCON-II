# Microsoft Developer Studio Project File - Name="cpptcl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=cpptcl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cpptcl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cpptcl.mak" CFG="cpptcl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cpptcl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "cpptcl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cpptcl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\build\Release\win32-ix86\"
# PROP Intermediate_Dir "..\build\Release\win32-ix86\cpptcl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /O2 /I "C:\progra~1\tcl\include" /D "NDEBUG" /D "USE_TCL_STUBS" /D "WIN32" /D "_WINDOWS" /D "BUILD_cpptcl" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "cpptcl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\build\Debug\win32-ix86\"
# PROP Intermediate_Dir "..\build\Debug\win32-ix86\cpptcl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /MDd /W3 /GX /ZI /Od /I "C:\progra~1\tcl\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "BUILD_cpptcl" /D "USE_TCL_STUBS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "cpptcl - Win32 Release"
# Name "cpptcl - Win32 Debug"
# Begin Source File

SOURCE=.\config.hpp
# End Source File
# Begin Source File

SOURCE=.\cpptcl.hpp
# End Source File
# Begin Source File

SOURCE=.\FastInvoke.cpp
# End Source File
# Begin Source File

SOURCE=.\FastInvoke.hpp
# End Source File
# Begin Source File

SOURCE=.\ItclAdapter.hpp
# End Source File
# Begin Source File

SOURCE=.\TclAdapter.hpp
# End Source File
# Begin Source File

SOURCE=.\TclChannelStreambuf.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\TclChannelStreambuf.hpp
# End Source File
# Begin Source File

SOURCE=.\TclConsoleRedirect.cpp
# End Source File
# Begin Source File

SOURCE=.\TclConsoleRedirect.hpp
# End Source File
# Begin Source File

SOURCE=.\TclHash.hpp
# End Source File
# Begin Source File

SOURCE=.\TclSock.cpp

!IF  "$(CFG)" == "cpptcl - Win32 Release"

# ADD CPP /W3
# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "cpptcl - Win32 Debug"

# ADD CPP /Zi

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TclSock.hpp
# End Source File
# End Target
# End Project
