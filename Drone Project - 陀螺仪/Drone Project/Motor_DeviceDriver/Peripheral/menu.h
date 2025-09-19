#ifndef _MENU_H
#define _MENU_H
	

typedef unsigned char uchar;

typedef struct
{
    unsigned char current;
    unsigned char left;
    unsigned char right;
    unsigned char lon_left;
    unsigned char lon_right;
    unsigned char two_short;
    unsigned char two_long;
	
    void (*current_operation)();
} key_table;

extern const key_table menu_table[];  // 只声明，不计算大小
extern unsigned char menu_index;

#define MENU_CURRENT  (menu_table[menu_index].current)

void op_main(void);
void op_settings(void);
void op_power(void);
void test1(void);
void test2(void);



// 提供一个函数声明
void menu_clamp(void);





//extern volatile  int dirty_flag;
//extern volatile  int Main_Menu_flag;
//extern volatile  int Second_Menu_flag;
//extern volatile  int Third_Menu_flag;
//extern volatile  int Second_Menu_Choice_flag;
//extern volatile  int Third_Menu_Choice_flag;
//extern volatile  int Frame_Format;
//extern volatile  int Mid_Flag;
//extern volatile  int Third_Menu_Format;


//void Main_Menu(void);
//void Second_Menu(void);
//void Third_Menu(void);
//void Flag_Limit(void);

#endif