#include <ctime>
#include <string>
#include <stdint.h>
#include <thread>

uint32_t utc_time_in_seconds()
{
	return std::time(0); //Returns UTC in Seconds
}

std::string get_time_str(bool in_local_time, time_t utc_seconds) {

	if (utc_seconds == 0) {
		utc_seconds = utc_time_in_seconds();
	}

	struct tm time_struct;

	// Convert epoch time to struct
	if (in_local_time) {
		localtime_r(&utc_seconds, &time_struct);
	} else {
		gmtime_r(&utc_seconds, &time_struct);
	}

	// Format local time as string
	char buffer[20];
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &time_struct);

	return std::string(buffer);
}

void sleep_thread_microseconds(time_t microseconds)
{
	auto start_time = std::chrono::steady_clock::now();
	auto sleep_duration = std::chrono::microseconds(microseconds);
	auto check_interval = std::chrono::microseconds(microseconds / 50);

	while (true) {
		auto current_time = std::chrono::steady_clock::now();

		if (std::chrono::duration_cast<std::chrono::microseconds>(current_time - start_time) >= sleep_duration) {
			return;
		}

		std::this_thread::sleep_until(current_time + check_interval);
	}
}