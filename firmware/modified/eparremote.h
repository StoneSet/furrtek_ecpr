#define PULLUPS 0b00001111

void com(uint8_t command);
void disp();
void cur_rel();
void cur_freq();
void cur_span();
void cur_code();
void cur_zone();
void wrt(const uint8_t *FlashLoc);
void EPAR_TX(uint8_t citycode, uint8_t zone, uint8_t relay, uint8_t state, uint8_t reverse, uint8_t rep);
void wrtr(char *cbuf);
void wrtc(char chr);
void Up();
void Down();
uint8_t hex(uint8_t num);

#define ZONE_A 0
#define ZONE_B 1
#define ZONE_C 2
#define ZONE_TP 3

#define RELAY1	1
#define RELAY2  0

#define OPEN 0
#define CLOSED 1

#define NORMAL 0
#define REVERSE 1

#define REPEAT 26

#define FREQ_MAX 40
#define FREQ_MIN 20

#define ECART_MAX 1
#define ECART_MIN 0.001

#define POWERDOWN 4

#define AD9850_W_CLK	PB0
#define AD9850_FQ_UD	PB1
#define AD9850_D7		PB2
#define AD9850_RESET	PB3

#define E	PC0
#define RS	PC1

enum MENUT {MENU_FREQ,MENU_SPAN,MENU_REL,MENU_CODE,MENU_ZONE,MENU_XMIT};

#define MENU_FIRST MENU_FREQ
#define MENU_LAST MENU_XMIT

const double power[6];

char test[8];

uint8_t editing,selected;
uint8_t reli,bitedit,digitedit;

uint8_t relay[2],menu;
double	fr;
uint8_t citycode;
uint8_t zone;
double ec;

uint8_t EEcode;
uint8_t EEzone;
uint8_t EEmenu;
uint32_t EEfr;
uint32_t EEec;
