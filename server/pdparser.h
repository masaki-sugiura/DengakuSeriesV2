// $Id: pdparser.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	pdparser.h
 *	Poke データをパースするクラスの定義
 */

#ifndef DENGAKUSERVER_CLASSES_PDPARSER
#define DENGAKUSERVER_CLASSES_PDPARSER

#include "cmdline.h"

class PokeDataParser : public CmdLineParser {
public:
	PokeDataParser(const StringBuffer&);
	int mergeArgv(const StringBuffer&);
};

#endif
