# Microsoft Developer Studio Project File - Name="toids" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=toids - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "toids.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "toids.mak" CFG="toids - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "toids - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "toids - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "toids - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "d:\tcl_workspace\tcl_head_stock\generic" /I "d:\tcl_workspace\tcl_head_stock\win" /I "d:\incrtcl_ws\incrTcl\itcl\generic" /I "..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "USE_TCL_STUBS" /D "USE_ITCL_STUBS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib /nologo /subsystem:windows /machine:I386 /libpath:"c:\progra~1\tcl\lib"

!ELSEIF  "$(CFG)" == "toids - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "d:\tcl_workspace\tcl_head_stock\generic" /I "d:\tcl_workspace\tcl_head_stock\win" /I "d:\incrtcl_ws\incrTcl\itcl\generic" /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "USE_TCL_STUBS" /D "USE_ITCL_STUBS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"c:\progra~1\tcl\lib"

!ENDIF 

# Begin Target

# Name "toids - Win32 Release"
# Name "toids - Win32 Debug"
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\FastInvoke.cpp
# End Source File
# Begin Source File

SOURCE=..\common\FastInvoke.hpp
# End Source File
# Begin Source File

SOURCE=..\common\gdiplus.itcl
# End Source File
# Begin Source File

SOURCE=..\common\GdiplusAdapter.cpp
# End Source File
# Begin Source File

SOURCE=..\common\GdiplusAdapter.hpp
# End Source File
# Begin Source File

SOURCE=..\common\GdiplusAdapterBitmap.cpp
# End Source File
# Begin Source File

SOURCE=..\common\GdiplusAdapterBrush.cpp
# End Source File
# Begin Source File

SOURCE=..\common\GdiplusAdapterFont.cpp
# End Source File
# Begin Source File

SOURCE=..\common\GdiplusAdapterGraphics.cpp
# End Source File
# Begin Source File

SOURCE=..\common\GdiplusAdapterMatrix.cpp
# End Source File
# Begin Source File

SOURCE=..\common\GdiplusAdapterPen.cpp
# End Source File
# Begin Source File

SOURCE=..\common\GdiplusAdapterRegion.cpp
# End Source File
# Begin Source File

SOURCE=..\common\GdiplusAdapterStringFormat.cpp
# End Source File
# Begin Source File

SOURCE=..\common\ItclAdapter.hpp
# End Source File
# Begin Source File

SOURCE=..\common\TclAdapter.hpp
# End Source File
# Begin Source File

SOURCE=..\common\TclHash.hpp
# End Source File
# Begin Source File

SOURCE=..\common\toids.itcl
# End Source File
# End Group
# Begin Source File

SOURCE=.\CustomGDIPAdapter.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomGDIPAdapter.hpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\tclsh.ico
# End Source File
# Begin Source File

SOURCE=.\tkcon.hst
# End Source File
# Begin Source File

SOURCE=.\toids.rc
# End Source File
# Begin Source File

SOURCE=.\toids.tcl
# End Source File
# End Target
# End Project
