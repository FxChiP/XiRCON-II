# Microsoft Developer Studio Project File - Name="ctcp2_parser" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ctcp2_parser - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ctcp2_parser.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ctcp2_parser.mak" CFG="ctcp2_parser - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ctcp2_parser - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ctcp2_parser - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ctcp2_parser - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\build\Release\win32-ix86\"
# PROP Intermediate_Dir "..\..\build\Release\win32-ix86\ctcp2_parser\"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "ctcp2_parser - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\build\Debug\win32-ix86"
# PROP Intermediate_Dir "..\..\build\Debug\win32-ix86\ctcp2_parser"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "ctcp2_parser - Win32 Release"
# Name "ctcp2_parser - Win32 Debug"
# Begin Group "reference"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\reference\ansi.gif
# End Source File
# Begin Source File

SOURCE=..\reference\ctcp2.1.txt
# End Source File
# Begin Source File

SOURCE=..\reference\ctcp2.2.txt
# End Source File
# Begin Source File

SOURCE=..\reference\ctcp2.3.txt
# End Source File
# Begin Source File

SOURCE=..\reference\ctcp2.4.txt
# End Source File
# Begin Source File

SOURCE=..\reference\ctcp2.5.txt
# End Source File
# Begin Source File

SOURCE=..\reference\ctcp2.A.txt
# End Source File
# Begin Source File

SOURCE=..\reference\ctcp_std1.txt
# End Source File
# Begin Source File

SOURCE=..\reference\dcc_info.txt
# End Source File
# Begin Source File

SOURCE=..\reference\mIRC_colors.bmp
# End Source File
# Begin Source File

SOURCE=..\reference\rfc0822.txt
# End Source File
# End Group
# Begin Source File

SOURCE=..\generic\CTCP2Parse.cpp
# End Source File
# Begin Source File

SOURCE=..\generic\CTCP2Parse.hpp
# End Source File
# End Target
# End Project
