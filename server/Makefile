# $Id: Makefile,v 1.1.1.1 2001/10/07 14:41:22 sugiura Exp $

CC = gcc
ifdef MINGW
CC_COPT_MINGW = -mno-cygwin
else
CC_COPT_MINGW =
endif
CC_COPT_COMMON = -c -O2 -Wall -pipe -mwindows $(CC_COPT_MINGW) \
                 -fvtable-thunks -I. -I../common -I-
ifdef DEBUG
CC_COPT = -g -D_DEBUG $(CC_COPT_COMMON)
else
CC_COPT = -DNO_DEBUG $(CC_COPT_COMMON)
endif
RC = windres
RC_OPT = --include-dir ../common --language 0x0111

SRCS = main.cpp ddeserv.cpp convdata.cpp opencomdlg.cpp pdparser.cpp \
       ddeobjs.cpp cmdname.cpp common.cpp \
       ss_handler.cpp fs_handler.cpp cs_handler.cpp ms_handler.cpp \
       cds_handler.cpp is_handler.cpp es_handler.cpp ds_handler.cpp
RCFILE = DengakuServer.rc

OBJS = $(SRCS:.cpp=.o)
RESFILE = $(RCFILE:.rc=.coff)

TARGET = dgserver.exe

COMMON_H_INC = common.h ../common/str_tbl.h
CMD_H_INC = cmd.h
DDEOBJS_H_INC = ddeobjs.h
OPENCOMDLG_H_INC = opencomdlg.h
DDESINFO_H_INC = ddesinfo.h ../common/strbuf.h
CONVDATA_H_INC = convdata.h $(DDEOBJS_H_INC) $(CMD_H_INC) $(OPENCOMDLG_H_INC) \
                 ../common/session.h
CMDNAME_H_INC = cmdname.h $(CONVDATA_H_INC)
PDPARSER_H_INC = pdparser.h ../common/cmdline.h
DDESERV_H_INC = ddeserv.h resource.h $(CMDNAME_H_INC) $(COMMON_H_INC) \
                ../common/file.h

COMMON_CPP_INC = $(COMMON_H_INC)
CMDNAME_CPP_INC = $(CMDNAME_H_INC)
PDPARSER_CPP_INC = $(PDPARSER_H_INC)
OPENCOMDLG_CPP_INC = $(OPENCOMDLG_H_INC) $(DDESERV_H_INC) $(CONVDATA_H_INC) \
                     ../common/cmdline.h ../common/seldir.h
MAIN_CPP_INC = resource.h $(COMMON_H_INC) $(DDESERV_H_INC) $(DDESINFO_H_INC)
DDEOBJS_CPP_INC = $(DDEOBJS_H_INC)
CONVDATA_CPP_INC = $(CONVDATA_H_INC) $(DDESERV_H_INC) $(COMMON_H_INC) \
                   ../common/cmdline.h
DDESERV_CPP_INC = $(COMMON_H_INC) $(DDESERV_H_INC) $(DDESINFO_H_INC) \
                  $(PDPARSER_H_INC) ../common/cmdline.h
CMDNAME_CPP_INC = $(CMDNAME_H_INC) $(DDESERV_H_INC) $(CONVDATA_H_INC) \
                  $(CMD_H_INC)
CDS_HANDLER_CPP_INC = $(DDESERV_H_INC) $(CONVDATA_H_INC) $(OPENCOMDLG_H_INC) \
                      ../common/cmdline.h
CS_HANDLER_CPP_INC = $(DDESERV_H_INC) $(CONVDATA_H_INC) $(COMMON_H_INC) \
                     ../common/cmdline.h
DS_HANDLER_CPP_INC = $(DDESERV_H_INC) $(CONVDATA_H_INC) $(COMMON_H_INC) \
                     $(CTRLDATA_H_INC) ../common/cmdline.h
ES_HANDLER_CPP_INC = $(DDESERV_H_INC) $(CONVDATA_H_INC) ../common/cmdline.h
FS_HANDLER_CPP_INC = $(DDESERV_H_INC) $(CONVDATA_H_INC) $(COMMON_H_INC) \
                     ../common/cmdline.h
IS_HANDLER_CPP_INC = $(DDESERV_H_INC) $(CONVDATA_H_INC) $(COMMON_H_INC) \
                     ../common/cmdline.h
MS_HANDLER_CPP_INC = $(DDESERV_H_INC) $(CONVDATA_H_INC) \
                     ../common/misc.h ../common/cmdline.h
SS_HANDLER_CPP_INC = $(DDESERV_H_INC) $(CONVDATA_H_INC) ../common/cmdline.h

.SUFFIXES: .cpp .o .rc .coff

.cpp.o:; $(CC) $(CC_COPT) -o $@ $<
.rc.coff:; $(RC) $(RC_OPT) $< $@

$(TARGET): $(OBJS) $(RESFILE) lib
	$(CC) $(CC_COPT_MINGW) -mwindows -fvtable-thunks -o $(TARGET) \
          $(OBJS) $(RESFILE) -L../common -lDengaku \
          -lcomctl32 -limm32

lib:; $(MAKE) -C ../common DEBUG=$(DEBUG) MINGW=$(MINGW)

main.o: main.cpp $(MAIN_CPP_INC)
ddeserv.o: ddeserv.cpp $(DDESERV_CPP_INC)
ddesinfo.o: ddesinfo.cpp $(DDESINFO_CPP_INC)
ddeobjs.o: ddeobjs.cpp $(DDEOBJS_CPP_INC)
convdata.o: convdata.cpp $(CONVDATA_CPP_INC)
common.o: common.cpp $(COMMON_CPP_INC)
cmdname.o: cmdname.cpp $(CMDNAME_CPP_INC)
pdparser.o: pdparser.cpp $(PDPARSER_CPP_INC)
opencomdlg.o: opencomdlg.cpp $(OPENCOMDLG_CPP_INC)
cds_handler.o: cds_handler.cpp $(CDS_HANDLER_CPP_INC)
cs_handler.o: cs_handler.cpp $(CS_HANDLER_CPP_INC)
ds_handler.o: ds_handler.cpp $(DS_HANDLER_CPP_INC)
es_handler.o: es_handler.cpp $(ES_HANDLER_CPP_INC)
fs_handler.o: fs_handler.cpp $(FS_HANDLER_CPP_INC)
is_handler.o: is_handler.cpp $(IS_HANDLER_CPP_INC)
ms_handler.o: ms_handler.cpp $(MS_HANDLER_CPP_INC)
ss_handler.o: ss_handler.cpp $(SS_HANDLER_CPP_INC)
DengakuServer.coff: DengakuServer.rc DengakuServer.ico ../common/sddlg_templ.rc

clean:; rm -f $(OBJS) $(RESFILE) $(TARGET)
clean_lib:; $(MAKE) -C ../common clean

