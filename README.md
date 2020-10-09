# computer-vision

Computer vision using C++ and the OpenCV library.

## Build and run

- Compile from working directory in Vagrant Virtualbox: 

```bash
g++ -g /usr/lib64/libopencv_core.so.2.4 /usr/lib64/libopencv_highgui.so.2.4 src/<TARGET_FILE>.cpp -o build/<TARGET_FILE>;
```

- And run with:

```bash
./build/<TARGET_FILE>
```

- Or add the following to your `~/.bashrc`:

```bash
opencv_g++ () { g++ -g /usr/lib64/libopencv_core.so.2.4 /usr/lib64/libopencv_highgui.so.2.4 /usr/lib64/libopencv_imgproc.so.2.4 src/"$@".cpp -o build/"$@"; }
opencv_run () { ./build/"$@"; }
alias opencv_clean='rm -r build && mkdir build'
```

to compile with `opencv_g++ <TARGET_FILE>`, run with `opencv_run <TARGET_FILE>` and `opencv_clean` to delete current builds.

---

*Proper Makefile shall be configured soon...*
