#ifndef _XSOSWVIEW_H
#define _XSOSWVIEW_H

#include <Windows.h>

typedef struct XsOsWView *XsOsWView;

XsOsWView XsOsWViewCreate(void);
void XsOsWViewDestroy(XsOsWView view);
void XsOsWViewPaint(XsOsWView view, HDC hdc);
void XsOsWViewUpdateArea(XsOsWView view, HDC hdc, LPRECT area);
int XsOsWViewHandleMouseDown(XsOsWView view, HDC hdc, int x, int y);
int XsOsWViewHandleMouseUp(XsOsWView view, HDC hdc, int x, int y, BOOL repaint);

#endif
