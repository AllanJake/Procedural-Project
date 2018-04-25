#include "PerlinNoise.h"

PerlinNoise::PerlinNoise()
{
}

PerlinNoise::PerlinNoise(unsigned int seed)
{
	p.resize(256);

	// Fill the vector with random values between 0, 255
	std::iota(p.begin(), p.end(), 0);
	// Create a random engine with the seed
	std::default_random_engine engine(seed);
	// Shuffle using the random engine above
	std::shuffle(p.begin(), p.end(), engine);
	// Duplicate the permutation vector
	p.insert(p.end(), p.begin(), p.end());
}

PerlinNoise::~PerlinNoise() {}

double PerlinNoise::noise(double x, double y, double z)
{
	// find unit cube that contains point
	int X = (int)floor(x) & 255;
	int Y = (int)floor(y) & 255;
	int Z = (int)floor(z) & 255;

	// Find relative x, y, z of point
	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	// Compute fade curves for x, y, z
	double u = fade(x);
	double v = fade(y);
	double w = fade(z);

	// Hash coords of the 8 cubes corners
	int A = p[X] + Y;
	int AA = p[A] + Z;
	int AB = p[A + 1] + Z;
	int B = p[X + 1] + Y;
	int BA = p[B] + Z;
	int BB = p[B + 1] + Z;

	// Add blended results from 8 corners of cube
	return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z),
		grad(p[BA], x - 1, y, z)),
		lerp(u, grad(p[AB], x, y - 1, z),
			grad(p[BB], x - 1, y - 1, z))),
		lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1),
			grad(p[BA + 1], x - 1, y, z - 1)),
			lerp(u, grad(p[AB + 1], x, y - 1, z - 1),
				grad(p[BB + 1], x - 1, y - 1, z - 1))));
}

double PerlinNoise::fade(double t)
{
	return t*t*t*(t*(t * 6 - 15) + 10);
}

double PerlinNoise::lerp(double t, double a, double b)
{
	return a + t * (b - a);
}

double PerlinNoise::grad(int hash, double x, double y, double z)
{
	int h = hash & 15;		// Convert Lo 4 bits of hash code into 12 gradient directions
	double u = h < 8 ? x : y;
	double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}