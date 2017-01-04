#include <sys/timerfd.h>

/*
struct timespec {
                time_t tv_sec;                // Seconds 
                long   tv_nsec;               // Nanoseconds 
               };
struct itimerspec {
   struct timespec it_interval;  // Interval for periodic timer 
   struct timespec it_value;     // Initial expiration 
};

int timerfd_create(int clockid, int flags);
int timerfd_settime(int fd, int flags, const struct itimerspec *new_value, struct itimerspec *old_value);
int timerfd_gettime(int fd, struct itimerspec *curr_value);
*/
int main(int argc, char const *argv[])
{

    struct timespec new_value = {
                            .tv_sec = 1,
                            .tv_nsec = 0,
                         };
    
    struct timespec old_value = {
                            .tv_sec = 0,
                            .tv_nsec = 0,
                         };



    int fd = timerfd_create(CLOCK_MONOTONIC, 0);

    return 0;
}