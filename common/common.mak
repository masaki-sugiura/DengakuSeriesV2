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
RSC=rc.exe

!IF  "$(CFG)" == "common - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\common.lib" "$(OUTDIR)\common.bsc"


CLEAN :
	-@erase "$(INTDIR)\bregexp_mngr.obj"
	-@erase "$(INTDIR)\bregexp_mngr.sbr"
	-@erase "$(INTDIR)\cmdline.obj"
	-@erase "$(INTDIR)\cmdline.sbr"
	-@erase "$(INTDIR)\colortbl.obj"
	-@erase "$(INTDIR)\colortbl.sbr"
	-@erase "$(INTDIR)\ctrldata.obj"
	-@erase "$(INTDIR)\ctrldata.sbr"
	-@erase "$(INTDIR)\ctrlname.obj"
	-@erase "$(INTDIR)\ctrlname.sbr"
	-@erase "$(INTDIR)\ddfile.obj"
	-@erase "$(INTDIR)\ddfile.sbr"
	-@erase "$(INTDIR)\dirlist.obj"
	-@erase "$(INTDIR)\dirlist.sbr"
	-@erase "$(INTDIR)\dlgdata.obj"
	-@erase "$(INTDIR)\dlgdata.sbr"
	-@erase "$(INTDIR)\enumpath.obj"
	-@erase "$(INTDIR)\enumpath.sbr"
	-@erase "$(INTDIR)\env_var.obj"
	-@erase "$(INTDIR)\env_var.sbr"
	-@erase "$(INTDIR)\file.obj"
	-@erase "$(INTDIR)\file.sbr"
	-@erase "$(INTDIR)\hashtbl.obj"
	-@erase "$(INTDIR)\hashtbl.sbr"
	-@erase "$(INTDIR)\hmjre_mngr.obj"
	-@erase "$(INTDIR)\hmjre_mngr.sbr"
	-@erase "$(INTDIR)\linklist.obj"
	-@erase "$(INTDIR)\linklist.sbr"
	-@erase "$(INTDIR)\menu.obj"
	-@erase "$(INTDIR)\menu.sbr"
	-@erase "$(INTDIR)\misc.obj"
	-@erase "$(INTDIR)\misc.sbr"
	-@erase "$(INTDIR)\optmap.obj"
	-@erase "$(INTDIR)\optmap.sbr"
	-@erase "$(INTDIR)\pathname.obj"
	-@erase "$(INTDIR)\pathname.sbr"
	-@erase "$(INTDIR)\rec_op.obj"
	-@erase "$(INTDIR)\rec_op.sbr"
	-@erase "$(INTDIR)\recfind.obj"
	-@erase "$(INTDIR)\recfind.sbr"
	-@erase "$(INTDIR)\redll_mngr.obj"
	-@erase "$(INTDIR)\redll_mngr.sbr"
	-@erase "$(INTDIR)\refcount.obj"
	-@erase "$(INTDIR)\refcount.sbr"
	-@erase "$(INTDIR)\seldir.obj"
	-@erase "$(INTDIR)\seldir.sbr"
	-@erase "$(INTDIR)\seq_op.obj"
	-@erase "$(INTDIR)\seq_op.sbr"
	-@erase "$(INTDIR)\session.obj"
	-@erase "$(INTDIR)\session.sbr"
	-@erase "$(INTDIR)\shicons.obj"
	-@erase "$(INTDIR)\shicons.sbr"
	-@erase "$(INTDIR)\shobj_wrapper.obj"
	-@erase "$(INTDIR)\shobj_wrapper.sbr"
	-@erase "$(INTDIR)\si_bregexp.obj"
	-@erase "$(INTDIR)\si_bregexp.sbr"
	-@erase "$(INTDIR)\si_colorref.obj"
	-@erase "$(INTDIR)\si_colorref.sbr"
	-@erase "$(INTDIR)\si_comdlg.obj"
	-@erase "$(INTDIR)\si_comdlg.sbr"
	-@erase "$(INTDIR)\si_common.obj"
	-@erase "$(INTDIR)\si_common.sbr"
	-@erase "$(INTDIR)\si_dialog.obj"
	-@erase "$(INTDIR)\si_dialog.sbr"
	-@erase "$(INTDIR)\si_file.obj"
	-@erase "$(INTDIR)\si_file.sbr"
	-@erase "$(INTDIR)\si_image.obj"
	-@erase "$(INTDIR)\si_image.sbr"
	-@erase "$(INTDIR)\si_menu.obj"
	-@erase "$(INTDIR)\si_menu.sbr"
	-@erase "$(INTDIR)\si_string.obj"
	-@erase "$(INTDIR)\si_string.sbr"
	-@erase "$(INTDIR)\si_variable.obj"
	-@erase "$(INTDIR)\si_variable.sbr"
	-@erase "$(INTDIR)\smalloc.obj"
	-@erase "$(INTDIR)\smalloc.sbr"
	-@erase "$(INTDIR)\spi_mngr.obj"
	-@erase "$(INTDIR)\spi_mngr.sbr"
	-@erase "$(INTDIR)\stack.obj"
	-@erase "$(INTDIR)\stack.sbr"
	-@erase "$(INTDIR)\str_tbl.obj"
	-@erase "$(INTDIR)\str_tbl.sbr"
	-@erase "$(INTDIR)\strbuf.obj"
	-@erase "$(INTDIR)\strbuf.sbr"
	-@erase "$(INTDIR)\strutils.obj"
	-@erase "$(INTDIR)\strutils.sbr"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\thread.sbr"
	-@erase "$(INTDIR)\tokenizer.obj"
	-@erase "$(INTDIR)\tokenizer.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\common.bsc"
	-@erase "$(OUTDIR)\common.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=df.exe
CPP_PROJ=/nologo /MT /W3 /GR /GX /O2 /I "..\common" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D _WIN32_WINNT=0x0501 /FAcs /Fa"$(INTDIR)\\" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\common.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\common.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\bregexp_mngr.sbr" \
	"$(INTDIR)\cmdline.sbr" \
	"$(INTDIR)\colortbl.sbr" \
	"$(INTDIR)\ctrldata.sbr" \
	"$(INTDIR)\ctrlname.sbr" \
	"$(INTDIR)\ddfile.sbr" \
	"$(INTDIR)\dirlist.sbr" \
	"$(INTDIR)\dlgdata.sbr" \
	"$(INTDIR)\enumpath.sbr" \
	"$(INTDIR)\env_var.sbr" \
	"$(INTDIR)\file.sbr" \
	"$(INTDIR)\hashtbl.sbr" \
	"$(INTDIR)\hmjre_mngr.sbr" \
	"$(INTDIR)\linklist.sbr" \
	"$(INTDIR)\menu.sbr" \
	"$(INTDIR)\misc.sbr" \
	"$(INTDIR)\optmap.sbr" \
	"$(INTDIR)\pathname.sbr" \
	"$(INTDIR)\rec_op.sbr" \
	"$(INTDIR)\recfind.sbr" \
	"$(INTDIR)\refcount.sbr" \
	"$(INTDIR)\seldir.sbr" \
	"$(INTDIR)\seq_op.sbr" \
	"$(INTDIR)\session.sbr" \
	"$(INTDIR)\shicons.sbr" \
	"$(INTDIR)\shobj_wrapper.sbr" \
	"$(INTDIR)\si_bregexp.sbr" \
	"$(INTDIR)\si_colorref.sbr" \
	"$(INTDIR)\si_comdlg.sbr" \
	"$(INTDIR)\si_common.sbr" \
	"$(INTDIR)\si_dialog.sbr" \
	"$(INTDIR)\si_file.sbr" \
	"$(INTDIR)\si_image.sbr" \
	"$(INTDIR)\si_menu.sbr" \
	"$(INTDIR)\si_string.sbr" \
	"$(INTDIR)\si_variable.sbr" \
	"$(INTDIR)\smalloc.sbr" \
	"$(INTDIR)\spi_mngr.sbr" \
	"$(INTDIR)\stack.sbr" \
	"$(INTDIR)\str_tbl.sbr" \
	"$(INTDIR)\strbuf.sbr" \
	"$(INTDIR)\strutils.sbr" \
	"$(INTDIR)\thread.sbr" \
	"$(INTDIR)\tokenizer.sbr" \
	"$(INTDIR)\redll_mngr.sbr"

"$(OUTDIR)\common.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\common.lib" 
LIB32_OBJS= \
	"$(INTDIR)\bregexp_mngr.obj" \
	"$(INTDIR)\cmdline.obj" \
	"$(INTDIR)\colortbl.obj" \
	"$(INTDIR)\ctrldata.obj" \
	"$(INTDIR)\ctrlname.obj" \
	"$(INTDIR)\ddfile.obj" \
	"$(INTDIR)\dirlist.obj" \
	"$(INTDIR)\dlgdata.obj" \
	"$(INTDIR)\enumpath.obj" \
	"$(INTDIR)\env_var.obj" \
	"$(INTDIR)\file.obj" \
	"$(INTDIR)\hashtbl.obj" \
	"$(INTDIR)\hmjre_mngr.obj" \
	"$(INTDIR)\linklist.obj" \
	"$(INTDIR)\menu.obj" \
	"$(INTDIR)\misc.obj" \
	"$(INTDIR)\optmap.obj" \
	"$(INTDIR)\pathname.obj" \
	"$(INTDIR)\rec_op.obj" \
	"$(INTDIR)\recfind.obj" \
	"$(INTDIR)\refcount.obj" \
	"$(INTDIR)\seldir.obj" \
	"$(INTDIR)\seq_op.obj" \
	"$(INTDIR)\session.obj" \
	"$(INTDIR)\shicons.obj" \
	"$(INTDIR)\shobj_wrapper.obj" \
	"$(INTDIR)\si_bregexp.obj" \
	"$(INTDIR)\si_colorref.obj" \
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
	"$(INTDIR)\stack.obj" \
	"$(INTDIR)\str_tbl.obj" \
	"$(INTDIR)\strbuf.obj" \
	"$(INTDIR)\strutils.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\tokenizer.obj" \
	"$(INTDIR)\redll_mngr.obj"

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
	-@erase "$(INTDIR)\colortbl.obj"
	-@erase "$(INTDIR)\ctrldata.obj"
	-@erase "$(INTDIR)\ctrlname.obj"
	-@erase "$(INTDIR)\ddfile.obj"
	-@erase "$(INTDIR)\dirlist.obj"
	-@erase "$(INTDIR)\dlgdata.obj"
	-@erase "$(INTDIR)\enumpath.obj"
	-@erase "$(INTDIR)\env_var.obj"
	-@erase "$(INTDIR)\file.obj"
	-@erase "$(INTDIR)\hashtbl.obj"
	-@erase "$(INTDIR)\hmjre_mngr.obj"
	-@erase "$(INTDIR)\linklist.obj"
	-@erase "$(INTDIR)\menu.obj"
	-@erase "$(INTDIR)\misc.obj"
	-@erase "$(INTDIR)\optmap.obj"
	-@erase "$(INTDIR)\pathname.obj"
	-@erase "$(INTDIR)\rec_op.obj"
	-@erase "$(INTDIR)\recfind.obj"
	-@erase "$(INTDIR)\redll_mngr.obj"
	-@erase "$(INTDIR)\refcount.obj"
	-@erase "$(INTDIR)\seldir.obj"
	-@erase "$(INTDIR)\seq_op.obj"
	-@erase "$(INTDIR)\session.obj"
	-@erase "$(INTDIR)\shicons.obj"
	-@erase "$(INTDIR)\shobj_wrapper.obj"
	-@erase "$(INTDIR)\si_bregexp.obj"
	-@erase "$(INTDIR)\si_colorref.obj"
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
	-@erase "$(INTDIR)\stack.obj"
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

F90=df.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "..\common" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D _WIN32_WINNT=0x0501 /Fp"$(INTDIR)\common.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\common.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\common.lib" 
LIB32_OBJS= \
	"$(INTDIR)\bregexp_mngr.obj" \
	"$(INTDIR)\cmdline.obj" \
	"$(INTDIR)\colortbl.obj" \
	"$(INTDIR)\ctrldata.obj" \
	"$(INTDIR)\ctrlname.obj" \
	"$(INTDIR)\ddfile.obj" \
	"$(INTDIR)\dirlist.obj" \
	"$(INTDIR)\dlgdata.obj" \
	"$(INTDIR)\enumpath.obj" \
	"$(INTDIR)\env_var.obj" \
	"$(INTDIR)\file.obj" \
	"$(INTDIR)\hashtbl.obj" \
	"$(INTDIR)\hmjre_mngr.obj" \
	"$(INTDIR)\linklist.obj" \
	"$(INTDIR)\menu.obj" \
	"$(INTDIR)\misc.obj" \
	"$(INTDIR)\optmap.obj" \
	"$(INTDIR)\pathname.obj" \
	"$(INTDIR)\rec_op.obj" \
	"$(INTDIR)\recfind.obj" \
	"$(INTDIR)\refcount.obj" \
	"$(INTDIR)\seldir.obj" \
	"$(INTDIR)\seq_op.obj" \
	"$(INTDIR)\session.obj" \
	"$(INTDIR)\shicons.obj" \
	"$(INTDIR)\shobj_wrapper.obj" \
	"$(INTDIR)\si_bregexp.obj" \
	"$(INTDIR)\si_colorref.obj" \
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
	"$(INTDIR)\stack.obj" \
	"$(INTDIR)\str_tbl.obj" \
	"$(INTDIR)\strbuf.obj" \
	"$(INTDIR)\strutils.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\tokenizer.obj" \
	"$(INTDIR)\redll_mngr.obj"

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


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("common.dep")
!INCLUDE "common.dep"
!ELSE 
!MESSAGE Warning: cannot find "common.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "common - Win32 Release" || "$(CFG)" == "common - Win32 Debug"
SOURCE=.\bregexp_mngr.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\bregexp_mngr.obj"	"$(INTDIR)\bregexp_mngr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\bregexp_mngr.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\cmdline.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\cmdline.obj"	"$(INTDIR)\cmdline.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\cmdline.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\colortbl.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\colortbl.obj"	"$(INTDIR)\colortbl.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\colortbl.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ctrldata.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\ctrldata.obj"	"$(INTDIR)\ctrldata.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\ctrldata.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ctrlname.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\ctrlname.obj"	"$(INTDIR)\ctrlname.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\ctrlname.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ddfile.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\ddfile.obj"	"$(INTDIR)\ddfile.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\ddfile.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dirlist.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\dirlist.obj"	"$(INTDIR)\dirlist.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\dirlist.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dlgdata.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\dlgdata.obj"	"$(INTDIR)\dlgdata.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\dlgdata.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\enumpath.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\enumpath.obj"	"$(INTDIR)\enumpath.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\enumpath.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\env_var.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\env_var.obj"	"$(INTDIR)\env_var.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\env_var.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\file.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\file.obj"	"$(INTDIR)\file.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\file.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\hashtbl.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\hashtbl.obj"	"$(INTDIR)\hashtbl.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\hashtbl.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\hmjre_mngr.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\hmjre_mngr.obj"	"$(INTDIR)\hmjre_mngr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\hmjre_mngr.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\linklist.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\linklist.obj"	"$(INTDIR)\linklist.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\linklist.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\menu.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\menu.obj"	"$(INTDIR)\menu.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\menu.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\misc.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\misc.obj"	"$(INTDIR)\misc.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\misc.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\optmap.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\optmap.obj"	"$(INTDIR)\optmap.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\optmap.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\pathname.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\pathname.obj"	"$(INTDIR)\pathname.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\pathname.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\rec_op.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\rec_op.obj"	"$(INTDIR)\rec_op.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\rec_op.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\recfind.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\recfind.obj"	"$(INTDIR)\recfind.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\recfind.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\redll_mngr.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\redll_mngr.obj"	"$(INTDIR)\redll_mngr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\redll_mngr.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\refcount.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\refcount.obj"	"$(INTDIR)\refcount.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\refcount.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\seldir.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\seldir.obj"	"$(INTDIR)\seldir.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\seldir.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\seq_op.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\seq_op.obj"	"$(INTDIR)\seq_op.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\seq_op.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\session.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\session.obj"	"$(INTDIR)\session.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\session.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\shicons.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\shicons.obj"	"$(INTDIR)\shicons.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\shicons.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\shobj_wrapper.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\shobj_wrapper.obj"	"$(INTDIR)\shobj_wrapper.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\shobj_wrapper.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\si_bregexp.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\si_bregexp.obj"	"$(INTDIR)\si_bregexp.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\si_bregexp.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\si_colorref.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\si_colorref.obj"	"$(INTDIR)\si_colorref.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\si_colorref.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\si_comdlg.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\si_comdlg.obj"	"$(INTDIR)\si_comdlg.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\si_comdlg.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\si_common.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\si_common.obj"	"$(INTDIR)\si_common.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\si_common.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\si_dialog.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\si_dialog.obj"	"$(INTDIR)\si_dialog.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\si_dialog.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\si_file.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\si_file.obj"	"$(INTDIR)\si_file.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\si_file.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\si_image.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\si_image.obj"	"$(INTDIR)\si_image.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\si_image.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\si_menu.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\si_menu.obj"	"$(INTDIR)\si_menu.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\si_menu.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\si_string.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\si_string.obj"	"$(INTDIR)\si_string.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\si_string.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\si_variable.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\si_variable.obj"	"$(INTDIR)\si_variable.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\si_variable.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\smalloc.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\smalloc.obj"	"$(INTDIR)\smalloc.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\smalloc.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\spi_mngr.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\spi_mngr.obj"	"$(INTDIR)\spi_mngr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\spi_mngr.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\stack.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\stack.obj"	"$(INTDIR)\stack.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\stack.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\str_tbl.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\str_tbl.obj"	"$(INTDIR)\str_tbl.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\str_tbl.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\strbuf.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\strbuf.obj"	"$(INTDIR)\strbuf.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\strbuf.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\strutils.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\strutils.obj"	"$(INTDIR)\strutils.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\strutils.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\thread.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\thread.obj"	"$(INTDIR)\thread.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\thread.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\tokenizer.cpp

!IF  "$(CFG)" == "common - Win32 Release"


"$(INTDIR)\tokenizer.obj"	"$(INTDIR)\tokenizer.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "common - Win32 Debug"


"$(INTDIR)\tokenizer.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 

