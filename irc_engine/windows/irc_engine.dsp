# Microsoft Developer Studio Project File - Name="irc_engine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=irc_engine - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "irc_engine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "irc_engine.mak" CFG="irc_engine - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "irc_engine - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "irc_engine - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "irc_engine - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\out\Release\win32-ix86\"
# PROP BASE Intermediate_Dir "..\..\build\Release\win32-ix86\irc_engine\"
# PROP BASE Target_Dir "..\..\out\Release\win32-ix86\"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\Release\win32-ix86\"
# PROP Intermediate_Dir "..\..\build\Release\win32-ix86\irc_engine\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "..\..\out\Release\win32-ix86\"
# ADD BASE CPP /nologo /G5 /MD /W3 /GX /O2 /I "c:\progra~1\tcl\include" /I "..\..\cpptcl" /I "..\generic" /I "d:\python22\include" /I "d:\perl\lib\CORE" /I "d:\jdk1.4\include" /I "d:\jdk1.4\include\win32" /D "NDEBUG" /D "WIN32" /D "USE_TCL_STUBS" /D "USE_ITCL_STUBS" /D IRCE_USEPYTHON=1 /D IRCE_USEPERL=1 /D IRCE_USEJAVA=1 /YX"IRCEngine.hpp" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /O2 /I "d:\tcl_workspace\tcl_84_branch\generic" /I "d:\incrtcl_ws\incrTcl\itcl\generic" /I "..\..\cpptcl" /I "..\..\ctcp2_parser\generic" /I "..\generic" /I "d:\python22\include" /I "d:\perl\lib\CORE" /I "d:\jdk1.4\include" /I "d:\jdk1.4\include\win32" /D "NDEBUG" /D "WIN32" /D "USE_TCL_STUBS" /D "USE_ITCL_STUBS" /D IRCE_USEPYTHON=0 /D IRCE_USEPERL=0 /D IRCE_USEJAVA=0 /YX"IRCEngine.hpp" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /dll /machine:IX86 /machine:I386"
# ADD LINK32 /nologo /dll /pdb:none /machine:I386 /libpath:"c:\progra~1\tcl\lib" /libpath:"d:\python22\libs" /libpath:"d:\perl\lib\CORE" /opt:nowin98
# Begin Special Build Tool
WkspDir=.
SOURCE="$(InputPath)"
PostBuild_Cmds=cd $(WkspDir)\out\Release\win32-ix86	del irc_engine.exp irc_engine.lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "irc_engine - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\out\Debug\win32-ix86\"
# PROP BASE Intermediate_Dir "..\..\build\Debug\win32-ix86\irc_engine\"
# PROP BASE Target_Dir "..\..\out\Debug\win32-ix86\"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\Debug\win32-ix86"
# PROP Intermediate_Dir "..\..\build\Debug\win32-ix86\irc_engine"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "..\..\out\Debug\win32-ix86\"
# ADD BASE CPP /nologo /G5 /MDd /W3 /Gm /Gi /GX /ZI /Od /I "c:\progra~1\tcl\include" /I "..\..\cpptcl" /I "..\generic" /I "d:\python22\include" /I "d:\perl\lib\CORE" /I "d:\jdk1.4\include" /I "d:\jdk1.4\include\win32" /D "_DEBUG" /D "WIN32" /D "USE_TCL_STUBS" /D "USE_ITCL_STUBS" /D IRCE_USEPYTHON=0 /D IRCE_USEPERL=0 /D IRCE_USEJAVA=0 /FR /YX"IRCEngine.hpp" /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /Gi /GX /ZI /Od /I "d:\tcl_workspace\tcl_84_branch\generic" /I "d:\incrtcl_ws\incrTcl\itcl\generic" /I "..\..\cpptcl" /I "..\..\ctcp2_parser\generic" /I "..\generic" /I "d:\python22\include" /I "d:\perl\lib\CORE" /I "d:\jdk1.4\include" /I "d:\jdk1.4\include\win32" /D "_DEBUG" /D "WIN32" /D "USE_TCL_STUBS" /D "USE_ITCL_STUBS" /D IRCE_USEPYTHON=0 /D IRCE_USEPERL=0 /D IRCE_USEJAVA=0 /YX"IRCEngine.hpp" /FD /GZ /c
# SUBTRACT CPP /WX /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /dll /debug /machine:I386 /pdbtype:sept"
# ADD LINK32 /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"c:\progra~1\tcl\lib" /libpath:"d:\python22\libs" /libpath:"d:\perl\lib\CORE"

!ENDIF 

# Begin Target

# Name "irc_engine - Win32 Release"
# Name "irc_engine - Win32 Debug"
# Begin Group "generic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\ctcp2_parser\generic\CTCP2Parse.cpp
# End Source File
# Begin Source File

SOURCE=..\generic\CTCP2ParserAction.hpp
# End Source File
# Begin Source File

SOURCE=..\generic\Engine.java

!IF  "$(CFG)" == "irc_engine - Win32 Release"

!ELSEIF  "$(CFG)" == "irc_engine - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\generic\IRCChanInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\generic\IRCChanInfo.hpp
# End Source File
# Begin Source File

SOURCE=..\generic\IRCEngine.cpp
# End Source File
# Begin Source File

SOURCE=..\generic\IRCEngine.hpp
# End Source File
# Begin Source File

SOURCE=..\generic\IRCEngineItclAdapter.cpp
# End Source File
# Begin Source File

SOURCE=..\generic\IRCEvents.hpp
# End Source File
# Begin Source File

SOURCE=..\generic\IRCParse.cpp
# End Source File
# Begin Source File

SOURCE=..\generic\IRCParse.hpp
# End Source File
# Begin Source File

SOURCE=..\generic\IRCSplitAndQ.cpp
# End Source File
# Begin Source File

SOURCE=..\generic\IRCSplitAndQ.hpp
# End Source File
# Begin Source File

SOURCE=..\generic\IRCUserScriptProvider.hpp
# End Source File
# Begin Source File

SOURCE=..\generic\IRCUserScriptProviderForJava.cpp
# End Source File
# Begin Source File

SOURCE=..\generic\IRCUserScriptProviderForPerl.cpp
# End Source File
# Begin Source File

SOURCE=..\generic\IRCUserScriptProviderForPython.cpp
# End Source File
# Begin Source File

SOURCE=..\generic\IRCUserScriptProviderForTcl.cpp
# End Source File
# Begin Source File

SOURCE=..\generic\IRCUserScriptProviderNub.cpp
# End Source File
# Begin Source File

SOURCE=..\generic\IRCUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\generic\IRCUtils.hpp
# End Source File
# Begin Source File

SOURCE=..\generic\JavaCore.cpp
# End Source File
# Begin Source File

SOURCE=..\generic\JavaCore.hpp
# End Source File
# Begin Source File

SOURCE=..\generic\numeric.h
# End Source File
# Begin Source File

SOURCE=..\generic\PerlCore.cpp
# End Source File
# Begin Source File

SOURCE=..\generic\PerlCore.hpp
# End Source File
# Begin Source File

SOURCE=..\generic\PythonCore.cpp
# End Source File
# Begin Source File

SOURCE=..\generic\PythonCore.hpp
# End Source File
# Begin Source File

SOURCE=..\generic\tclTestAdapter.cpp
# End Source File
# End Group
# Begin Group "win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\irc_engine.rc
# End Source File
# Begin Source File

SOURCE=.\IRCUserScriptProviderForDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\winDllMain.c
# End Source File
# End Group
# Begin Group "tests"

# PROP Default_Filter ".test"
# Begin Source File

SOURCE=..\tests\ctcp.test
# End Source File
# Begin Source File

SOURCE=..\tests\rfc1459.test
# End Source File
# Begin Source File

SOURCE=..\tests\unitest.tcl
# End Source File
# End Group
# Begin Group "library"

# PROP Default_Filter ""
# Begin Group "msgs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\library\msgs\de.msg

!IF  "$(CFG)" == "irc_engine - Win32 Release"

!ELSEIF  "$(CFG)" == "irc_engine - Win32 Debug"

# Begin Custom Build - updating library files
OutDir=.\..\..\out\Debug\win32-ix86
InputPath=..\library\msgs\de.msg
InputName=de

"$(OutDir)\msgs\$(InputName).msg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(OutDir)\msgs

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\library\msgs\en.msg

!IF  "$(CFG)" == "irc_engine - Win32 Release"

!ELSEIF  "$(CFG)" == "irc_engine - Win32 Debug"

# Begin Custom Build - updating library files
OutDir=.\..\..\out\Debug\win32-ix86
InputPath=..\library\msgs\en.msg
InputName=en

"$(OutDir)\msgs\$(InputName).msg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(OutDir)\msgs

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\library\msgs\es.msg

!IF  "$(CFG)" == "irc_engine - Win32 Release"

!ELSEIF  "$(CFG)" == "irc_engine - Win32 Debug"

# Begin Custom Build - updating library files
OutDir=.\..\..\out\Debug\win32-ix86
InputPath=..\library\msgs\es.msg
InputName=es

"$(OutDir)\msgs\$(InputName).msg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(OutDir)\msgs

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\library\msgs\es_BR.msg

!IF  "$(CFG)" == "irc_engine - Win32 Release"

!ELSEIF  "$(CFG)" == "irc_engine - Win32 Debug"

# Begin Custom Build - updating library files
OutDir=.\..\..\out\Debug\win32-ix86
InputPath=..\library\msgs\es_BR.msg
InputName=es_BR

"$(OutDir)\msgs\$(InputName).msg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(OutDir)\msgs

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\library\msgs\fr.msg

!IF  "$(CFG)" == "irc_engine - Win32 Release"

!ELSEIF  "$(CFG)" == "irc_engine - Win32 Debug"

# Begin Custom Build - updating library files
OutDir=.\..\..\out\Debug\win32-ix86
InputPath=..\library\msgs\fr.msg
InputName=fr

"$(OutDir)\msgs\$(InputName).msg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(OutDir)\msgs

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\library\msgs\ru.msg

!IF  "$(CFG)" == "irc_engine - Win32 Release"

!ELSEIF  "$(CFG)" == "irc_engine - Win32 Debug"

# Begin Custom Build - updating library files
OutDir=.\..\..\out\Debug\win32-ix86
InputPath=..\library\msgs\ru.msg
InputName=ru

"$(OutDir)\msgs\$(InputName).msg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(OutDir)\msgs

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\library\default.tcl

!IF  "$(CFG)" == "irc_engine - Win32 Release"

# Begin Custom Build - updating library files
OutDir=.\..\..\out\Release\win32-ix86\ 
InputPath=..\library\default.tcl

"$(OutDir)\default.tcl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(OutDir)

# End Custom Build

!ELSEIF  "$(CFG)" == "irc_engine - Win32 Debug"

# Begin Custom Build - updating library files
OutDir=.\..\..\out\Debug\win32-ix86
InputPath=..\library\default.tcl
InputName=default

"$(OutDir)\$(InputName).tcl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(OutDir)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\library\irc_engine.itcl

!IF  "$(CFG)" == "irc_engine - Win32 Release"

# Begin Custom Build - updating library files
OutDir=.\..\..\out\Release\win32-ix86\ 
InputPath=..\library\irc_engine.itcl

"$(OutDir)\irc_engine.itcl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(OutDir)

# End Custom Build

!ELSEIF  "$(CFG)" == "irc_engine - Win32 Debug"

# Begin Custom Build - updating library files
OutDir=.\..\..\out\Debug\win32-ix86
InputPath=..\library\irc_engine.itcl
InputName=irc_engine

"$(OutDir)\$(InputName).itcl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(OutDir)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\library\numerics.tcl

!IF  "$(CFG)" == "irc_engine - Win32 Release"

# Begin Custom Build - updating library files
OutDir=.\..\..\out\Release\win32-ix86\ 
InputPath=..\library\numerics.tcl

"$(OutDir)\numerics.tcl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(OutDir)

# End Custom Build

!ELSEIF  "$(CFG)" == "irc_engine - Win32 Debug"

# Begin Custom Build - updating library files
OutDir=.\..\..\out\Debug\win32-ix86
InputPath=..\library\numerics.tcl
InputName=numerics

"$(OutDir)\$(InputName).tcl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(OutDir)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\library\servers.ini

!IF  "$(CFG)" == "irc_engine - Win32 Release"

!ELSEIF  "$(CFG)" == "irc_engine - Win32 Debug"

# Begin Custom Build - updating library files
OutDir=.\..\..\out\Debug\win32-ix86
InputPath=..\library\servers.ini
InputName=servers

"$(OutDir)\$(InputName).ini" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(OutDir)

# End Custom Build

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
