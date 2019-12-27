# Restor

Restor is a punny REST API for Object Detection but this is not a good name because I might add classification functionality later.
Machine Learning Inference for Object Detection on the Google's EdgeTPU Platform.
You can send it non-garbage base64 encoded .bmp images, it can guess what objects are in that image and send you back a result as a json string.
Please check the Notes section for some limitations as this project is still on it's early stage.


## API Endpoints
| Method  | Endpoint  | Function                               | Requires                  |
|:--------|:---------:|:--------------------------------------:|--------------------------:|
| POST    | /detects  | Does Object Detection                  | json data of a .bmp image |
| GET     | /version  | Exposes EdgeTPU Runtime Version        | None                      |
| GET     | /info     | Exposes server's current configuration | None                      |
| GET     | /metrics  | Exposes prometheus metrics             | None                      |

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

## Example run (please run these examples from root dir)

After installing all third party libraries and finish building the binaries, don't forget to install [libedgetpu package](https://coral.ai/software/#debian-packages). Then:

* Running the server is as easy as:

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

* One liner example to send a POST to the server as a client:

<img width="200"
     src="https://github.com/Namburger/restor/blob/master/test_data/grace_hopper.bmp" />
<br><b>Figure 1.</b> grace_hopper.bmp

```
$ echo "{\"data\":\"`cat test_data/grace_hopper.bmp|base64 -w0`\"}" > /tmp/grace.json && curl -d@/tmp/grace.json -H "Content-Type: application/json" -X POST http://localhost:9090/detects | jq
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

Explaination of the one liner:
```
$ # create the JSON file with BMP encoded data
$ echo "{\"data\":\"`cat test_data/grace_hopper.bmp|base64 -w0`\"}" > /tmp/grace.json
$ # if no error, submit that JSON file
$ curl -d@/tmp/grace.json \
  -H "Content-Type: application/json" \
  -X POST \
  http://localhost:9090/detects | 
jq
```

## Metrics

If you are interested in server metrics, they are available at the /metrics endpoint.

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
<br><b>Figure 2.</b> metrics.png

## Notes

* Huge thanks to [google-coral/lstpu](https://github.com/google-coral/tflite/tree/master/cpp/examples/lstpu) and [google-coral/edgetpu](https://github.com/google-coral/edgetpu) for the build system and the opensource API because otherwise I would have a very hard time coming up with it myself.
* Huge thanks to all the opensource thirdparty libraries that I used for this project.
* All models and test data came from [edgetpu](https://github.com/google-coral/edgetpu/tree/master/test_data)
* The detector only support bmp images
* Honorably mention [snowzach/doods](github.com/snowzach/doods) for the great project, this is an attempt of doing something similar in a different language just for the joy of programming :)
* Will most likely add on Classifycation Engine later
