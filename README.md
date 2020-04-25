# Restor
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![made-with-coral](https://img.shields.io/badge/Made%20with-Coral-orange)](https://coral.ai/)
[![made-with-bash](https://img.shields.io/badge/Made%20with-Bash-1f425f.svg)](https://www.gnu.org/software/bash/)
[![made-with-c++](https://img.shields.io/badge/Made%20with-C%2B%2B-red)](https://www.cplusplus.com/)
[![made-with-opencv](https://img.shields.io/badge/Made%20with-OpenCV-blue)](https://opencv.org/)
[![made-with-tflite](https://img.shields.io/badge/Made%20with-Tensorflow--Lite-orange)](https://www.tensorflow.org/lite/)
[![made-with-cmake](https://img.shields.io/badge/Made%20with-cmake-Black)](https://cmake.org/)
[![ai-with-ai](https://img.shields.io/badge/AI%20with-AI-brightgreen)](https://en.wikipedia.org/wiki/Artificial_intelligence)
[![made-with-code-hoodies](https://img.shields.io/badge/Made%20with-coding%20hoodies-blue)](https://www.google.com/search?q=coding+hoodies&rlz=1CAPSFN_enUS898&source=lnms&tbm=isch&sa=X&ved=2ahUKEwjLyYPE2IHpAhW_HDQIHdqeBmwQ_AUoAXoECA8QAw&biw=1920&bih=961)
[![test-coverage](https://img.shields.io/badge/Test%20Coverage-0%25-yellow)](https://en.wikipedia.org/wiki/0)

Restor is a punny REST API for Object Detection.
You can send it non-garbage base64 encoded .bmp images, it can guess what objects are in that image and send you back a result as a json string.

Check out my [Medium](https://medium.com/@namvu1291/build-your-own-budget-ai-cloud-with-googles-coral-edgetpu-technology-66854340ecd3) article to see what you can do with this project:

[<img dth="777" src="https://github.com/Namburger/restor/blob/master/test_data/medium.png">](https://medium.com/@namvu1291/build-your-own-budget-ai-cloud-with-googles-coral-edgetpu-technology-66854340ecd3)
<br><b>Figure 1.</b>The AI Cloud

Please check the Notes section for some limitations as this project is still on it's early stage.

## API Endpoints
| Method  | Endpoint  | Function                               | Requires                              |
|:--------|:---------:|:--------------------------------------:|--------------------------------------:|
| POST    | /detects  | Does Object Detection                  | json string: {"data": base64(img.bmp)}|
| GET     | /version  | Exposes EdgeTPU Runtime Version        | None                                  |
| GET     | /info     | Exposes server's current configuration | None                                  |
| GET     | /metrics  | Exposes prometheus metrics             | None                                  |

## Requirements

* Hardware
  * Grab the Coral Dev Board or USB Accelerator or a PCIe Module from [coral.ai](https://coral.ai/products/) to get started.
* Software
  * 64bits x86-64 Debian based OS for the build (I develop this on Ubuntu 18.04 and Debian 10)
  * Install [libedgetpu runtime](https://coral.ai/software/#debian-packages) where the code needs to be ran 

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

After installing all third party libraries and finish building the binaries, don't forget to install [libedgetpu package](https://coral.ai/software/#debian-packages). Then:

* Running the server is as easy as (run from project root dir):

```
$ ./out/k8/restor
I1223 22:54:52.376971 22708 main.cc:28] RESTOR
I1223 22:54:52.390523 22708 main.cc:35] found 1 TPU(s)
I1223 22:54:52.390568 22708 main.cc:37] config: config/restor.yaml
I1223 22:54:56.403568 22708 server.cc:30] Engine initialized 
I1223 22:54:56.403604 22708 server.cc:31] model: test_data/mobilenet_ssd_v2_coco_quant_postprocess_edgetpu.tflite
I1223 22:54:56.403623 22708 server.cc:32] label: test_data/coco_labels.txt
I1223 22:54:56.403671 22708 server.cc:33] num_results: 5
I1223 22:54:56.403692 22708 server.cc:34] input_tensor_shape: [1,300,300,3]
I1223 22:54:56.403978 22708 server.cc:42] Serving on port: 8888
```

You can change the config/restor.yaml to configure your model, ports, etc...

* I have written 2 very simple example clients in the `example_client` directory.

1) A simple C++ CLI Client that allows you to send a POST images or access all available GET endpoints of the restor server.

This example is placed in `example_client/cpp_cli_client`.
To build:

```
$ cd example_client
$ make install
...
$ make
g++ -Iinclude -std=c++17 -Wall -Wextra -pthread -o restor_client main.cc
$ ls
include  install_dependencies.sh  main.cc  Makefile  restor_client
```

Then you can send any .bmp image to the server for object detection with this client by providing the ip and port of the server and the image you want it to detect:

* Take `grace_hopper.bmp` as an example POST request:

 <img width="200"
     src="https://github.com/Namburger/restor/blob/master/test_data/grace_hopper.bmp" />
<br><b>Figure 2.</b> grace_hopper.bmp


```
$ ./restor_client --host localhost --port 8888 --method post --endpoint /detects --image ../test_data/grace_hopper.bmp
Sending POST @data={"data": base64_encode(../test_data/grace_hopper.bmp)} to localhost:8888/detects
{
  "req_id": 1020,
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

* Here is an example GET request:

```
$ ./restor_client --host localhost --port 8888 --method get --endpoint /version
Sending GET to localhost:8888/version
{
  "edgetpu": "RuntimeVersion(12)",
  "req_id": 1022
}
```

2) A python client that takes an image with the cv library and send it to restor.

This example is placed in `example_client/cv_client`.
```
$ python3 cv_client.py --host localhost --port 8888
```

 <img width="500"
     src="https://github.com/Namburger/restor/blob/master/test_data/banana.png" />
<br><b>Figure 3.</b> banana.png


## Metrics

If you are interested in server metrics, they are available at the GET /metrics endpoint:
```
$ curl localhost:8888/metrics
# HELP server_request_total Number of total http requests handled
# TYPE server_request_total counter
server_request_total{endpoint="metrics",method="GET"} 63.000000
server_request_total{endpoint="version",method="GET"} 84.000000
server_request_total{endpoint="detects",method="POST"} 84.000000
# HELP process_open_fds Number of open file descriptors
# TYPE process_open_fds gauge
process_open_fds 18.000000
# HELP process_resident_memory_bytes Process resident memory size in bytes.
# TYPE process_resident_memory_bytes gauge
process_resident_memory_bytes 441085952.000000
# HELP process_virtual_memory_bytes Process virtual memory size in bytes.
# TYPE process_virtual_memory_bytes gauge
process_virtual_memory_bytes 704843776.000000
# HELP process_virtual_memory_max_bytes Process peak virtual memory size in bytes.
# TYPE process_virtual_memory_max_bytes gauge
process_virtual_memory_max_bytes 911925248.000000

```

An example prometheus.yaml config is provided in the config directory. You can easily setup a docker container by creating a Dockerfile with this content (also provided in the config directory):
```
FROM prom/prometheus
ADD prometheus.yml /etc/prometheus/
```

Then run:
```
$ docker build -t prometheus . 
$ docker run -p 9090:9090 prometheus 
```

You will then be able to query metrics by visiting localhost:9090/graph on your browser:

<img width="777"
     src="https://github.com/Namburger/restor/blob/master/test_data/metrics.png" />
<br><b>Figure 4.</b> metrics.png

## Notes

* Huge thanks to [google-coral/lstpu](https://github.com/google-coral/tflite/tree/master/cpp/examples/lstpu) and [google-coral/edgetpu](https://github.com/google-coral/edgetpu) for the build system and the opensource API because otherwise I would have a very hard time coming up with it myself.
* Huge thanks to all the opensource thirdparty libraries that I used for this project.
* All models and test data came from [edgetpu](https://github.com/google-coral/edgetpu/tree/master/test_data)
* The detector only support bmp images
* Honorably mention [snowzach/doods](github.com/snowzach/doods) for the great project, this is an attempt of doing something similar in a different language just for the joy of programming :)
* Will most likely add on Classifycation Engine later
