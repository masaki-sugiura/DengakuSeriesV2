// $Id: pdparser.h,v 1.2 2007/03/05 13:04:54 sugiura Exp $
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
