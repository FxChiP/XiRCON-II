# Microsoft Developer Studio Project File - Name="ctcp2_parser_test" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ctcp2_parser_test - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ctcp2_parser_test.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ctcp2_parser_test.mak" CFG="ctcp2_parser_test - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ctcp2_parser_test - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ctcp2_parser_test - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ctcp2_parser_test - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\Release\win32-ix86\"
# PROP Intermediate_Dir "..\..\build\Release\win32-ix86\ctcp2_parser_test\"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CTCP2_PARSER_TEST_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /O2 /I "d:\tcl_workspace\tcl_head_stock\generic" /I "..\..\cpptcl" /D "WIN32" /D "NDEBUG" /D "USE_TCL_STUBS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 /nologo /dll /machine:I386 /libpath:"..\..\build\Release\win32-ix86" /libpath:"C:\progra~1\tcl\lib"

!ELSEIF  "$(CFG)" == "ctcp2_parser_test - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\Debug\win32-ix86"
# PROP Intermediate_Dir "..\..\build\Debug\win32-ix86\ctcp2_parser_test"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CTCP2_PARSER_TEST_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "d:\tcl_workspace\tcl_head_stock\generic" /I "..\..\cpptcl" /D "WIN32" /D "_DEBUG" /D "USE_TCL_STUBS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"..\..\build\Debug\win32-ix86" /libpath:"d:\tcl_workspace\tcl_head_stock\win\release"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "ctcp2_parser_test - Win32 Release"
# Name "ctcp2_parser_test - Win32 Debug"
# Begin Group "tests"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\tests\all.tcl
# End Source File
# Begin Source File

SOURCE=..\tests\ansi.test
# End Source File
# Begin Source File

SOURCE=..\tests\ctcp2.test
# End Source File
# Begin Source File

SOURCE=..\tests\hydra.test
# End Source File
# Begin Source File

SOURCE=..\tests\ircii.test
# End Source File
# Begin Source File

SOURCE=..\tests\mirc.test
# End Source File
# End Group
# Begin Source File

SOURCE=..\generic\CTCP2Parse.cpp
# End Source File
# Begin Source File

SOURCE=..\generic\CTCP2Parse.hpp
# End Source File
# Begin Source File

SOURCE=..\generic\tclTestAdapter.cpp
# End Source File
# End Target
# End Project
