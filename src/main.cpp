#include "Arduino.h"
#include "ros.h"
#include "std_msgs/String.h"

#define SYSGOOD 13
#define ENABLE12V 14 
#define ENABLE5VD 15
#define ENABLE3V3D 16
#define STARTCHARGING 17
#define DETECTCHARGING 26
#define SCL 22
#define SDA 21

bool is12Venabled = 0;
bool is5VDenabled = 0;
bool is3V3Denabled = 0;
bool isCharging = 0;

//PCU FUNCTIONS:
void startCharging(bool isChargingDetected);
void enableRouterSpeakerPower();
void enable5VdigitalPower(void);
void enable3V3digitalPower(void);

//ROS DEFINITIONS: ============================================================================
#define ESP32
const char* ssid     = "AutoBot1_2G";
const char* password = "mse2021cap";
IPAddress ip(192, 168, 1, 3);
IPAddress server(192,168,100,100);
const uint16_t serverPort = 11413; //port 1141x, where x =1 for DCU1, 2 for DCU2, and 3 for PCU
ros::NodeHandle nh;
// Make a chatter publisher
std_msgs::String str_msg;
ros::Publisher chatter("pcu", &str_msg);

// Be polite and say hello
char hello[13] = "PCU IS HERE";
uint16_t period = 20;
uint32_t last_time = 0;

// MAIN FUNCTION ==============================================================================
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(SYSGOOD, OUTPUT);
  pinMode(ENABLE12V, OUTPUT);
  pinMode(ENABLE3V3D, OUTPUT);
  pinMode(ENABLE5VD, OUTPUT);
  pinMode(STARTCHARGING, OUTPUT);

  digitalWrite(ENABLE5VD, LOW);
  digitalWrite(ENABLE3V3D, LOW);
  digitalWrite(ENABLE12V, LOW);
  digitalWrite(STARTCHARGING, LOW);

  delay(10000);

  //ROS PART --------------------------------------------------------------------------------------------------
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Set the connection to rosserial socket server
  nh.getHardware()->setConnection(server, serverPort);
  nh.initNode();

  // Another way to get IP
  Serial.print("IP = ");
  Serial.println(nh.getHardware()->getLocalIP());

  // Start to be polite
  nh.advertise(chatter);
}

// LOOP FUNCTION ==========================================================================
void loop() {
  // put your main code here, to run repeatedly:

  //test codes -> uncomment certain function to use is separately
  //enableRouterSpeakerPower();
  //enable5VdigitalPower();
  //enable3V3digitalPower();
  //digitalWrite(SYSGOOD, HIGH);
  //startCharging(true);

  if(millis() - last_time >= period)
  {
    last_time = millis();
    if (nh.connected())
    {
      Serial.println("Connected");
      // Say hello
      str_msg.data = hello;
      chatter.publish( &str_msg );
    } else {
      Serial.println("Not Connected");
    }
  }
  nh.spinOnce();
  delay(1);
}

//functions
void startCharging(bool isChargingDetected) {
  if (isChargingDetected) {
    digitalWrite(STARTCHARGING, HIGH);
    digitalWrite(ENABLE5VD, LOW);
    digitalWrite(ENABLE3V3D, LOW);
    Serial.println("charging started");
    delay(5000);
  } else {
    Serial.println("ERROR: CHARGING NOT DETECTED");
  }

  digitalWrite(STARTCHARGING, LOW);
  delay(2000);
}

void enableRouterSpeakerPower() {
  digitalWrite(ENABLE12V, HIGH);
  Serial.println("12V power rail is enabled");
  delay(1000);
  //Serial.println("12V power rail is disabled");
  //digitalWrite(ENABLE12V, LOW);
  //delay(20000);
}

void enable5VdigitalPower() {
  digitalWrite(ENABLE5VD, HIGH);
  Serial.println("5V digital rail is enabled");
  delay(1000);
  //Serial.println("5V digital rail is disabled");
  //digitalWrite(ENABLE5VD, LOW);
  //delay(20000);
}

void enable3V3digitalPower() {
  digitalWrite(ENABLE3V3D, HIGH);
  Serial.println("3V3 digital rail is enabled");
  delay(1000);
  //Serial.println("3V3 digital rail is disabled");
  //digitalWrite(ENABLE3V3D, LOW);
  //delay(20000);
}