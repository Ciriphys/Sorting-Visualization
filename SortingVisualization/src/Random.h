#pragma once

#include <random>
#include <memory>

/* Singleton utility class to generate random numbers with a given range. */
class Random
{
public:
	static std::shared_ptr<Random> Get()
	{
		if(!s_Instance)	s_Instance = std::shared_ptr<Random>(new Random());
		return s_Instance;
	}

	int GetInt(int min, int max)
	{
		std::uniform_int_distribution<int> distribution(min, max);
		return distribution(m_Generator);
	}

	float GetFloat(float min, float max)
	{
		std::uniform_real_distribution<float> distribution(min, max);
		return distribution(m_Generator);
	}

	double GetDouble(double min, double max)
	{
		std::uniform_real_distribution<double> distribution(min, max);
		return distribution(m_Generator);
	}

	Random() : m_Generator(m_Device()) {}
	~Random() {}

private:

	std::mt19937 m_Generator;
	std::random_device m_Device;

	static std::shared_ptr<Random> s_Instance;

};