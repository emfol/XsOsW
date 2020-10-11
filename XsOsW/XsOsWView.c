#include "XsOsWView.h"
#include <stdlib.h>
#include <math.h>

#define N_SYMBOLS 9
#define SZ_SYMBOL_AREA 8.0f

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
	int selected;
	struct rect area;
	struct rect symbolAreas[N_SYMBOLS];
	char symbols[N_SYMBOLS];
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
			fpoint.x += SZ_SYMBOL_AREA;
			fpoint.y += SZ_SYMBOL_AREA;
			getScaledPoint(&fpoint, &fscale, &area->br, 32.0f);
			++area;
		}
	}
}

static int XsOsWViewGetSymbolAreaIndex(XsOsWView view, int x, int y) {
	int i;
	struct rect *area, *areas;
	for (i = 0, areas = view->symbolAreas; i < N_SYMBOLS; ++i) {
		area = areas + i;
		if (x >= area->tl.x && x < area->br.x && y >= area->tl.y && y < area->br.y)
			return i;
	}
	return -1;
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
	HGDIOBJ oldObj = SelectObject(hdc, GetStockObject(WHITE_BRUSH));
	XsOsWViewFillRect(hdc, &view->area);
	DeleteObject(SelectObject(hdc, oldObj));
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
	if (index >= 0 && index < N_SYMBOLS) {
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
	HGDIOBJ oldObj;
	struct rect *areas;
	char *symbols;
	int i, selected;
	for (i = 0, selected = view->selected, areas = view->symbolAreas, symbols = view->symbols; i < N_SYMBOLS; ++i) {
		if (i == selected) {
			oldObj = SelectObject(hdc, GetStockObject(GRAY_BRUSH));
			XsOsWViewFillRect(hdc, areas + i);
			DeleteObject(SelectObject(hdc, oldObj));
		}
		XsOsWViewPaintSymbol(view, hdc, i, *(symbols + i));
	}
}

/*
 * API
 */

XsOsWView XsOsWViewCreate(void) {
	XsOsWView view = (XsOsWView)malloc(sizeof(struct XsOsWView));
	if (view != NULL) {
		struct rect *area, *areas;
		char *symbols;
		int i;
		view->selected = -1;
		view->area.tl.x = 0;
		view->area.tl.y = 0;
		view->area.br.x = 0;
		view->area.br.y = 0;
		for (i = 0, areas = view->symbolAreas, symbols = view->symbols; i < N_SYMBOLS; ++i) {
			area = areas + i;
			area->tl.x = 0;
			area->tl.y = 0;
			area->br.x = 0;
			area->br.y = 0;
			*(symbols + i) = ' ';
		}
	}
	return view;
}

void XsOsWViewDestroy(XsOsWView view) {
	if (view != NULL)
		free(view);
}

void XsOsWViewPaint(XsOsWView view, HDC hdc) {
	HGDIOBJ oldObj;
	// Clear View Area
	XsOsWViewClear(view, hdc);
	// Draw X
	oldObj = SelectObject(hdc, GetStockObject(BLACK_BRUSH));
	XsOsWViewPaintGrid(view, hdc);
	XsOsWViewPaintSymbols(view, hdc);
	// Temp
	XsOsWViewPaintSymbol(view, hdc, 8, 'x');
	DeleteObject(SelectObject(hdc, oldObj));
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
	int selected = XsOsWViewGetSymbolAreaIndex(view, x, y);
	if (selected >= 0 && selected < N_SYMBOLS && view->selected == -1) {
		view->selected = selected;
		XsOsWViewPaint(view, hdc);
		return selected;
	}
	return -1;
}

int XsOsWViewHandleMouseUp(XsOsWView view, HDC hdc, int x, int y, BOOL repaint) {
	int selected, click = -1;
	if (view->selected != -1) {
		selected = XsOsWViewGetSymbolAreaIndex(view, x, y);
		if (selected >= 0 && selected < N_SYMBOLS && selected == view->selected)
			click = selected;
		view->selected = -1;
		if (repaint)
			XsOsWViewPaint(view, hdc);
	}
	return click;
}
