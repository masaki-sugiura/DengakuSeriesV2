# Microsoft Developer Studio Generated NMAKE File, Based on common.dsp
!IF "$(CFG)" == ""
CFG=common - Win32 Debug
!MESSAGE 構成が指定されていません。ﾃﾞﾌｫﾙﾄの common - Win32 Debug を設定します。
!ENDIF 

!IF "$(CFG)" != "common - Win32 Release" && "$(CFG)" != "common - Win32 Debug"
!MESSAGE 指定された ﾋﾞﾙﾄﾞ ﾓｰﾄﾞ "$(CFG)" は正しくありません。
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "common.mak" CFG="common - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "common - Win32 Release" ("Win32 (x86) Static Library" 用)
!MESSAGE "common - Win32 Debug" ("Win32 (x86) Static Library" 用)
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
RSC=rc.exe

!IF  "$(CFG)" == "common - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\common.lib"


CLEAN :
	-@erase "$(INTDIR)\bregexp_mngr.obj"
	-@erase "$(INTDIR)\cmdline.obj"
	-@erase "$(INTDIR)\ctrldata.obj"
	-@erase "$(INTDIR)\ctrlname.obj"
	-@erase "$(INTDIR)\ddfile.obj"
	-@erase "$(INTDIR)\dirlist.obj"
	-@erase "$(INTDIR)\dlgdata.obj"
	-@erase "$(INTDIR)\enumpath.obj"
	-@erase "$(INTDIR)\env_var.obj"
	-@erase "$(INTDIR)\file.obj"
	-@erase "$(INTDIR)\hashtbl.obj"
	-@erase "$(INTDIR)\linklist.obj"
	-@erase "$(INTDIR)\menu.obj"
	-@erase "$(INTDIR)\misc.obj"
	-@erase "$(INTDIR)\pathname.obj"
	-@erase "$(INTDIR)\rec_op.obj"
	-@erase "$(INTDIR)\recfind.obj"
	-@erase "$(INTDIR)\refcount.obj"
	-@erase "$(INTDIR)\seldir.obj"
	-@erase "$(INTDIR)\seq_op.obj"
	-@erase "$(INTDIR)\session.obj"
	-@erase "$(INTDIR)\shicons.obj"
	-@erase "$(INTDIR)\si_bregexp.obj"
	-@erase "$(INTDIR)\si_comdlg.obj"
	-@erase "$(INTDIR)\si_common.obj"
	-@erase "$(INTDIR)\si_dialog.obj"
	-@erase "$(INTDIR)\si_file.obj"
	-@erase "$(INTDIR)\si_image.obj"
	-@erase "$(INTDIR)\si_menu.obj"
	-@erase "$(INTDIR)\si_string.obj"
	-@erase "$(INTDIR)\si_variable.obj"
	-@erase "$(INTDIR)\smalloc.obj"
	-@erase "$(INTDIR)\spi_mngr.obj"
	-@erase "$(INTDIR)\str_tbl.obj"
	-@erase "$(INTDIR)\strbuf.obj"
	-@erase "$(INTDIR)\strutils.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\tokenizer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\common.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90_PROJ=/compile_only /include:"$(INTDIR)\\" /nologo /warn:nofileopt /module:"Release/" /object:"Release/" 
F90_OBJS=.\Release/
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\common" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\common.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\common.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\common.lib" 
LIB32_OBJS= \
	"$(INTDIR)\bregexp_mngr.obj" \
	"$(INTDIR)\cmdline.obj" \
	"$(INTDIR)\ctrldata.obj" \
	"$(INTDIR)\ctrlname.obj" \
	"$(INTDIR)\ddfile.obj" \
	"$(INTDIR)\dirlist.obj" \
	"$(INTDIR)\dlgdata.obj" \
	"$(INTDIR)\enumpath.obj" \
	"$(INTDIR)\env_var.obj" \
	"$(INTDIR)\file.obj" \
	"$(INTDIR)\hashtbl.obj" \
	"$(INTDIR)\linklist.obj" \
	"$(INTDIR)\menu.obj" \
	"$(INTDIR)\misc.obj" \
	"$(INTDIR)\pathname.obj" \
	"$(INTDIR)\rec_op.obj" \
	"$(INTDIR)\recfind.obj" \
	"$(INTDIR)\refcount.obj" \
	"$(INTDIR)\seldir.obj" \
	"$(INTDIR)\seq_op.obj" \
	"$(INTDIR)\session.obj" \
	"$(INTDIR)\shicons.obj" \
	"$(INTDIR)\si_bregexp.obj" \
	"$(INTDIR)\si_comdlg.obj" \
	"$(INTDIR)\si_common.obj" \
	"$(INTDIR)\si_dialog.obj" \
	"$(INTDIR)\si_file.obj" \
	"$(INTDIR)\si_image.obj" \
	"$(INTDIR)\si_menu.obj" \
	"$(INTDIR)\si_string.obj" \
	"$(INTDIR)\si_variable.obj" \
	"$(INTDIR)\smalloc.obj" \
	"$(INTDIR)\spi_mngr.obj" \
	"$(INTDIR)\str_tbl.obj" \
	"$(INTDIR)\strbuf.obj" \
	"$(INTDIR)\strutils.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\tokenizer.obj"

"$(OUTDIR)\common.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "common - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\common.lib"


CLEAN :
	-@erase "$(INTDIR)\bregexp_mngr.obj"
	-@erase "$(INTDIR)\cmdline.obj"
	-@erase "$(INTDIR)\ctrldata.obj"
	-@erase "$(INTDIR)\ctrlname.obj"
	-@erase "$(INTDIR)\ddfile.obj"
	-@erase "$(INTDIR)\dirlist.obj"
	-@erase "$(INTDIR)\dlgdata.obj"
	-@erase "$(INTDIR)\enumpath.obj"
	-@erase "$(INTDIR)\env_var.obj"
	-@erase "$(INTDIR)\file.obj"
	-@erase "$(INTDIR)\hashtbl.obj"
	-@erase "$(INTDIR)\linklist.obj"
	-@erase "$(INTDIR)\menu.obj"
	-@erase "$(INTDIR)\misc.obj"
	-@erase "$(INTDIR)\pathname.obj"
	-@erase "$(INTDIR)\rec_op.obj"
	-@erase "$(INTDIR)\recfind.obj"
	-@erase "$(INTDIR)\refcount.obj"
	-@erase "$(INTDIR)\seldir.obj"
	-@erase "$(INTDIR)\seq_op.obj"
	-@erase "$(INTDIR)\session.obj"
	-@erase "$(INTDIR)\shicons.obj"
	-@erase "$(INTDIR)\si_bregexp.obj"
	-@erase "$(INTDIR)\si_comdlg.obj"
	-@erase "$(INTDIR)\si_common.obj"
	-@erase "$(INTDIR)\si_dialog.obj"
	-@erase "$(INTDIR)\si_file.obj"
	-@erase "$(INTDIR)\si_image.obj"
	-@erase "$(INTDIR)\si_menu.obj"
	-@erase "$(INTDIR)\si_string.obj"
	-@erase "$(INTDIR)\si_variable.obj"
	-@erase "$(INTDIR)\smalloc.obj"
	-@erase "$(INTDIR)\spi_mngr.obj"
	-@erase "$(INTDIR)\str_tbl.obj"
	-@erase "$(INTDIR)\strbuf.obj"
	-@erase "$(INTDIR)\strutils.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\tokenizer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\common.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90_PROJ=/check:bounds /compile_only /debug:full /include:"$(INTDIR)\\" /nologo /traceback /warn:argument_checking /warn:nofileopt /module:"Debug/" /object:"Debug/" /pdbfile:"Debug/DF60.PDB" 
F90_OBJS=.\Debug/
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\common" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\common.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\common.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\common.lib" 
LIB32_OBJS= \
	"$(INTDIR)\bregexp_mngr.obj" \
	"$(INTDIR)\cmdline.obj" \
	"$(INTDIR)\ctrldata.obj" \
	"$(INTDIR)\ctrlname.obj" \
	"$(INTDIR)\ddfile.obj" \
	"$(INTDIR)\dirlist.obj" \
	"$(INTDIR)\dlgdata.obj" \
	"$(INTDIR)\enumpath.obj" \
	"$(INTDIR)\env_var.obj" \
	"$(INTDIR)\file.obj" \
	"$(INTDIR)\hashtbl.obj" \
	"$(INTDIR)\linklist.obj" \
	"$(INTDIR)\menu.obj" \
	"$(INTDIR)\misc.obj" \
	"$(INTDIR)\pathname.obj" \
	"$(INTDIR)\rec_op.obj" \
	"$(INTDIR)\recfind.obj" \
	"$(INTDIR)\refcount.obj" \
	"$(INTDIR)\seldir.obj" \
	"$(INTDIR)\seq_op.obj" \
	"$(INTDIR)\session.obj" \
	"$(INTDIR)\shicons.obj" \
	"$(INTDIR)\si_bregexp.obj" \
	"$(INTDIR)\si_comdlg.obj" \
	"$(INTDIR)\si_common.obj" \
	"$(INTDIR)\si_dialog.obj" \
	"$(INTDIR)\si_file.obj" \
	"$(INTDIR)\si_image.obj" \
	"$(INTDIR)\si_menu.obj" \
	"$(INTDIR)\si_string.obj" \
	"$(INTDIR)\si_variable.obj" \
	"$(INTDIR)\smalloc.obj" \
	"$(INTDIR)\spi_mngr.obj" \
	"$(INTDIR)\str_tbl.obj" \
	"$(INTDIR)\strbuf.obj" \
	"$(INTDIR)\strutils.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\tokenizer.obj"

"$(OUTDIR)\common.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
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
!IF EXISTS("common.dep")
!INCLUDE "common.dep"
!ELSE 
!MESSAGE Warning: cannot find "common.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "common - Win32 Release" || "$(CFG)" == "common - Win32 Debug"
SOURCE=.\bregexp_mngr.cpp

"$(INTDIR)\bregexp_mngr.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cmdline.cpp

"$(INTDIR)\cmdline.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ctrldata.cpp

"$(INTDIR)\ctrldata.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ctrlname.cpp

"$(INTDIR)\ctrlname.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ddfile.cpp

"$(INTDIR)\ddfile.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dirlist.cpp

"$(INTDIR)\dirlist.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dlgdata.cpp

"$(INTDIR)\dlgdata.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\enumpath.cpp

"$(INTDIR)\enumpath.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\env_var.cpp

"$(INTDIR)\env_var.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\file.cpp

"$(INTDIR)\file.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hashtbl.cpp

"$(INTDIR)\hashtbl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\linklist.cpp

"$(INTDIR)\linklist.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\menu.cpp

"$(INTDIR)\menu.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\misc.cpp

"$(INTDIR)\misc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pathname.cpp

"$(INTDIR)\pathname.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\rec_op.cpp

"$(INTDIR)\rec_op.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\recfind.cpp

"$(INTDIR)\recfind.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\refcount.cpp

"$(INTDIR)\refcount.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\seldir.cpp

"$(INTDIR)\seldir.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\seq_op.cpp

"$(INTDIR)\seq_op.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\session.cpp

"$(INTDIR)\session.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\shicons.cpp

"$(INTDIR)\shicons.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\si_bregexp.cpp

"$(INTDIR)\si_bregexp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\si_comdlg.cpp

"$(INTDIR)\si_comdlg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\si_common.cpp

"$(INTDIR)\si_common.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\si_dialog.cpp

"$(INTDIR)\si_dialog.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\si_file.cpp

"$(INTDIR)\si_file.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\si_image.cpp

"$(INTDIR)\si_image.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\si_menu.cpp

"$(INTDIR)\si_menu.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\si_string.cpp

"$(INTDIR)\si_string.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\si_variable.cpp

"$(INTDIR)\si_variable.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\smalloc.cpp

"$(INTDIR)\smalloc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\spi_mngr.cpp

"$(INTDIR)\spi_mngr.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\str_tbl.cpp

"$(INTDIR)\str_tbl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\strbuf.cpp

"$(INTDIR)\strbuf.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\strutils.cpp

"$(INTDIR)\strutils.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\thread.cpp

"$(INTDIR)\thread.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\tokenizer.cpp

"$(INTDIR)\tokenizer.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

