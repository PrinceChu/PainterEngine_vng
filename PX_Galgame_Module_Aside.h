//////////////////////////////////////////////////////////////////////////
//Galgame Aside scene
//PainterEngine x Unique hackday
//Codeby 不如学成狗 all rights reserved
//Power by PainterEngine
//2021-6-13
//////////////////////////////////////////////////////////////////////////

#ifndef PX_GALGAME_MODULE_ASIDE
#define PX_GALGAME_MODULE_ASIDE

#include "PainterEngine_Startup.h"

#define PX_GALGAME_MODULE_ASIDE_EXIT 0x00010000 //场景退出消息

#define PX_GALGAME_MODULE_ASIDE_STATE_DISPLAY_DURATION 1500 //渐变时间
#define PX_GALGAME_MODULE_ASIDE_STATE_EXIT_DURATION 1000 //退出渐变时间

typedef enum
{
	PX_Galgame_Module_Aside_State_backgrounddisplay=0,//显示背景
	PX_Galgame_Module_Aside_State_content,//显示文本
	PX_Galgame_Module_Aside_State_exit,//渐变退出
}PX_Galgame_Module_Aside_State;

typedef struct
{
	PX_Runtime *pRuntime;//运行时
	PX_Object *messagebox;//消息框
	PX_Json *pJsonFramework;//json框架
	PX_FontModule *fm;//字模
	px_texture texture;//背景图
	PX_SoundPlay *soundPlay;//混音器
	PX_SoundData soundData;//音乐数据
	px_vector scripts;//剧本
	px_int currentScript;//当前播放剧本
	px_dword elpased;//计时器
	PX_Galgame_Module_Aside_State state;//当前场景状态
}PX_Galgame_Module_Aside;

PX_Object *PX_Galgame_Module_AsideInstall(PX_Object *pParent,PX_Runtime *pRuntime,PX_Json *pJson,const px_char payload[],PX_SoundPlay *soundPlay,PX_FontModule *fm);
#endif
