//////////////////////////////////////////////////////////////////////////
//Galgame Aside scene
//PainterEngine x Unique hackday
//Codeby ����ѧ�ɹ� all rights reserved
//Power by PainterEngine
//2021-6-13
//////////////////////////////////////////////////////////////////////////

#ifndef PX_GALGAME_MODULE_ASIDE
#define PX_GALGAME_MODULE_ASIDE

#include "PainterEngine_Startup.h"

#define PX_GALGAME_MODULE_ASIDE_EXIT 0x00010000 //�����˳���Ϣ

#define PX_GALGAME_MODULE_ASIDE_STATE_DISPLAY_DURATION 1500 //����ʱ��
#define PX_GALGAME_MODULE_ASIDE_STATE_EXIT_DURATION 1000 //�˳�����ʱ��

typedef enum
{
	PX_Galgame_Module_Aside_State_backgrounddisplay=0,//��ʾ����
	PX_Galgame_Module_Aside_State_content,//��ʾ�ı�
	PX_Galgame_Module_Aside_State_exit,//�����˳�
}PX_Galgame_Module_Aside_State;

typedef struct
{
	PX_Runtime *pRuntime;//����ʱ
	PX_Object *messagebox;//��Ϣ��
	PX_Json *pJsonFramework;//json���
	PX_FontModule *fm;//��ģ
	px_texture texture;//����ͼ
	PX_SoundPlay *soundPlay;//������
	PX_SoundData soundData;//��������
	px_vector scripts;//�籾
	px_int currentScript;//��ǰ���ž籾
	px_dword elpased;//��ʱ��
	PX_Galgame_Module_Aside_State state;//��ǰ����״̬
}PX_Galgame_Module_Aside;

PX_Object *PX_Galgame_Module_AsideInstall(PX_Object *pParent,PX_Runtime *pRuntime,PX_Json *pJson,const px_char payload[],PX_SoundPlay *soundPlay,PX_FontModule *fm);
#endif
