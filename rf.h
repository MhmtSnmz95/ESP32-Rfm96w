#ifndef RF
#define RF

#if (ARDUINO >=100)
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

void RFM96W_Init();
int  RFM96W_ScanChannel();
void RFM96W_SendMessage(String rfm96Message);
void RFM96W_RxMode();
void setFlag();
String RFM96W_reciveHandler(int timeout);

int RFM96W_checkInterruptFlag();
void RFM96W_clearInterruptFlag();
int g_rssi();
float g_snr();
#endif
