#include "Arduino.h"
#include "ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Int8.h"

#define IGRNOREDEBUG 1 //must be set to 0 to enable fully working 

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
bool chargingDetected = 0;

//PCU FUNCTIONS:
void detectCharging();
void enableRouterSpeakerPower();
void enable5VdigitalPower(void);
void enable3V3digitalPower(void);

//WI-FI DEFINITIONS: ============================================================================
#define ESP32
const char* ssid     = "AutoBot1_2G";
const char* password = "mse2021cap";
IPAddress ip(192, 168, 1, 3);
IPAddress server(192,168,100,100);
const uint16_t serverPort = 11413; //port 1141x, where x =1 for DCU1, 2 for DCU2, and 3 for PCU

//ROS DEFINITIONS: ============================================================================
ros::NodeHandle PCU;
// Make a chatter publisher
std_msgs::String str_msg;
ros::Publisher chatter("pcu_test", &str_msg);

// Be polite and say hello
char hello[13] = "PCU IS HERE";
uint16_t period = 20;
uint32_t last_time = 0;

//contactor power
int contactorEnabled;
void startChargingByROS(const std_msgs::Int8& msg1);

ros::Subscriber<std_msgs::Int8> ChargingLock("/pcu/chargingunlock", startChargingByROS);
std_msgs::String chargingStatus_msg;
ros::Publisher chargingStatusPub("gigavac_feedback", &chargingStatus_msg);

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
  PCU.getHardware()->setConnection(server, serverPort);
  PCU.initNode();

  // Another way to get IP
  Serial.print("IP = ");
  Serial.println(PCU.getHardware()->getLocalIP());

  //SUBSCRIBER - PUBLISHER SECTION
  PCU.subscribe(ChargingLock); //charging lock
  PCU.advertise(chargingStatusPub); //charging status publisher

  // Start to be polite
  PCU.advertise(chatter);
}

// LOOP FUNCTION ==========================================================================
void loop() {
  // put your main code here, to run repeatedly:
  //first of all check DCU connection to ROS -> do not start program if no ROS node
  while (!PCU.connected() && !IGRNOREDEBUG) {
    Serial.println("ERROR: NO ROS CONNECTION");
  }
  
  //Automatically enable all the secondary voltage rails
  enableRouterSpeakerPower(); //must be automatic
  enable5VdigitalPower(); //must be automatic
  enable3V3digitalPower(); //must be automatic

  if (PCU.connected() && is12Venabled && is5VDenabled && is3V3Denabled) {
    digitalWrite(SYSGOOD, HIGH); //indicates that system correctly peforms, BLUE LED
  }

  //publish charging state
  chargingStatusPub.publish( &chargingStatus_msg ); //power locker, str_msg.data is generated based on the input from the ROS power locker

  //publish battery voltage and battery current


  PCU.spinOnce();
  delay(1);
}

// FUNCTIONS =====================================================================================
void startChargingByROS(const std_msgs::Int8& msg1) {
  //function that controls charging by ROS
  if (msg1.data == 1 && chargingDetected) {
    digitalWrite(ENABLE5VD, LOW);
    digitalWrite(ENABLE3V3D, LOW);
    digitalWrite(STARTCHARGING, HIGH);
    chargingStatus_msg.data = "Charging...";
  } else {
    digitalWrite(ENABLE5VD, HIGH);
    digitalWrite(ENABLE3V3D, HIGH);
    digitalWrite(STARTCHARGING, LOW);
  }
}

void detectCharging() {
  if (digitalRead(DETECTCHARGING)) {
    chargingDetected = 1;
  } else {
    Serial.println("CHARGING NOT DETECTED");
  }
}

void enableRouterSpeakerPower() {
  digitalWrite(ENABLE12V, HIGH);
  Serial.println("12V power rail is enabled");
  delay(1000);
  is12Venabled = 1;
}

void enable5VdigitalPower() {
  digitalWrite(ENABLE5VD, HIGH);
  Serial.println("5V digital rail is enabled");
  delay(1000);
  is5VDenabled = 1;
}

void enable3V3digitalPower() {
  digitalWrite(ENABLE3V3D, HIGH);
  Serial.println("3V3 digital rail is enabled");
  delay(1000);
  is3V3Denabled = 1;
}