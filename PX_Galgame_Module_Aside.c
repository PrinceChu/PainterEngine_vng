#include "PX_Galgame_Module_Aside.h"

px_void PX_Galgame_Module_AsideOnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	//�������ʱ
	PX_Galgame_Module_Aside *pDesc=PX_ObjectGetDesc(PX_Galgame_Module_Aside,(PX_Object *)ptr);
	if(pDesc->state==PX_Galgame_Module_Aside_State_content)
	{
		//������һ����Ϣ
		if (pDesc->currentScript<pDesc->scripts.size-1)
		{
			pDesc->currentScript++;
			PX_Object_MessageBoxAlert(pDesc->messagebox,PX_VECTORAT(px_string,&pDesc->scripts,pDesc->currentScript)->buffer);
		}
		else
		{
			//���߾͹ر���Ϣ��
			PX_Object_MessageBoxClose(pDesc->messagebox);
			pDesc->state=PX_Galgame_Module_Aside_State_exit;
			pDesc->elpased=0;
		}
	}
}


px_void PX_Galgame_Module_AsideRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	PX_Galgame_Module_Aside *pDesc=PX_ObjectGetDesc(PX_Galgame_Module_Aside,pObject);
	pDesc->elpased+=elpased;
	switch(pDesc->state)
	{
	//������ʾ״̬
	case PX_Galgame_Module_Aside_State_backgrounddisplay:
		{

			//���û�б���ͼƬ,ֱ���л�����һ������
			if (!pDesc->texture.surfaceBuffer)
			{
				pDesc->state=PX_Galgame_Module_Aside_State_content;
				pDesc->elpased=0;
				return;
			}

			//���㾭��ʱ��
			if (pDesc->elpased>=PX_GALGAME_MODULE_ASIDE_STATE_DISPLAY_DURATION)
			{
				//�л�����һ��״̬
				pDesc->state=PX_Galgame_Module_Aside_State_content;
				pDesc->elpased=0;
				return;
			}

			//��Ⱦ����ͼ
			if (pDesc->texture.surfaceBuffer)
			{
				PX_TEXTURERENDER_BLEND blend;//���ģʽ
				blend.alpha=pDesc->elpased*1.0f/PX_GALGAME_MODULE_ASIDE_STATE_DISPLAY_DURATION;
				blend.hdr_R=1;
				blend.hdr_G=1;
				blend.hdr_B=1;
				PX_TextureRender(psurface,&pDesc->texture,psurface->width/2,psurface->height/2,PX_ALIGN_CENTER,&blend);
			}
		}
		break;
	case PX_Galgame_Module_Aside_State_content:
		{
			//��ʾ��Ϣ
			if (pDesc->scripts.size==0)
			{
				//��ʾ���˾��������״̬
				pDesc->state=PX_Galgame_Module_Aside_State_exit;
				return;
			}

			if (pDesc->currentScript==0&&pDesc->messagebox->Visible==PX_FALSE)
			{
				//������Ϣ��
				PX_Object_MessageBoxAlert(pDesc->messagebox,PX_VECTORAT(px_string,&pDesc->scripts,0)->buffer);
			}
			//��Ⱦ����
			PX_TextureRender(psurface,&pDesc->texture,psurface->width/2,psurface->height/2,PX_ALIGN_CENTER,PX_NULL);
		}
		break;
	case PX_Galgame_Module_Aside_State_exit:
		{

			//���û�б���ͼƬ,ֱ���л�����һ������
			if (!pDesc->texture.surfaceBuffer)
			{
				//����������ͳ���������Ϣ
				PX_ObjectExecuteEvent(pObject->pParent,PX_OBJECT_BUILD_EVENT(PX_GALGAME_MODULE_ASIDE_EXIT));
				return;
			}

			//���㾭��ʱ��
			if (pDesc->elpased>=PX_GALGAME_MODULE_ASIDE_STATE_EXIT_DURATION)
			{
				//����������ͳ���������Ϣ
				PX_ObjectExecuteEvent(pObject->pParent,PX_OBJECT_BUILD_EVENT(PX_GALGAME_MODULE_ASIDE_EXIT));
				return;
			}

			//��Ⱦ����ͼ
			if (pDesc->texture.surfaceBuffer)
			{
				PX_TEXTURERENDER_BLEND blend;//���ģʽ
				blend.alpha=1.0f-pDesc->elpased*1.0f/PX_GALGAME_MODULE_ASIDE_STATE_EXIT_DURATION;
				blend.hdr_R=1;
				blend.hdr_G=1;
				blend.hdr_B=1;
				PX_TextureRender(psurface,&pDesc->texture,psurface->width/2,psurface->height/2,PX_ALIGN_CENTER,&blend);
			}
		}
		break;
	}
}

px_void PX_Galgame_Module_AsideFree(PX_Object *pObject)
{
	//ж��ģ��,�ͷų�����Դ
	PX_Galgame_Module_Aside *pAside=PX_ObjectGetDesc(PX_Galgame_Module_Aside,pObject);
	px_int i;
	for (i=0;i<pAside->scripts.size;i++)
	{
		px_string *pstring=PX_VECTORAT(px_string,&pAside->scripts,i);
		PX_StringFree(pstring);
	}
	PX_VectorFree(&pAside->scripts);

	if (pAside->soundData.buffer)
	{
		PX_SoundPlayClear(pAside->soundPlay);
		PX_SoundStaticDataFree(&pAside->soundData);
	}
	
	if (pAside->texture.surfaceBuffer)
	{
		PX_TextureFree(&pAside->texture);
	}
}

PX_Object *PX_Galgame_Module_AsideInstall(PX_Object *pParent,PX_Runtime *pRuntime,PX_Json *pJson,const px_char payload[],PX_SoundPlay *soundPlay,PX_FontModule *fm)
{
	PX_Object *pObject;
	PX_Galgame_Module_Aside aside,*paside;
	PX_memset(&aside,0,sizeof(PX_Galgame_Module_Aside));
	pObject=PX_ObjectCreateEx(&pRuntime->mp_game,pParent,0,0,0,0,0,0,0,PX_NULL,PX_Galgame_Module_AsideRender,PX_Galgame_Module_AsideFree,&aside,sizeof(aside));
	paside=PX_ObjectGetDesc(PX_Galgame_Module_Aside,pObject);

	//��ʼ������
	paside->pRuntime=pRuntime;
	paside->pJsonFramework=pJson;
	paside->soundPlay=soundPlay;
	paside->elpased=0;
	paside->fm=fm;
	//������Ϣ��
	paside->messagebox=PX_Object_MessageBoxCreate(&pRuntime->mp_ui,pObject,fm);
	PX_VectorInitialize(&pRuntime->mp_game,&paside->scripts,sizeof(px_string),32);
	PX_Object_MessageBoxSetContentDuration(paside->messagebox,100);
	PX_Object_MessageBoxSetBackgroundColor(paside->messagebox,PX_COLOR(128,255,255,255));
	PX_Object_MessageBoxSetFontColor(paside->messagebox,PX_COLOR_BLACK);

	//������Դ
	//���ر���ͼƬ
	do 
	{
		px_char spayload[256]={0};
		PX_strcat(spayload,payload);
		PX_strcat(spayload,".image");
		PX_LoadTextureFromFile(&pRuntime->mp_resources,&paside->texture,PX_JsonGetString(pJson,spayload));
	} while (0);


	//���ؽű�
	do 
	{
		px_int i;
		px_char spayload[256]={0};
		px_string script;
		for (i=1;i<128;i++)
		{
			const px_char *content;
			PX_sprintf2(spayload,sizeof(spayload),"%1.script%2",PX_STRINGFORMAT_STRING(payload),PX_STRINGFORMAT_INT(i));
			content=PX_JsonGetString(pJson,spayload);
			if (content[0])
			{
				PX_StringInitialize(&pRuntime->mp_game,&script);
				PX_StringCat(&script,content);
				PX_VectorPushback(&paside->scripts,&script);
			}
			else
			{
				break;
			}
		}
	} while (0);

	//���ر�������
	do 
	{
		px_char spayload[256]={0};
		PX_strcat(spayload,payload);
		PX_strcat(spayload,".sound");
		PX_LoadSoundFromFile(&pRuntime->mp_resources,&paside->soundData,PX_JsonGetString(pJson,spayload));
	} while (0);

	//////////////////////////////////////////////////////////////////////////
	//����б�������,ֱ�Ӳ���
	if (paside->soundData.size)
	{
		PX_SoundPlayAdd(paside->soundPlay,PX_SoundCreate(&paside->soundData,PX_TRUE));
	}

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Galgame_Module_AsideOnCursorDown,pObject);
	return pObject;
}
