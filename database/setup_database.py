import sqlite3

# Connect to the database (or create it if it doesn't exist)
connection = sqlite3.connect('SmartLight.db')

# Create a cursor object to execute SQL commands
cursor = connection.cursor()

cursor.execute('''
            CREATE TABLE IF NOT EXISTS Logs (
               log_id       INTEGER PRIMARY KEY AUTOINCREMENT,
               light_on     INTEGER NOT NULL,
               timestamp    TEXT NOT NULL,
               method       TEXT NOT NULL
            )
''')

# Commit the changes and close the connection
cursor.close()
connection.commit()
connection.close()