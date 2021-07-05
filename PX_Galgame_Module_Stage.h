//////////////////////////////////////////////////////////////////////////
//Galgame Stage scene
//PainterEngine x Unique hackday
//Codeby 不如学成狗 all rights reserved
//Power by PainterEngine
//2021-6-13
//////////////////////////////////////////////////////////////////////////

#ifndef PX_GALGAME_MODULE_STAGE
#define PX_GALGAME_MODULE_STAGE

#include "PainterEngine_Startup.h"

#define PX_GALGAME_MODULE_STAGE_EXIT 0x00020000

#define PX_GALGAME_MODULE_STAGE_STATE_DISPLAY_DURATION 1500 //开场图片渐变
#define PX_GALGAME_MODULE_STAGE_STATE_ENTER_DURATION 500 //右边人物入
#define PX_GALGAME_MODULE_STAGE_STATE_OPTIONAL_DURATION 1000//选择卡牌进入右出
#define PX_GALGAME_MODULE_STAGE_STATE_EXIT_DURATION 1000 //退场图片渐变

typedef enum
{
	PX_Galgame_Module_Stage_State_backgrounddisplay=0,//显示背景
	PX_Galgame_Module_Stage_State_entry,//人物入场
	PX_Galgame_Module_Stage_State_content,//显示文本
	PX_Galgame_Module_Stage_State_optionentry,//卡牌入场
	PX_Galgame_Module_Stage_State_optional,//选择状态
	PX_Galgame_Module_Stage_State_optionexit,//卡牌离场
	PX_Galgame_Module_Stage_State_end,//
	PX_Galgame_Module_Stage_State_exit,//渐变退出
}PX_Galgame_Module_Stage_State;

typedef struct  
{
	px_texture image;
	px_char animtion[16];
	px_int score;
	px_char c_content[128];
	PX_Object *pushButton;
}PX_Galgame_Module_Stage_Option;//选项结构体

typedef struct  
{
	px_char t_content[128];
	px_char animation[16];
}PX_Galgame_Module_Stage_Talk;//文本结构体

typedef struct
{
	PX_Runtime *pRuntime;
	PX_Json *pJsonFramework;
	PX_FontModule *fm;

	px_int time;
	px_int actorx;
	px_texture background;//背景图
	px_texture left_msgbox;//左边的消息框图
	px_texture right_msgbox;//右边的消息框图
	px_texture actor;//左边人的图片
	px_int livex;
	PX_LiveFramework live;//live framework

	PX_Object *ui_root;//ui控件对象
	PX_Galgame_Module_Stage_Option option[9];//选项结构体
	px_int option_count;//选项个数
	px_vector talks;//存储说话内容
	px_int currenttalk;//当前说话内容

	px_int choose;//选择第几个
	px_int score;//分数

	PX_SoundPlay *soundPlay;
	PX_SoundData soundData;
	
	px_dword elpased;
	PX_Galgame_Module_Stage_State state;
}PX_Galgame_Module_Stage;

PX_Object *PX_Galgame_Module_StageInstall(PX_Object *pParent,PX_Runtime *pRuntime,PX_Json *pJson,const px_char payload[],PX_SoundPlay *soundPlay,PX_FontModule *fm);
#endif
