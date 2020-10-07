#include "XsOsWView.h"
#include <stdlib.h>
#include <math.h>

#define N_CLICK_AREAS 9

/*
 * Type Definitions and Data Structures
 */

struct fpoint {
	float x;
	float y;
};

struct fscale {
	float dx;
	float dy;
	float sx;
	float sy;
};

struct point {
	int x;
	int y;
};

struct rect {
	struct point tl;
	struct point br;
};

struct XsOsWView {
	struct rect area;
	struct rect clickAreas[N_CLICK_AREAS];
	struct point lastClick;
};

/*
 * Static Methods
 */

static struct point *getScaledPoint(const struct fpoint* fp, const struct fscale* fs, struct point* p) {
	p->x = (int)roundf(fp->x * fs->sx + fs->dx);
	p->y = (int)roundf(fp->y * fs->sy + fs->dy);
	return p;
}

static void XsOsWViewPaintX(HDC hdc, int x, int y, int w, int h) {
	struct point point;
	struct fscale scale;
	struct fpoint points[] = {
		{ 0.25f, 0.00f },
		{ 0.50f, 0.25f },
		{ 0.75f, 0.00f },
		{ 1.00f, 0.25f },
		{ 0.75f, 0.50f },
		{ 1.00f, 0.75f },
		{ 0.75f, 1.00f },
		{ 0.50f, 0.75f },
		{ 0.25f, 1.00f },
		{ 0.00f, 0.75f },
		{ 0.25f, 0.50f },
		{ 0.00f, 0.25f }
	};
	int i, count = sizeof points / sizeof(struct point);
	scale.dx = (float)x, scale.dy = (float)y, scale.sx = (float)w, scale.sy = (float)h;
	BeginPath(hdc);
	getScaledPoint(points, &scale, &point);
	MoveToEx(hdc, point.x, point.y, NULL);
	for (i = 1; i < count; ++i) {
		getScaledPoint(points + i, &scale, &point);
		LineTo(hdc, point.x, point.y);
	}
	CloseFigure(hdc);
	EndPath(hdc);
	FillPath(hdc);
}

static void XsOsWViewClear(XsOsWView view, HDC hdc) {
	HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(WHITE_BRUSH));
	BeginPath(hdc);
	MoveToEx(hdc, view->area.tl.x, view->area.tl.y, NULL);
	LineTo(hdc, view->area.br.x + 1, view->area.tl.y);
	LineTo(hdc, view->area.br.x + 1, view->area.br.y + 1);
	LineTo(hdc, view->area.tl.x, view->area.br.y + 1);
	CloseFigure(hdc);
	EndPath(hdc);
	FillPath(hdc);
	SelectObject(hdc, oldBrush);
}

/*
 * API
 */

XsOsWView XsOsWViewCreate(void) {
	XsOsWView view = (XsOsWView)malloc(sizeof(struct XsOsWView));
	if (view != NULL) {
		int i;
		view->area.tl.x = 0;
		view->area.tl.y = 0;
		view->area.br.x = 0;
		view->area.br.y = 0;
		for (i = 0; i < N_CLICK_AREAS; ++i) {
			struct rect *area = view->clickAreas + i;
			area->tl.x = 0;
			area->tl.y = 0;
			area->br.x = 0;
			area->br.y = 0;
		}
		view->lastClick.x = 0;
		view->lastClick.y = 0;
	}
	return view;
}

void XsOsWViewDestroy(XsOsWView view) {
	if (view != NULL)
		free(view);
}

void XsOsWViewPaint(XsOsWView view, HDC hdc) {
	HGDIOBJ oldBrush;
	// Clear View Area
	XsOsWViewClear(view, hdc);
	// TextOutA(hdc, 10, 10, "XsOs", 4);
	// Draw X
	oldBrush = SelectObject(hdc, GetStockObject(BLACK_BRUSH));
	XsOsWViewPaintX(hdc, view->lastClick.x, view->lastClick.y, 200, 200);
	SelectObject(hdc, oldBrush);
}

void XsOsWViewUpdateArea(XsOsWView view, HDC hdc, LPRECT area) {
	view->area.tl.x = (int)area->left;
	view->area.tl.y = (int)area->top;
	view->area.br.x = (int)area->right;
	view->area.br.y = (int)area->bottom;
	XsOsWViewPaint(view, hdc);
}

int XsOsWViewHandleClick(XsOsWView view, HDC hdc, int x, int y) {
	view->lastClick.x = x;
	view->lastClick.y = y;
	XsOsWViewPaint(view, hdc);
	return 0;
}
