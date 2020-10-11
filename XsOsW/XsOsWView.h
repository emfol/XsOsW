#ifndef _XSOSWVIEW_H
#define _XSOSWVIEW_H

#include <Windows.h>

typedef struct XsOsWView *XsOsWView;

XsOsWView XsOsWViewCreate(void);
void XsOsWViewDestroy(XsOsWView view);
void XsOsWViewPaint(XsOsWView view, HDC hdc);
void XsOsWViewUpdateArea(XsOsWView view, HDC hdc, LPRECT area, BOOL repaint);
int XsOsWViewUpdateSymbols(XsOsWView view, HDC hdc, char *buffer, int size, BOOL repaint);
int XsOsWViewHandleMouseDown(XsOsWView view, HDC hdc, int x, int y, BOOL repaint);
int XsOsWViewHandleMouseUp(XsOsWView view, HDC hdc, int x, int y, BOOL repaint);

#endif
