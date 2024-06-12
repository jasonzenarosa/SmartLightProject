import requests as HTTP_request
import json

ON = 'on'
OFF = 'off'

def control_light(light_mac, req_state):
    url = "https://developer-api.govee.com/v1/devices/control"

    payload = {
        "model": "YOUR_MODEL",
        "cmd": {
            "value": f"{req_state}",
            "name": "turn"
        },
        "device": f"{light_mac}"
    }
    headers = {
        "accept": "application/json",
        "content-type": "application/json",
        "Govee-API-Key": "YOUR_GOVEE_API_KEY"
    }

    return HTTP_request.put(url, json=payload, headers=headers)

def get_light_mac(light_num):
    url = "https://developer-api.govee.com/v1/devices"

    headers = {
        "accept": "application/json",
        "Govee-API-Key": "YOUR_GOVEE_API_KEY"
    }

    response = HTTP_request.get(url, headers=headers)
    response_dict = json.loads(response.text)
    return response_dict['data']['devices'][light_num-1]['device']

