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

static struct point *getScaledPoint(const struct fpoint* fp, const struct fscale* fs, struct point* p, float viewport) {
	p->x = (int)roundf((fp->x / viewport) * fs->sx + fs->dx);
	p->y = (int)roundf((fp->y / viewport) * fs->sy + fs->dy);
	return p;
}

static void XsOsWViewPaintX(HDC hdc, int x, int y, int w, int h) {
	int i;
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
	scale.dx = (float)x, scale.dy = (float)y, scale.sx = (float)w, scale.sy = (float)h;
	BeginPath(hdc);
	getScaledPoint(points, &scale, &point, 1.0f);
	MoveToEx(hdc, point.x, point.y, NULL);
	for (i = 1; i < sizeof points / sizeof(struct point); ++i) {
		getScaledPoint(points + i, &scale, &point, 1.0f);
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

static void XsOsWViewPaintGrid(XsOsWView view, HDC hdc) {
	int i;
	struct point point;
	struct fscale scale;
	struct fpoint center[] = {
		{ 11.0f, 11.0f },
		{ 21.0f, 11.0f },
		{ 21.0f, 21.0f },
		{ 11.0f, 21.0f },
	};
	struct fpoint grid[] = {
		{ 0.0f, 9.0f },   // 01
		{ 9.0f, 9.0f },   // 02
		{ 9.0f, 0.0f },   // 03
		{ 11.0f, 0.0f },  // 04
		{ 11.0f, 9.0f },  // 05
		{ 21.0f, 9.0f },  // 06
		{ 21.0f, 0.0f },  // 07
		{ 23.0f, 0.0f },  // 08
		{ 23.0f, 9.0f },  // 09
		{ 32.0f, 9.0f },  // 10
		{ 32.0f, 11.0f }, // 11
		{ 23.0f, 11.0f }, // 12
		{ 23.0f, 21.0f }, // 13
		{ 32.0f, 21.0f }, // 14
		{ 32.0f, 23.0f }, // 15
		{ 23.0f, 23.0f }, // 16
		{ 23.0f, 32.0f }, // 17
		{ 21.0f, 32.0f }, // 18
		{ 21.0f, 23.0f }, // 19
		{ 11.0f, 23.0f }, // 20
		{ 11.0f, 32.0f }, // 21
		{ 9.0f, 32.0f },  // 22
		{ 9.0f, 23.0f },  // 23
		{ 0.0f, 23.0f },  // 24
		{ 0.0f, 21.0f },  // 25
		{ 9.0f, 21.0f },  // 26
		{ 9.0f, 11.0f },  // 27
		{ 0.0f, 11.0f }   // 28
	};
	scale.dx = (float)view->area.tl.x;
	scale.dy = (float)view->area.tl.y;
	scale.sx = (float)abs(view->area.br.x - view->area.tl.x);
	scale.sy = (float)abs(view->area.br.y - view->area.tl.y);
	BeginPath(hdc);
	getScaledPoint(grid, &scale, &point, 32.0f);
	MoveToEx(hdc, point.x, point.y, NULL);
	for (i = 1; i < sizeof grid / sizeof(struct fpoint); ++i) {
		getScaledPoint(grid + i, &scale, &point, 32.0f);
		LineTo(hdc, point.x, point.y);
	}
	CloseFigure(hdc);
	getScaledPoint(center, &scale, &point, 32.0f);
	MoveToEx(hdc, point.x, point.y, NULL);
	for (i = 1; i < sizeof center / sizeof(struct fpoint); ++i) {
		getScaledPoint(center + i, &scale, &point, 32.0f);
		LineTo(hdc, point.x, point.y);
	}
	CloseFigure(hdc);
	EndPath(hdc);
	FillPath(hdc);
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
	XsOsWViewPaintGrid(view, hdc);
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
