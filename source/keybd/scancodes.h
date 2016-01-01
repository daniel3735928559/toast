#define ESC_KEY 0
#define BACKSP_KEY 0x8
#define TAB_KEY 0x9
#define ENTER_KEY -1
#define LCTRL_KEY -2
#define LSHIFT_KEY -3
#define RSHIFT_KEY -4
#define LALT_KEY -5
#define CAPS_KEY -6
#define F1_KEY 0
#define F2_KEY 0
#define F3_KEY 0
#define F4_KEY 0
#define F5_KEY 0
#define F6_KEY 0
#define F7_KEY 0
#define F8_KEY 0
#define F9_KEY 0
#define F10_KEY 0
#define NLOCK_KEY 0
#define SLOCK_KEY 0
#define HOME_KEY -7
#define UP_KEY -8
#define DOWN_KEY -9
#define RIGHT_KEY -10
#define LEFT_KEY -11
#define END_KEY -13
#define PGUP_KEY -14
#define PGDN_KEY -15
#define INS_KEY -16
#define DEL_KEY -17

s8 scancodes[84] = {
0,ESC_KEY,'1','2','3','4','5','6','7','8','9','0','-','=',BACKSP_KEY, //0xf
TAB_KEY,'q','w','e','r','t','y','u','i','o','p','[',']',ENTER_KEY,LCTRL_KEY,'a','s',//0x1f
'd','f','g','h','j','k','l',';','\'','`',LSHIFT_KEY,'\\','z','x','c','v',//0x2f
'b','n','m',',','.','/',RSHIFT_KEY,'*',LALT_KEY,' ',CAPS_KEY,F1_KEY,F2_KEY,F3_KEY,F4_KEY,F5_KEY,//0x3f
F6_KEY,F7_KEY,F8_KEY,F9_KEY,F10_KEY,NLOCK_KEY,SLOCK_KEY,HOME_KEY,UP_KEY,PGUP_KEY,'-',LEFT_KEY,'5',RIGHT_KEY,'+',END_KEY,//0x4f
DOWN_KEY,PGDN_KEY,INS_KEY,DEL_KEY};

s8 scancodes_shift[84] = {
0,ESC_KEY,'!','@','#','$','%','^','&','*','(',')','_','+',BACKSP_KEY, //0xf
TAB_KEY,'Q','W','E','R','T','Y','U','I','O','P','{','}',ENTER_KEY,LCTRL_KEY,'A','S',//0x1f
'D','F','G','H','J','K','L',':','\"','~',LSHIFT_KEY,'|','Z','X','C','V',//0x2f
'B','N','M','<','>','?',RSHIFT_KEY,'*',LALT_KEY,' ',CAPS_KEY,F1_KEY,F2_KEY,F3_KEY,F4_KEY,F5_KEY,//0x3f
F6_KEY,F7_KEY,F8_KEY,F9_KEY,F10_KEY,NLOCK_KEY,SLOCK_KEY,HOME_KEY,UP_KEY,PGUP_KEY,'-',LEFT_KEY,'5',RIGHT_KEY,'+',END_KEY,//0x4f
DOWN_KEY,PGDN_KEY,INS_KEY,DEL_KEY};
