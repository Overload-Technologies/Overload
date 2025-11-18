/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <random>


namespace OvTools::Utils
{
	/*
	* Handle random numbers generation
	*/
	class Random
	{
	public:

		/**
		* Disabled constructor
		*/
		Random() = delete;

		/**
		* Generate a random between two given integers (Closed interval)
		* @param p_min
		* @param p_max
		*/
		static int Generate(int p_min, int p_max);

		/**
		* Generate a random between two given floats (Closed interval)
		* @param p_min
		* @param p_max
		*/
		static float Generate(float p_min, float p_max);

		/**
		* Verify if the percentage is satisfied
		* @param p_percentage (must be between 0 and 1)
		*/
		static bool CheckPercentage(float p_percentage);

	private:
		static std::default_random_engine __GENERATOR;
	};
}