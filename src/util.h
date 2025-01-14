#ifndef UTIL_H
#define UTIL_H
#include <string>

#define FPS 60
#define MENU_SCREENWIDTH 800
#define MENU_SCREENHEIGHT 480

#define MSCREENWIDTH 200
#define MSCREENHEIGHT 120
#define MBARHEIGHT 20
#define DEFAULTSCALE 4
#define MAXSCALE 4

const int b_x[13] = {1, 20, 39, 58, 77, 96, 115, 126, 137, 117, 137, 152, 152};
const int b_y[13] = {8, 8,   8,  8,  8,  8,  10,  10,  10,   0,   0,   0,  14};
const int b_w[13] = {18, 18,18, 18, 18, 18,  10,  10,  10,   8,   8,   9,   9};
const int b_h[13] = {12, 12,12, 12, 12, 12,  10,  10,  10,   8,   8,   6,   6};


//! exception class
class Exiter
{
public:
	std::string message;
	int value;
	Exiter(std::string m);
	Exiter(std::string m, int val);
};

// clip number to range
void clip(int &x, int l, int u);
int ndclip(int x, int l, int u);

// remove trailing newline and spaces
void rmnl(char *s);

#endif

