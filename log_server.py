from flask import Flask, request, jsonify
import database.database_subroutines as subroutines

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

if __name__ == "__main__":
    app.run()