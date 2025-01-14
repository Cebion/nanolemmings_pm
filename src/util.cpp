#include "util.h"

Exiter::Exiter(std::string m)
{
	message = m;
	value = 0;
}
Exiter::Exiter(std::string m,int val)
{
	message = m;
	value = val;
}

void clip(int &x, int l, int u) {
    if (x>u) x = u;
    if (x<l) x = l;
}



int ndclip(int x, int l, int u) {
    if (x>u) x = u;
    if (x<l) x = l;
    return x;
}



void rmnl(char *s) {
	int i=0;
	for (; i<200; i++) {
	    if (s[i]=='\n') s[i]=0;
	    if (s[i]==0) break;
	}
	while(i-- > 0 && s[i]==' ') s[i]=0;
}
