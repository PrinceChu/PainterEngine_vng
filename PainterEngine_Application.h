#ifndef PAINTERENGINE_APPLICATION_H
#define PAINTERENGINE_APPLICATION_H

#ifdef __cplusplus
extern "C"{
#endif

#include "PainterEngine_Startup.h"

#ifdef __cplusplus
}
#endif


#include "PX_Galgame_Module_Aside.h"
#include "PX_Galgame_Module_Stage.h"

typedef struct
{
	PX_Object *schedular;//调度器
	PX_Object *module_activating;//当前激活模块

	px_int scene;//当前场景索引
	px_int score;//得分

	PX_Json  scriptJson;
	PX_FontModule fontmodule;
	PX_SoundPlay soundPlay;

	PX_Runtime runtime;
}PX_Application;

extern PX_Application App;

px_bool PX_ApplicationInitialize(PX_Application *App,px_int screen_Width,px_int screen_Height);
px_void PX_ApplicationUpdate(PX_Application *App,px_dword elpased);
px_void PX_ApplicationRender(PX_Application *App,px_dword elpased);
px_void PX_ApplicationPostEvent(PX_Application *App,PX_Object_Event e);

#endif
