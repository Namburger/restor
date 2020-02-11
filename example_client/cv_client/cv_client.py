import cv2
import requests
import json
import base64
import argparse

parser = argparse.ArgumentParser(
    description='Send video frames to restor for processing.')
parser.add_argument('--host', type=str, default='localhost',
                    help="ip of the restor server.")
parser.add_argument('--port', type=str, default='8888',
                    help='port of the restor server.')

# send the post request
def send_request(URL, payload):
    print('Sending @data={\"data\": base64_encode(captured.bmp)} to', URL)
    resp = requests.post(url=URL, data=payload, headers={
        'content-type': 'application/json'})
    return json.loads(resp.text)

def main():
    args = parser.parse_args()
    print(args)
    URL = 'http://' + args.host + ':' + args.port + '/detects'
    tmp_file = '/tmp/captured.bmp'
    frame = cv2.VideoCapture(0)
    frame.set(cv2.CAP_PROP_FPS, 30)
    frame_num = 0
    while True:
        print('-----------------------------------')
        print('FRAME NUM:', frame_num)
        ret, img = frame.read()
        cv2.imwrite(tmp_file, img)
        # get payload
        with open(tmp_file, 'rb') as image:
            data = base64.b64encode(image.read()).decode('utf-8')
            payload = json.dumps({'data': data})
        j = send_request(URL, payload)
        print(json.dumps(j))
        cv2.imshow('Restor CV Client Demo', img)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
        frame_num += 1
        print('\n')
    frame.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
