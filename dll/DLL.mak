# Microsoft Developer Studio Generated NMAKE File, Based on DLL.dsp
!IF "$(CFG)" == ""
CFG=DLL - Win32 Debug
!MESSAGE 構成が指定されていません。ﾃﾞﾌｫﾙﾄの DLL - Win32 Debug を設定します。
!ENDIF 

!IF "$(CFG)" != "DLL - Win32 Release" && "$(CFG)" != "DLL - Win32 Debug"
!MESSAGE 指定された ﾋﾞﾙﾄﾞ ﾓｰﾄﾞ "$(CFG)" は正しくありません。
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "DLL.mak" CFG="DLL - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "DLL - Win32 Release" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE "DLL - Win32 Debug" ("Win32 (x86) Dynamic-Link Library" 用)
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

!IF  "$(CFG)" == "DLL - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\DengakuDLL.dll"

!ELSE 

ALL : "common - Win32 Release" "$(OUTDIR)\DengakuDLL.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"common - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\bs_func.obj"
	-@erase "$(INTDIR)\cds_func.obj"
	-@erase "$(INTDIR)\cs_func.obj"
	-@erase "$(INTDIR)\DengakuDLL.obj"
	-@erase "$(INTDIR)\ds_func.obj"
	-@erase "$(INTDIR)\es_func.obj"
	-@erase "$(INTDIR)\fs_func.obj"
	-@erase "$(INTDIR)\is_func.obj"
	-@erase "$(INTDIR)\ms_func.obj"
	-@erase "$(INTDIR)\seldir.res"
	-@erase "$(INTDIR)\ss_func.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\DengakuDLL.dll"
	-@erase "$(OUTDIR)\DengakuDLL.exp"
	-@erase "$(OUTDIR)\DengakuDLL.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90_PROJ=/compile_only /include:"$(INTDIR)\\" /dll /nologo /warn:nofileopt /module:"Release/" /object:"Release/" 
F90_OBJS=.\Release/
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DENGAKUDLL_EXPORTS" /Fp"$(INTDIR)\DLL.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x411 /fo"$(INTDIR)\seldir.res" /i "..\common" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DLL.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib imm32.lib ..\common\Release\common.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\DengakuDLL.pdb" /machine:I386 /out:"$(OUTDIR)\DengakuDLL.dll" /implib:"$(OUTDIR)\DengakuDLL.lib" 
LINK32_OBJS= \
	"$(INTDIR)\cds_func.obj" \
	"$(INTDIR)\cs_func.obj" \
	"$(INTDIR)\DengakuDLL.obj" \
	"$(INTDIR)\ds_func.obj" \
	"$(INTDIR)\es_func.obj" \
	"$(INTDIR)\fs_func.obj" \
	"$(INTDIR)\is_func.obj" \
	"$(INTDIR)\ms_func.obj" \
	"$(INTDIR)\ss_func.obj" \
	"$(INTDIR)\seldir.res" \
	"$(INTDIR)\bs_func.obj" \
	"..\common\Release\common.lib"

"$(OUTDIR)\DengakuDLL.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DLL - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\DengakuDLL.dll"

!ELSE 

ALL : "common - Win32 Debug" "$(OUTDIR)\DengakuDLL.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"common - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\bs_func.obj"
	-@erase "$(INTDIR)\cds_func.obj"
	-@erase "$(INTDIR)\cs_func.obj"
	-@erase "$(INTDIR)\DengakuDLL.obj"
	-@erase "$(INTDIR)\ds_func.obj"
	-@erase "$(INTDIR)\es_func.obj"
	-@erase "$(INTDIR)\fs_func.obj"
	-@erase "$(INTDIR)\is_func.obj"
	-@erase "$(INTDIR)\ms_func.obj"
	-@erase "$(INTDIR)\seldir.res"
	-@erase "$(INTDIR)\ss_func.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\DengakuDLL.dll"
	-@erase "$(OUTDIR)\DengakuDLL.exp"
	-@erase "$(OUTDIR)\DengakuDLL.ilk"
	-@erase "$(OUTDIR)\DengakuDLL.lib"
	-@erase "$(OUTDIR)\DengakuDLL.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90_PROJ=/check:bounds /compile_only /debug:full /include:"$(INTDIR)\\" /dll /nologo /traceback /warn:argument_checking /warn:nofileopt /module:"Debug/" /object:"Debug/" /pdbfile:"Debug/DF60.PDB" 
F90_OBJS=.\Debug/
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DENGAKUDLL_EXPORTS" /Fp"$(INTDIR)\DLL.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x411 /fo"$(INTDIR)\seldir.res" /i "..\common" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DLL.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib imm32.lib ..\common\Debug\common.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\DengakuDLL.pdb" /debug /machine:I386 /out:"$(OUTDIR)\DengakuDLL.dll" /implib:"$(OUTDIR)\DengakuDLL.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\cds_func.obj" \
	"$(INTDIR)\cs_func.obj" \
	"$(INTDIR)\DengakuDLL.obj" \
	"$(INTDIR)\ds_func.obj" \
	"$(INTDIR)\es_func.obj" \
	"$(INTDIR)\fs_func.obj" \
	"$(INTDIR)\is_func.obj" \
	"$(INTDIR)\ms_func.obj" \
	"$(INTDIR)\ss_func.obj" \
	"$(INTDIR)\seldir.res" \
	"$(INTDIR)\bs_func.obj" \
	"..\common\Debug\common.lib"

"$(OUTDIR)\DengakuDLL.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("DLL.dep")
!INCLUDE "DLL.dep"
!ELSE 
!MESSAGE Warning: cannot find "DLL.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "DLL - Win32 Release" || "$(CFG)" == "DLL - Win32 Debug"
SOURCE=.\bs_func.cpp

"$(INTDIR)\bs_func.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cds_func.cpp

"$(INTDIR)\cds_func.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cs_func.cpp

"$(INTDIR)\cs_func.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DengakuDLL.cpp

"$(INTDIR)\DengakuDLL.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_func.cpp

"$(INTDIR)\ds_func.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\es_func.cpp

"$(INTDIR)\es_func.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\fs_func.cpp

"$(INTDIR)\fs_func.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\is_func.cpp

"$(INTDIR)\is_func.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ms_func.cpp

"$(INTDIR)\ms_func.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ss_func.cpp

"$(INTDIR)\ss_func.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\seldir.rc

!IF  "$(CFG)" == "DLL - Win32 Release"


"$(INTDIR)\seldir.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x411 /fo"$(INTDIR)\seldir.res" /i "..\common" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "DLL - Win32 Debug"


"$(INTDIR)\seldir.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x411 /fo"$(INTDIR)\seldir.res" /i "..\common" /d "_DEBUG" $(SOURCE)


!ENDIF 

!IF  "$(CFG)" == "DLL - Win32 Release"

"common - Win32 Release" : 
   cd "\Program Files\DevStudio\MyProjects\DengakuSeriesV2\common"
   $(MAKE) /$(MAKEFLAGS) /F ".\common.mak" CFG="common - Win32 Release" 
   cd "..\DLL"

"common - Win32 ReleaseCLEAN" : 
   cd "\Program Files\DevStudio\MyProjects\DengakuSeriesV2\common"
   $(MAKE) /$(MAKEFLAGS) /F ".\common.mak" CFG="common - Win32 Release" RECURSE=1 CLEAN 
   cd "..\DLL"

!ELSEIF  "$(CFG)" == "DLL - Win32 Debug"

"common - Win32 Debug" : 
   cd "\Program Files\DevStudio\MyProjects\DengakuSeriesV2\common"
   $(MAKE) /$(MAKEFLAGS) /F ".\common.mak" CFG="common - Win32 Debug" 
   cd "..\DLL"

"common - Win32 DebugCLEAN" : 
   cd "\Program Files\DevStudio\MyProjects\DengakuSeriesV2\common"
   $(MAKE) /$(MAKEFLAGS) /F ".\common.mak" CFG="common - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\DLL"

!ENDIF 


!ENDIF 

