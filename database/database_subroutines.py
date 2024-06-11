import sqlite3
from datetime import datetime

def create_log(light_on:bool, method:str):
    conn = sqlite3.connect('SmartLight.db')
    cursor = conn.cursor()

    cmd = """
    INSERT INTO Logs (light_on, timestamp, method)
        VALUES (?, ?, ?)
    """
    args = (int(light_on), str(datetime.now()), method)

    cursor.execute(cmd, args)
    cursor.close()
    conn.commit()
    conn.close()

def get_all_logs():
    conn = sqlite3.connect('SmartLight.db')
    cursor = conn.cursor()

    cmd = "SELECT * FROM Logs"
    cursor.execute(cmd)
    results = cursor.fetchall()
    cursor.close()
    conn.close()

    return results

def get_most_recent_log():
    logs = get_all_logs()
    return sorted(logs, reverse=True)[0]
