# Microsoft Developer Studio Project File - Name="minimal" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=minimal - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "espeakedit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "espeakedit.mak" CFG="minimal - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "minimal - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 DLL Universal Release" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 DLL Universal Debug" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 DLL Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 DLL Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 DLL Release" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 DLL Debug" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 Universal Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 Universal Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 Universal Release" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 Universal Debug" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "minimal - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_dll\mswunivu" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivudll\minimal.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_dll\mswunivu" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivudll\minimal.pdb" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswunivu" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswunivu" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv28u_core.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmswuniv28u_core.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "minimal - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_dll\mswunivud" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivuddll\minimal.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_dll\mswunivud" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivuddll\minimal.pdb" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswunivud" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswunivud" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv28ud_core.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmswuniv28ud_core.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "minimal - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_dll\mswuniv" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivdll\minimal.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_dll\mswuniv" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivdll\minimal.pdb" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswuniv" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswuniv" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv28_core.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmswuniv28_core.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "minimal - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_dll\mswunivd" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivddll\minimal.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_dll\mswunivd" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivddll\minimal.pdb" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswunivd" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswunivd" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv28d_core.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmswuniv28d_core.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "minimal - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_dll\mswu" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswudll\minimal.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_dll\mswu" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswudll\minimal.pdb" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswu" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswu" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28u_core.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmsw28u_core.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "minimal - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_dll\mswud" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswuddll\minimal.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_dll\mswud" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswuddll\minimal.pdb" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswud" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswud" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28ud_core.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmsw28ud_core.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "minimal - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_dll\msw" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswdll\minimal.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_dll\msw" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswdll\minimal.pdb" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_dll\msw" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_dll\msw" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28_core.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmsw28_core.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "minimal - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_dll\mswd" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswddll\minimal.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_dll\mswd" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswddll\minimal.pdb" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswd" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswd" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28d_core.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmsw28d_core.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "minimal - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_lib\mswunivu" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivu\minimal.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_lib\mswunivu" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivu\minimal.pdb" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswunivu" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswunivu" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv28u_core.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmswuniv28u_core.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_lib"

!ELSEIF  "$(CFG)" == "minimal - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_lib\mswunivud" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivud\minimal.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_lib\mswunivud" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivud\minimal.pdb" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswunivud" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswunivud" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv28ud_core.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmswuniv28ud_core.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_lib"

!ELSEIF  "$(CFG)" == "minimal - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_lib\mswuniv" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswuniv\minimal.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_lib\mswuniv" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswuniv\minimal.pdb" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswuniv" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswuniv" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv28_core.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmswuniv28_core.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_lib"

!ELSEIF  "$(CFG)" == "minimal - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_lib\mswunivd" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivd\minimal.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_lib\mswunivd" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivd\minimal.pdb" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswunivd" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswunivd" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv28d_core.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmswuniv28d_core.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_lib"

!ELSEIF  "$(CFG)" == "minimal - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswu"
# PROP Intermediate_Dir "vc_mswu\minimal"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_lib\mswu" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswu\minimal.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W3 /GR /O2 /I ".\..\..\lib\vc_lib\mswu" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswu\minimal.pdb" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswu" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswu" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28u_core.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmsw28u_core.lib wxbase28u.lib wxmsw28u_adv.lib winmm.lib PAStaticWMME.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib wxmsw28u_html.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"espeakedit.exe" /libpath:".\..\..\lib\vc_lib"

!ELSEIF  "$(CFG)" == "minimal - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswud"
# PROP Intermediate_Dir "vc_mswud\minimal"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_lib\mswud" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswud\minimal.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_lib\mswud" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswud\minimal.pdb" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswud" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswud" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28ud_core.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmsw28ud_core.lib wxbase28ud.lib wxmsw28ud_adv.lib wxmsw28ud_html.lib winmm.lib PAStaticWMME.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib PAStaticWMME.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_lib"

!ELSEIF  "$(CFG)" == "minimal - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_msw"
# PROP Intermediate_Dir "vc_msw\minimal"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_lib\msw" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_msw\minimal.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_lib\msw" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_msw\minimal.pdb" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_lib\msw" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_lib\msw" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28_core.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmsw28_core.lib winmm.lib PAStaticWMME.lib wxbase28.lib wxmsw28_adv.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_lib"

!ELSEIF  "$(CFG)" == "minimal - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswd"
# PROP Intermediate_Dir "vc_mswd\minimal"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_lib\mswd" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswd\minimal.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_lib\mswd" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswd\minimal.pdb" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswd" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswd" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28d_core.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmsw28d_core.lib winmm.lib PAStaticWMME.lib wxbase28d.lib wxmsw28d_adv.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_lib"

!ENDIF 

# Begin Target

# Name "minimal - Win32 DLL Universal Unicode Release"
# Name "minimal - Win32 DLL Universal Unicode Debug"
# Name "minimal - Win32 DLL Universal Release"
# Name "minimal - Win32 DLL Universal Debug"
# Name "minimal - Win32 DLL Unicode Release"
# Name "minimal - Win32 DLL Unicode Debug"
# Name "minimal - Win32 DLL Release"
# Name "minimal - Win32 DLL Debug"
# Name "minimal - Win32 Universal Unicode Release"
# Name "minimal - Win32 Universal Unicode Debug"
# Name "minimal - Win32 Universal Release"
# Name "minimal - Win32 Universal Debug"
# Name "minimal - Win32 Unicode Release"
# Name "minimal - Win32 Unicode Debug"
# Name "minimal - Win32 Release"
# Name "minimal - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sample.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\compiledata.cpp
# End Source File
# Begin Source File

SOURCE=.\src\compiledict.cpp
# End Source File
# Begin Source File

SOURCE=.\src\debug.cpp
# End Source File
# Begin Source File

SOURCE=.\src\debug.h
# End Source File
# Begin Source File

SOURCE=.\src\dictionary.cpp
# End Source File
# Begin Source File

SOURCE=.\src\espeak_command.cpp
# End Source File
# Begin Source File

SOURCE=.\src\espeak_command.h
# End Source File
# Begin Source File

SOURCE=.\src\espeakedit.cpp
# End Source File
# Begin Source File

SOURCE=.\src\espeakedit.h
# End Source File
# Begin Source File

SOURCE=.\src\event.cpp
# End Source File
# Begin Source File

SOURCE=.\src\event.h
# End Source File
# Begin Source File

SOURCE=.\src\extras.cpp
# End Source File
# Begin Source File

SOURCE=.\src\fifo.cpp
# End Source File
# Begin Source File

SOURCE=.\src\fifo.h
# End Source File
# Begin Source File

SOURCE=.\src\formantdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\intonation.cpp
# End Source File
# Begin Source File

SOURCE=.\src\klatt.cpp
# End Source File
# Begin Source File

SOURCE=.\src\klatt.h
# End Source File
# Begin Source File

SOURCE=.\src\main.h
# End Source File
# Begin Source File

SOURCE=.\src\menus.cpp
# End Source File
# Begin Source File

SOURCE=.\src\numbers.cpp
# End Source File
# Begin Source File

SOURCE=.\src\options.cpp
# End Source File
# Begin Source File

SOURCE=.\src\options.h
# End Source File
# Begin Source File

SOURCE=.\src\phoneme.h
# End Source File
# Begin Source File

SOURCE=.\src\phonemelist.cpp
# End Source File
# Begin Source File

SOURCE=.\src\portaudio.h
# End Source File
# Begin Source File

SOURCE=.\src\portaudio18.h
# End Source File
# Begin Source File

SOURCE=.\src\portaudio19.h
# End Source File
# Begin Source File

SOURCE=.\src\prosodydisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\src\prosodydisplay.h
# End Source File
# Begin Source File

SOURCE=.\src\readclause.cpp
# End Source File
# Begin Source File

SOURCE=.\src\setlengths.cpp
# End Source File
# Begin Source File

SOURCE=.\src\sintab.h
# End Source File
# Begin Source File

SOURCE=.\src\sonic.cpp
# End Source File
# Begin Source File

SOURCE=.\src\sonic.h
# End Source File
# Begin Source File

SOURCE=.\src\speak_lib.cpp
# End Source File
# Begin Source File

SOURCE=.\src\speak_lib.h
# End Source File
# Begin Source File

SOURCE=.\src\spect.cpp
# End Source File
# Begin Source File

SOURCE=.\src\spect.h
# End Source File
# Begin Source File

SOURCE=.\src\spectdisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\src\spectseq.cpp
# End Source File
# Begin Source File

SOURCE=.\include\speech.h
# End Source File
# Begin Source File

SOURCE=.\speech.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\src\synth_mbrola.cpp
# End Source File
# Begin Source File

SOURCE=.\src\synthdata.cpp
# End Source File
# Begin Source File

SOURCE=.\src\synthesize.cpp
# End Source File
# Begin Source File

SOURCE=.\src\synthesize.h
# End Source File
# Begin Source File

SOURCE=.\src\tr_languages.cpp
# End Source File
# Begin Source File

SOURCE=.\src\translate.cpp
# End Source File
# Begin Source File

SOURCE=.\src\translate.h
# End Source File
# Begin Source File

SOURCE=.\src\transldlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\voice.h
# End Source File
# Begin Source File

SOURCE=.\src\voicedlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\voices.cpp
# End Source File
# Begin Source File

SOURCE=.\src\vowelchart.cpp
# End Source File
# Begin Source File

SOURCE=.\src\wave.cpp
# End Source File
# Begin Source File

SOURCE=.\src\wave.h
# End Source File
# Begin Source File

SOURCE=.\src\wavegen.cpp
# End Source File
# End Target
# End Project
