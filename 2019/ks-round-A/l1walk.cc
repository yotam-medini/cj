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
            has = ((cm1 - x) + (rm1 - y) >= d);
            if (has)
            {
                xb = x + d; yb = y; xe = x; ye = y + d;
                if (xb > cm1)
                {
                    yb += (xb - cm1);
                    xb = cm1;
                }
                if (ye > r)
                {
                    xe += (ye - r);
                    ye = r;
                }
            }
            break;
         case 1:
            has = (x + (rm1 - y) >= d);
            if (has)
            {
                xb = x; yb = y + d; xe = x - d; ye = y;
                if (yb > rm1)
                {
                    xb -= (yb - rm1);
                    yb = rm1;
                }
		if (xe < -1)
		{
		    ye -= xe + 1;
		    xe = -1;
		}
            }
            break;
         case 2:
            has = ((x + y) >= d);
            if (has)
            {
                xb = x - d; yb = y; xe = x; ye = y - d;
                if (xb < 0)
                {
                    yb += xb;
                    xb = 0;
                }
                if (ye < -1)
                {
                    xe += ye + 1;
                    ye = -1;
                }
            }
            break;
         case 3:
            has = (((cm1 - x) + y) >= d);
            if (has)
            {
                xb = x; yb = y - d; xe = x + d; ye = y;
		if (yb < 0)
		{
		    xb -= yb;
		    yb = 0;
		}
		if (xe >= c)
		{
		    ye -= (c - xe);
		    xe = c;
		}
            }
            break;
        }
        cout << "Quad="<<q;
        if (has)
        {
            cout << " [(" << xb << ", " << yb <<
                ") , (" << xe << ", " << ye << ") )\n";
	    const int xstep = (xb < xe ? 1 : -1);
	    const int ystep = (yb < ye ? 1 : -1);
	    for (int wx = xb, wy = yb; wx != xe; wx += xstep, wy += ystep)
	    {
	        cout << "  (" << wx << ", " << wy << ")\n";
	    }
        }
        else
        {
            cout << " None\n";
        }
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
