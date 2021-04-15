#include<string.h>
#define Relay1 D1
#define Relay2 D2

char Relay1_state[4]="ON";
char Relay2_state[4]="ON";
char Relay3_state[4]="ON";
char Relay4_state[4]="ON";

char* ROUTER_ID="UWARL";
char* GROUP_ID="STAFF";

#define Relay1_Name "LIGHT"
#define Relay2_Name "FAN"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "Shashank007";
const char* password = "asskii16997$";
const char* mqtt_server = "3.17.28.238";
const char* username = "username";
const char* password_mqtt="qwerty";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  write_command("ON",Relay1_state,Relay1);
  write_command("ON",Relay2_state,Relay2);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void send_data(char* router_id,char *group_id,char* device_id,char* state)
{
    char s[100];
    strcpy(s,router_id);
    strcat(s,"/");
    strcat(s,group_id);
    strcat(s,"/");
    strcat(s,device_id);
    strcat(s,"/");
    strcat(s,state);
//    printf("%s\n",s);
    snprintf (msg,50,s);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("smarthome/status", msg);
}

void receive_data(char *data)
{
    char str[20]="";
    strcpy(str,data);
    char *token;
    token=strtok(str,"/");
    int i=0;
    char s1[4][1000];
    while(token!=NULL)
    {
        strcpy(s1[i++],token);
        token=strtok(NULL,"/");
    }
    int j=0;
    char router_id[20],group_id[20],device_id[20],state[20];
    strcpy(router_id,s1[0]);
    strcpy(group_id,s1[1]);
    strcpy(device_id,s1[2]);
    strcpy(state,s1[3]);
    Serial.print("router_id");
    Serial.println(router_id);
    Serial.print("group_id");
    Serial.println(group_id);
    Serial.print("device_id");
    Serial.println(device_id);
    Serial.print("state");
    Serial.println(state);
    Serial.println("***");
    if(strncmp(router_id,ROUTER_ID,strlen(ROUTER_ID))==0)
    {
      if(strncmp(group_id,GROUP_ID,strlen(GROUP_ID))==0)
      {
//        Serial.println("Same");
        if(strncmp(device_id,Relay1_Name,strlen(Relay1_Name))==0)
        {
//          Serial.println("Relay1");
          write_command(state,Relay1_state,Relay1);
          publish();
        }
        else if(strncmp(device_id,Relay2_Name,strlen(Relay2_Name))==0)
        {
//          Serial.println("Relay2");
          write_command(state,Relay2_state,Relay2);
          publish();
        }
      }
    }

}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  delay(1);
  String payl=String((char*)payload);
  String payl2=payl.substring(0,length);
//  for (int i = 0; i < length; i++) {
//    Serial.print((char)payload[i]);
//
//  }
  Serial.println(payl2);
  char payl23[30];
  strcpy(payl23,payl2.c_str());
  receive_data(payl23);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),username,password_mqtt)) {
      Serial.println("connected");
      // Once connected, publish an announcement..
      // ... and resubscribe
      client.subscribe("smarthome/command");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  publish();
}

void write_command(char* State,char* Relay_State,char Relay_Pin)
{
    Serial.println(Relay_State);
    if(strncmp(State,"ON",2)==0)
    {
      strcpy(Relay_State,"ON");
      digitalWrite(Relay_Pin,HIGH);
    }
    else
    {
      strcpy(Relay_State,"OFF");
      digitalWrite(Relay_Pin,LOW);
    }
    
}

void publish(){
    send_data(ROUTER_ID,GROUP_ID,Relay1_Name,Relay1_state);
    send_data(ROUTER_ID,GROUP_ID,Relay2_Name,Relay2_state);
}
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;
    ++value;
    publish();
  }
}
