//	$Id: enumerator.h,v 1.2 2002/03/05 14:09:40 sugiura Exp $
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
	virtual ~Enumerator() = 0 {}
	virtual BOOL isValid() const = 0;
	virtual BOOL findNext() = 0;
	virtual T getValue() const = 0;
};

#endif
