# $Id: Makefile,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $

CC = gcc
ifdef MINGW
CC_COPT_MINGW = -mno-cygwin
else
CC_COPT_MINGW = 
endif
CC_COPT_COMMON = -c -O2 -Wall -pipe -mwindows $(CC_COPT_MINGW) \
                 -fvtable-thunks
ifdef DEBUG
CC_COPT = -g -D_DEBUG $(CC_COPT_COMMON)
else
CC_COPT = -DNO_DEBUG $(CC_COPT_COMMON)
endif
AR = ar
AR_OPT = rcv

SRCS =	si_string.cpp si_menu.cpp si_file.cpp si_common.cpp si_comdlg.cpp \
		si_variable.cpp si_image.cpp si_dialog.cpp \
		strbuf.cpp tokenizer.cpp linklist.cpp hashtbl.cpp refcount.cpp \
		thread.cpp misc.cpp str_tbl.cpp evalexpr.cpp \
		pathname.cpp file.cpp seq_op.cpp seldir.cpp enumpath.cpp \
		recfind.cpp rec_op.cpp dirlist.cpp \
		env_var.cpp smalloc.cpp \
		dlgdata.cpp ctrldata.cpp ctrlname.cpp shicons.cpp ddfile.cpp \
		spi_mngr.cpp menu.cpp cmdline.cpp session.cpp
OBJS =	$(SRCS:.cpp=.o)

TARGET = libDengaku.a

AUTO_PTR_H_INC  = auto_ptr.h
THREAD_H_INC    = thread.h
STACK_H_INC     = stack.h
REFCOUNT_H_INC  = refcount.h
SEQ_OP_H_INC    = seq_op.h
STRBUF_H_INC    = strbuf.h $(REFCOUNT_H_INC)
HASHTBL_H_INC   = hashtbl.h $(STRBUF_H_INC)
LINKLIST_H_INC  = linklist.h $(HASHTBL_H_INC)
CMDLINE_H_INC   = cmdline.h $(LINKLIST_H_INC)
TOKENIZER_H_INC = tokenizer.h $(STRBUF_H_INC)
SHAREDBUF_H_INC = sharedbuf.h $(LINKLIST_H_INC)
EVALEXPR_H_INC  = evalexpr.h $(STRBUF_H_INC)
STR_TBL_H_INC   = str_tbl.h $(STRBUF_H_INC)
MISC_H_INC      = misc.h $(STRBUF_H_INC)
PATHNAME_H_INC  = pathname.h $(STRBUF_H_INC)
DIRLIST_H_INC   = dirlist.h $(STRBUF_H_INC)
ENUMPATH_H_INC  = enumpath.h $(LINKLIST_H_INC) $(STACK_H_INC) $(PATHNAME_H_INC)
REC_OP_H_INC    = rec_op.h $(PATHNAME_H_INC)
RECFIND_H_INC   = recfind.h $(STRBUF_H_INC) $(ENUMPATH_H_INC)
SHICONS_H_INC   = shicons.h $(HASHTBL_H_INC)
SELDIR_H_INC    = seldir.h $(STRBUF_H_INC)
SPI_MNGR_H_INC  = spi_mngr.h $(STRBUF_H_INC)
SMALLOC_H_INC   = smalloc.h $(STRBUF_H_INC)
ENV_VAR_H_INC   = env_var.h $(SMALLOC_H_INC)
DDFILE_H_INC    = ddfile.h $(STRBUF_H_INC)
CTRL_H_INC      = ctrl.h
CTRLNAME_H_INC  = ctrlname.h $(STRBUF_H_INC) $(CTRL_H_INC)
CTRLDATA_H_INC  = ctrldata.h $(DDFILE_H_INC) $(CTRLNAME_H_INC) \
                  $(LINKLIST_H_INC)
DLGDATA_H_INC   = dlgdata.h $(LINKLIST_H_INC) $(CTRLNAME_H_INC)
MENU_H_INC      = menu.h $(LINKLIST_H_INC)
SESSION_H_INC   = session.h $(AUTO_PTR_H_INC) $(DLGDATA_H_INC) \
                  $(MENU_H_INC) $(RECFIND_H_INC) $(ENV_VAR_H_INC) \
                  $(SPI_MNGR_H_INC) $(SHAREDBUF_H_INC) $(TOKENIZER_H_INC) \
                  $(THREAD_H_INC)

.SUFFIXES:	.cpp .o .a

.cpp.o:; $(CC) $(CC_COPT) -o $@ $<


lib: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) $(AR_OPT) $(TARGET) $(OBJS)

clean:; rm -f $(OBJS) $(TARGET)

cmdline.o: cmdline.cpp $(CMDLINE_H_INC)
ctrldata.o: ctrldata.cpp $(SESSION_H_INC) $(CTRLDATA_H_INC) \
            $(DLGDATA_H_INC) $(CMDLINE_H_INC) $(TOKENIZER_H_INC) \
            $(MISC_H_INC)
ctrlname.o: ctrlname.cpp $(CTRLNAME_H_INC) $(CTRLDATA_H_INC) $(CTRL_H_INC)
ddfile.o: ddfile.cpp $(DDFILE_H_INC) $(PATHNAME_H_INC)
dirlist.o: dirlist.cpp $(DIRLIST_H_INC) $(PATHNAME_H_INC) $(STACK_H_INC)
dlgdata.o: dlgdata.cpp $(SESSION_H_INC) $(DLGDATA_H_INC) $(STACK_H_INC) \
           $(DDFILE_H_INC) $(CTRLDATA_H_INC) $(MISC_H_INC) $(STRBUF_H_INC)
enumpath.o: enumpath.cpp $(ENUMPATH_H_INC) $(TOKENIZER_H_INC)
env_var.o: env_var.cpp $(ENV_VAR_H_INC)
evalexpr.o: evalexpr.cpp $(EVALEXPR_H_INC)
file.o: file.cpp $(FILE_H_INC)
linklist.o: linklist.cpp $(LINKLIST_H_INC)
menu.o: menu.cpp $(SESSION_H_INC) $(MENU_H_INC)
misc.o: misc.cpp $(MISC_H_INC)
pathname.o: pathname.cpp $(PATHNAME_H_INC)
recfind.o: recfind.cpp $(RECFIND_H_INC)
rec_op.o: rec_op.cpp $(REC_OP_H_INC) $(FILE_H_INC) $(DIRLIST_H_INC) \
          $(AUTO_PTR_H_INC)
refcount.o: refcount.cpp $(REFCOUNT_H_INC)
seldir.o: seldir.cpp $(SESSION_H_INC) $(SELDIR_H_INC) $(SHICONS_H_INC) \
          resource.h
seq_op.o: seq_op.cpp $(SEQ_OP_H_INC) $(DIRLIST_H_INC) $(PATHNAME_H_INC) \
          $(PATHNAME_H_INC) $(REC_OP_H_INC) $(CMDLINE_H_INC)
session.o: session.cpp $(SESSION_H_INC)
shicons.o: shicons.cpp $(SHICONS_H_INC)
si_comdlg.o: si_comdlg.cpp $(SESSION_H_INC) $(DIRLIST_H_INC) \
             $(CMDLINE_H_INC) $(SELDIR_H_INC) $(MISC_H_INC)
si_common.o: si_common.cpp $(SESSION_H_INC) $(CMDLINE_H_INC) \
             $(EVALEXPR_H_INC) $(STR_TBL_H_INC)
si_dialog.o: si_dialog.cpp $(SESSION_H_INC) $(DLGDATA_H_INC) \
             $(CTRLDATA_H_INC) $(CMDLINE_H_INC) $(DDFILE_H_INC) \
             $(PATHNAME_H_INC) $(DIRLIST_H_INC) $(MISC_H_INC) $(STR_TBL_H_INC)
si_file.o: si_file.cpp $(SESSION_H_INC) $(SEQ_OP_H_INC) $(RECFIND_H_INC) \
           $(FILE_H_INC) $(CMDLINE_H_INC)
si_image.o: si_image.cpp $(SESSION_H_INC) $(CMDLINE_H_INC) $(SPI_MNGR_H_INC)
si_menu.o: si_menu.cpp $(SESSION_H_INC) $(CMDLINE_H_INC) $(MENU_H_INC) \
           $(STR_TBL_H_INC)
si_string.o: si_string.cpp $(SESSION_H_INC) $(CMDLINE_H_INC) $(TOKENIZER_H_INC)
si_variable.o: si_variable.cpp $(SESSION_H_INC) $(CMDLINE_H_INC) \
               $(ENV_VAR_H_INC)
smalloc.o: smalloc.cpp $(SMALLOC_H_INC)
spi_mngr.o: spi_mngr.cpp $(SPI_MNGR_H_INC) $(PATHNAME_H_INC) $(FILE_H_INC)
strbuf.o: strbuf.cpp $(STRBUF_H_INC)
str_tbl.o: str_tbl.cpp $(STR_TBL_H_INC)
thread.o: thread.cpp $(THREAD_H_INC)
tokenizer.o: tokenizer.cpp $(TOKENIZER_H_INC)

