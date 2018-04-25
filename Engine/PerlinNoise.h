#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#include <vector>
#include <numeric>
#include <random>
#include <algorithm>
#include <math.h>

#pragma once
class PerlinNoise
{
public:
	PerlinNoise();
	PerlinNoise(unsigned int seed);
	~PerlinNoise();
	double noise(double x, double y, double z);
	
private:
	double fade(double t);
	double lerp(double t, double a, double b);
	double grad(int hash, double x, double y, double z);
	std::vector<int> p;
};



#endif // !PERLINNOISE_H