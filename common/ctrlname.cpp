// $Id: ctrlname.cpp,v 1.3 2003/12/03 17:17:58 sugiura Exp $
/*
 *	コントロール名テーブル
 */

#include "strutils.h"
#include "ctrlname.h"
#include "ctrldata.h"

#define	DECLARE_CTRL_HEADER		CTRLSEARCHKEY	ctrlname[] = {
#define	DECLARE_CTRL_CTRL(name)		{sizeof(#name) - 1, #name, CTRLID_##name},
#define	DECLARE_CTRL_LAST			{0,"UNKNOWN",CTRLID_UNKNOWN}
#define	DECLARE_CTRL_FOOTER		};
#include "ctrl.h"
#undef	DECLARE_CTRL_HEADER
#undef	DECLARE_CTRL_CTRL
#undef	DECLARE_CTRL_LAST
#undef	DECLARE_CTRL_FOOTER

CTRL_ID
CtrlListItem::getCtrlTypeFromName(const StringBuffer& name)
{
	int	len = name.length();
	if (len <= 0) return CTRLID_UNKNOWN;
	StringBuffer name2(name);
	name2.toUpper();
	for (int i = 0; ; i++) {
		if (ctrlname[i].m_nCtrlNameLength == 0) break;
		else if (ctrlname[i].m_nCtrlNameLength == len &&
				 lstrcmpn(name2,ctrlname[i].m_pszCtrlName,len) == 0) {
			return ctrlname[i].m_ctrlID;
		}
	}
	return CTRLID_UNKNOWN;
}

