# tads-plugin
alsa plugin extplug pass-through

## Run in Ubuntu
``` shell
cd tads-plugin
mkdir build && cd build
cmake ..
make
sudo make install
sudo ldconfig
```
## How to verify
``` shell
cd tads-plugin
cd asound-conf
cat asound.conf >> ~/.asoundrc
aplay -D tads_plug ./Chains.wav
```

## Issue you may meet
``` shell
swtest:~/PROJECT/tads-plugin/asound-conf$ aplay -D tads_plug ./Chains.wav 
ALSA lib dlmisc.c:252:(snd1_dlobj_cache_get) Cannot open shared library /usr/lib/x86_64-linux-gnu/alsa-lib/libasound_module_pcm_tads.so
aplay: main:722: audio open error: No such device or address
```
set .so install location when you call the cmake
```shell
cmake -DCMAKE_INSTALL_PREFIX=/usr/lib/x86_64-linux-gnu ..
```
