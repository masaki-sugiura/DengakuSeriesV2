//	$Id: cmd.h,v 1.20 2008-08-15 05:47:40 sugiura Exp $
/*
 *	cmd.h
 *	コマンド名一覧
 */

#if	defined(DECLARE_DDECMD_EXECUTE)&&defined(DECLARE_DDECMD_REQUEST)&&defined(DECLARE_DDECMD_POKE)&&defined(DECLARE_DDECMD_FINAL)&&defined(DECLARE_DDECMD_HEADER)&&defined(DECLARE_DDECMD_FOOTER)
DECLARE_DDECMD_HEADER
	DECLARE_DDECMD_EXECUTE(exit)
	DECLARE_DDECMD_EXECUTE(sleep)
	DECLARE_DDECMD_EXECUTE(getcolor)
	DECLARE_DDECMD_EXECUTE(getfont)
	DECLARE_DDECMD_EXECUTE(setcurdir)
	DECLARE_DDECMD_EXECUTE(copy)
	DECLARE_DDECMD_EXECUTE(appendcopy)
	DECLARE_DDECMD_EXECUTE(appendcopytext)
	DECLARE_DDECMD_EXECUTE(rename)
	DECLARE_DDECMD_EXECUTE(delete)
	DECLARE_DDECMD_EXECUTE(remove)
	DECLARE_DDECMD_EXECUTE(move)
	DECLARE_DDECMD_EXECUTE(mkdir)
	DECLARE_DDECMD_EXECUTE(rmdir)
	DECLARE_DDECMD_EXECUTE(setattribute)
	DECLARE_DDECMD_EXECUTE(touch)
	DECLARE_DDECMD_EXECUTE(run)
	DECLARE_DDECMD_EXECUTE(iconrun)
	DECLARE_DDECMD_EXECUTE(maxrun)
	DECLARE_DDECMD_EXECUTE(hiderun)
	DECLARE_DDECMD_EXECUTE(enumpath)
	DECLARE_DDECMD_EXECUTE(enumfile)
	DECLARE_DDECMD_EXECUTE(enumdir)
	DECLARE_DDECMD_EXECUTE(getfilename)
	DECLARE_DDECMD_EXECUTE(getdirname)
	DECLARE_DDECMD_EXECUTE(dialog)
	DECLARE_DDECMD_EXECUTE(enddialog)
	DECLARE_DDECMD_EXECUTE(setstate)
	DECLARE_DDECMD_EXECUTE(setstring)
	DECLARE_DDECMD_EXECUTE(setimestate)
	DECLARE_DDECMD_EXECUTE(setsort)
	DECLARE_DDECMD_EXECUTE(setitem)
	DECLARE_DDECMD_EXECUTE(changeitem)
	DECLARE_DDECMD_EXECUTE(deleteitem)
	DECLARE_DDECMD_EXECUTE(resetlist)
	DECLARE_DDECMD_EXECUTE(enablectrl)
	DECLARE_DDECMD_EXECUTE(setctrlfont)
	DECLARE_DDECMD_EXECUTE(setfocuseditem)
	DECLARE_DDECMD_EXECUTE(message)
	DECLARE_DDECMD_EXECUTE(question)
	DECLARE_DDECMD_EXECUTE(savedialog)
	DECLARE_DDECMD_EXECUTE(loaddialog)
	DECLARE_DDECMD_EXECUTE(setdlgtitle)
	DECLARE_DDECMD_EXECUTE(setcurdlgpage)
	DECLARE_DDECMD_EXECUTE(setfocusedctrl)
	DECLARE_DDECMD_EXECUTE(setdlgpos)
	DECLARE_DDECMD_EXECUTE(setdlgimestate)
	DECLARE_DDECMD_EXECUTE(menu)
	DECLARE_DDECMD_EXECUTE(changemenuitemex)
	DECLARE_DDECMD_EXECUTE(deletemenuitemex)
	DECLARE_DDECMD_EXECUTE(delvar)
	DECLARE_DDECMD_EXECUTE(enumvar)
	DECLARE_DDECMD_EXECUTE(loadspi)
	DECLARE_DDECMD_EXECUTE(freespi)
	DECLARE_DDECMD_EXECUTE(bre_load)
	DECLARE_DDECMD_EXECUTE(bre_free)
	DECLARE_DDECMD_EXECUTE(hmjre_load)
	DECLARE_DDECMD_EXECUTE(hmjre_free)
	DECLARE_DDECMD_EXECUTE(hmjre_find_regular)
	DECLARE_DDECMD_EXECUTE(hmjre_find_regular_no_case_sense)
	DECLARE_DDECMD_EXECUTE(hmjre_get_last_match_length)
	DECLARE_DDECMD_EXECUTE(hmjre_option_dialog)
	DECLARE_DDECMD_EXECUTE(hmjre_get_last_match_tag_position)
	DECLARE_DDECMD_EXECUTE(hmjre_get_last_match_tag_length)
	DECLARE_DDECMD_EXECUTE(hmjre_env_changed)
	DECLARE_DDECMD_EXECUTE(clr_setname)
	DECLARE_DDECMD_EXECUTE(clr_load)
	DECLARE_DDECMD_EXECUTE(clr_save)
	DECLARE_DDECMD_REQUEST(result)
	DECLARE_DDECMD_REQUEST(keepalive)
	DECLARE_DDECMD_REQUEST(getversion)
	DECLARE_DDECMD_REQUEST(getlibversion)
	DECLARE_DDECMD_REQUEST(getosinfo)
	DECLARE_DDECMD_REQUEST(getcaretpos)
	DECLARE_DDECMD_REQUEST(getcursorpos)
	DECLARE_DDECMD_REQUEST(msgbox)
	DECLARE_DDECMD_REQUEST(getcurdir)
	DECLARE_DDECMD_REQUEST(isreadonly)
	DECLARE_DDECMD_REQUEST(sizeof)
	DECLARE_DDECMD_REQUEST(timestampof)
	DECLARE_DDECMD_REQUEST(timecountof)
	DECLARE_DDECMD_REQUEST(attributeof)
	DECLARE_DDECMD_REQUEST(existfile)
	DECLARE_DDECMD_REQUEST(findnext)
	DECLARE_DDECMD_REQUEST(fileopresult)
	DECLARE_DDECMD_REQUEST(getdrives)
	DECLARE_DDECMD_REQUEST(getlongname)
	DECLARE_DDECMD_REQUEST(getshortname)
	DECLARE_DDECMD_REQUEST(atoi)
	DECLARE_DDECMD_REQUEST(itoa)
	DECLARE_DDECMD_REQUEST(stricmp)
	DECLARE_DDECMD_REQUEST(strcount)
	DECLARE_DDECMD_REQUEST(gsub)
	DECLARE_DDECMD_REQUEST(reverse)
	DECLARE_DDECMD_REQUEST(tolower)
	DECLARE_DDECMD_REQUEST(toupper)
	DECLARE_DDECMD_REQUEST(strstr)
	DECLARE_DDECMD_REQUEST(strrstr)
	DECLARE_DDECMD_REQUEST(strlen)
	DECLARE_DDECMD_REQUEST(leftstr)
	DECLARE_DDECMD_REQUEST(midstr)
	DECLARE_DDECMD_REQUEST(rightstr)
	DECLARE_DDECMD_REQUEST(reverse2)
	DECLARE_DDECMD_REQUEST(tolower2)
	DECLARE_DDECMD_REQUEST(toupper2)
	DECLARE_DDECMD_REQUEST(tohankaku)
	DECLARE_DDECMD_REQUEST(tozenkakuhira)
	DECLARE_DDECMD_REQUEST(tozenkakukata)
	DECLARE_DDECMD_REQUEST(strstr2)
	DECLARE_DDECMD_REQUEST(strrstr2)
	DECLARE_DDECMD_REQUEST(strlen2)
	DECLARE_DDECMD_REQUEST(leftstr2)
	DECLARE_DDECMD_REQUEST(midstr2)
	DECLARE_DDECMD_REQUEST(rightstr2)
	DECLARE_DDECMD_REQUEST(ltrim)
	DECLARE_DDECMD_REQUEST(rtrim)
	DECLARE_DDECMD_REQUEST(gettoken)
	DECLARE_DDECMD_REQUEST(hasmoretokens)
	DECLARE_DDECMD_REQUEST(getdlgtitle)
	DECLARE_DDECMD_REQUEST(getcurdlgpage)
	DECLARE_DDECMD_REQUEST(getfocusedctrl)
	DECLARE_DDECMD_REQUEST(getstate)
	DECLARE_DDECMD_REQUEST(getstring)
	DECLARE_DDECMD_REQUEST(getimestate)
	DECLARE_DDECMD_REQUEST(getsort)
	DECLARE_DDECMD_REQUEST(getctrlfont)
	DECLARE_DDECMD_REQUEST(getitem)
	DECLARE_DDECMD_REQUEST(getfocuseditem)
	DECLARE_DDECMD_REQUEST(getsignature)
	DECLARE_DDECMD_REQUEST(getdlgpos)
	DECLARE_DDECMD_REQUEST(getdlgimestate)
	DECLARE_DDECMD_REQUEST(getdlgsize)
	DECLARE_DDECMD_REQUEST(getvar)
	DECLARE_DDECMD_REQUEST(existvar)
	DECLARE_DDECMD_REQUEST(nextvar)
	DECLARE_DDECMD_REQUEST(hasmorevars)
	DECLARE_DDECMD_REQUEST(getspiinfo)
	DECLARE_DDECMD_REQUEST(issupportedpic)
	DECLARE_DDECMD_REQUEST(getpicinfo)
	DECLARE_DDECMD_REQUEST(bre_errormsg)
	DECLARE_DDECMD_REQUEST(bre_version)
	DECLARE_DDECMD_REQUEST(bre_match)
	DECLARE_DDECMD_REQUEST(bre_subst)
	DECLARE_DDECMD_REQUEST(bre_trans)
	DECLARE_DDECMD_REQUEST(bre_split)
	DECLARE_DDECMD_REQUEST(bre_getnext)
	DECLARE_DDECMD_REQUEST(bre_postostr)
	DECLARE_DDECMD_REQUEST(bre_hasmoreresults)
	DECLARE_DDECMD_REQUEST(hmjre_get_version)
	DECLARE_DDECMD_REQUEST(hmjre_match)
	DECLARE_DDECMD_REQUEST(hmjre_get_tag_position)
	DECLARE_DDECMD_REQUEST(hmjre_get_match_string)
	DECLARE_DDECMD_REQUEST(clr_getrgb)
	DECLARE_DDECMD_REQUEST(clr_getname)
#if 0
	DECLARE_DDECMD_REQUEST(dt_format)
	DECLARE_DDECMD_REQUEST(dt_gettime)
	DECLARE_DDECMD_REQUEST(dt_filetime_to_time)
#endif
	DECLARE_DDECMD_POKE(newdialog)
	DECLARE_DDECMD_POKE(newdialogex)
	DECLARE_DDECMD_POKE(newdlgpage)
	DECLARE_DDECMD_POKE(newcontrol)
	DECLARE_DDECMD_POKE(item)
	DECLARE_DDECMD_POKE(height)
	DECLARE_DDECMD_POKE(default)
	DECLARE_DDECMD_POKE(ctrlfont)
	DECLARE_DDECMD_POKE(focuseditem)
	DECLARE_DDECMD_POKE(imestate)
	DECLARE_DDECMD_POKE(sort)
	DECLARE_DDECMD_POKE(notify)
	DECLARE_DDECMD_POKE(notifyex)
	DECLARE_DDECMD_POKE(newcolumn)
	DECLARE_DDECMD_POKE(newpage)
	DECLARE_DDECMD_POKE(part)
	DECLARE_DDECMD_POKE(dlgimestate)
	DECLARE_DDECMD_POKE(newmenu)
	DECLARE_DDECMD_POKE(newmenuex)
	DECLARE_DDECMD_POKE(menuitem)
	DECLARE_DDECMD_POKE(menuitemex)
	DECLARE_DDECMD_POKE(setvar)
	DECLARE_DDECMD_FINAL
DECLARE_DDECMD_FOOTER
#endif
