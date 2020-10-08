# computer-vision

Computer vision using C++ and the OpenCV library.

---

Compile from working directory in Vagrant Virtualbox: 

```bash
opencv_g++ () { g++ -g /usr/lib64/libopencv_core.so.2.4 /usr/lib64/libopencv_highgui.so.2.4 src/"$@".cpp -o build/"$@"; }
```

And run with:

```bash
opencv_run () { ./build/"$@"; }
```
