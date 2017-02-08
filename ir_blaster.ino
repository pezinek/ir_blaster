#include <Homie.h>
#include <IRremoteESP8266.h>
#include <ArduinoJson.h>

//const int LED_PIN = LED_BUILTIN;
const int LED_PIN = 5;
const int RECV_PIN = 2;
const int SEND_PIN = 0;
const int REFLASH_PIN = 4;

int brightness = PWMRANGE;
bool led_on = false;
enum ir_status_enum {
  IR_OFFLINE,
  IR_LISTENING,
  IR_SENDING,
} ir_status;


IRrecv irrecv(RECV_PIN);
IRsend irsend(SEND_PIN);

decode_results results;

HomieNode ledNode("led", "switch");
HomieNode infraredNode("infrared", "infrared");

void updateLed() {
  if (brightness == 0) {
    led_on = false;
  }
  
  if (led_on) {
    analogWrite(LED_PIN, PWMRANGE-brightness);
  } else {
    analogWrite(LED_PIN, PWMRANGE);
  }

  ledNode.setProperty("brightness").send(String(brightness));
  ledNode.setProperty("status").send(led_on ? "ON" : "OFF");
}


bool ledBrightnessHandler(const HomieRange& range, const String& value) {
  brightness = value.toInt();
  if (brightness > PWMRANGE) { brightness = PWMRANGE; }
  if (brightness < 0) { brightness = 0; }

  updateLed();
  return true;
}

bool ledStatusHandler(const HomieRange& range, const String& value) {
  if ((value != "ON") && (value != "OFF")) return false;

  if (value == "ON") {
    led_on = true;
  } else {
    led_on = false;
  }

  updateLed();
  return true;
}

char *code_type2name(int decode_type) {
  if (decode_type == UNKNOWN) {
    return "UNKNOWN";
  } else if (decode_type == NEC) {
    return "NEC";
  } else if (decode_type == SONY) {
    return "SONY";
  } else if (decode_type == RC5) {
    return "RC5";    
  } else if (decode_type == RC6) {
    return "RC6";
  } else {
    return "UNKNOWN";   
  }
}



void setInfraredStatus(ir_status_enum s) {
  if (ir_status == s) { return; }
  
  char *state_name;
  switch(s) {
    case IR_OFFLINE: 
        state_name="OFF"; 
        break;
    case IR_LISTENING: 
        state_name="LISTENING"; 
        irrecv.enableIRIn();
        irrecv.resume();
        break;
    case IR_SENDING: 
        state_name="SENDING"; 
        //irrecv.disableIRIn();
        break;
    default: state_name="UNKNOWN"; break;
  }  

  ir_status = s;  

  infraredNode.setProperty("status").send(state_name);  
}

const int send_buf_len=128;
unsigned int sendbuf[send_buf_len];

void infraredReceiver(decode_results *results) {
  char name[21];
  int npos=0;  

  strcpy(name, code_type2name(results->decode_type));
  npos+=strlen(name);

  snprintf((char*)(&name)+npos, sizeof(name)-npos,": 0x%X", results->value);    
  Homie.getLogger() << "IR Code received: " << name << " (len: " << results->rawlen << ")" << endl;

  StaticJsonBuffer<1024> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  root["name"]=name;
  root["value"]=results->value;
  root["frequency"]=38;

  JsonObject& type = root.createNestedObject("type");
  type["id"] = results->decode_type;
  type["name"] = code_type2name(results->decode_type);

  JsonArray& raw = root.createNestedArray("raw");
  for (int i = 1; i < results->rawlen; i++) {
    raw.add(results->rawbuf[i]*USECPERTICK);
  }

  char buf[1024];
  root.printTo(buf, sizeof(buf));
  infraredNode.setProperty("code").send(buf);  
}

bool infraredSender(const HomieRange& range, const String& value) {  
  StaticJsonBuffer<3076> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(value);

  if (!root.success()) {
    infraredNode.setProperty("status").send("500 - Failed to parse code as JSON");
    return true;
  }

  if (!root.containsKey("raw")) {
    infraredNode.setProperty("status").send("501 - Key \"raw\" is missing in JSON");
    return true;
  }

  if (!root.containsKey("frequency")) {
    infraredNode.setProperty("status").send("502 - Key \"frequency\" is missing in JSON");
    return true;
  }

  int hz = root["frequency"];
  int len = root["raw"].size();
  if (len > send_buf_len) {
    len = send_buf_len;
  }

  for (int i=0; i<len; i++) {
    sendbuf[i]=(unsigned int)(root["raw"][i]);
  }

  setInfraredStatus(IR_SENDING);
  irsend.sendRaw(sendbuf, len, hz);
  
  JsonObject& new_root = jsonBuffer.createObject();
  new_root["frequency"] = hz;
  JsonArray& raw = new_root.createNestedArray("raw");
  for (int i = 0; i < len; i++) {
    raw.add(sendbuf[i]);
  }

  char buf[1024];
  new_root.printTo(buf, sizeof(buf));
  infraredNode.setProperty("code").send(buf);

  setInfraredStatus(IR_LISTENING);

  return true;
}


void loopHandler() {
  if (irrecv.decode(&results)) {
    infraredReceiver(&results);
    irrecv.resume();
  }
}

void setupHandler() {
  ledNode.setProperty("brightness").send(String(brightness));
  ledNode.setProperty("status").send("OFF");
  setInfraredStatus(IR_LISTENING);
}

void setup() {
  Serial.begin(115200);
  Serial << endl << endl << "[IR_blaster]" << endl;
  //Serial.println("[homie_test]");
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  irsend.begin();

  Homie.setLedPin(LED_PIN, LOW);
  Homie_setFirmware("homie_test", "1.0.0");
  Homie.setSetupFunction(setupHandler);
  Homie.setLoopFunction(loopHandler);
  Homie.setResetTrigger(REFLASH_PIN, LOW, 5000);

  ledNode.advertise("brightness").settable(ledBrightnessHandler);
  ledNode.advertise("status").settable(ledStatusHandler);
  infraredNode.advertise("code").settable(infraredSender);
  infraredNode.advertise("status");

  Homie.setup();  
}

void loop() {
  Homie.loop();
}
