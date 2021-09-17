while true
do
	adb exec-out "getevent /dev/input/event2" > ev_fifo
	sleep 1
done
