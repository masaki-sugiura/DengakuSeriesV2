// $Id: ThemeWrapper.h,v 1.2 2004/04/30 15:27:18 sugiura Exp $
/*
 *	stack.h
 *	スタッククラス(テンプレート)
 */

#ifndef THEME_WRAPPER_H_INC
#define THEME_WRAPPER_H_INC

#include <windows.h>
#include <UxTheme.h>
#include <TmSchema.h>

#define GET_ADDR(func) \
	m_pfn##func = (pfn##func)::GetProcAddress(m_hmUxTheme, #func); \
	if (!m_pfn##func) { \
		::FreeLibrary(m_hmUxTheme); \
		throw ThemeNotSupportedError(); \
	}

class ThemeNotSupportedError {};

class ThemeWrapper {
public:
	ThemeWrapper()
	{
		m_hmUxTheme = ::LoadLibrary("UxTheme.dll");
		if (!m_hmUxTheme) throw ThemeNotSupportedError();

		GET_ADDR(SetThemeAppProperties);
		GET_ADDR(IsThemeActive);
		GET_ADDR(EnableThemeDialogTexture);
		GET_ADDR(OpenThemeData);
		GET_ADDR(CloseThemeData);
		GET_ADDR(GetThemeColor);
		GET_ADDR(DrawThemeBackground);
		GET_ADDR(DrawThemeParentBackground);
		GET_ADDR(GetThemeBackgroundContentRect);
		GET_ADDR(GetWindowTheme);
		GET_ADDR(SetWindowTheme);
	}
	~ThemeWrapper()
	{
		::FreeLibrary(m_hmUxTheme);
	}

	void STDAPICALLTYPE SetThemeAppProperties(DWORD prop)
	{
		(*m_pfnSetThemeAppProperties)(prop);
	}

	BOOL STDAPICALLTYPE IsThemeActive()
	{
		return (*m_pfnIsThemeActive)();
	}

	HRESULT STDAPICALLTYPE EnableThemeDialogTexture(HWND hWnd, DWORD dwFlags)
	{
		return (*m_pfnEnableThemeDialogTexture)(hWnd, dwFlags);
	}

	HTHEME STDAPICALLTYPE OpenThemeData(HWND hWnd, LPCWSTR pszwClassName)
	{
		return (*m_pfnOpenThemeData)(hWnd, pszwClassName);
	}

	HRESULT STDAPICALLTYPE CloseThemeData(HTHEME hTheme)
	{
		return (*m_pfnCloseThemeData)(hTheme);
	}

	HRESULT STDAPICALLTYPE GetThemeColor(HTHEME hTheme,
										 int iPart, int iState, int iProp,
										 COLORREF* pCRef)
	{
		return (*m_pfnGetThemeColor)(hTheme, iPart, iState, iProp, pCRef);
	}

	HRESULT STDAPICALLTYPE DrawThemeBackground(HTHEME hTheme, HDC hDC,
											   int iPart, int iState,
											   const RECT* pRect,
											   const RECT* pClipRect)
	{
		return (*m_pfnDrawThemeBackground)(hTheme, hDC, iPart, iState, pRect, pClipRect);
	}

	HRESULT STDAPICALLTYPE DrawThemeParentBackground(HWND hWnd, HDC hDC,
													 const RECT* pRect)
	{
		return (*m_pfnDrawThemeParentBackground)(hWnd, hDC, pRect);
	}

	HRESULT STDAPICALLTYPE GetThemeBackgroundContentRect(HTHEME hTheme, HDC hDC,
														 int iPart, int iState,
														 const RECT* pRect,
														 RECT* pContentRect)
	{
		return (*m_pfnGetThemeBackgroundContentRect)(hTheme, hDC, iPart, iState,
													 pRect, pContentRect);
	}

	HTHEME STDAPICALLTYPE GetWindowTheme(HWND hWnd)
	{
		return (*m_pfnGetWindowTheme)(hWnd);
	}

	HRESULT STDAPICALLTYPE SetWindowTheme(HWND hWnd,
										  LPCWSTR pszSubAppName,
										  LPCWSTR pszSubIdList)
	{
		return (*m_pfnSetWindowTheme)(hWnd, pszSubAppName, pszSubIdList);
	}

private:
	HMODULE m_hmUxTheme;

	// SetThemeAppProperties()
	typedef void (STDAPICALLTYPE *pfnSetThemeAppProperties)(DWORD);
	pfnSetThemeAppProperties m_pfnSetThemeAppProperties;

	// IsThemeActive()
	typedef BOOL (STDAPICALLTYPE *pfnIsThemeActive)();
	pfnIsThemeActive m_pfnIsThemeActive;

	// EnableThemeDialogTexture()
	typedef HRESULT (STDAPICALLTYPE *pfnEnableThemeDialogTexture)(HWND hwnd,
															DWORD dwFlags);
	pfnEnableThemeDialogTexture m_pfnEnableThemeDialogTexture;

	// OpenThemeData()
	typedef HTHEME  (STDAPICALLTYPE *pfnOpenThemeData)(HWND hWnd,
													   LPCWSTR pszwClassName);
	pfnOpenThemeData m_pfnOpenThemeData;

	// CloseThemeData()
	typedef HRESULT (STDAPICALLTYPE *pfnCloseThemeData)(HTHEME hTheme);
	pfnCloseThemeData m_pfnCloseThemeData;

	// GetThemeColor()
	typedef HRESULT (STDAPICALLTYPE *pfnGetThemeColor)(HTHEME hTheme,
													   int iPart, int iState, int iProp,
													   COLORREF* pCRef);
	pfnGetThemeColor m_pfnGetThemeColor;

	// DrawThemeBackground()
	typedef HRESULT (STDAPICALLTYPE *pfnDrawThemeBackground)(HTHEME hTheme, HDC hDC,
													   int iPart, int iState,
													   const RECT* pRect,
													   const RECT* pClipRect);
	pfnDrawThemeBackground m_pfnDrawThemeBackground;

	// DrawThemeParentBackground()
	typedef HRESULT (STDAPICALLTYPE *pfnDrawThemeParentBackground)(HWND hWnd, HDC hDC,
															 const RECT* pRect);
	pfnDrawThemeParentBackground m_pfnDrawThemeParentBackground;

	// GetThemeBackgroundContentRect()
	typedef HRESULT (STDAPICALLTYPE *pfnGetThemeBackgroundContentRect)(HTHEME hTheme, HDC hDC,
																 int iPart, int iState,
																 const RECT* pRect,
																 RECT* pContentRect);
	pfnGetThemeBackgroundContentRect m_pfnGetThemeBackgroundContentRect;

	// GetWindowTheme()
	typedef HTHEME (STDAPICALLTYPE *pfnGetWindowTheme)(HWND hWnd);
	pfnGetWindowTheme m_pfnGetWindowTheme;

	// SetWindowTheme()
	typedef HRESULT (STDAPICALLTYPE *pfnSetWindowTheme)(HWND hWnd,
														LPCWSTR pszSubAppName,
														LPCWSTR pszSubIdList);
	pfnSetWindowTheme m_pfnSetWindowTheme;
};

#endif
