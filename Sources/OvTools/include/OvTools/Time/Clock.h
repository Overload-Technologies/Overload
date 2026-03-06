/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>
#include <chrono>


namespace OvTools::Time
{
	/*
	* Handle time calculations
	*/
	class Clock
	{
	public:
		/**
		* Update the clock
		*/
		void Update();

		/**
		* Return the actual framerate (Depend on the delta time)
		*/
		float GetFramerate();

		/**
		* Return the actual delta time (Scale applied)
		*/
		float GetDeltaTime();

		/**
		* Return the actual delta time (Scale not applied)
		*/
		float GetDeltaTimeUnscaled();

		/**
		* Return the time elapsed since clock creation
		*/
		float GetTimeSinceStart();

		/**
		* Return the current timescale
		*/
		float GetTimeScale();

		/**
		* Multiply the timescale by the given coefficient
		* @param p_coeff
		*/
		void Scale(float p_coeff);

		/**
		* Define a timescale
		* @param p_timescale
		*/
		void SetTimeScale(float p_timeScale);

	private:
		void Initialize();

		std::chrono::steady_clock::time_point	__START_TIME;
		std::chrono::steady_clock::time_point	__LAST_TIME;
		std::chrono::steady_clock::time_point	__CURRENT_TIME;
		std::chrono::duration<double>			__ELAPSED;

		bool	__INITIALIZED = false;
		float	__TIME_SCALE = 1.0f;
		float	__DELTA_TIME = 0.0f;
		float	__TIME_SINCE_START = 0.0f;
	};
}