#include "xsos.h"

unsigned int xsos_count_selected(unsigned int state) {
	unsigned int s = state & 0x1ff;
	unsigned int c = 0;
	while (s != 0) {
		if ((s & 1) == 1)
			++c;
		s >>= 1;
	}
	return c;
}

unsigned int xsos_start(unsigned int symbol) {
	return (((symbol & 1) << 18) & 0x7ffff);
}

unsigned int xsos_select(unsigned int state, unsigned int position) {
	const unsigned int s = state & 0x7ffff;
	const unsigned int c = xsos_count_selected(s);
	if (c < 9) {
		const unsigned int p = position % 9;
		if (((s >> p) & 1) == 0) {
			const unsigned int sym = ((s >> 18) + (c & 1)) & 1;
			return (s | (sym << (p + 9)) | (1 << p));
		}
	}
	return 0;
}

unsigned int xsos_winner(unsigned int state) {
	const unsigned int patterns[] = { 0x007, 0x038, 0x049, 0x054, 0x092, 0x111, 0x124, 0x1c0 };
	const unsigned int s = state & 0x7ffff;
	const unsigned int sels = s & 0x1ff;
	const unsigned int syms = (s >> 9) & 0x1ff;
	unsigned int i;
	for (i = 0; i < sizeof patterns / sizeof(unsigned int); ++i) {
		const unsigned int winp = patterns[i];
		if ((sels & winp) == winp) {
			const unsigned int symp = syms & winp;
			if (symp == winp || symp == 0) {
				return (winp & 0x1ff);
			}
		}
	}
	return 0;
}

unsigned int xsos_done(unsigned int state) {
	return (xsos_count_selected(state) == 9 || xsos_winner(state) != 0);
}

char *xsos_string(unsigned int state, char *buffer) {
	const unsigned int s = state & 0x7ffff;
	const unsigned int sels = state & 0x1ff;
	const unsigned int syms = (state >> 9) & 0x1ff;
	const unsigned int wins = xsos_winner(s);
	unsigned int i;
	for (i = 0; i < 9; ++i) {
		if (((sels >> i) & 1) == 1) {
			char sym = ((syms >> i) & 1) == 1 ? 'x' : 'o';
			if (((wins >> i) & 1) == 1) {
				sym = 'A' + (sym - 'a');
			}
			*(buffer + i) = sym;
		} else {
			*(buffer + i) = ' ';
		}
	}
	*(buffer + i) = '\0';
	return buffer;
}

char *xsos_grid_string(unsigned int state, char *buffer) {
	const unsigned int s = state & 0x7ffff;
	char *p = buffer;
	char string[9 + 1];
	unsigned int i;
	xsos_string(s, string);
	for (i = 0; i < 3; ++i) {
		const unsigned int n = i * 3;
		*p++ = string[n + 0];
		*p++ = '|';
		*p++ = string[n + 1];
		*p++ = '|';
		*p++ = string[n + 2];
		*p++ = '\n';
	}
	*p++ = '\0';
	return buffer;
}
