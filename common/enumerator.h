//	$Id: enumerator.h,v 1.1 2002-02-15 17:46:08 sugiura Exp $
/*
 *	enumerator.h
 *	—ñ‹“ŽqƒNƒ‰ƒX
 */

#ifndef DENGAKUSERIES_CLASSES_ENUMERATOR
#define DENGAKUSERIES_CLASSES_ENUMERATOR

#include <windows.h>

template<class T>
class Enumerator {
public:
	virtual BOOL isValid() const = 0;
	virtual BOOL findNext() = 0;
	virtual T getValue() const = 0;
};

#endif
