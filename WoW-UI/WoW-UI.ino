/*
  此项目为 WouoUI 的精简和优化版本，又名 WouoUI - Lite_General_Pro_Plus_Ultra 
  
  与 WouoUI - Lite_General 相比，减少的功能和特性：

    * 主题切换：去除了白色主题遮罩
    * 列表循环：去除了选择框在头尾继续操作时，跳转至另一头的功能
    * 任意行高：去除了支持屏幕高度与行高度不能整除的特性，这个版本需要能够整除
    * 背景虚化：去除了弹窗背景虚化的可选项

  与 WouoUI - Lite_General 相比，增加和有区别的功能和特性有：

    * 展开列表动画从Y轴展开改为X轴排队进入
    * 进入更深层级时，选择框从头展开改为跳转
    * 增加选择时拖影，选择滚动越快拖影越长，XY轴都有拖影
    * 增加跳转时拖影，选择跳转越远拖影越长，XY轴都有拖影
    * 撞墙特性：滚动时当后续还有选择项时，有Y轴拖影，没有时没有Y轴拖影
  
  此项目使用的开源协议为GPL，以下简单解释权利和义务，详情请参考文档

    权利：

    * 不限制数量的复制
    * 不限制方式的传播
    * 允许增加，删除和修改代码
    * 允许收费，但以服务的形式

    义务：

    * 新项目也要开源，不能通过代码本身盈利
    * 新项目也要使用该许可证
    * 出售时要让买家知道可以免费获得

  请保留以下该信息以大致遵守开源协议

    * 作者：B站用户名：音游玩的人
    * 开源：https://github.com/RQNG/WoW-UI
*/
/************************************* 屏幕驱动 *************************************/

//分辨率128*128，硬件SPI接口

#include <U8g2lib.h>
#include <SPI.h>

#define   SCL   PA5
#define   SDA   PA7
#define   RES   U8X8_PIN_NONE
#define   DC    PB6
#define   CS    PB7

U8G2_SH1107_SEEED_128X128_F_4W_HW_SPI u8g2(U8G2_R0, CS, DC, RES);

/************************************* 定义页面 *************************************/

//目录
enum 
{
  M_WIN,
  M_SLEEP,
    M_MAIN,
      M_F0,
      M_EDITOR,
        M_EDIT_F0,
      M_SETTING,
        M_ABOUT,
};

//过场
enum
{
  S_LAYER_IN,
  S_LAYER_OUT,
  S_FADE,
  S_MENU,
};

//菜单
typedef struct MENU
{
  char *m_select;
} M_SELECT;

/*********************************** 定义列表内容 ***********************************/

M_SELECT main_menu[]
{
  {"[ Main ]"},
  {"- Fidget Toy"},
  {"- Func 1"},
  {"- Func 2"},
  {"- Func 3"},
  {"- Func 4"},
  {"- Func 5"},
  {"- Editor"},
  {"- Setting"},
};

M_SELECT editor_menu[]
{
  {"[ Editor ]"},
  {"- Edit Fidget Toy"},
  {"- Edit Func 1"},
  {"- Edit Func 2"},
  {"- Edit Func 3"},
  {"- Edit Func 4"},
  {"- Edit Func 5"},
};

M_SELECT edit_f0_menu[]
{
  {"[ Edit Fidget Toy ]"},
  {"~ Box X OS"},
  {"~ Box Y OS"},
  {"~ Box Ani"},
};

M_SELECT setting_menu[]
{
  {"[ Setting ]"},
  {"~ Disp Bri"},
  {"~ Box X OS"},
  {"~ Box Y OS"},
  {"~ Win Y OS"},
  {"~ List Ani"},
  {"~ Win Ani"},
  {"~ Fade Ani"},
  {"~ Btn SPT"},
  {"~ Btn LPT"},
  {"+ Come Fm Scr"},
  {"+ Knob Rot Dir"},
  {"- [ About ]"},
};

M_SELECT about_menu[]
{
  {"[ WoW-UI ]"},
  {"- Version: v1.0"},
  {"- Creator: RQNG"},
  {"- Bili UID: 9182439"}, 
};

/************************************* 页面变量 *************************************/

//UI变量
#define   UI_DISP_H           128
#define   UI_DISP_W           128
#define   UI_DEPTH            10   
#define   UI_PARAM            11  
enum 
{
  DISP_BRI,
  BOX_X_OS,
  BOX_Y_OS,
  WIN_Y_OS,
  LIST_ANI,
  WIN_ANI,
  FADE_ANI,
  BTN_SPT,
  BTN_LPT,
  COME_SCR,
  KNOB_DIR,
};
struct 
{
  bool      init_flag;
  bool      oper_flag;

  uint16_t  buf_len;
  uint8_t   *buf_ptr;

  uint8_t   layer;
  uint8_t   fade = 1;

  uint8_t   index = M_SLEEP;
  uint8_t   state = S_MENU;

  uint8_t   select[UI_DEPTH];
  uint8_t   param[UI_PARAM];
} ui;

//列表变量
#define   LIST_FONT           u8g2_font_HelvetiPixel_tr 
#define   LIST_NUM            100
#define   LIST_LINE_H         16 
#define   LIST_TEXT_W         100 
#define   LIST_TEXT_H         8 
#define   LIST_TEXT_S         4
#define   LIST_BAR_W          3
#define   LIST_BOX_R          0.5f
struct
{
  uint8_t   select;

  int16_t   text_x_temp;
  int16_t   text_y_temp;
  int16_t   text_w_temp;

  float     text_x;
  float     text_x_trg;

  float     text_y;
  float     text_y_trg;

  float     box_y;
  float     box_y_trg[UI_DEPTH];

  float     box_W;
  float     box_w;
  float     box_w_trg;

  float     box_H;
  float     box_h;
  float     box_h_trg;

  float     bar_h;
  float     bar_h_trg;

  uint8_t   num[LIST_NUM];
} list;

//选择框变量
#define   CB_U                2
#define   CB_W                12
#define   CB_H                12
#define   CB_D                2
struct
{
  uint8_t *v;
  uint8_t *m;
  uint8_t *s;
  uint8_t *s_p;
} check_box;

//弹窗变量
#define   WIN_FONT            u8g2_font_HelvetiPixel_tr 
#define   WIN_H               27
#define   WIN_W               90
#define   WIN_TITLE_W         20
#define   WIN_TITLE_H         8 
#define   WIN_TITLE_S         5
#define   WIN_VALUE_S         67
#define   WIN_BAR_H           3 
struct
{
  char      title[WIN_TITLE_W];
  uint8_t   *value;
  uint8_t   max;
  uint8_t   min;
  uint8_t   step;
  uint8_t   index;
  MENU      *bg;

  bool      init_flag;
  bool      exit_flag;
  bool      oper_flag;

  float     box_x;

  float     box_y;
  float     box_y_trg;

  float     box_w;
  float     box_w_trg;

  float     box_H;
  float     box_h;
  float     box_h_trg;
  
  float     bar_x;
  float     bar_x_trg;
} win;

/********************************** 自定义功能变量 **********************************/

//解压玩具变量
#define   F0_PARAM            3
#define   F0_BOX_H            20
#define   F0_BOX_W            20
#define   F0_POS_N            4
#define   F0_BOX_X            0
#define   F0_BOX_Y            1
#define   F0_BOX_R            2
enum 
{
  F0_X_OS,
  F0_Y_OS,
  F0_ANI
};
struct 
{
  float     box_x;
  float     box_x_trg;
  
  float     box_y; 
  float     box_y_trg;

  float     box_W = F0_BOX_W;
  float     box_w;
  float     box_w_trg;

  float     box_H = F0_BOX_H;
  float     box_h;
  float     box_h_trg;

  int8_t    select;
  uint8_t   pos[F0_POS_N][2] = {{                   0,                    0},   //pos1
                                {UI_DISP_W - F0_BOX_W,                    0},   //pos2
                                {UI_DISP_W - F0_BOX_W, UI_DISP_H - F0_BOX_H},   //pos3
                                {                   0, UI_DISP_H - F0_BOX_H}};  //pos4

  uint8_t   param[F0_PARAM];
} f0;

/************************************* 断电保存 *************************************/

#include <EEPROM.h>

//EEPROM变量
#define   EEPROM_CHECK        11
struct
{
  bool    init;
  bool    change;
  int     address;
  uint8_t check;
  uint8_t check_param[EEPROM_CHECK] = { 'a', 'b', 'c', 'd', 'e', 'f','g', 'h', 'i', 'j', 'k' }; 
} eeprom;

//EEPROM写数据，回到睡眠时执行一遍
void eeprom_write_all_data()
{
  eeprom.address = 0;
  for (uint8_t i = 0; i < EEPROM_CHECK; ++i)    EEPROM.write(eeprom.address + i, eeprom.check_param[i]);  eeprom.address += EEPROM_CHECK;
  for (uint8_t i = 0; i < UI_PARAM; ++i)        EEPROM.write(eeprom.address + i, ui.param[i]);            eeprom.address += UI_PARAM;
  for (uint8_t i = 0; i < F0_PARAM; ++i)        EEPROM.write(eeprom.address + i, f0.param[i]);            eeprom.address += F0_PARAM;
}

//EEPROM读数据，开机初始化时执行一遍
void eeprom_read_all_data()
{
  eeprom.address = EEPROM_CHECK;   
  for (uint8_t i = 0; i < UI_PARAM; ++i)        ui.param[i]   = EEPROM.read(eeprom.address + i);          eeprom.address += UI_PARAM;
  for (uint8_t i = 0; i < F0_PARAM; ++i)        f0.param[i]   = EEPROM.read(eeprom.address + i);          eeprom.address += F0_PARAM;
}

//开机检查是否已经修改过，没修改过则跳过读配置步骤，用默认设置
void eeprom_init()
{
  eeprom.check = 0;
  eeprom.address = 0; for (uint8_t i = 0; i < EEPROM_CHECK; ++i)  if (EEPROM.read(eeprom.address + i) != eeprom.check_param[i])  eeprom.check ++;
  if (eeprom.check <= 1) eeprom_read_all_data();  //允许一位误码
  else device_init();
}

/************************************* 旋钮相关 *************************************/

//旋钮引脚
#define   AIO   PB12
#define   BIO   PB13
#define   SW    PB14

//按键ID
#define   BTN_ID_CC           0 
#define   BTN_ID_CW           1 
#define   BTN_ID_SP           2 
#define   BTN_ID_LP           3 

//按键变量
struct
{
  uint8_t   id;
  bool      flag;
  bool      pressed;
  bool      CW_1;
  bool      CW_2;
  bool      val;
  bool      val_last;  
  bool      alv;  
  bool      blv;
  long      count;
} volatile btn;

void knob_inter() 
{
  btn.alv = digitalRead(AIO);
  btn.blv = digitalRead(BIO);
  if (!btn.flag && btn.alv == LOW) 
  {
    btn.CW_1 = btn.blv;
    btn.flag = true;
  }
  if (btn.flag && btn.alv) 
  {
    btn.CW_2 = !btn.blv;
    if (btn.CW_1 && btn.CW_2)
     {
      btn.id = ui.param[KNOB_DIR];
      btn.pressed = true;
    }
    if (btn.CW_1 == false && btn.CW_2 == false) 
    {
      btn.id = !ui.param[KNOB_DIR];
      btn.pressed = true;
    }
    btn.flag = false;
  }
}

void btn_scan() 
{
  btn.val = digitalRead(SW);
  if (btn.val != btn.val_last)
  {
    btn.val_last = btn.val;
    delay(ui.param[BTN_SPT]);
    btn.val = digitalRead(SW);
    if (btn.val == LOW)
    {
      btn.pressed = true;
      btn.count = 0;
      while (!digitalRead(SW))
      {
        btn.count++;
        delay(1);
      }
      if (btn.count < ui.param[BTN_LPT])  btn.id = BTN_ID_SP;
      else  btn.id = BTN_ID_LP;
    }
  }
}

void btn_init() 
{
  pinMode(AIO, INPUT);
  pinMode(BIO, INPUT);
  pinMode(SW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(AIO), knob_inter, CHANGE);
}

/************************************ 初始化函数 ***********************************/

//单选框初始化
void check_box_s_init(uint8_t *param, uint8_t *param_p)
{
  check_box.s = param;
  check_box.s_p = param_p;
}

//多选框初始化
void check_box_m_init(uint8_t *param)
{
  check_box.m = param;
}

//数值初始化
void check_box_v_init(uint8_t *param)
{
  check_box.v = param;
}

//单选框处理函数
void check_box_s_select(uint8_t val, uint8_t pos)
{
  *check_box.s = val;
  *check_box.s_p = pos;
  eeprom.change = true;
}

//多选框处理函数
void check_box_m_select(uint8_t param)
{
  check_box.m[param] = !check_box.m[param];
  eeprom.change = true;
}

//弹窗数值初始化
void win_init(char title[], uint8_t *value, uint8_t max, uint8_t min, uint8_t step, uint8_t index, MENU *bg)
{
  ui.index = M_WIN;
  ui.state = S_MENU;
  strcpy(win.title, title);
  win.value = value;
  win.max = max;
  win.min = min;
  win.step = step;
  win.index = index;
  win.bg = bg;
}

/*********************************** UI 初始化函数 *********************************/

//列表行数
void list_init()
{
  list.num[M_MAIN]      = sizeof( main_menu     )   / sizeof(M_SELECT);
  list.num[M_EDITOR]    = sizeof( editor_menu   )   / sizeof(M_SELECT);
  list.num[M_EDIT_F0]   = sizeof( edit_f0_menu  )   / sizeof(M_SELECT);
  list.num[M_SETTING]   = sizeof( setting_menu  )   / sizeof(M_SELECT);
  list.num[M_ABOUT]     = sizeof( about_menu    )   / sizeof(M_SELECT);   
}

//默认设置
void device_init()
{
  //UI初始化
  ui.param[DISP_BRI]  = 255;
  ui.param[BOX_X_OS]  = 10;
  ui.param[BOX_Y_OS]  = 10;
  ui.param[WIN_Y_OS]  = 30;
  ui.param[LIST_ANI]  = 200;
  ui.param[WIN_ANI]   = 50;
  ui.param[FADE_ANI]  = 30;
  ui.param[BTN_SPT]   = 50;
  ui.param[BTN_LPT]   = 255;
  ui.param[COME_SCR]  = 0;
  ui.param[KNOB_DIR]  = 1;

  //功能初始化
  f0.param[F0_X_OS]  = 40;
  f0.param[F0_Y_OS]  = 40;
  f0.param[F0_ANI]   = 100;
}

/********************************* 分页面初始化函数 ********************************/

//F0编辑页初始化
void edit_f0_init()
{
  check_box_v_init(f0.param);
}

//设置页初始化
void setting_init()
{
  check_box_v_init(ui.param);
  check_box_m_init(ui.param);
}

/********************************** 通用初始化函数 *********************************/

/*
  页面层级管理逻辑是，把所有页面都先当作列表类初始化，不是列表类的按标签运行对应的初始化函数
  这样做会浪费一些资源，但跳转页面时只需要考虑页面层级，逻辑上更清晰
*/

//进入更深层级时的初始化
void layer_in()
{
  ui.layer ++;
  ui.state = S_FADE;
  ui.init_flag = false;
  ui.select[ui.layer] = 0;
  list.box_y_trg[ui.layer] = 0;
  list.box_w_trg += ui.param[BOX_X_OS] * (list.box_y_trg[ui.layer - 1] / LIST_LINE_H);
  list.box_h_trg += ui.param[BOX_Y_OS] * (list.box_y_trg[ui.layer - 1] / LIST_LINE_H);
  switch (ui.index)
  {   
    case M_EDIT_F0: edit_f0_init();   break;
    case M_SETTING: setting_init();   break;
  }
}

//进入更浅层级时的初始化
void layer_out()
{
  ui.layer --;
  ui.state = S_FADE;
  ui.init_flag = false;
  list.box_w_trg += ui.param[BOX_X_OS] * abs((list.box_y_trg[ui.layer] - list.box_y_trg[ui.layer + 1]) / LIST_LINE_H);
  list.box_h_trg += ui.param[BOX_Y_OS] * abs((list.box_y_trg[ui.layer] - list.box_y_trg[ui.layer + 1]) / LIST_LINE_H);
}

/************************************* 动画函数 *************************************/

//消失动画
void fade()
{
  delay(ui.param[FADE_ANI]);
  switch (ui.fade)
  {
    case 1: for (uint16_t i = 0; i < ui.buf_len; ++i)  if (i % 2 != 0) ui.buf_ptr[i] = ui.buf_ptr[i] & 0xAA; break;
    case 2: for (uint16_t i = 0; i < ui.buf_len; ++i)  if (i % 2 != 0) ui.buf_ptr[i] = ui.buf_ptr[i] & 0x00; break;
    case 3: for (uint16_t i = 0; i < ui.buf_len; ++i)  if (i % 2 == 0) ui.buf_ptr[i] = ui.buf_ptr[i] & 0x55; break;
    case 4: for (uint16_t i = 0; i < ui.buf_len; ++i)  if (i % 2 == 0) ui.buf_ptr[i] = ui.buf_ptr[i] & 0x00; break;
    default: ui.state = S_MENU; ui.fade = 0; break;
  }
  ui.fade++;
}

//渐近动画
void animation(float *a, float *a_trg, uint8_t n)
{
  if (*a != *a_trg)
  {
    if (fabs(*a - *a_trg) < 0.15f) *a = *a_trg;
    else *a += (*a_trg - *a) / (n / 10.0f);
  }
}

/************************************* 显示函数 *************************************/

/*********************************** 通用显示函数 ***********************************/

//绘制行末尾元素
void list_draw_val(int n) { u8g2.setCursor  (list.text_w_temp, LIST_TEXT_H + LIST_TEXT_S + list.text_y_temp); u8g2.print(check_box.v[n - 1]); }                 //数值
void list_draw_cbf()      { u8g2.drawRFrame (list.text_w_temp, CB_U + list.text_y_temp, CB_W, CB_H, 0.5f); }                                                    //外框
void list_draw_cbd()      { u8g2.drawBox    (list.text_w_temp + CB_D + 1, CB_U + CB_D + 1 + list.text_y_temp, CB_W - (CB_D + 1) * 2, CB_H - (CB_D + 1) * 2); }  //外框里面的点

//列表类页面通用显示函数
void list_show(uint8_t ui_index, struct MENU arr[])
{
  //在初始化时更新一次的参数
  if (!ui.init_flag)
  {
    ui.init_flag = true;
    ui.oper_flag = true;
    list.select = ui.select[ui.layer];
    list.text_x = - UI_DISP_W;
    list.text_y = list.box_y_trg[ui.layer] - LIST_LINE_H * ui.select[ui.layer];
    list.text_y_trg = list.text_y;
    list.box_H = LIST_LINE_H;
    u8g2.setFont(LIST_FONT);
    u8g2.setDrawColor(2);
  }

  //在每次操作后都会更新的参数
  if (ui.oper_flag)
  {
    ui.oper_flag = false;
    list.box_W = u8g2.getUTF8Width(arr[ui.select[ui.layer]].m_select) + LIST_TEXT_S * 2;
    list.box_w_trg += ui.param[BOX_X_OS];
    list.bar_h_trg = ceil((ui.select[ui.layer]) * ((float)UI_DISP_H / (list.num[ui_index] - 1)));
  }

  //计算动画过渡值
  animation(&list.text_x, &list.text_x_trg, ui.param[LIST_ANI]);
  animation(&list.text_y, &list.text_y_trg, ui.param[LIST_ANI]);
  animation(&list.box_y, &list.box_y_trg[ui.layer], ui.param[LIST_ANI]);
  animation(&list.box_w, &list.box_w_trg, ui.param[LIST_ANI]);
  animation(&list.box_w_trg, &list.box_W, ui.param[LIST_ANI]);
  animation(&list.box_h, &list.box_h_trg, ui.param[LIST_ANI]);
  animation(&list.box_h_trg, &list.box_H, ui.param[LIST_ANI]);
  animation(&list.bar_h, &list.bar_h_trg, ui.param[LIST_ANI]);

  //绘制列表文字和行末尾元素
  for (int i = 0; i < list.num[ui_index]; ++ i)
  {
    switch (ui.param[COME_SCR])
    {
      case 0: list.text_x_temp = list.text_x * (abs(list.select - i) + 1); break;
      case 1: list.text_x_temp = list.text_x * (i + 1); break;
    }
    list.text_y_temp = list.text_y + LIST_LINE_H * i;
    list.text_w_temp = list.text_x_temp + LIST_TEXT_W;
    u8g2.setCursor(list.text_x_temp + LIST_TEXT_S, LIST_TEXT_S + LIST_TEXT_H + list.text_y_temp);
    u8g2.print(arr[i].m_select);
    switch (arr[i].m_select[0])
    {
      case '~': list_draw_val(i); break;
      case '+': list_draw_cbf();  if (check_box.m[i - 1] == 1)  list_draw_cbd();  break;
      case '=': list_draw_cbf();  if (*check_box.s_p == i)      list_draw_cbd();  break;
    }
  }

  //绘制进度条和选择框
  u8g2.drawBox(UI_DISP_W - LIST_BAR_W, 0, LIST_BAR_W, list.bar_h);
  u8g2.drawRBox(0, list.box_y - (list.box_h - LIST_LINE_H) / 2, list.box_w, list.box_h, LIST_BOX_R);
}

//弹窗通用显示函数
void win_show()
{
  //在进场时更新的参数
  if (!win.init_flag)
  {
    win.init_flag = true;
    win.oper_flag = true;
    win.box_y = 0;
    win.box_y_trg = (UI_DISP_H - WIN_H) / 2;
    win.box_w = UI_DISP_W;
    win.box_w_trg = WIN_W;
    win.box_H = WIN_H;
    win.box_h = 0;
    win.box_h_trg = win.box_H + ui.param[WIN_Y_OS];
    win.bar_x = 0;
    u8g2.setFont(WIN_FONT);
  }

  //在离场时更新的参数
  if (win.exit_flag)
  {
    win.box_H = 0; 
    win.box_y_trg = 0;
    win.box_w_trg = UI_DISP_W;
    if (!win.box_y)
    {
      ui.index = win.index;
      win.init_flag = false;
      win.exit_flag = false;
    }
  }

  //在每次操作后都会更新的参数
  if (win.oper_flag)
  {
    win.oper_flag = false;
    win.bar_x_trg = (float)(*win.value - win.min) / (float)(win.max - win.min) * (win.box_w_trg - 2 * WIN_TITLE_S);
  }

  //计算动画过渡值
  animation(&win.box_y, &win.box_y_trg, ui.param[WIN_ANI]);
  animation(&win.box_w, &win.box_w_trg, ui.param[WIN_ANI]);
  animation(&win.box_h, &win.box_h_trg, ui.param[WIN_ANI]);
  animation(&win.box_h_trg, &win.box_H, ui.param[WIN_ANI]);
  animation(&win.bar_x, &win.bar_x_trg, ui.param[WIN_ANI]);

  //绘制背景列表和窗口
  list_show(win.index, win.bg);
  win.box_x = (UI_DISP_W - win.box_w) / 2;
  u8g2.setDrawColor(0); u8g2.drawBox  (win.box_x, win.box_y, win.box_w, win.box_h);  //绘制外框背景
  u8g2.setDrawColor(2); u8g2.drawFrame(win.box_x, win.box_y, win.box_w, win.box_h);  //绘制外框描边
  if (win.box_h > (WIN_TITLE_H + 2 * WIN_TITLE_S))
  {
    u8g2.setCursor(win.box_x + WIN_VALUE_S, win.box_y + WIN_TITLE_S + WIN_TITLE_H); u8g2.print(*win.value);             //绘制数值
    u8g2.setCursor(win.box_x + WIN_TITLE_S, win.box_y + WIN_TITLE_S + WIN_TITLE_H); u8g2.print(win.title);              //绘制标题
    u8g2.drawBox  (win.box_x + WIN_TITLE_S, win.box_y + win.box_h - WIN_TITLE_S - WIN_BAR_H - 1, win.bar_x, WIN_BAR_H); //绘制进度条
  }

  //需要在窗口修改参数时立即见效的函数
  if (!strcmp(win.title, "Disp Bri")) u8g2.setContrast(ui.param[DISP_BRI]);
}

/********************************** 分页面显示函数 **********************************/

void  f0_show()
{
  //在进场时更新的参数
  if (!ui.init_flag)
  {
    ui.init_flag = true;

    //进场时元素从屏幕外入场
    f0.box_x = - F0_BOX_W;
    f0.box_y = - F0_BOX_H;

    //进场时元素移动到初始位置
    f0.box_x_trg = 0;
    f0.box_y_trg = 0;

    //进场时元素效果
    f0.box_w_trg += f0.param[F0_X_OS];
    f0.box_h_trg += f0.param[F0_Y_OS];

    //其它初始化
    u8g2.setDrawColor(1);
  }

  //在每次操作后都会更新的参数
  if (ui.oper_flag)
  {
    ui.oper_flag = false;
    if (f0.box_x != f0.box_x_trg) f0.box_w_trg += f0.param[F0_X_OS];
    if (f0.box_y != f0.box_y_trg) f0.box_h_trg += f0.param[F0_Y_OS];
  }

  //计算动画过渡值
  animation(&f0.box_x, &f0.box_x_trg, f0.param[F0_ANI]);
  animation(&f0.box_y, &f0.box_y_trg, f0.param[F0_ANI]);
  animation(&f0.box_w, &f0.box_w_trg, f0.param[F0_ANI]);
  animation(&f0.box_w_trg, &f0.box_W, f0.param[F0_ANI]);
  animation(&f0.box_h, &f0.box_h_trg, f0.param[F0_ANI]);
  animation(&f0.box_h_trg, &f0.box_H, f0.param[F0_ANI]);

  //绘制元素
  u8g2.drawRBox((int16_t)f0.box_x, (int16_t)f0.box_y, f0.box_w, f0.box_h, F0_BOX_R);
}

/************************************* 处理函数 *************************************/

/*********************************** 通用处理函数 ***********************************/

//列表旋钮通用处理函数
void list_rot_sw()
{
  ui.oper_flag = true;
  switch (btn.id)
  {
    case BTN_ID_CC:
      if (ui.select[ui.layer] == 0) break;
      if (list.box_y_trg[ui.layer] == 0) list.text_y_trg += LIST_LINE_H;
      else list.box_y_trg[ui.layer] -= LIST_LINE_H;
      list.box_h_trg += ui.param[BOX_Y_OS];
      ui.select[ui.layer] -= 1;
      break;

    case BTN_ID_CW:
      if (ui.select[ui.layer] == (list.num[ui.index] - 1)) break;
      if (list.box_y_trg[ui.layer] == (UI_DISP_H - LIST_LINE_H)) list.text_y_trg -= LIST_LINE_H;
      else list.box_y_trg[ui.layer] += LIST_LINE_H;
      list.box_h_trg += ui.param[BOX_Y_OS];
      ui.select[ui.layer] += 1;
      break;
  }
}

//列表通用处理函数
void list_proc(struct MENU arr[], void (*Func)())
{
  list_show(ui.index, arr); 
  if (btn.pressed) 
  { 
    btn.pressed = false; 
    switch (btn.id) 
    { 
      case BTN_ID_CW: case BTN_ID_CC: list_rot_sw(); break; 
      case BTN_ID_LP: ui.select[ui.layer] = 0; 
      case BTN_ID_SP: Func();
    }
  }
}

//弹窗通用处理函数
void win_proc()
{
  win_show();
  if (btn.pressed && win.box_y == win.box_y_trg && win.box_y_trg != 0)
  {
    btn.pressed = false;
    win.oper_flag = true;
    switch (btn.id)
    {
      case BTN_ID_CW: if (*win.value < win.max) *win.value += win.step;  eeprom.change = true;  break;
      case BTN_ID_CC: if (*win.value > win.min) *win.value -= win.step;  eeprom.change = true;  break;  
      case BTN_ID_LP: case BTN_ID_SP: win.exit_flag = true;  break;
    }
  }
}

/********************************** 分页面处理函数 **********************************/

//睡眠页处理函数
void sleep_proc()
{
  if (!ui.init_flag)
  {
    ui.init_flag = true;
    u8g2.setPowerSave(1);
    if (eeprom.change)
    {
      eeprom_write_all_data();
      eeprom.change = false;
    }
  }
  btn_scan();
  if (btn.pressed) 
  { 
    btn.pressed = false; 
    switch (btn.id) 
    {
      case BTN_ID_CW: break;  //顺时针旋转执行的函数
      case BTN_ID_CC: break;  //逆时针旋转执行的函数
      case BTN_ID_SP: break;  //短按执行的函数
      case BTN_ID_LP: 
      
        ui.index = M_MAIN;
        ui.state = S_LAYER_IN;

        list.box_y = 0;
        list.box_w = 0;
        list.box_w_trg = 0;
        list.box_h = 0;
        list.box_h_trg = 0;
        list.bar_h = 0;
        u8g2.setPowerSave(0);
        break;
    }
  }
}

//主菜单处理函数
void main_proc()
{
  switch (ui.select[ui.layer]) 
  {
    case 0:   ui.index = M_SLEEP;   ui.state = S_LAYER_OUT; break;
    case 1:   ui.index = M_F0;      ui.state = S_LAYER_IN;  break;
    case 7:   ui.index = M_EDITOR;  ui.state = S_LAYER_IN;  break;
    case 8:   ui.index = M_SETTING; ui.state = S_LAYER_IN;  break;
  }
}

//解压玩具处理函数
void f0_proc()
{
  f0_show();
  btn_scan();
  if (btn.pressed) 
  { 
    btn.pressed = false; 
    ui.oper_flag = true;
    switch (btn.id) 
    {
      case BTN_ID_CW: f0.select ++; if (f0.select > F0_POS_N - 1) f0.select = 0;  break;
      case BTN_ID_CC: f0.select --; if (f0.select < 0) f0.select = F0_POS_N - 1;  break;
      case BTN_ID_SP: case BTN_ID_LP: ui.index = M_MAIN;  ui.state = S_LAYER_OUT; break;
    }
    f0.box_x_trg = f0.pos[f0.select][F0_BOX_X];
    f0.box_y_trg = f0.pos[f0.select][F0_BOX_Y];
  }
}

//编辑器菜单处理函数
void editor_proc()
{
  switch (ui.select[ui.layer]) 
  {
    case 0:   ui.index = M_MAIN;    ui.state = S_LAYER_OUT; break;
    case 1:   ui.index = M_EDIT_F0; ui.state = S_LAYER_IN;  break;
  }
}

//编辑解压玩具菜单
void edit_f0_proc()
{
  switch (ui.select[ui.layer]) 
  {
    case 0:   ui.index = M_EDITOR;  ui.state = S_LAYER_OUT; break;
    case 1:   win_init("F0 X OS",  &f0.param[F0_X_OS],   100,  0,  1, ui.index, edit_f0_menu);  break;
    case 2:   win_init("F0 Y OS",  &f0.param[F0_Y_OS],   100,  0,  1, ui.index, edit_f0_menu);  break;
    case 3:   win_init("F0 Ani",   &f0.param[F0_ANI],    255,  0,  1, ui.index, edit_f0_menu);  break;
  }
}

//设置菜单处理函数
void setting_proc()
{
  switch (ui.select[ui.layer]) 
  {
    //返回
    case 0:   ui.index = M_MAIN;    ui.state = S_LAYER_OUT; break;
        
    //弹出窗口
    case 1:   win_init("Disp Bri", &ui.param[DISP_BRI],  255,  0,  5, ui.index, setting_menu);  break;
    case 2:   win_init("Box X OS", &ui.param[BOX_X_OS],   40,  0,  1, ui.index, setting_menu);  break;
    case 3:   win_init("Box Y OS", &ui.param[BOX_Y_OS],   40,  0,  1, ui.index, setting_menu);  break;
    case 4:   win_init("Win Y OS", &ui.param[WIN_Y_OS],   40,  0,  1, ui.index, setting_menu);  break;
    case 5:   win_init("List Ani", &ui.param[LIST_ANI],  255, 20,  1, ui.index, setting_menu);  break;
    case 6:   win_init("Win Ani",  &ui.param[WIN_ANI],   255, 20,  1, ui.index, setting_menu);  break;
    case 7:   win_init("Fade Ani", &ui.param[FADE_ANI],  255,  0,  1, ui.index, setting_menu);  break;
    case 8:   win_init("Btn SPT",  &ui.param[BTN_SPT],   255,  0,  1, ui.index, setting_menu);  break;
    case 9:   win_init("Btn LPT",  &ui.param[BTN_LPT],   255,  0,  1, ui.index, setting_menu);  break;

    //多选框
    case 10:  check_box_m_select( COME_SCR );  break;
    case 11:  check_box_m_select( KNOB_DIR );  break;

    //关于本机
    case 12:  ui.index = M_ABOUT;   ui.state = S_LAYER_IN;  break;
  }
}

//关于本机
void about_proc()
{
  switch (ui.select[ui.layer]) 
  {
    case 0:   ui.index = M_SETTING; ui.state = S_LAYER_OUT; break;
  }
}

//总进程
void ui_proc()
{
  u8g2.sendBuffer();
  switch (ui.state)
  {
    case S_LAYER_IN:    layer_in();           break;
    case S_LAYER_OUT:   layer_out();          break;
    case S_FADE:        fade();               break;
    case S_MENU:        u8g2.clearBuffer();   switch (ui.index)
    {
      case M_WIN:             win_proc();                                         break;
      case M_SLEEP:           sleep_proc();                                       break;
        case M_MAIN:          list_proc(  main_menu,          main_proc     );    break;
          case M_F0:          f0_proc();                                          break;
          case M_EDITOR:      list_proc(  editor_menu,        editor_proc   );    break;
            case M_EDIT_F0:   list_proc(  edit_f0_menu,       edit_f0_proc  );    break;
          case M_SETTING:     list_proc(  setting_menu,       setting_proc  );    break;
            case M_ABOUT:     list_proc(  about_menu,         about_proc    );    break;
    }
  }
}

//OLED初始化函数
void oled_init()
{
  u8g2.setBusClock(10000000);
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setContrast(ui.param[DISP_BRI]);
  ui.buf_ptr = u8g2.getBufferPtr();
  ui.buf_len = 8 * u8g2.getBufferTileHeight() * u8g2.getBufferTileWidth();
}

void setup() 
{
  eeprom_init();
  list_init();
  oled_init();
  btn_init();
}

void loop() 
{
  btn_scan();
  ui_proc();
}


