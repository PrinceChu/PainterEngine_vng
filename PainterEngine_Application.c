#include "PainterEngine_Application.h"
PX_Application App;

px_void PX_ApplicationSchedular(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Application *pApp=(PX_Application *)ptr;
	switch(e.Event)
	{
	case PX_GALGAME_MODULE_ASIDE_EXIT:
		{
			//卸载模块
			PX_ObjectDelete(pApp->module_activating);
			pApp->module_activating=PX_NULL;
			//切换到下一个场景
			pApp->scene++;
		}
		break;
	case PX_GALGAME_MODULE_STAGE_EXIT:
		{
			PX_Galgame_Module_Stage *stage=PX_ObjectGetDesc(PX_Galgame_Module_Stage,pApp->module_activating);
			//卸载模块
			pApp->score+=stage->score;
			PX_ObjectDelete(pApp->module_activating);
			pApp->module_activating=PX_NULL;
			//切换到下一个场景
			pApp->scene++;
		}
		break;
	}
}


px_bool PX_ApplicationInitialize(PX_Application *pApp,px_int screen_width,px_int screen_height)
{
	PX_memset(pApp,0,sizeof(PX_Application));
	//初始化PainterEngine运行时
	PX_ApplicationInitializeDefault(&pApp->runtime, screen_width, screen_height);

	//初始化字模库
	if(!PX_FontModuleInitialize(&pApp->runtime.mp_resources,&pApp->fontmodule)) return PX_FALSE;
	if(!PX_LoadFontModuleFromFile(&pApp->fontmodule,"assert/font.pxf"))return PX_FALSE;//加载字模

	//初始化脚本库
	if(!PX_JsonInitialize(&pApp->runtime.mp_game,&pApp->scriptJson))return PX_FALSE;
	if (!PX_LoadJsonFromFile(&pApp->scriptJson,"assert/script.json"))return PX_FALSE;//加载游戏脚本
	
	//初始化混音器
	if(!PX_SoundPlayInitialize(&pApp->runtime.mp_game,&pApp->soundPlay))return PX_FALSE;
	if(!PX_AudioInitialize(&pApp->soundPlay))return PX_FALSE;//对接平台Audio

	//创建调度器对象
	pApp->schedular=PX_ObjectCreate(&pApp->runtime.mp_game,0,0,0,0,0,0,0);
	PX_ObjectRegisterEvent(pApp->schedular,PX_OBJECT_EVENT_ANY,PX_ApplicationSchedular,pApp);

	return PX_TRUE;
}

px_bool PX_ApplicationLoadScene(PX_Application *pApp,const px_char spayload[])
{
	PX_Json_Value *pValue;

	//读取json场景
	pValue=PX_JsonGetValue(&pApp->scriptJson,spayload);
	if (pValue)
	{
		px_char type_payload[256]={0};
		const px_char *pContent;
		PX_sprintf1(type_payload,sizeof(type_payload),"%1.type",PX_STRINGFORMAT_STRING(spayload));
		//获取场景类型
		pContent=PX_JsonGetString(&pApp->scriptJson,type_payload);

		if (PX_strequ(pContent,"aside"))
		{
			//装载旁白模块
			pApp->module_activating=PX_Galgame_Module_AsideInstall(pApp->schedular,&pApp->runtime,&pApp->scriptJson,spayload,&pApp->soundPlay,&pApp->fontmodule);
			return PX_TRUE;
		}
		else if(PX_strequ(pContent,"stage"))
		{
			//装载实际场景模块
			pApp->module_activating=PX_Galgame_Module_StageInstall(pApp->schedular,&pApp->runtime,&pApp->scriptJson,spayload,&pApp->soundPlay,&pApp->fontmodule);
			return PX_TRUE;
		}
		else
		{
			//不应该运行到这里
			PX_ASSERT();
			return PX_FALSE;
		}
	}
	return PX_FALSE;
}

px_void PX_ApplicationUpdate(PX_Application *pApp,px_dword elpased)
{

	static px_bool thanks=0;//是否播放过鸣谢
	static px_bool end=0;//是否播放过结局
	static px_bool dead_end=0;//是否死亡
	if (pApp->module_activating==PX_NULL)//上一个模块卸载,装载下一个场景
	{
		px_char spayload[256]={0};//生成payload
		if (dead_end==1)
		{
			//你死了,直接退出
			exit(0);
		}
		if (pApp->score<-3000)
		{
			//你死了,播放死亡场景
			PX_ApplicationLoadScene(pApp,"dead");
			dead_end=1;
			return;
		}

		//生成scene payload
		PX_sprintf1(spayload,sizeof(spayload),"scene%1",PX_STRINGFORMAT_INT(pApp->scene));
		if(!PX_ApplicationLoadScene(pApp,spayload))
		{
			//是否播放过鸣谢
			if (thanks==1)
			{
				//游戏完成,退出
				exit(0);
			}
			if (end)
			{
				//是否播放过结局,放过了就放鸣谢
				PX_ApplicationLoadScene(pApp,"thanks");
				thanks=1;
				return;
			}
			if (pApp->score>200)
			{
				//分数大于200,结局1
				PX_ApplicationLoadScene(pApp,"end1");
				end=1;
			}
			else if(pApp->score<0)
			{
				//分数小于0,结局3
				PX_ApplicationLoadScene(pApp,"end3");
				end=1;
			}
			else
			{
				//否者结局2
				PX_ApplicationLoadScene(pApp,"end2");
				end=1;
			}
		}
	}
}

px_void PX_ApplicationRender(PX_Application *pApp,px_dword elpased)
{
	px_surface *pRenderSurface=&pApp->runtime.RenderSurface;
	PX_RuntimeRenderClear(&pApp->runtime,PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR);
	if (pApp->module_activating)
	{
		//渲染当前模块
		PX_ObjectRender(pRenderSurface,pApp->module_activating,elpased);
	}
}

px_void PX_ApplicationPostEvent(PX_Application *pApp,PX_Object_Event e)
{
	//分发消息事件
	PX_ApplicationEventDefault(&pApp->runtime, e);
	PX_ObjectPostEvent(pApp->module_activating,e);
}

