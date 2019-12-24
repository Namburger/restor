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

## Example run

After installing all third party library finish building the binary, running the server is as easy as:

```
$ ./out/k8/restor
I1223 22:54:52.376971 22708 main.cc:28] RESTOR
I1223 22:54:52.390523 22708 main.cc:35] found 1 TPU(s)
I1223 22:54:52.390568 22708 main.cc:37] config: config/restor.json
I1223 22:54:56.403568 22708 server.cc:30] Engine initialized 
I1223 22:54:56.403604 22708 server.cc:31] model: test_data/mobilenet_ssd_v2_coco_quant_postprocess_edgetpu.tflite
I1223 22:54:56.403623 22708 server.cc:32] label: test_data/coco_labels.txt
I1223 22:54:56.403671 22708 server.cc:33] num_results: 5
I1223 22:54:56.403692 22708 server.cc:34] input_tensor_shape: [1,300,300,3]
I1223 22:54:56.403978 22708 server.cc:42] Serving on port: 8888
```

You can change the config/restor.json. One liner example to send a POST to the server:

```
$ echo "{\"detector\":\"please\", \"data\":\"`cat test_data/grace_hopper.bmp|base64 -w0`\"}" > /tmp/grace.json && curl -d@/tmp/grace.json -H "Content-Type: application/json" -X POST http://localhost:8888/detects | jq
  % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed
100 1224k  100   161  100 1224k   2555  18.9M --:--:-- --:--:-- --:--:-- 18.9M
{
  "req_id": 2,
  "result1": {
    "candidate": "person",
    "score": 0.83984375
  },
  "result2": {
    "candidate": "tie",
    "score": 0.58203125
  },
  "result3": {
    "candidate": "tie",
    "score": 0.2109375
  }
}

```

I also use [jq](https://stedolan.github.io/jq/) to make the json string prettier, optionally you can just run the above command without `| jq` at the end if you don't have it installed.

## Notes

* Huge thanks to [google-coral/lstpu](https://github.com/google-coral/tflite/tree/master/cpp/examples/lstpu) and [google-coral/edgetpu](https://github.com/google-coral/edgetpu) for the build system and the opensource API because otherwise I would have a very hard time coming up with it myself.
* Huge thanks to all the opensource thirdparty libraries that I used for this project.
* All models and test data came from [edgetpu](https://github.com/google-coral/edgetpu/tree/master/test_data)
* The detector only support bmp images
