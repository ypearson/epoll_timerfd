#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <time.h>

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

// void print_current_time_with_ms (void)
// {
//     long            ms; // Milliseconds
//     time_t          s;  // Seconds
//     struct timespec spec;

//     clock_gettime(CLOCK_REALTIME, &spec);

//     s  = spec.tv_sec;
//     ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds

//     printf("Current time: %"PRIdMAX".%03ld seconds since the Epoch\n",
//            (intmax_t)s, ms);
// }

double get_current_time_with_ms (void)
{
    double ret;
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    ret = ((double)spec.tv_nsec) / 1.0e9;
    ret += (double)spec.tv_sec;
    return ret;
}

int main(int argc, char const *argv[])
{

    char v[8];

    struct timespec it_interval = {
                            .tv_sec = 1,
                            .tv_nsec = 900000000,
                            };

    struct timespec it_value = {
                            .tv_sec = 1,
                            .tv_nsec = 900000000,
                            };

    const struct itimerspec new_value = {
                                    .it_interval = it_interval,
                                    .it_value = it_value,
                                  };

    int timerfd = timerfd_create(CLOCK_MONOTONIC, 0);
    int ret = timerfd_settime(timerfd, 0, & new_value, 0);
    printf("ret = %d\n", ret);
    double t;

    int efd = epoll_create(1);
    struct epoll_event ev, events[1];
    ev.events = EPOLLIN;
    ev.data.fd = timerfd;

    epoll_ctl(efd, EPOLL_CTL_ADD, timerfd, &ev);
    int i = 0;
    while(1)
    {
        t = get_current_time_with_ms();
        epoll_wait(efd, events, 1, -1);
        printf("Current time: %f  dt\n", get_current_time_with_ms()-t);
        read(timerfd, v, 8);

        if(events[0].data.fd == timerfd)
        {
            printf("%d ", i++);
            fflush(stdout);
        }
        else
        {
            printf("%s\n", "X");
        }
    }
    return 0;
}