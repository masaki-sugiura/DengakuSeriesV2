//	$Id: evalexpr.cpp,v 1.1.1.1 2001/10/07 14:41:22 sugiura Exp $
/*
 *	evalexpr.cpp
 *	class EvalExpression ‚ÌŽÀ‘•
 */

#include <stdio.h>
#include "evalexpr.h"

StringBuffer EvalExpression::m_expr(40);
LPCSTR EvalExpression::m_pstr;

StringBuffer
EvalExpression::eval(const StringBuffer &fmt, const StringBuffer &expr)
{
	m_expr = expr;
	m_pstr = (LPCSTR)m_expr;
	static TCHAR buf[40];
	buf[0] = '\0';
	try {
		double ret = expression();
		sprintf(buf,(LPCSTR)fmt,ret);
	} catch (InvalidExpressionException &e) {
		::MessageBox(NULL,e.what(),"“cŠyDLL Error:",MB_OK);
	}
	return buf;
}

void
EvalExpression::next()
{
	do {
		if (*m_pstr == '\0' || *++m_pstr == '\0') return;
	} while (*m_pstr == ' ' || *m_pstr == '\t') ;
}

double
EvalExpression::expression()
{
	double ret = term();
	for ( ; ; ) {
		if (*m_pstr == '+') {
			next();
			ret += term();
		} else if (*m_pstr == '-') {
			next();
			ret -= term();
		} else break;
	}
	return ret;
}

double
EvalExpression::term()
{
	double ret = factor();
	for ( ; ; ) {
		if (*m_pstr == '*') {
			next();
			ret *= factor();
		} else if (*m_pstr == '/') {
			next();
			double dev = factor();
			if (dev == 0.0) throw InvalidExpressionException("Devided by zero.");
			ret /= dev;
		} else break;
	}
	return ret;
}

double
EvalExpression::factor()
{
	double ret = 0.0;
	if (*m_pstr == '(') {
		next();
		ret = expression();
		if (*m_pstr != ')') throw InvalidExpressionException("Parenthesis does not match.");
	} else if (lstrchr("+-.0123456789",*m_pstr) != NULL) {
		ret = dval(m_pstr);
		if (*m_pstr == '+' || *m_pstr == '-') m_pstr++;
		while (IsCharSpace(*m_pstr)) m_pstr++;
		while (lstrchr(".0123456789",*m_pstr) != NULL) m_pstr++;
		m_pstr--;
	} else {
		StringBuffer msg("Unrecognizable character is found : ");
		msg.append((TCHAR)*m_pstr);
		throw InvalidExpressionException(msg);
	}
	next();
	return ret;
}

