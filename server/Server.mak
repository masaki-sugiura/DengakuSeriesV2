# Microsoft Developer Studio Generated NMAKE File, Based on Server.dsp
!IF "$(CFG)" == ""
CFG=Server - Win32 Debug
!MESSAGE 構成が指定されていません。ﾃﾞﾌｫﾙﾄの Server - Win32 Debug を設定します。
!ENDIF 

!IF "$(CFG)" != "Server - Win32 Release" && "$(CFG)" != "Server - Win32 Debug"
!MESSAGE 指定された ﾋﾞﾙﾄﾞ ﾓｰﾄﾞ "$(CFG)" は正しくありません。
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "Server.mak" CFG="Server - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "Server - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "Server - Win32 Debug" ("Win32 (x86) Application" 用)
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

!IF  "$(CFG)" == "Server - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\dgserver.exe"

!ELSE 

ALL : "common - Win32 Release" "$(OUTDIR)\dgserver.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"common - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\cds_handler.obj"
	-@erase "$(INTDIR)\cmdname.obj"
	-@erase "$(INTDIR)\common.obj"
	-@erase "$(INTDIR)\convdata.obj"
	-@erase "$(INTDIR)\cs_handler.obj"
	-@erase "$(INTDIR)\ddeobjs.obj"
	-@erase "$(INTDIR)\ddeserv.obj"
	-@erase "$(INTDIR)\DengakuServer.res"
	-@erase "$(INTDIR)\ds_handler.obj"
	-@erase "$(INTDIR)\es_handler.obj"
	-@erase "$(INTDIR)\fs_handler.obj"
	-@erase "$(INTDIR)\is_handler.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\ms_handler.obj"
	-@erase "$(INTDIR)\opencomdlg.obj"
	-@erase "$(INTDIR)\pdparser.obj"
	-@erase "$(INTDIR)\ss_handler.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\dgserver.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90_PROJ=/compile_only /include:"$(INTDIR)\\" /nologo /warn:nofileopt /winapp /module:"Release/" /object:"Release/" 
F90_OBJS=.\Release/
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\Server.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x411 /fo"$(INTDIR)\DengakuServer.res" /i "..\common" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Server.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib imm32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\dgserver.pdb" /machine:I386 /out:"$(OUTDIR)\dgserver.exe" 
LINK32_OBJS= \
	"$(INTDIR)\cds_handler.obj" \
	"$(INTDIR)\cmdname.obj" \
	"$(INTDIR)\common.obj" \
	"$(INTDIR)\convdata.obj" \
	"$(INTDIR)\cs_handler.obj" \
	"$(INTDIR)\ddeobjs.obj" \
	"$(INTDIR)\ddeserv.obj" \
	"$(INTDIR)\ds_handler.obj" \
	"$(INTDIR)\es_handler.obj" \
	"$(INTDIR)\fs_handler.obj" \
	"$(INTDIR)\is_handler.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\ms_handler.obj" \
	"$(INTDIR)\opencomdlg.obj" \
	"$(INTDIR)\pdparser.obj" \
	"$(INTDIR)\ss_handler.obj" \
	"$(INTDIR)\DengakuServer.res" \
	"..\common\Release\common.lib"

"$(OUTDIR)\dgserver.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Server - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\dgserver.exe"

!ELSE 

ALL : "common - Win32 Debug" "$(OUTDIR)\dgserver.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"common - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\cds_handler.obj"
	-@erase "$(INTDIR)\cmdname.obj"
	-@erase "$(INTDIR)\common.obj"
	-@erase "$(INTDIR)\convdata.obj"
	-@erase "$(INTDIR)\cs_handler.obj"
	-@erase "$(INTDIR)\ddeobjs.obj"
	-@erase "$(INTDIR)\ddeserv.obj"
	-@erase "$(INTDIR)\DengakuServer.res"
	-@erase "$(INTDIR)\ds_handler.obj"
	-@erase "$(INTDIR)\es_handler.obj"
	-@erase "$(INTDIR)\fs_handler.obj"
	-@erase "$(INTDIR)\is_handler.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\ms_handler.obj"
	-@erase "$(INTDIR)\opencomdlg.obj"
	-@erase "$(INTDIR)\pdparser.obj"
	-@erase "$(INTDIR)\ss_handler.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dgserver.exe"
	-@erase "$(OUTDIR)\dgserver.ilk"
	-@erase "$(OUTDIR)\dgserver.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90_PROJ=/check:bounds /compile_only /debug:full /include:"$(INTDIR)\\" /nologo /traceback /warn:argument_checking /warn:nofileopt /winapp /module:"Debug/" /object:"Debug/" /pdbfile:"Debug/DF60.PDB" 
F90_OBJS=.\Debug/
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\Server.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x411 /fo"$(INTDIR)\DengakuServer.res" /i "..\common" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Server.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib imm32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\dgserver.pdb" /debug /machine:I386 /out:"$(OUTDIR)\dgserver.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\cds_handler.obj" \
	"$(INTDIR)\cmdname.obj" \
	"$(INTDIR)\common.obj" \
	"$(INTDIR)\convdata.obj" \
	"$(INTDIR)\cs_handler.obj" \
	"$(INTDIR)\ddeobjs.obj" \
	"$(INTDIR)\ddeserv.obj" \
	"$(INTDIR)\ds_handler.obj" \
	"$(INTDIR)\es_handler.obj" \
	"$(INTDIR)\fs_handler.obj" \
	"$(INTDIR)\is_handler.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\ms_handler.obj" \
	"$(INTDIR)\opencomdlg.obj" \
	"$(INTDIR)\pdparser.obj" \
	"$(INTDIR)\ss_handler.obj" \
	"$(INTDIR)\DengakuServer.res" \
	"..\common\Debug\common.lib"

"$(OUTDIR)\dgserver.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Server.dep")
!INCLUDE "Server.dep"
!ELSE 
!MESSAGE Warning: cannot find "Server.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Server - Win32 Release" || "$(CFG)" == "Server - Win32 Debug"
SOURCE=.\cds_handler.cpp

"$(INTDIR)\cds_handler.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cmdname.cpp

"$(INTDIR)\cmdname.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\common.cpp

"$(INTDIR)\common.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\convdata.cpp

"$(INTDIR)\convdata.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cs_handler.cpp

"$(INTDIR)\cs_handler.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ddeobjs.cpp

"$(INTDIR)\ddeobjs.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ddeserv.cpp

"$(INTDIR)\ddeserv.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_handler.cpp

"$(INTDIR)\ds_handler.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\es_handler.cpp

"$(INTDIR)\es_handler.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\fs_handler.cpp

"$(INTDIR)\fs_handler.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\is_handler.cpp

"$(INTDIR)\is_handler.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\main.cpp

"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ms_handler.cpp

"$(INTDIR)\ms_handler.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\opencomdlg.cpp

"$(INTDIR)\opencomdlg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pdparser.cpp

"$(INTDIR)\pdparser.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ss_handler.cpp

"$(INTDIR)\ss_handler.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DengakuServer.rc

!IF  "$(CFG)" == "Server - Win32 Release"


"$(INTDIR)\DengakuServer.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x411 /fo"$(INTDIR)\DengakuServer.res" /i "..\common" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "Server - Win32 Debug"


"$(INTDIR)\DengakuServer.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x411 /fo"$(INTDIR)\DengakuServer.res" /i "..\common" /d "_DEBUG" $(SOURCE)


!ENDIF 

!IF  "$(CFG)" == "Server - Win32 Release"

"common - Win32 Release" : 
   cd "\Program Files\DevStudio\MyProjects\DengakuSeriesV1\common"
   $(MAKE) /$(MAKEFLAGS) /F ".\common.mak" CFG="common - Win32 Release" 
   cd "..\Server"

"common - Win32 ReleaseCLEAN" : 
   cd "\Program Files\DevStudio\MyProjects\DengakuSeriesV1\common"
   $(MAKE) /$(MAKEFLAGS) /F ".\common.mak" CFG="common - Win32 Release" RECURSE=1 CLEAN 
   cd "..\Server"

!ELSEIF  "$(CFG)" == "Server - Win32 Debug"

"common - Win32 Debug" : 
   cd "\Program Files\DevStudio\MyProjects\DengakuSeriesV1\common"
   $(MAKE) /$(MAKEFLAGS) /F ".\common.mak" CFG="common - Win32 Debug" 
   cd "..\Server"

"common - Win32 DebugCLEAN" : 
   cd "\Program Files\DevStudio\MyProjects\DengakuSeriesV1\common"
   $(MAKE) /$(MAKEFLAGS) /F ".\common.mak" CFG="common - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\Server"

!ENDIF 


!ENDIF 

