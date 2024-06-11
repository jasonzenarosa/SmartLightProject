from flask import Flask
from flask import request as flask_request
import requests as HTTP_request
import json

app = Flask(__name__)
@app.route("/")

def get_light_mac(light_num):
    url = "https://developer-api.govee.com/v1/devices"

    headers = {
        "accept": "application/json",
        "Govee-API-Key": "061548ff-f891-4fa6-a32d-ea8b26e848b3"
    }

    response = HTTP_request.get(url, headers=headers)
    response_dict = json.loads(response.text)
    return response_dict['data']['devices'][light_num-1]['device']

def turn_on_light(light_mac, req_state):
    url = "https://developer-api.govee.com/v1/devices/control"

    payload = {
        "model": "H6008",
        "cmd": {
            "value": f"{req_state}",
            "name": "turn"
        },
        "device": f"{light_mac}"
    }
    headers = {
        "accept": "application/json",
        "content-type": "application/json",
        "Govee-API-Key": "061548ff-f891-4fa6-a32d-ea8b26e848b3"
    }

    return HTTP_request.put(url, json=payload, headers=headers)

def hello():
    light_1_mac = get_light_mac(1);
    light_2_mac = get_light_mac(2);


    return "We received value: "+ str(flask_request.args.get("light"))
