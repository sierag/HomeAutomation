
// --------------------- Libraries ---------------------- //

#include <idDHT11.h>

#include <SPI.h>
#include <Ethernet.h>

// --------------------- idDTHT ------------------------- //

int idDHT11pin = 2; //Digital pin for comunications
int idDHT11intNumber = 0; //interrupt number (must be the one that use the previus defined pin (see table above)

//declaration
void dht11_wrapper(); // must be declared before the lib initialization

// Lib instantiate
idDHT11 DHT11(idDHT11pin,idDHT11intNumber,dht11_wrapper);

// --------------------- idDTHT ------------------------- //

String queryString;

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "weather.sierag.nl";    // name address for Google (using DNS)

//
int i = 10001;

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192,168,1,150);//192.168.1.111

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
}

void push() {

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }

  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println("GET /log.php?1=1" + queryString +  " HTTP/1.1");
    client.println("Host: weather.sierag.nl");
    client.println("Connection: close");
    client.println();
    queryString = "";    
  } 
  else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void loop() {

  // if there are incoming bytes available 
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    client.stop();
    // do nothing forevermore:
    // while(true);
  }
  
  if( i > 10000) {
    buildQueryString();
    push();
    i =0;
  } 
  i++;

  delay(1);
}

void buildQueryString() {
  
  int result = DHT11.acquireAndWait();
  switch (result)
  {
  case IDDHTLIB_OK: 
    Serial.println("OK"); 
    break;
  case IDDHTLIB_ERROR_CHECKSUM: 
    Serial.println("Error\n\r\tChecksum error"); 
    break;
  case IDDHTLIB_ERROR_TIMEOUT: 
    Serial.println("Error\n\r\tTime out error"); 
    break;
  case IDDHTLIB_ERROR_ACQUIRING: 
    Serial.println("Error\n\r\tAcquiring"); 
    break;
  case IDDHTLIB_ERROR_DELTA: 
    Serial.println("Error\n\r\tDelta time to small"); 
    break;
  case IDDHTLIB_ERROR_NOTSTARTED: 
    Serial.println("Error\n\r\tNot started"); 
    break;
  default: 
    Serial.println("Unknown error"); 
    break;
  }
  queryString += "&humidity=";
  char buffer[5];
  queryString +=  dtostrf(DHT11.getHumidity(), 1, 2, buffer);

  queryString +=  "&celsius=";;
  queryString +=  dtostrf(DHT11.getCelsius(), 1, 2, buffer);

  queryString +=  "&fahrenheit=";;
  queryString +=  dtostrf(DHT11.getFahrenheit(), 1, 2, buffer);

  queryString +=  "&kelvin=";;
  queryString +=  dtostrf(DHT11.getKelvin(), 1, 2, buffer);

  queryString +=  "&dewpoint=";;
  queryString +=  dtostrf(DHT11.getDewPoint(), 1, 2, buffer);

  queryString +=  "&dewpointslow=";;
  queryString +=  dtostrf(DHT11.getDewPointSlow(), 1, 2, buffer);

}

// This wrapper is in charge of calling 
// mus be defined like this for the lib work
void dht11_wrapper() {
  DHT11.isrCallback();
}
