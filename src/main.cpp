#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>

#include <libevdev-1.0/libevdev/libevdev.h>
#include <libevdev-1.0/libevdev/libevdev-uinput.h>
#include <linux/input-event-codes.h>

struct event_runner{
    libevdev *dev;
    libevdev_uinput *uidev;

    void init(){
        /*
        KEY (0001): 008b  009e  00fb  00fc  00fd 
        ABS (0003): 002f  : value 0, min 0, max 9, fuzz 0, flat 0, resolution 0
                    0030  : value 0, min 0, max 100, fuzz 0, flat 0, resolution 0
                    0031  : value 0, min 0, max 100, fuzz 0, flat 0, resolution 0
                    0032  : value 0, min 0, max 1066, fuzz 0, flat 0, resolution 0
                    0035  : value 0, min 0, max 799, fuzz 0, flat 0, resolution 0
                    0036  : value 0, min 0, max 1279, fuzz 0, flat 0, resolution 0
                    0039  : value 0, min 0, max 65535, fuzz 0, flat 0, resolution 0
                    003c  : value 0, min -90, max 90, fuzz 0, flat 0, resolution 0
                    003d  : value 0, min 0, max 1, fuzz 0, flat 0, resolution 0
        LED (0011): 0008 
        */
        dev = libevdev_new();
        libevdev_set_name(dev, "Tablet touchscreen");
        //libevdev_enable_property(dev, INPUT_PROP_DIRECT);
        libevdev_enable_property(dev, INPUT_PROP_POINTER);

        input_absinfo info;

        libevdev_enable_event_type(dev, EV_SYN);
        libevdev_enable_event_code(dev, EV_SYN, SYN_REPORT, NULL);

        libevdev_enable_event_type(dev, EV_ABS);

        int abs_codes[] = {0x002f, 0x0030, 0x0031, 0x0032, 0x0035, 0x0036, 0x0039, 0x003c, 0x003d};
        int abs_maxes[] = {9     , 100   , 100   , 1066  , 799   , 1279  , 65535 , 90    , 1     };

        for(int idx = 0; idx < 9; idx++){
            int i = abs_codes[idx];
            printf("%04X\n", i);
            info.value = 0;
            info.minimum = i == 0x003c ? -90 : 0;
            info.maximum = abs_maxes[idx];
            info.fuzz = 0;
            info.flat = 0;
            info.resolution = 0;
            libevdev_enable_event_code(dev, EV_ABS, i, &info);
            //libevdev_set_abs_info(dev, i, &info);
        }

        libevdev_enable_event_type(dev, EV_LED);
        libevdev_enable_event_code(dev, EV_LED, 0x0008, NULL);

        libevdev_enable_event_type(dev, EV_KEY);
        libevdev_enable_event_code(dev, EV_KEY, 0x008b, NULL);
        libevdev_enable_event_code(dev, EV_KEY, 0x009e, NULL);
        libevdev_enable_event_code(dev, EV_KEY, 0x00fb, NULL);
        libevdev_enable_event_code(dev, EV_KEY, 0x00fc, NULL);
        libevdev_enable_event_code(dev, EV_KEY, 0x00fd, NULL);

        int err = libevdev_uinput_create_from_device(
            dev,
            LIBEVDEV_UINPUT_OPEN_MANAGED,
            &uidev
        );
        if(err != 0){
            printf("Got create error %d", err);
            destroy();
            exit(1);
        }
    }

    void init_s_pen(){
        /*
        name:     "sec_e-pen"
        events:
            KEY (0001): 008b  008f* 009e  00f0  0140  0141  014a  014b 
                        0230  02f1  02f2  02f3  02f4  02f5  02f6 
            ABS (0003): 0000  : value 0, min 100, max 10804, fuzz 4, flat 0, resolution 0
                        0001  : value 0, min 100, max 17322, fuzz 4, flat 0, resolution 0
                        0018  : value 0, min 0, max 1024, fuzz 0, flat 0, resolution 0
            SW  (0005): 000e*
            LED (0011): 0008 
        input props:
            <none>

        */
        //Input device ID: bus 0x3 vendor 0x28bd product 0x920 version 0x100
        dev = libevdev_new();
        libevdev_set_name(dev, "Tablet s-pen");
        libevdev_enable_property(dev, INPUT_PROP_DIRECT);
        libevdev_set_id_bustype(dev, 0x3);
        libevdev_set_id_product(dev, 0x920);
        libevdev_set_id_vendor(dev, 0x28bd);
        libevdev_set_id_version(dev, 0x100);
        /*
        Event type 0 (EV_SYN)
        Event type 1 (EV_KEY)
            Event code 320 (BTN_TOOL_PEN)
            Event code 321 (BTN_TOOL_RUBBER)
            Event code 330 (BTN_TOUCH)
            Event code 331 (BTN_STYLUS)
        Event type 3 (EV_ABS)
            Event code 0 (ABS_X)
            Value   3320
            Min        0
            Max    32767
            Resolution     153
            Event code 1 (ABS_Y)
            Value  18231
            Min        0
            Max    32767
            Resolution     239
            Event code 24 (ABS_PRESSURE)
            Value      0
            Min        0
            Max     8191
            Event code 26 (ABS_TILT_X)
            Value      0
            Min     -127
            Max      127
            Event code 27 (ABS_TILT_Y)
            Value      0
            Min     -127
            Max      127
        Event type 4 (EV_MSC)
            Event code 4 (MSC_SCAN)
        */

        //libevdev_enable_property(dev, INPUT_PROP_POINTER);

        input_absinfo info;

        libevdev_enable_event_type(dev, EV_KEY);
        libevdev_enable_event_code(dev, EV_KEY, 0x008b, NULL);
        libevdev_enable_event_code(dev, EV_KEY, 0x008f, NULL);
        libevdev_enable_event_code(dev, EV_KEY, 0x009e, NULL);
        libevdev_enable_event_code(dev, EV_KEY, 0x00f0, NULL);
        libevdev_enable_event_code(dev, EV_KEY, 0x0140, NULL);
        libevdev_enable_event_code(dev, EV_KEY, 0x0141, NULL);
        libevdev_enable_event_code(dev, EV_KEY, 0x014a, NULL);
        libevdev_enable_event_code(dev, EV_KEY, 0x014b, NULL);
        libevdev_enable_event_code(dev, EV_KEY, 0x0230, NULL);
        libevdev_enable_event_code(dev, EV_KEY, 0x02f1, NULL);
        libevdev_enable_event_code(dev, EV_KEY, 0x02f2, NULL);
        libevdev_enable_event_code(dev, EV_KEY, 0x02f3, NULL);
        libevdev_enable_event_code(dev, EV_KEY, 0x02f4, NULL);
        libevdev_enable_event_code(dev, EV_KEY, 0x02f5, NULL);
        libevdev_enable_event_code(dev, EV_KEY, 0x02f6, NULL);

        libevdev_enable_event_type(dev, EV_SYN);
        //libevdev_enable_event_code(dev, EV_SYN, SYN_REPORT, NULL);

        libevdev_enable_event_type(dev, EV_ABS);

        int abs_codes[]       = {0x0000, 0x0001, 0x0018, 0x001a, 0x001b};
        int abs_maxes[]       = {10504 , 17022 , 256   , 127   , 127   };
        int abs_mins[]        = {400   , 400   , 0     , -127  , -127  };
        int abs_fuzz[]        = {4     , 4     , 0     , 0     , 0     };
        int abs_resolution[]  = {153   , 239   , 0     , 0     , 0     };

        for(int idx = 0; idx < 5; idx++){
            int i = abs_codes[idx];
            printf("%04X\n", i);
            info.value = 0;
            info.minimum = abs_mins[idx];
            info.maximum = abs_maxes[idx];
            info.fuzz = abs_fuzz[idx];
            info.flat = 0;
            info.resolution = abs_resolution[idx];
            libevdev_enable_event_code(dev, EV_ABS, i, &info);
            //libevdev_set_abs_info(dev, i, &info);
        }

        libevdev_enable_event_type(dev, EV_LED);
        libevdev_enable_event_code(dev, EV_LED, 0x0008, NULL);

        libevdev_enable_event_type(dev, EV_SW);
        libevdev_enable_event_code(dev, EV_SW, 0x000e, NULL);

        libevdev_enable_event_type(dev, EV_MSC);
        libevdev_enable_event_code(dev, EV_MSC, MSC_SCAN, NULL);

        int err = libevdev_uinput_create_from_device(
            dev,
            LIBEVDEV_UINPUT_OPEN_MANAGED,
            &uidev
        );
        if(err != 0){
            printf("Got create error %d", err);
            destroy();
            exit(1);
        }
    }

    void destroy(){
        libevdev_uinput_destroy(uidev);
        libevdev_free(dev);
    }

    void run_event(unsigned int type, unsigned int code, int value){
        int err = libevdev_uinput_write_event(uidev, type, code, value);
        if(err != 0) {
            printf("Got write error %d", err);
            destroy();
            exit(1);
        }
    }
};

int main(){
    const int MAX_X = 10804;
    const int MIN_X = 100;
    const int MAX_Y = 17322;
    const int MIN_Y = 100;
    event_runner er;
    er.init_s_pen();
    printf("Done initing\n");
    uint32_t vals[3];
    int64_t eid = 0;
    while(true){
        int ret = scanf("%x %x %x", &vals[0], &vals[1], &vals[2]);
        if(ret != 3){
            break;
        }
        if(vals[0] == EV_ABS && vals[1] == ABS_X){
            vals[1] = ABS_Y;
            vals[2] = MIN_X + (MAX_X - MIN_X - (vals[2] - MIN_X));
            vals[2] = (int)(((double)(vals[2] - MIN_X)/((double)(MAX_X - MIN_X))) * (MAX_Y - MIN_Y) + MIN_Y);
        } else if(vals[0] == EV_ABS && vals[1] == ABS_Y){
            vals[1] = ABS_X;
            vals[2] = (int)(((double)(vals[2] - MIN_Y)/((double)(MAX_Y - MIN_Y))) * (MAX_X - MIN_X) + MIN_X);
        }
        er.run_event(vals[0], vals[1], vals[2]);
        //printf("%s\n", libevdev_event_value_get_name(vals[0], vals[1], vals[2]));
        /*
        printf("Event ran %ld\t: %s %s %d\n",
            eid++,
            libevdev_event_type_get_name(vals[0]),
            libevdev_event_code_get_name(vals[0], vals[1]),
            vals[2]
        );
        */
    }
    er.destroy();
    return 0;
}