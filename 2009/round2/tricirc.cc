#include <cmath>
#include <limits>
// #include <iostream>

bool tricirc(
    double &x, double &y, double &r,
    double x1, double y1, double r1,
    double x2, double y2, double r2,
    double x3, double y3, double r3)
{
    bool ok = true;
    // Eqi:  (x-xi)^2 + (y-yi)^2 = (r-ri)^2    i=1,2,3
    // Eq1 - Eq3:
    // -2(x1-x3)x -2(y1-y3)y = -2(r1-r3)r + r1^2 - r3^2 + x3^2-x1^2 + y3^2-y1^2
    // 2(x1-x3)x 2(y1-y3)y = 2(r1-r3)r + r3^2-r1^2 + x1^2-x3^2 + y1^2-y3^2

    double x1sq = x1*x1;
    double x2sq = x2*x2;
    double x3sq = x3*x3;

    double y1sq = y1*y1;
    double y2sq = y2*y2;
    double y3sq = y3*y3;

    double r1sq = r1*r1;
    double r2sq = r2*r2;
    double r3sq = r3*r3;

    double a11 = 2*(x1 - x3),  a12 = 2*(y1 - y3);
    double a21 = 2*(x2 - x3),  a22 = 2*(y2 - y3);

    double b1 = 2*(r1 - r3);
    double b2 = 2*(r2 - r3);

    double c1 = r3sq - r1sq + x1sq - x3sq + y1sq - y3sq;
    double c2 = r3sq - r2sq + x2sq - x3sq + y2sq - y3sq;
    
    double det = a11*a22 - a12*a21;
    double eps = std::numeric_limits<float>::epsilon();
    ok = ok && (fabs(det) > eps);
    double ddet = ok ? 1./det : 1.;

    double ia11 =  a22*ddet,  ia12 = -a12*ddet;
    double ia21 = -a21*ddet,  ia22 =  a11*ddet;

#if 0
    double id11 = a11*ia11 + a12*ia21,  id12 = a11*ia12 + a12*ia22;
    double id21 = a21*ia11 + a22*ia21,  id22 = a21*ia12 + a22*ia22;
    std::cerr << "Id:\n";
    std::cerr << "  " << id11 << "  " << id12 << "\n";
    std::cerr << "  " << id21 << "  " << id22 << "\n";
#endif
    // A(x y) = Br + C
    // (x y) = IA(Br + C)
    // x = g1 r + h1,   y = g2 r + h2
    double g1 = ia11*b1 + ia12*b2;
    double g2 = ia21*b1 + ia22*b2;
    double h1 = ia11*c1 + ia12*c2;
    double h2 = ia21*c2 + ia22*c2;

    // Eq3:  (x-x3)^2 + (y-y3)^2 = (r-r3)^2 
    // (g1r + h1 - x3)^2 + (g2r + h2 - y3)^2 = (r-r3)^2 
    double h1_x3 = h1 - x3;
    double h2_y3 = h2 - y3;
    double a = g1*g1 + g2*g2 - 1.;
    double b = 2*(g1*h1_x3 + g2*h2_y3 + r3);
    double c = h1_x3*h1_x3 + h2_y3*h2_y3 - r3sq;
    double disc = b*b - 4*a*c;
    ok = ok && disc >= 0.;
    double sqrt_disc = ok ? sqrt(disc) : 0.;
    if (a < 0) { sqrt_disc = -sqrt_disc; }

    r = (-b + sqrt_disc)/(2*a);
    x = g1*r + h1;    
    y = g2*r + h2;
    return ok;
}

#if defined(TEST)

#include <iostream>
#include <cstdlib>

int main(int argc, char **argv)
{
    int rc = (argc == 1 + 3*3 ? 0 : 1);
    if (rc == 0)
    {
        double x[3], y[3], r[3];
        for (int ai = 1, ci = 0; ai < argc; ai += 3, ++ci)
        {
            x[ci] = strtod(argv[ai + 0], 0);
            y[ci] = strtod(argv[ai + 1], 0);
            r[ci] = strtod(argv[ai + 2], 0);
        }
        double apx, apy, apr;
        bool ok = tricirc(apx, apy, apr,
            x[0], y[0], r[0],
            x[1], y[1], r[1],
            x[2], y[2], r[2]);
        if (ok) 
        {
            std::cout << apx << " " << apy << " " << apr << "\n";
        }
        else
        { 
            rc = 1; 
        }
    }
    return rc;
}


#endif
