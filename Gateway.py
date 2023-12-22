import paho.mqtt.client as mqtt
import json
import serial
import time

# Serial initialization
serial_port = '/dev/rfcomm0'  # Replace with your Arduino serial port
baud_rate = 9600
btSerial = serial.Serial(serial_port, baud_rate)
btSerial.timeout = 1

# MQTT topics
client_id = 'df19fda8-4db2-4229-99c4-dda1aae946fa'
telemetry_topic = 'df19fda8-4db2-4229-99c4-dda1aae946fa/telemetry'
commands_topic = 'df19fda8-4db2-4229-99c4-dda1aae946fa/commands'

# MQTT initialization
mqtt_client = mqtt.Client(client_id)
mqtt_client.connect('test.mosquitto.org') 
mqtt_client.loop_start()

# Commands handling part
def handle_commands(client, userdata, message):
    command = json.loads(message.payload.decode())
    print("Command received:", command)
    
    if "setThreshold" in command:
        new_threshold = command["setThreshold"]
        # Forward the threshold to Arduino
        btSerial.write(f"setThreshold {new_threshold}\n".encode("utf-8"))

mqtt_client.subscribe(commands_topic)
mqtt_client.on_message = handle_commands

# Telemetry handling part
while True:
    # Receive telemetry from Arduino
    bt_telemetry = btSerial.readline().decode('ascii').strip()
    
    if bt_telemetry:
        print('Received BT telemetry:', bt_telemetry)

        # Process and send telemetry to MQTT
        try:
            # Parse telemetry from Arduino string
            distance1, percentage = map(int, bt_telemetry.split(','))
            
            # Create telemetry JSON
            telemetry_json = {
                'distance1': distance1,
                'percentage': percentage,
                'timestamp': round(time.time() * 1000)
            }

            # Send telemetry to MQTT
            telemetry_json_str = json.dumps(telemetry_json)
            print("Sending telemetry:", telemetry_json_str)
            mqtt_client.publish(telemetry_topic, telemetry_json_str, qos=1)
        except ValueError:
            print('Wrong telemetry format')

    time.sleep(1)  # Adjust delay as needed
