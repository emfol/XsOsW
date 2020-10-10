#include "XsOsWView.h"
#include <stdlib.h>
#include <math.h>

#define N_SYMBOL_AREAS 9
#define SZ_SYMBOL_AREAS 8.0f

/*
 * Type Definitions and Data Structures
 */

typedef void (*XsOsWViewSymbolPainter)(HDC hdc, int x, int y, int w, int h);

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
	struct point lastClick;
	struct rect area;
	struct rect symbolAreas[N_SYMBOL_AREAS];
};

/*
 * Static Methods
 */

static struct point *getScaledPoint(const struct fpoint* fp, const struct fscale* fs, struct point* p, float viewport) {
	p->x = (int)roundf((fp->x / viewport) * fs->sx + fs->dx);
	p->y = (int)roundf((fp->y / viewport) * fs->sy + fs->dy);
	return p;
}

static void XsOsWViewUpdateSymbolAreas(XsOsWView view) {
	const float points[] = { 0.0f, 12.0f, 24.0f };
	struct fscale fscale;
	struct fpoint fpoint;
	struct rect *area;
	int i, j;
	area = &view->area;
	fscale.dx = (float)area->tl.x;
	fscale.dy = (float)area->tl.y;
	fscale.sx = (float)area->br.x - area->tl.x;
	fscale.sy = (float)area->br.y - area->tl.y;
	area = view->symbolAreas;
	for (i = 0; i < sizeof points / sizeof(float); ++i) {
		for (j = 0; j < sizeof points / sizeof(float); ++j) {
			fpoint.x = points[j];
			fpoint.y = points[i];
			getScaledPoint(&fpoint, &fscale, &area->tl, 32.0f);
			fpoint.x += SZ_SYMBOL_AREAS;
			fpoint.y += SZ_SYMBOL_AREAS;
			getScaledPoint(&fpoint, &fscale, &area->br, 32.0f);
			++area;
		}
	}
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

static void XsOsWViewFillRect(HDC hdc, struct rect *area) {
	BeginPath(hdc);
	MoveToEx(hdc, area->tl.x, area->tl.y, NULL);
	LineTo(hdc, area->br.x, area->tl.y);
	LineTo(hdc, area->br.x, area->br.y);
	LineTo(hdc, area->tl.x, area->br.y);
	CloseFigure(hdc);
	EndPath(hdc);
	FillPath(hdc);
}

static void XsOsWViewClear(XsOsWView view, HDC hdc) {
	HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(WHITE_BRUSH));
	XsOsWViewFillRect(hdc, &view->area);
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

static void XsOsWViewPaintSymbol(XsOsWView view, HDC hdc, int index, char symbol) {
	if (index >= 0 && index < N_SYMBOL_AREAS) {
		XsOsWViewSymbolPainter painter = NULL;
		if (symbol == 'x' || symbol == 'X')
			painter = XsOsWViewPaintX;
		else if (symbol == 'o' || symbol == 'O')
			painter = NULL;
		if (painter != NULL) {
			struct rect *area = view->symbolAreas + index;
			(*painter)(hdc, area->tl.x, area->tl.y, area->br.x - area->tl.x, area->br.y - area->tl.y);
		}
	}
}

static void XsOsWViewPaintSymbols(XsOsWView view, HDC hdc) {
	int i;
	HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(GRAY_BRUSH));
	for (i = 0; i < N_SYMBOL_AREAS; ++i)
		XsOsWViewFillRect(hdc, view->symbolAreas + i);
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
		for (i = 0; i < N_SYMBOL_AREAS; ++i) {
			struct rect *area = view->symbolAreas + i;
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
	// Draw X
	oldBrush = SelectObject(hdc, GetStockObject(BLACK_BRUSH));
	XsOsWViewPaintGrid(view, hdc);
	XsOsWViewPaintSymbols(view, hdc);
	XsOsWViewPaintSymbol(view, hdc, 8, 'x');
	SelectObject(hdc, oldBrush);
}

void XsOsWViewUpdateArea(XsOsWView view, HDC hdc, LPRECT area) {
	view->area.tl.x = (int)area->left;
	view->area.tl.y = (int)area->top;
	view->area.br.x = (int)area->right;
	view->area.br.y = (int)area->bottom;
	XsOsWViewUpdateSymbolAreas(view);
	XsOsWViewPaint(view, hdc);
}

int XsOsWViewHandleMouseDown(XsOsWView view, HDC hdc, int x, int y) {
	view->lastClick.x = x;
	view->lastClick.y = y;
	XsOsWViewPaint(view, hdc);
	return 0;
}

int XsOsWViewHandleMouseUp(XsOsWView view, HDC hdc, int x, int y) {
	return 0;
}
