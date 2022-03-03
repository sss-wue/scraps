import time
import paho.mqtt.client as paho
broker="broker"
#define callback
def on_message(client, userdata, message):
    time.sleep(1)
    print("received message =",str(message.payload.decode("utf-8")))

client= paho.Client("client-001") #create client object client1.on_publish = on_publish #assign function to callback client1.connect(broker,port) #establish connection client1.publish("house/bulb1","on")
######Bind function to callback
client.on_message=on_message
#####
print("connecting to broker ",broker)
client.connect(broker)#connect
client.loop_start() #start loop to process received messages
print("subscribing ")
client.subscribe("Simpl")#subscribe
while True:
   time.sleep(2)
   print("publishing ")
   client.publish("Simpl","on")#publish
   time.sleep(2)
   print("publishing ")
   client.publish("Simpl","off")#publish
client.disconnect() #disconnect
client.loop_stop() #stop loop
