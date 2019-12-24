# Restor

A punny REST Object Detector but this is not a good name because I might add classifycation functionality later.

## Requirements

Grab the Coral Dev Board or USB Accelerator or a PCIe Module from [coral.ai](https://coral.ai/products/) to get started.

## Compile the project

* For the native compilation:

```
$ sudo apt-get install -y build-essential
$ python3 install_thirdparty.py
$ make
```

* For cross-compilation you need to install `build-essential` packages for
the corresponding architectures:

```
$ sudo apt-get install -y crossbuild-essential-armhf crossbuild-essential-arm64
$ python3 install_thirdparty.py
```

Then run `make CPU=k8` or `make CPU=aarch64`.
Default build will be for k8.

Find the output binary file inside the out directory.

# Misc

* Huge thanks to [google-coral/lstpu](https://github.com/google-coral/tflite/tree/master/cpp/examples/lstpu) and [google-coral/edgetpu](https://github.com/google-coral/edgetpu) for the build system and the opensource API because otherwise I would have a very hard time coming up with it myself.
* Huge thanks to all the opensource thirdparty libraries that I used for this project.
* All models and test data came from [edgetpu](https://github.com/google-coral/edgetpu/tree/master/test_data)
