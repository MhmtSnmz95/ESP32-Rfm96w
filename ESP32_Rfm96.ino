#include "rf.h"
String _msg;
int _lenStr = 0;
char message[300] = "";
int ClockNex=8;
String A="A";
String K="K";

void Clock(char *Buffer)//Ana karttan gelen Clock bilgisi 
{
  char Bufstr[50];
  String Buffstr;
   if(Buffer[0]=='S' && Buffer[1]=='a'&& Buffer[2]=='a' && Buffer[3]=='t' && Buffer[4]==':') //
   {
    int i=0;
    for(i=5;i<strlen(Buffer);i++)
    {
      Bufstr[i-5]=Buffer[i];
      }
      Buffstr=Bufstr;  // char dizisini string degiskenine at
      ClockNex=Buffstr.toInt();
      Serial.print("Saat:"); //Debug
      Serial.println(ClockNex);//Debug
   }
}
void SendMotorDurum(int channel,String durum) 
{
  char SendBuff[50];
  sprintf(SendBuff,"MPOL%d:%s",channel,durum); //
  RFM96W_SendMessage(SendBuff); //RF uzerinde yolla
  RFM96W_RxMode(); //RX moda cevir
  RFM96W_clearInterruptFlag(); //interruptlari temizle
}

void setup() {
 RFM96W_Init(); //RF kurulumu
  RFM96W_RxMode(); //RX modda ac
Serial.println("RF Start!");
}

void loop() {
  if (RFM96W_checkInterruptFlag()) { //RF'den veri geldimi 
      _msg = RFM96W_reciveHandler(1000); //butun veriyi al
      _lenStr = _msg.length() + 1; //mesaj boyutunu bul 
      _msg.toCharArray(message, _lenStr); //Stringi char dizisine aktar
      Clock(message); //Gelen saat bilgisi
    }
SendMotorDurum(1,A); delay(1000);
SendMotorDurum(2,K); delay(1000);
}
