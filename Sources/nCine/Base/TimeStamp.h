#pragma once

#include "Clock.h"

namespace nCine
{
	/// Represents a point in time or a duration
	class TimeStamp
	{
	public:
		TimeStamp();

		/// Returns a new time stamp initialized to the current clock value
		inline static TimeStamp now() {
			return TimeStamp(clock().now());
		}

		bool operator>(const TimeStamp& other) const;
		bool operator<(const TimeStamp& other) const;
		TimeStamp& operator+=(const TimeStamp& other);
		TimeStamp& operator-=(const TimeStamp& other);
		TimeStamp operator+(const TimeStamp& other) const;
		TimeStamp operator-(const TimeStamp& other) const;

		/// Returns a new time stamp with the time elapsed since this one
		TimeStamp timeSince() const;

		/// Returns the time elapsed since the timestamp, as seconds in a `float` number
		float secondsSince() const;

		/// Returns the time elapsed since the timestamp, as milliseconds in a `float` number
		float millisecondsSince() const;

		/// Returns the time elapsed since the timestamp, as microseconds in a `float` number
		float microsecondsSince() const;

		/// Returns the time elapsed since the timestamp, as seconds in a `nanoseconds` number
		float nanosecondsSince() const;

		// Returns the number of elapsed ticks
		inline std::uint64_t ticks() const {
			return _counter;
		}

		/// Returns the timestamp counter value as seconds in a `float` number
		float seconds() const;

		/// Returns the timestamp counter value as milliseconds in a `float` number
		float milliseconds() const;

		/// Returns the timestamp counter value as microseconds in a `float` number
		float microseconds() const;

		/// Returns the timestamp counter value as nanoseconds in a `float` number
		float nanoseconds() const;

	private:
		std::uint64_t _counter;

		explicit TimeStamp(std::uint64_t counter);
	};
}