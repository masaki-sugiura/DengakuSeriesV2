# Microsoft Developer Studio Generated NMAKE File, Based on DllTest.dsp
!IF "$(CFG)" == ""
CFG=DllTest - Win32 Debug
!MESSAGE 構成が指定されていません。ﾃﾞﾌｫﾙﾄの DllTest - Win32 Debug を設定します。
!ENDIF 

!IF "$(CFG)" != "DllTest - Win32 Release" && "$(CFG)" != "DllTest - Win32 Debug"
!MESSAGE 指定された ﾋﾞﾙﾄﾞ ﾓｰﾄﾞ "$(CFG)" は正しくありません。
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "DllTest.mak" CFG="DllTest - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "DllTest - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "DllTest - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE 
!ERROR 無効な構成が指定されています。
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
F90=df.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DllTest - Win32 Release"

OUTDIR=.\DllTest___Win32_Release
INTDIR=.\DllTest___Win32_Release
# Begin Custom Macros
OutDir=.\DllTest___Win32_Release
# End Custom Macros

ALL : "$(OUTDIR)\DllTest.exe"


CLEAN :
	-@erase "$(INTDIR)\dlltest.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\DllTest.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90_PROJ=/compile_only /include:"$(INTDIR)\\" /nologo /warn:nofileopt /winapp /module:"DllTest___Win32_Release/" /object:"DllTest___Win32_Release/" 
F90_OBJS=.\DllTest___Win32_Release/
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\DllTest.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DllTest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\DllTest.pdb" /machine:I386 /out:"$(OUTDIR)\DllTest.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dlltest.obj"

"$(OUTDIR)\DllTest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DllTest - Win32 Debug"

OUTDIR=.\DllTest___Win32_Debug
INTDIR=.\DllTest___Win32_Debug
# Begin Custom Macros
OutDir=.\DllTest___Win32_Debug
# End Custom Macros

ALL : "$(OUTDIR)\DllTest.exe"


CLEAN :
	-@erase "$(INTDIR)\dlltest.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\DllTest.exe"
	-@erase "$(OUTDIR)\DllTest.ilk"
	-@erase "$(OUTDIR)\DllTest.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90_PROJ=/check:bounds /compile_only /debug:full /include:"$(INTDIR)\\" /nologo /traceback /warn:argument_checking /warn:nofileopt /winapp /module:"DllTest___Win32_Debug/" /object:"DllTest___Win32_Debug/" /pdbfile:"DllTest___Win32_Debug/DF60.PDB" 
F90_OBJS=.\DllTest___Win32_Debug/
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\DllTest.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DllTest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\DllTest.pdb" /debug /machine:I386 /out:"$(OUTDIR)\DllTest.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\dlltest.obj"

"$(OUTDIR)\DllTest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.SUFFIXES: .fpp

.for{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f90{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.fpp{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("DllTest.dep")
!INCLUDE "DllTest.dep"
!ELSE 
!MESSAGE Warning: cannot find "DllTest.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "DllTest - Win32 Release" || "$(CFG)" == "DllTest - Win32 Debug"
SOURCE=.\dlltest.cpp

"$(INTDIR)\dlltest.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

