/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvTools/Utils/Random.h"

std::default_random_engine OvTools::Utils::Random::__GENERATOR;

int OvTools::Utils::Random::Generate(int p_min, int p_max)
{
	std::uniform_int_distribution<int> distribution(p_min, p_max);
	return distribution(__GENERATOR);
}

float OvTools::Utils::Random::Generate(float p_min, float p_max)
{
	std::uniform_real_distribution<float> distribution(p_min, p_max);
	return distribution(__GENERATOR);
}

bool OvTools::Utils::Random::CheckPercentage(float p_percentage)
{
	std::uniform_real_distribution<float> distribution(0.0f, 100.0f);
	return distribution(__GENERATOR) <= p_percentage;
}
