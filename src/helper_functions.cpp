#include <ctime>
#include <string>
#include <stdint.h>
#include <thread>

constexpr time_t MICROSECONDS_IN_A_SECOND = 1000000;

time_t get_microseconds_in_second()
{ 
	return MICROSECONDS_IN_A_SECOND;
}

uint32_t utc_time_in_seconds()
{
	return std::time(0); //Returns UTC in Seconds
}

std::string get_time_str(bool in_local_time, time_t utc_seconds) {

	if (utc_seconds == 0) {
		utc_seconds = utc_time_in_seconds();
	}

	struct tm time_struct;

#ifdef _WIN32
	// Convert epoch time to struct
	if (in_local_time) {
		localtime_s(&time_struct, &utc_seconds);
	}
	else {
		gmtime_s(&time_struct, &utc_seconds);
	}
#else
	// Convert epoch time to struct
	if (in_local_time) {
		localtime_r(&utc_seconds, &time_struct);
	}
	else {
		gmtime_r(&utc_seconds, &time_struct);
	}
#endif

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
		if (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start_time) >= sleep_duration) {
			return;
		}

		std::this_thread::sleep_for(check_interval);
	}
}