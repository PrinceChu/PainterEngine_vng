//////////////////////////////////////////////////////////////////////////
//Galgame Stage scene
//PainterEngine x Unique hackday
//Codeby ����ѧ�ɹ� all rights reserved
//Power by PainterEngine
//2021-6-13
//////////////////////////////////////////////////////////////////////////

#ifndef PX_GALGAME_MODULE_STAGE
#define PX_GALGAME_MODULE_STAGE

#include "PainterEngine_Startup.h"

#define PX_GALGAME_MODULE_STAGE_EXIT 0x00020000

#define PX_GALGAME_MODULE_STAGE_STATE_DISPLAY_DURATION 1500 //����ͼƬ����
#define PX_GALGAME_MODULE_STAGE_STATE_ENTER_DURATION 500 //�ұ�������
#define PX_GALGAME_MODULE_STAGE_STATE_OPTIONAL_DURATION 1000//ѡ���ƽ����ҳ�
#define PX_GALGAME_MODULE_STAGE_STATE_EXIT_DURATION 1000 //�˳�ͼƬ����

typedef enum
{
	PX_Galgame_Module_Stage_State_backgrounddisplay=0,//��ʾ����
	PX_Galgame_Module_Stage_State_entry,//�����볡
	PX_Galgame_Module_Stage_State_content,//��ʾ�ı�
	PX_Galgame_Module_Stage_State_optionentry,//�����볡
	PX_Galgame_Module_Stage_State_optional,//ѡ��״̬
	PX_Galgame_Module_Stage_State_optionexit,//�����볡
	PX_Galgame_Module_Stage_State_end,//
	PX_Galgame_Module_Stage_State_exit,//�����˳�
}PX_Galgame_Module_Stage_State;

typedef struct  
{
	px_texture image;
	px_char animtion[16];
	px_int score;
	px_char c_content[128];
	PX_Object *pushButton;
}PX_Galgame_Module_Stage_Option;//ѡ��ṹ��

typedef struct  
{
	px_char t_content[128];
	px_char animation[16];
}PX_Galgame_Module_Stage_Talk;//�ı��ṹ��

typedef struct
{
	PX_Runtime *pRuntime;
	PX_Json *pJsonFramework;
	PX_FontModule *fm;

	px_int time;
	px_int actorx;
	px_texture background;//����ͼ
	px_texture left_msgbox;//��ߵ���Ϣ��ͼ
	px_texture right_msgbox;//�ұߵ���Ϣ��ͼ
	px_texture actor;//����˵�ͼƬ
	px_int livex;
	PX_LiveFramework live;//live framework

	PX_Object *ui_root;//ui�ؼ�����
	PX_Galgame_Module_Stage_Option option[9];//ѡ��ṹ��
	px_int option_count;//ѡ�����
	px_vector talks;//�洢˵������
	px_int currenttalk;//��ǰ˵������

	px_int choose;//ѡ��ڼ���
	px_int score;//����

	PX_SoundPlay *soundPlay;
	PX_SoundData soundData;
	
	px_dword elpased;
	PX_Galgame_Module_Stage_State state;
}PX_Galgame_Module_Stage;

PX_Object *PX_Galgame_Module_StageInstall(PX_Object *pParent,PX_Runtime *pRuntime,PX_Json *pJson,const px_char payload[],PX_SoundPlay *soundPlay,PX_FontModule *fm);
#endif
