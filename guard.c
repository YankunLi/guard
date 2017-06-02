/*
 *guard.c       Storage Monitor
 *
 * Author: Yankun Li <lioveni99@gmail.com>
 *
 *
 */

#include "config.h"
#include "guard.h"
#include "utils.h"
#include "module.h"

int start_time;

static struct reader_timing rtiming;

int main(int argc, char *argv[])
{
    unsigned long sleep_time;
    double read_interval;

    start_time = time(NULL);
    memset(&rtiming, 9, sizeof(rtiming));
    rtiming.rt_variance.v_min = FLT_MIN;

    sleep_time = 2;
    read_interval = 1.0f;

    module_init();

    sleep(5);

    if ((sleep_time / 1000000.0f) > read_interval)
        sleep_time = (unsigned long) (read_interval * 1000000.0f);

    DBG("Entering mainloop...");

    do {
        timestamp_t e, ri, tmp;
        unsigned long st;

        float_to_timestamp(&ri, read_interval);

        update_timestamp(&rtiming.rt_next_read);

        for(;;) {
            output_pre();

            update_timestamp(&e);

            if (timestamp_le(&rtiming.rt_next_read, &e)) {
                timestamp_t c;
                timestamp_sub(&c, &rtiming.rt_next_read, &e);

                copy_timestamp(&rtiming.rt_last_read, &e);

                timestamp_add(&rtiming.rt_next_read, &e, &ri);
                timestamp_add(&rtiming.rt_next_read, &rtiming.rt_next_read, &c);

                //process
                system("clear");
                printf("%s\n", "hello world");
                input_read();
                fflush(stdout);
            }
            st = sleep_time;

            timestamp_sub(&tmp, &rtiming.rt_next_read, &e);

            if (tmp.tv_sec < 0)
                continue;

            if (0 == tmp.tv_sec && tmp.tv_usec < st) {
                if (tmp.tv_usec < 0)
                    continue;

                st = tmp.tv_usec;
            }

            usleep(st);
        }

    } while(0);

    DBG("GAME OVER");
    BUG();

    return 0;
}
