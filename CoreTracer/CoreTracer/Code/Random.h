#pragma once

class DRandom
{
public:
	DRandom() :u(0), v(0) {}

	DRandom(int seed)
	{
		u = seed & 0xffff;
		v = (seed>>16) & 0xffff;
	}

	float GetNextFloat() const
	{
		return float(GetRandom()&0xFFFF) / float(0xFFFF);
	}

	int GetRandom() const
	{
		v = 36969*(v & 65535) + (v >> 16);
		u = 18000*(u & 65535) + (u >> 16);
		return (v << 16) + u;
	}

	void SetSeed(int seed) const
	{
		u = seed & 0xffff;
		v = (seed>>16) & 0xffff;
	}

private:
	mutable int u, v;
};
