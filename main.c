#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <stdio.h>

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

    struct timespec it_interval = {
                            .tv_sec = 1,
                            .tv_nsec = 0,
                            };

    struct timespec it_value = {
                            .tv_sec = 1,
                            .tv_nsec = 0,
                            };

    const struct itimerspec new_value = {
                                    .it_interval = it_interval,
                                    .it_value = it_value,
                                  };

    struct itimerspec old_value = {
                                    .it_interval = it_interval,
                                    .it_value = it_value,
                                  };

    int timerfd = timerfd_create(CLOCK_MONOTONIC, 0);
    int ret = timerfd_settime(timerfd, 0, & new_value, 0);
    printf("ret = %d\n", ret);
    char v[8];
    read(timerfd, v, 8);


    // for(;;)
    // {
    //     read(timerfd, v, 8);
    //     printf("%s\n","timeout\n" );
    // }



    int efd = epoll_create(1);
    struct epoll_event ev, events[1];
    ev.events = EPOLLIN;
    ev.data.fd = timerfd;
    epoll_ctl(efd, EPOLL_CTL_ADD, timerfd, &ev);

    while(1)
    {

        epoll_wait(efd, events, 1, -1);
        read(timerfd, v, 8);

        printf("%s\n", "some event...\n");

        if(events[0].data.fd == timerfd)
        {
            printf("%s\n", ".");
        }

    }










    return 0;
}