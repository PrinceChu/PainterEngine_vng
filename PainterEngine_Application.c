#include "PainterEngine_Application.h"
PX_Application App;

px_void PX_ApplicationSchedular(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Application *pApp=(PX_Application *)ptr;
	switch(e.Event)
	{
	case PX_GALGAME_MODULE_ASIDE_EXIT:
		{
			//ж��ģ��
			PX_ObjectDelete(pApp->module_activating);
			pApp->module_activating=PX_NULL;
			//�л�����һ������
			pApp->scene++;
		}
		break;
	case PX_GALGAME_MODULE_STAGE_EXIT:
		{
			PX_Galgame_Module_Stage *stage=PX_ObjectGetDesc(PX_Galgame_Module_Stage,pApp->module_activating);
			//ж��ģ��
			pApp->score+=stage->score;
			PX_ObjectDelete(pApp->module_activating);
			pApp->module_activating=PX_NULL;
			//�л�����һ������
			pApp->scene++;
		}
		break;
	}
}


px_bool PX_ApplicationInitialize(PX_Application *pApp,px_int screen_width,px_int screen_height)
{
	PX_memset(pApp,0,sizeof(PX_Application));
	//��ʼ��PainterEngine����ʱ
	PX_ApplicationInitializeDefault(&pApp->runtime, screen_width, screen_height);

	//��ʼ����ģ��
	if(!PX_FontModuleInitialize(&pApp->runtime.mp_resources,&pApp->fontmodule)) return PX_FALSE;
	if(!PX_LoadFontModuleFromFile(&pApp->fontmodule,"assert/font.pxf"))return PX_FALSE;//������ģ

	//��ʼ���ű���
	if(!PX_JsonInitialize(&pApp->runtime.mp_game,&pApp->scriptJson))return PX_FALSE;
	if (!PX_LoadJsonFromFile(&pApp->scriptJson,"assert/script.json"))return PX_FALSE;//������Ϸ�ű�
	
	//��ʼ��������
	if(!PX_SoundPlayInitialize(&pApp->runtime.mp_game,&pApp->soundPlay))return PX_FALSE;
	if(!PX_AudioInitialize(&pApp->soundPlay))return PX_FALSE;//�Խ�ƽ̨Audio

	//��������������
	pApp->schedular=PX_ObjectCreate(&pApp->runtime.mp_game,0,0,0,0,0,0,0);
	PX_ObjectRegisterEvent(pApp->schedular,PX_OBJECT_EVENT_ANY,PX_ApplicationSchedular,pApp);

	return PX_TRUE;
}

px_bool PX_ApplicationLoadScene(PX_Application *pApp,const px_char spayload[])
{
	PX_Json_Value *pValue;

	//��ȡjson����
	pValue=PX_JsonGetValue(&pApp->scriptJson,spayload);
	if (pValue)
	{
		px_char type_payload[256]={0};
		const px_char *pContent;
		PX_sprintf1(type_payload,sizeof(type_payload),"%1.type",PX_STRINGFORMAT_STRING(spayload));
		//��ȡ��������
		pContent=PX_JsonGetString(&pApp->scriptJson,type_payload);

		if (PX_strequ(pContent,"aside"))
		{
			//װ���԰�ģ��
			pApp->module_activating=PX_Galgame_Module_AsideInstall(pApp->schedular,&pApp->runtime,&pApp->scriptJson,spayload,&pApp->soundPlay,&pApp->fontmodule);
			return PX_TRUE;
		}
		else if(PX_strequ(pContent,"stage"))
		{
			//װ��ʵ�ʳ���ģ��
			pApp->module_activating=PX_Galgame_Module_StageInstall(pApp->schedular,&pApp->runtime,&pApp->scriptJson,spayload,&pApp->soundPlay,&pApp->fontmodule);
			return PX_TRUE;
		}
		else
		{
			//��Ӧ�����е�����
			PX_ASSERT();
			return PX_FALSE;
		}
	}
	return PX_FALSE;
}

px_void PX_ApplicationUpdate(PX_Application *pApp,px_dword elpased)
{

	static px_bool thanks=0;//�Ƿ񲥷Ź���л
	static px_bool end=0;//�Ƿ񲥷Ź����
	static px_bool dead_end=0;//�Ƿ�����
	if (pApp->module_activating==PX_NULL)//��һ��ģ��ж��,װ����һ������
	{
		px_char spayload[256]={0};//����payload
		if (dead_end==1)
		{
			//������,ֱ���˳�
			exit(0);
		}
		if (pApp->score<-3000)
		{
			//������,������������
			PX_ApplicationLoadScene(pApp,"dead");
			dead_end=1;
			return;
		}

		//����scene payload
		PX_sprintf1(spayload,sizeof(spayload),"scene%1",PX_STRINGFORMAT_INT(pApp->scene));
		if(!PX_ApplicationLoadScene(pApp,spayload))
		{
			//�Ƿ񲥷Ź���л
			if (thanks==1)
			{
				//��Ϸ���,�˳�
				exit(0);
			}
			if (end)
			{
				//�Ƿ񲥷Ź����,�Ź��˾ͷ���л
				PX_ApplicationLoadScene(pApp,"thanks");
				thanks=1;
				return;
			}
			if (pApp->score>200)
			{
				//��������200,���1
				PX_ApplicationLoadScene(pApp,"end1");
				end=1;
			}
			else if(pApp->score<0)
			{
				//����С��0,���3
				PX_ApplicationLoadScene(pApp,"end3");
				end=1;
			}
			else
			{
				//���߽��2
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
		//��Ⱦ��ǰģ��
		PX_ObjectRender(pRenderSurface,pApp->module_activating,elpased);
	}
}

px_void PX_ApplicationPostEvent(PX_Application *pApp,PX_Object_Event e)
{
	//�ַ���Ϣ�¼�
	PX_ApplicationEventDefault(&pApp->runtime, e);
	PX_ObjectPostEvent(pApp->module_activating,e);
}

