#include <ctime>

uint32_t utc_time_in_seconds(); //Returns UTC in Seconds
std::string get_time_str(bool in_local_time, time_t utc_seconds = 0);
void sleep_thread_microseconds(time_t microseconds);
