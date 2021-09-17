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

    bool is_touch;
    int min_x;
    int max_x;
    int min_y;
    int max_y;

    void init_touch_screen(int _minx, int _maxx, int _miny, int _maxy){
        min_x = _minx;
        max_x = _maxx;
        min_y = _miny;
        max_y = _maxy;
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
        is_touch = true;
        dev = libevdev_new();
        libevdev_set_name(dev, "Tablet touchscreen");
        libevdev_enable_property(dev, INPUT_PROP_DIRECT);
        //libevdev_enable_property(dev, INPUT_PROP_POINTER);

        input_absinfo info;

        libevdev_enable_event_type(dev, EV_SYN);
        libevdev_enable_event_code(dev, EV_SYN, SYN_REPORT, NULL);

        libevdev_enable_event_type(dev, EV_ABS);

        int abs_codes[] =   {0x002f, 0x0030, 0x0031, 0x0032, 0x0035, 0x0036, 0x0039, 0x003c, 0x003d};
        int abs_maxes[] =   {9     , 100   , 100   , 1066  , 799   , 1279  , 65535 , 90    , 1     };
        int resolutions[] = {1     , 1     , 1     , 1     , 1     , 1     , 1     , 1     , 1     };

        for(int idx = 0; idx < 9; idx++){
            int i = abs_codes[idx];
            printf("%04X\n", i);
            info.value = 0;
            info.minimum = i == 0x003c ? -90 : 0;
            info.maximum = abs_maxes[idx];
            info.fuzz = 0;
            info.flat = 0;
            info.resolution = resolutions[idx];
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

    void init_s_pen(int _minx, int _maxx, int _miny, int _maxy){
        min_x = _minx;
        max_x = _maxx;
        min_y = _miny;
        max_y = _maxy;
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
        is_touch = false;
        dev = libevdev_new();
        libevdev_set_name(dev, "Tablet s-pen");
        libevdev_enable_property(dev, INPUT_PROP_DIRECT);
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
        int abs_resolution[]  = {1     , 1     , 0     , 0     , 0     };

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
        //This is the transformation I should use for how I use my device
        if(is_touch){
            if(type == EV_ABS && code == ABS_MT_POSITION_X){
                code = ABS_MT_POSITION_Y;
                value = min_x + (max_x - min_x - (value - min_x));
                value = (int)(((double)(value - min_x)/((double)(max_x - min_x))) * (max_y - min_y) + min_y);
            } else if(type == EV_ABS && code == ABS_MT_POSITION_Y){
                code = ABS_MT_POSITION_X;
                value = (int)(((double)(value - min_y)/((double)(max_y - min_y))) * (max_x - min_x) + min_x);
            }
        } else {
            if(type == EV_ABS && code == ABS_X){
                code = ABS_Y;
                value = min_x + (max_x - min_x - (value - min_x));
                value = (int)(((double)(value - min_x)/((double)(max_x - min_x))) * (max_y - min_y) + min_y);
            } else if(type == EV_ABS && code == ABS_Y){
                code = ABS_X;
                value = (int)(((double)(value - min_y)/((double)(max_y - min_y))) * (max_x - min_x) + min_x);
            }
        }
        int err = libevdev_uinput_write_event(uidev, type, code, value);
        if(err != 0) {
            printf("Got write error %d", err);
            destroy();
            exit(1);
        }
    }
};

int main(){
    event_runner er;
    //for GTN5100 these values are OK
    er.init_touch_screen(0, 799, 0, 1279);
    //er.init_s_pen(100, 10804, 100, 17322);
    printf("Done initing\n");
    uint32_t vals[3];
    int64_t eid = 0;
    while(true){
        int ret = scanf("%x %x %x", &vals[0], &vals[1], &vals[2]);
        if(ret != 3){
            break;
        }
        er.run_event(vals[0], vals[1], vals[2]);
    }
    er.destroy();
    return 0;
}
