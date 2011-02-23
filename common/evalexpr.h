// $Id: evalexpr.h,v 1.1.1.1 2001/10/07 14:41:22 sugiura Exp $
/*
 * evalexpr.h
 * 計算式の評価を行うクラス
 */

#ifndef DENGAKUSERIES_CLASSES_EVAL_EXPRESSION
#define DENGAKUSERIES_CLASSES_EVAL_EXPRESSION

#include "strbuf.h"
#include <exception>

class InvalidExpressionException : public exception {
public:
	InvalidExpressionException(const StringBuffer& msg)
		:	m_strMsg(msg) {}
	LPCSTR what() const { return (LPCSTR)m_strMsg; }

private:
	const StringBuffer m_strMsg;
};

class EvalExpression {
public:
	static StringBuffer eval(const StringBuffer& fmt,
							const StringBuffer& expr);

private:
	static StringBuffer	m_expr;
	static LPCSTR m_pstr;

	static void next();
	static double expression();
	static double term();
	static double factor();
};

#endif

