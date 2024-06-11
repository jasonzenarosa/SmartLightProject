from flask import Flask, request, jsonify
import database.database_subroutines as subroutines
import light_controls

app = Flask(__name__)

@app.route('/create-log', methods=['post'])
def add_new_alarm():
    try:
        light_on = bool(int(request.args.get('light-on')))
        method = request.args.get('method')
        subroutines.create_log(light_on, method)

    except Exception as e:
        return jsonify({'error': e}), 500
    
    return jsonify({'message': 'log added successfully'}), 200

@app.route('/logs', methods=['get'])
def retrieve_all_logs():
    try:
        logs = subroutines.get_all_logs()

    except Exception as e:
        return jsonify({'error': e}), 500
    
    return jsonify({'result': logs}), 200

@app.route('/lights-on', methods=['get'])
def lights_on():
    try:
        l1mac = light_controls.get_light_mac(1)
        l2mac = light_controls.get_light_mac(2)

        light_controls.control_light(l1mac, light_controls.ON)
        light_controls.control_light(l2mac, light_controls.ON)
    
    except Exception as e:
        return jsonify({'error': e}), 500
    
    return jsonify({'message': 'lights turned on successfully'}), 200

@app.route('/lights-off', methods=['get'])
def lights_off():
    try:
        l1mac = light_controls.get_light_mac(1)
        l2mac = light_controls.get_light_mac(2)

        light_controls.control_light(l1mac, light_controls.OFF)
        light_controls.control_light(l2mac, light_controls.OFF)
    
    except Exception as e:
        return jsonify({'error': e}), 500
    
    return jsonify({'message': 'lights turned on successfully'}), 200

if __name__ == "__main__":
    app.run()