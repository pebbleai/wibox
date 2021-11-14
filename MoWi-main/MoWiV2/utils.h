#include <pgmspace.h>

#define LED_BUILTIN 2

//RTC_DATA_ATTR short read_buffer_btcnt = 0;
//RTC_DATA_ATTR short write_buffer_btcnt = 0;

int read_buffer_btcnt, write_buffer_btcnt;


char* string2char(String command) {
  if (command.length() != 0) {
    char *p = const_cast<char*>(command.c_str());
    return p;
  }
}

void initializeBCBuffer()
{
  preferences.begin("cBuffer", false); // Creating a namespace with name cBuffer that has our key and values of the buffer
  int value = -1;
  for (int i = 0; i < CIRCULAR_BUFFER_SIZE; i++)
  {
    String key = "btcnt" + String(i);
    preferences.putInt(string2char(key), value);
  }
  preferences.end();
}

void writeBCbuffer(int val)
{
  preferences.begin("cBuffer", false); 
  read_buffer_btcnt = preferences.getInt("read", 0);
  write_buffer_btcnt = preferences.getInt("write", 0);
  preferences.end();

  int index = write_buffer_btcnt;
  String key = "btcnt" + String(index);
  preferences.begin("cBuffer", false);
  preferences.putInt(string2char(key), val);
  preferences.end();
  write_buffer_btcnt = (write_buffer_btcnt+1) % CIRCULAR_BUFFER_SIZE;

  preferences.begin("cBuffer", false); 
  preferences.putInt("read", read_buffer_btcnt);
  preferences.putInt("write", write_buffer_btcnt);
  preferences.end();
}

void publishBCBuffer()
{
  preferences.begin("cBuffer", false); 
  read_buffer_btcnt = preferences.getInt("read", 0);
  write_buffer_btcnt = preferences.getInt("write", 0);
  preferences.end();
  
  int read_index, bootValue;
  preferences.begin("cBuffer", false);
  while (read_buffer_btcnt != write_buffer_btcnt)
  {
    read_index = read_buffer_btcnt % CIRCULAR_BUFFER_SIZE;
    bootValue = preferences.getInt(string2char("btcnt"+String(read_index)), 0); // get boardId or if key doesn't exist set variable to 0
    publishBootCount(bootValue);
    read_buffer_btcnt = (read_buffer_btcnt+1) % CIRCULAR_BUFFER_SIZE;
  }
  preferences.end();

  preferences.begin("cBuffer", false); 
  preferences.putInt("read", read_buffer_btcnt);
  preferences.putInt("write", write_buffer_btcnt);
  preferences.end();
}


void delay1Second()
{
  int period = 1000;
  unsigned long time_now = 0;
  time_now = millis();
  while (millis() < time_now + period) {
    //wait approx. [period] ms
  }
}

void Read_DHT (DHT dht, float *humidity, float *temperature_c, float *temperature_f)
{
  delay(2000);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  *humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  *temperature_c = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  *temperature_f = dht.readTemperature(true);
}

void blink3time()
{
  //  Blue LED blinks when the esp32 wakes up
  pinMode(LED_BUILTIN, OUTPUT); // for builtin leds to blink on startup
  digitalWrite(LED_BUILTIN, HIGH);
  delay1Second();
  digitalWrite(LED_BUILTIN, LOW);
  delay1Second();
  digitalWrite(LED_BUILTIN, HIGH);
  delay1Second();
  digitalWrite(LED_BUILTIN, LOW);
  delay1Second();
  digitalWrite(LED_BUILTIN, HIGH);
  delay1Second();
  digitalWrite(LED_BUILTIN, LOW);
  delay1Second();
}
