# Microsoft Developer Studio Project File - Name="tes" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=tes - Win32 Release Static
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tes.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tes.mak" CFG="tes - Win32 Release Static"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tes - Win32 Release Static" (based on "Win32 (x86) Static Library")
!MESSAGE "tes - Win32 Debug Static" (based on "Win32 (x86) Static Library")
!MESSAGE "tes - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tes - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tes - Win32 Release Static"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "tes___Win32_Release_Static"
# PROP BASE Intermediate_Dir "tes___Win32_Release_Static"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib"
# PROP Intermediate_Dir "ReleaseS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "../../tcl8.1.0/generic" /I "../../Mcl/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "c:\progra~1\tcl\include" /I "..\..\mcl\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../lib/tes.lib" /link50compat
# ADD LIB32 /nologo /out:"..\..\lib\tess.lib" /link50compat

!ELSEIF  "$(CFG)" == "tes - Win32 Debug Static"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tes___Win32_Debug_Static"
# PROP BASE Intermediate_Dir "tes___Win32_Debug_Static"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib"
# PROP Intermediate_Dir "DebugS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../tcl8.1.0/generic" /I "../../Mcl/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "c:\progra~1\tcl\include" /I "..\..\mcl\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../lib/tesd.lib" /link50compat
# ADD LIB32 /nologo /out:"..\..\lib\tessd.lib" /link50compat

!ELSEIF  "$(CFG)" == "tes - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "tes___Win32_Release"
# PROP BASE Intermediate_Dir "tes___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "c:\progra~1\tcl\include" /I "..\..\mcl\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\include" /I "..\..\mcl\include" /I "d:\tcl_workspace\tcl_head_stock\generic" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "USE_TCL_STUBS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\tess.lib" /link50compat
# ADD LIB32 /nologo /link50compat

!ELSEIF  "$(CFG)" == "tes - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tes___Win32_Debug"
# PROP BASE Intermediate_Dir "tes___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "c:\progra~1\tcl\include" /I "..\..\mcl\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\mcl\include" /I "d:\tcl_workspace\tcl_head_stock\generic" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "USE_TCL_STUBS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\tessd.lib" /link50compat
# ADD LIB32 /nologo /out:"..\..\lib\tesd.lib" /link50compat

!ENDIF 

# Begin Target

# Name "tes - Win32 Release Static"
# Name "tes - Win32 Debug Static"
# Name "tes - Win32 Release"
# Name "tes - Win32 Debug"
# Begin Group "generic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\generic\TclEventSystemInt.cpp
# End Source File
# Begin Source File

SOURCE=..\generic\TclEventSystemInt.hpp
# End Source File
# End Group
# Begin Group "win"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\winFindTcl.cpp
# End Source File
# Begin Source File

SOURCE=.\winTclEventSystemInt.cpp
# End Source File
# Begin Source File

SOURCE=.\winTclEventSystemInt.hpp
# End Source File
# End Group
# Begin Group "tests"

# PROP Default_Filter "test"
# End Group
# Begin Source File

SOURCE=..\..\include\TclEventSystem.hpp
# End Source File
# End Target
# End Project
