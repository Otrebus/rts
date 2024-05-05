#include <vector>
#include <algorithm>
#include "Math.h"
#include "Polysolver.h"
#include "Quartic.h"
#include <iostream>


std::vector<ld> diff(std::vector<ld> v) { // Differentiates a polynomial
    std::vector<ld> w;
    for(int i = 0, deg = v.size()-1; i < v.size()-1; i++)
        w.push_back(v[i]*deg--);
    return w;   
}

ld eval(std::vector<ld> v, ld x) // Evaluates a polynomial with Horner's rule
{ 
    ld sum = 0;
    for(int i = 0; i < v.size(); i++)
        sum = sum*x + v[i];
    return sum;
}

ld bisect(ld x1, ld x2, std::vector<ld> v) // Finds a root of a poly between x1 and x2
{ 
    auto a = eval(v, x1), b = eval(v, x2);

    while(x2-x1 > 1e-12) {
        ld mid = x1 + (x2-x1)/2;
        auto r = eval(v, mid);
        if((b > a) ? (r <= 0) : (r >= 0))
            x1 = mid;
        else
            x2 = mid;
    }
    return std::abs(eval(v, x1)) < 1e-4 ? x1 : inf; // Return the root if a root was found
}

std::vector<ld> findPolynomialRoots(std::vector<ld> v)
{
    if(v.size() < 2)
        return {};

    // Find roots of the derived function and add a couple fake roots at both extremes
    auto dd = findPolynomialRoots(diff(v));
    std::vector<ld> d;
    d.push_back(-1e15);
    d.insert(d.end(), dd.begin(), dd.end());
    d.push_back(1e15);

    // Check for roots between the roots of the derivative
    std::vector<ld> d1;
    for(int i = 0; i < d.size()-1; i++)
        if(auto x = bisect(d[i], d[i+1], v); x < inf)
            d1.push_back(x);

    std::sort(d1.begin(), d1.end());

    if(v.size() == 5)
    {
        for(auto d : d1)
        {
            auto val = v[0]*d*d*d*d + v[1]*d*d*d + v[2]*d*d + v[3]*d + v[4];
            if(std::abs(val) > 0.001)
                __debugbreak();
        }
    }

    if(v.size() == 5)
    {
        auto w = v;
        for(int i = 1; i < 5; i++)
            v[i] /= v[0];
        auto ret = solve_quartic(v[1], v[2], v[3], v[4]);
        std::vector<ld> ans;
        for(int i = 0; i < 4; i++)
            if(!ret[i].imag())
                ans.push_back(ld(ret[i].real()));
        std::sort(ans.begin(), ans.end());

        for(auto d : ans)
        {
            auto val = w[0]*d*d*d*d + w[1]*d*d*d + w[2]*d*d + w[3]*d + w[4];
            if(std::abs(val) > 0.001)
                __debugbreak();
        }

        return ans;
    }
    else
        return d1;
}
