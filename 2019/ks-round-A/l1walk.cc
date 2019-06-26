#include <iostream>
#include <cstdlib>
#include <array>

using namespace std;

typedef unsigned u_t;

class L1Segment
{
 public:
     L1Segment(int vxb=0, int vyb=0, int vxe=0, int vye=0) :
         xb(vxb),
         yb(vyb),
         xe(vxe),
         ye(vye),
	 xstep(vxb < vxe ? 1 : -1),
	 ystep(vyb < vye ? 1 : -1)
     {}
     bool empty() const { return xb == xe; }
     int xb, yb, xe, ye;
     int xstep, ystep;
};

typedef array<L1Segment, 4> QL1Segment_t;

void get_quad_l1segments(QL1Segment_t &ql1seg, int r, int c, int x, int y, int d)
{
    int cm1 = c - 1;
    int rm1 = r - 1;
    for (int q = 0; q < 4; ++q)
    {
        int xb = 0, xe = 0, yb = 0, ye = 0;
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
		if (xe > c)
		{
		    ye -= (c - xe);
		    xe = c;
		}
            }
            break;
        }
	ql1seg[q] = L1Segment(xb, yb, xe, ye);
    }
}

void l1_walk(int r, int c, int x, int y, int d)
{
    QL1Segment_t qseg;
    get_quad_l1segments(qseg, r, c, x, y, d);
    for (int q = 0; q < 4; ++q)
    {
        const L1Segment &seg = qseg[q];
	cout << "Q[" << q << "}:" << (seg.empty() ? " Empty" : "") << "\n";
	for (int wx = seg.xb, wy = seg.yb; wx != seg.xe; wx += seg.xstep, wy += seg.ystep)
	{
	    cout << "  (" << wx << ", " << wy << ")\n";
	}
    }
}

void l1_walk_old(int r, int c, int x, int y, int d)
{
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
		if (xe > c)
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
