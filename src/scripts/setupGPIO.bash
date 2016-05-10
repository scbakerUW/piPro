echo 23 > /sys/class/gpio/export
echo in > /sys/class/gpio/gpio23/direction
echo falling > /sys/class/gpio/gpio23/edge
