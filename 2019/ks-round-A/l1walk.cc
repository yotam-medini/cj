#include <iostream>
#include <cstdlib>

using namespace std;

typedef unsigned u_t;

void l1_walk(int r, int c, int x, int y, int d)
{
    // static const int steps[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    int cm1 = c - 1;
    int rm1 = r - 1;
    for (int q = 0; q < 4; ++q)
    {
        int xb, xe, yb, ye;
	bool has = false;
	switch (q)
	{
	 case 0:
	    has = ((cm1 -x) + (rm1 - y) <= d);
	    if (has)
	    {
	        xb = x + d; yb = y; xe = x; ye = y + d;
		if (xb > cm1)
		{
		    yb += (xb - cm1);
		}
	    }
	    break;
	 case 1:
	    xb = x; yb = y + d; xe = x - d; ye = y;
	    break;
	 case 2:
	    xb = x - d; yb = y; xe = x; ye = y - d;
	    break;
	 case 3:
	    xb = x; yb = y - d; xe = x + d; ye = y;
	    break;
	}
	cout << "q="<<q << " [(" << xb << ", " << yb <<
	    ") , (" << xe << ", " << ye << ") )\n";
    }
}

int main(int argc, char **argv)
{
    int ai = 0;
    u_t r = strtoul(argv[++ai], 0, 0);
    u_t c = strtoul(argv[++ai], 0, 0);
    u_t x = strtoul(argv[++ai], 0, 0);
    u_t y = strtoul(argv[++ai], 0, 0);
    u_t d = strtoul(argv[++ai], 0, 0);
    l1_walk(r, c, x, y, d);
    return 0;
}
