cat tests/buffer/$1|nc -l -p 2042 -q 0 &
sleep 0.5
./buffer
