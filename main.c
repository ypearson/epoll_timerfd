#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define TIMERFD_FLAGS (0)
#define BLOCK_INDEFINITELY (-1)
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

double get_current_time_us (void)
{
    double ret;
    struct timespec spec;

    clock_gettime(CLOCK_MONOTONIC_RAW, &spec);

    ret = ((double)spec.tv_nsec) / 1.0e3;
    ret += ((double)spec.tv_sec) * 1000000;
    return ret;
}

double get_current_time_ms (void)
{
    double ret;
    struct timespec spec;

    clock_gettime(CLOCK_MONOTONIC_RAW, &spec);

    ret = ((double)spec.tv_nsec) / 1.0e6;
    ret += ((double)spec.tv_sec) * 1000;
    return ret;
}

double get_current_time_s (void)
{
    double ret;
    struct timespec spec;

    clock_gettime(CLOCK_MONOTONIC_RAW, &spec);

    ret = ((double)spec.tv_nsec) / 1.0e9;
    ret += (double)spec.tv_sec;
    return ret;
}

int main(int argc, char const *argv[])
{
    // int timerfd_create(int clockid, int flags);
    //
    // timerfd_create() creates a new timer object, and returns a file
    // descriptor that refers to that timer.

    int timer_fd = timerfd_create(CLOCK_MONOTONIC, TIMERFD_FLAGS);


    // int timerfd_settime(int fd, int flags,
    //                     const struct itimerspec *new_value,
    //                     struct itimerspec *old_value);

    // timerfd_settime() starts or stops the timer referred to by the file descriptor fd.

    // The new_value argument specifies the initial expiration and interval
    // for the timer.  The itimerspec structure used for this argument
    // contains two fields, each of which is in turn a structure of type
    // timespec:

    //    struct timespec {
    //        time_t tv_sec;                /* Seconds */
    //        long   tv_nsec;               /* Nanoseconds */
    //    };

    //    struct itimerspec {
    //        struct timespec it_interval;  /* Interval for periodic timer */
    //        struct timespec it_value;     /* Initial expiration */
    //    };

    // new_value.it_value specifies the initial expiration of the timer, in
    // seconds and nanoseconds.  Setting either field of new_value.it_value
    // to a nonzero value arms the timer.  Setting both fields of
    // new_value.it_value to zero disarms the timer.

    struct timespec it_interval = {
                            .tv_sec = 0,
                            .tv_nsec = 1000000,
                            };

    struct timespec it_value = {
                            .tv_sec = 0,
                            .tv_nsec = 1000000,
                            };

    const struct itimerspec timer_interval = {
                                            .it_interval = it_interval,
                                            .it_value    = it_value,
                                        };

    if(!timerfd_settime(timer_fd, TIMERFD_FLAGS, &timer_interval, NULL))
    {
        printf("Timer successfully created");

        double t;

       // int epoll_create(int size);

       // epoll_create() returns a file descriptor referring to the new epoll
       // instance.  This file descriptor is used for all the subsequent calls
       // to the epoll interface.  When no longer required, the file descriptor
       // returned by epoll_create() should be closed by using close(2).  When
       // all file descriptors referring to an epoll instance have been closed,
       // the kernel destroys the instance and releases the associated
       // resources for reuse.

       // In the initial epoll_create() implementation, the size argument
       // informed the kernel of the number of file descriptors that the caller
       // expected to add to the epoll instance.  The kernel used this
       // information as a hint for the amount of space to initially allocate
       // in internal data structures describing events.  (If necessary, the
       // kernel would allocate more space if the caller's usage exceeded the
       // hint given in size.)  Nowadays, this hint is no longer required (the
       // kernel dynamically sizes the required data structures without needing
       // the hint), but size must still be greater than zero, in order to
       // ensure backward compatibility when new epoll applications are run on
       // older kernels.

        int epfd = epoll_create(1);

        // typedef union epoll_data {
        //        void        *ptr;
        //        int          fd;
        //        uint32_t     u32;
        //        uint64_t     u64;
        //    } epoll_data_t;

        //    struct epoll_event {
        //        uint32_t     events;      /* Epoll events */
        //        epoll_data_t data;        /* User data variable */
        //    };

        struct epoll_event event;
        // EPOLLIN
        //        The associated file is available for read operations.
        event.events  = EPOLLIN;

        // The target file descriptor to read from
        event.data.fd = timer_fd;

        // Control interface for an epoll file descriptor
        // int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);

        // EPOLL_CTL_ADD
        //       Add fd to the interest list and associate the settings
        //       specified in event with the internal file linked to fd.
        int maxevents = 1;

        epoll_ctl(epfd, EPOLL_CTL_ADD, timer_fd, &event);

        struct epoll_event incoming_events[maxevents];

        double dt;
        double dt_max=1000;
        double dt_min=1000;
        int count_max = 0;

        for (uint32_t i = 0; i < 100000; i++)
        {
            uint64_t event_occurences;
            t = get_current_time_us();
            // epoll_wait, Wait for an I/O event on an epoll file descriptor
            // int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
            epoll_wait(epfd, incoming_events, maxevents, BLOCK_INDEFINITELY);

            read(timer_fd, &event_occurences, sizeof(event_occurences));

            dt = get_current_time_us() - t;

            if(incoming_events[0].data.fd == timer_fd)
            {
                // printf("%f, μs %\n", dt);
            }
            else
            {
                printf("%s\n", "?????????");
            }

            if(dt_max < dt)
            {
                dt_max = dt;
            }
            if(dt_min > dt)
            {
                dt_min = dt;
            }
            if(count_max < event_occurences)
            {
                count_max = event_occurences;
            }
        }

    printf("%s = %f\n","dt_max = ", dt_max );
    printf("%s = %f\n","dt_min = ", dt_min );
    printf("%s = %d\n","event_occurences max", count_max );

        close(epfd);
    }
    return 0;
}