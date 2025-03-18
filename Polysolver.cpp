#include "Math.h"
#include "Polysolver.h"
#include <algorithm>

const int MAX_N = 10;

void diff(ld* w, ld* v, int n) // Differentiates a polynomial
{
    for(int i = 0, deg = n-1; i < n-1; i++)
        w[i] = v[i]*deg--;
}

ld eval(ld* v, ld x, int n) // Evaluates a polynomial with Horner's rule
{
    ld sum = 0;
    for(int i = 0; i < n; i++)
        sum = sum*x + v[i];
    return sum;
}

ld bisect(ld x1, ld x2, ld* v, int n) // Finds a root of a poly between x1 and x2
{
    auto a = eval(v, x1, n), b = eval(v, x2, n);

    while(x2-x1 > 1e-9)
    {
        ld mid = (x1+x2)/2;
        auto r = eval(v, mid, n);
        if((b > a) ? (r <= 0) : (r >= 0))
            x1 = mid;
        else
            x2 = mid;
    }
    return std::abs(eval(v, x1, n)) < 1e-4 ? x1 : inf; // Return the root if a root was found
}

int findPolynomialRoots_(ld* roots, ld* v, int N)
{
    ld A[MAX_N];

    if(N < 2)
        return 0;
    if(N == 2)
        return roots[0] = -v[1], 1;
    if(N == 3)
    {
        auto a = v[0], b = v[1], c = v[2];
        auto r0 = -b;
        auto d = b*b - 4*a*c;
        if(d < 0)
            return 0;
        auto r1 = (r0 - std::sqrt(d))/(2.0*a);
        auto r2 = (r0 + std::sqrt(d))/(2.0*a);
        roots[0] = r1;
        roots[1] = r2;
        return 2;
    }

    // Find roots of the derived function and add a couple fake roots at both extremes
    ld df[MAX_N], dd[MAX_N];
    diff(df, v, N);
    auto I = findPolynomialRoots_(dd, df, N-1);
    A[0] = -1e15;
    for(int i = 1; i <= I; i++)
        A[i] = dd[i-1];
    A[I+1] = 1e15;

    // Check for roots between the roots of the derivative
    int r = 0;
    for(int i = 0; i <= I; i++)
        if(auto x = bisect(A[i], A[i+1], v, N); x < inf)
            roots[r++] = x;

    std::sort(roots, roots+r);
    return r;
}

std::vector<ld> findPolynomialRoots(std::vector<ld> v)
{
    ld w[MAX_N], o[MAX_N];
    for(int i = 0; i < v.size(); i++)
        w[i] = v[i];

    auto n = findPolynomialRoots_(o, w, int(v.size()));
    std::vector<ld> ret;
    for(int i = 0; i < n; i++)
        ret.push_back(o[i]);
    return ret;
}
