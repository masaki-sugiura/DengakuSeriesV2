# Microsoft Developer Studio Project File - Name="common" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=common - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "common.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
F90=df.exe
RSC=rc.exe

!IF  "$(CFG)" == "common - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE F90 /compile_only /include:"Release/" /nologo /warn:nofileopt
# ADD F90 /compile_only /include:"Release/" /nologo /warn:nofileopt
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "..\common" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /i "..\common" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "common - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE F90 /check:bounds /compile_only /debug:full /include:"Debug/" /nologo /traceback /warn:argument_checking /warn:nofileopt
# ADD F90 /check:bounds /compile_only /debug:full /include:"Debug/" /nologo /traceback /warn:argument_checking /warn:nofileopt
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "..\common" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /i "..\common" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "common - Win32 Release"
# Name "common - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;f90;for;f;fpp"
# Begin Source File

SOURCE=.\bregexp_mngr.cpp
# End Source File
# Begin Source File

SOURCE=.\cmdline.cpp
# End Source File
# Begin Source File

SOURCE=.\colortbl.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrldata.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrlname.cpp
# End Source File
# Begin Source File

SOURCE=.\ddfile.cpp
# End Source File
# Begin Source File

SOURCE=.\dirlist.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgdata.cpp
# End Source File
# Begin Source File

SOURCE=.\enumpath.cpp
# End Source File
# Begin Source File

SOURCE=.\env_var.cpp
# End Source File
# Begin Source File

SOURCE=.\file.cpp
# End Source File
# Begin Source File

SOURCE=.\hashtbl.cpp
# End Source File
# Begin Source File

SOURCE=.\linklist.cpp
# End Source File
# Begin Source File

SOURCE=.\menu.cpp
# End Source File
# Begin Source File

SOURCE=.\misc.cpp
# End Source File
# Begin Source File

SOURCE=.\pathname.cpp
# End Source File
# Begin Source File

SOURCE=.\rec_op.cpp
# End Source File
# Begin Source File

SOURCE=.\recfind.cpp
# End Source File
# Begin Source File

SOURCE=.\refcount.cpp
# End Source File
# Begin Source File

SOURCE=.\seldir.cpp
# End Source File
# Begin Source File

SOURCE=.\seq_op.cpp
# End Source File
# Begin Source File

SOURCE=.\session.cpp
# End Source File
# Begin Source File

SOURCE=.\shicons.cpp
# End Source File
# Begin Source File

SOURCE=.\shobj_wrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\si_bregexp.cpp
# End Source File
# Begin Source File

SOURCE=.\si_colorref.cpp
# End Source File
# Begin Source File

SOURCE=.\si_comdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\si_common.cpp
# End Source File
# Begin Source File

SOURCE=.\si_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\si_file.cpp
# End Source File
# Begin Source File

SOURCE=.\si_image.cpp
# End Source File
# Begin Source File

SOURCE=.\si_menu.cpp
# End Source File
# Begin Source File

SOURCE=.\si_string.cpp
# End Source File
# Begin Source File

SOURCE=.\si_variable.cpp
# End Source File
# Begin Source File

SOURCE=.\smalloc.cpp
# End Source File
# Begin Source File

SOURCE=.\spi_mngr.cpp
# End Source File
# Begin Source File

SOURCE=.\stack.cpp
# End Source File
# Begin Source File

SOURCE=.\str_tbl.cpp
# End Source File
# Begin Source File

SOURCE=.\strbuf.cpp
# End Source File
# Begin Source File

SOURCE=.\strutils.cpp
# End Source File
# Begin Source File

SOURCE=.\thread.cpp
# End Source File
# Begin Source File

SOURCE=.\tokenizer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\array.h
# End Source File
# Begin Source File

SOURCE=.\auto_ptr.h
# End Source File
# Begin Source File

SOURCE=.\bitarray.h
# End Source File
# Begin Source File

SOURCE=.\block_alloc.h
# End Source File
# Begin Source File

SOURCE=.\BREGEXP.H
# End Source File
# Begin Source File

SOURCE=.\bregexp_mngr.h
# End Source File
# Begin Source File

SOURCE=.\cmdline.h
# End Source File
# Begin Source File

SOURCE=.\colortbl.h
# End Source File
# Begin Source File

SOURCE=.\ctrl.h
# End Source File
# Begin Source File

SOURCE=.\ctrldata.h
# End Source File
# Begin Source File

SOURCE=.\ctrlname.h
# End Source File
# Begin Source File

SOURCE=.\ddfile.h
# End Source File
# Begin Source File

SOURCE=.\dirlist.h
# End Source File
# Begin Source File

SOURCE=.\dlgdata.h
# End Source File
# Begin Source File

SOURCE=.\enumerator.h
# End Source File
# Begin Source File

SOURCE=.\enumpath.h
# End Source File
# Begin Source File

SOURCE=.\env_var.h
# End Source File
# Begin Source File

SOURCE=.\file.h
# End Source File
# Begin Source File

SOURCE=.\hashtbl.h
# End Source File
# Begin Source File

SOURCE=.\linklist.h
# End Source File
# Begin Source File

SOURCE=.\menu.h
# End Source File
# Begin Source File

SOURCE=.\misc.h
# End Source File
# Begin Source File

SOURCE=.\pathname.h
# End Source File
# Begin Source File

SOURCE=.\rec_op.h
# End Source File
# Begin Source File

SOURCE=.\recfind.h
# End Source File
# Begin Source File

SOURCE=.\refcount.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\seldir.h
# End Source File
# Begin Source File

SOURCE=.\seq_op.h
# End Source File
# Begin Source File

SOURCE=.\session.h
# End Source File
# Begin Source File

SOURCE=.\sharedbuf.h
# End Source File
# Begin Source File

SOURCE=.\shicons.h
# End Source File
# Begin Source File

SOURCE=.\shobj_wrapper.h
# End Source File
# Begin Source File

SOURCE=.\smalloc.h
# End Source File
# Begin Source File

SOURCE=.\sorter.h
# End Source File
# Begin Source File

SOURCE=.\spi_mngr.h
# End Source File
# Begin Source File

SOURCE=.\stack.h
# End Source File
# Begin Source File

SOURCE=.\str_tbl.h
# End Source File
# Begin Source File

SOURCE=.\strbuf.h
# End Source File
# Begin Source File

SOURCE=.\strutils.h
# End Source File
# Begin Source File

SOURCE=.\thread.h
# End Source File
# Begin Source File

SOURCE=.\tokenizer.h
# End Source File
# End Group
# End Target
# End Project
