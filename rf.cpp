#include "rf.h"
#include <LoRaLib.h>
#define FREQ            868.0
int _g_rssi = 0;
float _g_snr = 0;

//cs IO0 IO1
SX1276  lora = new LoRa(5,2,35); // nss DIO0 DIO1
String str_rfm96wBuffer;
// flag to indicate that a packet was received
volatile bool receivedFlag = false;

// disable interrupt when it's not needed
volatile bool enableInterrupt = true;

// this function is called when a complete packet
// is received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
void setFlag(void) {
  // check if the interrupt is enabled
  if(!enableInterrupt) {
    return;
  }

  // we got a packet, set the flag
  receivedFlag = true;
}
void RFM96W_Init(){
    // initialize SX1278 with default settings
  Serial.print(F("Initializing ... "));
  // carrier frequency:           434.0 MHz
  // bandwidth:                   125.0 kHz
  // spreading factor:            9
  // coding rate:                 7
  // sync word:                   0x12
  // output power:                17 dBm
  // current limit:               100 mA
  // preamble length:             8 symbols
  // amplifier gain:              0 (automatic gain control)
  int state = lora.begin(FREQ, 250.0, 12, 5, 0x12, 20, 100, 16);
  
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    RFM96W_Init();
    //while (true);
  }

  // set the function that will be called
  // when new packet is received
  lora.setDio0Action(setFlag);
  lora.setCRC(true);
  
  // start listening for LoRa packets
  // NOTE: for spreading factor 6, the packet length
  //       must be known in advance, and provided to both
  //       startReceive() and readData() methods!
 // Serial.print(F("Starting to listen ... "));
  state = lora.startReceive();
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    //while (true);
  }

  // NOTE: 'listen' mode will be disabled
  // automatically by calling any of the
  // following methods:
  //
  // lora.standby()
  // lora.sleep()
  // lora.transmit()
  // lora.receive()
  // lora.scanChannel()
  //
  // LoRa module will not receive any new
  // packets until 'listen' mode is re-enabled
  // by calling lora.startReceive()
}
int RFM96W_ScanChannel(){
    Serial.print("Scanning channel for LoRa preamble ... ");

  // start scanning current channel
  int state = lora.scanChannel();

  if(state == PREAMBLE_DETECTED) {
    // LoRa preamble was detected
    Serial.println(" detected preamble!");
    return 0;

  } 
  else if(state == CHANNEL_FREE) {
    // no preamble was detected, channel is free
    Serial.println(" channel is free!");
    return 1;
    }
  return -1;
}
void RFM96W_SendMessage(String rfm96Message){
  Serial.print(F("Sending packet ... "));
  while(1){
    if(RFM96W_ScanChannel()){
      break;
    }
  }
  
  int state = lora.transmit(rfm96Message);
  if (state == ERR_NONE) {
    // the packet was successfully transmitted
    Serial.println(F(" success!"));
    Serial.print("Message :");
    Serial.println(rfm96Message);

    // print measured data rate
    Serial.print(F("Datarate:\t"));
    Serial.print(lora.getDataRate());
    Serial.println(F(" bps"));

  } 
  else if (state == ERR_PACKET_TOO_LONG) {
    // the supplied packet was longer than 256 bytes
    Serial.println(F(" too long!"));

  } 
  else if (state == ERR_TX_TIMEOUT) {
    // timeout occurred while transmitting packet
    Serial.println(F(" timeout!"));
  }
}
void RFM96W_RxMode(){
  enableInterrupt = true;
    // set the function that will be called
  Serial.print(F("Starting to listen ... "));
 int state = lora.startReceive();
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
  }

  // NOTE: 'listen' mode will be disabled
  // automatically by calling any of the
  // following methods:
  //
  // lora.standby()
  // lora.sleep()
  // lora.transmit()
  // lora.receive()
  // lora.scanChannel()
  //
  // LoRa module will not receive any new
  // packets until 'listen' mode is re-enabled
  // by calling lora.startReceive()
}

String RFM96W_reciveHandler(int timeout){
        if(receivedFlag) {
        // disable the interrupt service routine while
        // processing the data
        enableInterrupt = false;
    
        // reset flag
        receivedFlag = false;
    
        // you can read received data as an Arduino String
        String str;
        int state = lora.readData(str);
        if (state == ERR_NONE) {
          // packet was successfully received
          Serial.println(F("Received packet!"));
    
          // print data of the packet
          Serial.print(F("Data:\t\t\t"));
          Serial.println(str);
          str_rfm96wBuffer = str;
    
          // print RSSI (Received Signal Strength Indicator)
          Serial.print(F("RSSI:\t\t\t"));
          _g_rssi = lora.getRSSI();
          Serial.print(_g_rssi);
          Serial.println(F(" dBm"));
    
          // print SNR (Signal-to-Noise Ratio)
          Serial.print(F("SNR:\t\t\t"));
          _g_snr = lora.getSNR();
          Serial.print(_g_snr);
          Serial.println(F(" dB"));
    
          // print frequency error
          Serial.print(F("Frequency error:\t"));
          Serial.print(lora.getFrequencyError());
          Serial.println(F(" Hz"));
    
        } else if (state == ERR_CRC_MISMATCH) {
          // packet was received, but is malformed
          Serial.println(F("CRC error!"));
          str_rfm96wBuffer ="";
    
        }
    
        // we're ready to receive more packets,
        // enable interrupt service routine
          state = lora.startReceive();
      if (state == ERR_NONE) {
        Serial.println("success!");
      } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true);
      }
        enableInterrupt = true;
     }
     return str_rfm96wBuffer;
}
int RFM96W_checkInterruptFlag(){
  return receivedFlag;
}

void RFM96W_clearInterruptFlag(){
  receivedFlag = false;
}
int g_rssi(){
  return _g_rssi;
}
float g_snr(){
  return _g_snr;
  
}
