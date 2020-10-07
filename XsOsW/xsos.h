#ifndef _XSOS_H
#define _XSOS_H

#define XSOS_STRING_SIZE 9
#define XSOS_GRID_STRING_SIZE 18

unsigned int xsos_count_selected(unsigned int state);
unsigned int xsos_start(unsigned int symbol);
unsigned int xsos_select(unsigned int state, unsigned int position);
unsigned int xsos_winner(unsigned int state);
unsigned int xsos_done(unsigned int state);
char *xsos_string(unsigned int state, char *buffer);
char *xsos_grid_string(unsigned int state, char *buffer);

#endif
