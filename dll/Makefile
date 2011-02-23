# $Id: Makefile,v 1.1.1.1 2001/10/07 14:41:22 sugiura Exp $

CC = gcc
ifdef DEBUG
CC_COPT = -c -g -D_DEBUG -DDENGAKUDLL_EXPORTS \
          -O2 -Wall -pipe -mwindows -mno-cygwin \
          -fvtable-thunks -I. -I../common -I-
else
CC_COPT = -c -DNO_DEBUG -DDENGAKUDLL_EXPORTS \
          -O2 -Wall -pipe -mwindows -mno-cygwin \
          -fvtable-thunks -I. -I../common -I-
endif
RC = windres
RC_OPT = --include-dir ../common --language 0x0111

SRCS = DengakuDLL.cpp ss_func.cpp fs_func.cpp cs_func.cpp ms_func.cpp \
       cds_func.cpp is_func.cpp es_func.cpp ds_func.cpp
RCFILE = seldir.rc

OBJS = $(SRCS:.cpp=.o)
RESFILE = $(RCFILE:.rc=.coff)

TARGET = DengakuDLL.dll

DENGAKUDLL_H_INC = DengakuDLL.h ../common/session.h
CDS_FUNC_CPP_INC = $(DENGAKUDLL_H_INC) ../common/seldir.h ../common/cmdline.h
CS_FUNC_CPP_INC = $(DENGAKUDLL_H_INC) ../common/cmdline.h ../common/evalexpr.h
DENGAKUDLL_CPP_INC = $(DENGAKUDLL_H_INC)
DS_FUNC_CPP_INC = $(DENGAKUDLL_H_INC) ../common/cmdline.h
ES_FUNC_CPP_INC = $(DENGAKUDLL_H_INC)
FS_FUNC_CPP_INC = $(DENGAKUDLL_H_INC) ../common/cmdline.h
IS_FUNC_CPP_INC = $(DENGAKUDLL_H_INC)
MS_FUNC_CPP_INC = $(DENGAKUDLL_H_INC)
SS_FUNC_CPP_INC = $(DENGAKUDLL_H_INC)

.SUFFIXES: .cpp .o .rc .coff

.cpp.o:; $(CC) $(CC_COPT) -o $@ $<
.rc.coff:; $(RC) $(RC_OPT) $< $@

$(TARGET): $(OBJS) $(RESFILE) lib
	$(CC) -shared -mwindows -mno-cygwin -o $(TARGET) $(OBJS) $(RESFILE) \
          -L../common -lDengaku -lcomctl32 -limm32

lib:; make -C ../common DEBUG=$(DEBUG)

DengakuDLL.o: DengakuDLL.cpp $(DENGAKUDLL_CPP_INC)
cds_func.o: cds_func.cpp $(CDS_FUNC_CPP_INC)
cs_func.o: cs_func.cpp $(CS_FUNC_CPP_INC)
ds_func.o: ds_func.cpp $(DS_FUNC_CPP_INC)
es_func.o: es_func.cpp $(ES_FUNC_CPP_INC)
fs_func.o: fs_func.cpp $(FS_FUNC_CPP_INC)
is_func.o: is_func.cpp $(IS_FUNC_CPP_INC)
ms_func.o: ms_func.cpp $(MS_FUNC_CPP_INC)
ss_func.o: ss_func.cpp $(SS_FUNC_CPP_INC)
seldir.coff: seldir.rc ../common/sddlg_templ.rc

clean:; rm -f $(OBJS) $(RES) $(TARGET)

