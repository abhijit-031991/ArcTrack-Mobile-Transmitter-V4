const char* tagID = "ArcTrack-Yagi-120";

#define SCK     PA5    // GPIO5  -- SX1278's SCK
#define MISO    PA6   // GPIO19 -- SX1278's MISO
#define MOSI    PA7   // GPIO27 -- SX1278's MOSI
#define SS      PA4   // GPIO18 -- SX1278's CS
#define RST     PA1   // GPIO14 -- SX1278's RESET
#define DI0     PA2   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define LED     PA10


#define PING_SIZE 14
#define DATA_SIZE 16
#define SETTING_SIZE 27
#define REQ_SIZE 3

struct data{
    uint32_t datetime;
    uint16_t locktime;
    float lat;
    float lng;
    byte hdop;
    byte id;
    }__attribute__((__packed__));

struct settings{
    uint16_t tag;
    int gpsFrq;
    int gpsTout;
    int hdop;
    int radioFrq;
    int startHour;
    int endHour;
    bool scheduled;
}__attribute__((__packed__));

struct reqPing{
    uint16_t tag;
    byte request;
  }__attribute__((__packed__));;

struct resPing{
    uint16_t tag;
    byte resp;
  }__attribute__((__packed__));;

  struct longPing{
    uint16_t ta;    
    uint16_t cnt;
    float la;
    float ln;
    uint8_t devtyp;
    bool mortality;
  }__attribute__((__packed__));
    