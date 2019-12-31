import cv2
import requests
import json
import base64
import argparse

parser = argparse.ArgumentParser(
    description='Taking an image and send it to restor for processing.')
parser.add_argument('--host', type=str, default='localhost',
                    help="ip of the restor server.")
parser.add_argument('--port', type=str, default='8888',
                    help='port of the restor server.')

# Take an image
def take_image(file):
    camera_port = 0
    camera = cv2.VideoCapture(camera_port)

    def get_image():
        retval, im = camera.read()
        return im

    # will take 10 tmp shots to warm up the camera
    for i in range(10):
        temp = get_image()
    print('Taking image...')
    captured = get_image()
    print('Writing image to captured.bmp')
    cv2.imwrite(file, captured)
    del(camera)

# send the post request
def send_request(URL, payload):
    print('Sending @data={\"data\": base64_encode(captured.bmp) to', URL)
    resp = requests.post(url=URL, data=payload, headers={
        'content-type': 'application/json'})
    j = json.loads(resp.text)
    print(json.dumps(j, indent=2, sort_keys=True))


def main():
    args = parser.parse_args()
    print(args)
    URL = 'http://' + args.host + ':' + args.port + '/detects'

    file = '/tmp/captured.bmp'
    take_image(file)

    # get payload
    with open(file, 'rb') as image:
        data = base64.b64encode(image.read()).decode('utf-8')
        payload = json.dumps({'data': data})
    send_request(URL, payload)

    # show image
    img = cv2.imread(file)
    cv2.imshow("captured", img)
    cv2.waitKey(0)
    cv.destroyAllWindows()


if __name__ == "__main__":
    main()
