#include "PX_Galgame_Module_Stage.h"

px_void PX_Galgame_Module_StageOnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	//鼠标单击时
	PX_Galgame_Module_Stage *pDesc=PX_ObjectGetDesc(PX_Galgame_Module_Stage,(PX_Object *)ptr);
	if(pDesc->state==PX_Galgame_Module_Stage_State_content)
	{
		//下一条说话文本
		pDesc->currenttalk++;
	}

	if (pDesc->state==PX_Galgame_Module_Stage_State_end)
	{
		//选完后,退出
		pDesc->state=PX_Galgame_Module_Stage_State_exit;
		pDesc->elpased=0;
	}
}


px_void PX_Galgame_Module_StageRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	PX_Galgame_Module_Stage *pDesc=PX_ObjectGetDesc(PX_Galgame_Module_Stage,pObject);
	if (elpased>200)
	{
		elpased=200;
	}

	pDesc->elpased+=elpased;
	switch(pDesc->state)
	{
	//背景显示状态
	case PX_Galgame_Module_Stage_State_backgrounddisplay:
		{

			//如果没有背景图片,直接切换到下一个场景
			if (!pDesc->background.surfaceBuffer)
			{
				pDesc->state=PX_Galgame_Module_Stage_State_entry;
				pDesc->elpased=0;
				return;
			}

			//计算经历时间
			if (pDesc->elpased>=PX_GALGAME_MODULE_STAGE_STATE_DISPLAY_DURATION)
			{
				//切换到下一个状态
				pDesc->state=PX_Galgame_Module_Stage_State_entry;
				pDesc->elpased=0;
				return;
			}

			//渲染背景图
			if (pDesc->background.surfaceBuffer)
			{
				PX_TEXTURERENDER_BLEND blend;//混合模式
				blend.alpha=pDesc->elpased*1.0f/PX_GALGAME_MODULE_STAGE_STATE_DISPLAY_DURATION;
				blend.hdr_R=1;
				blend.hdr_G=1;
				blend.hdr_B=1;
				PX_TextureRender(psurface,&pDesc->background,psurface->width/2,psurface->height/2,PX_ALIGN_CENTER,&blend);
			}
		}
		break;
	case PX_Galgame_Module_Stage_State_entry:
		{
			//background
			PX_TextureRender(psurface,&pDesc->background,psurface->width/2,psurface->height/2,PX_ALIGN_CENTER,PX_NULL);

			//actor
			if (pDesc->actorx<10)
			{
				pDesc->actorx+=elpased;
			}
			if (pDesc->actorx>=10)
			{
				pDesc->actorx=10;
			}
			//绘制左边人物
			PX_TextureRender(psurface,&pDesc->actor,pDesc->actorx,psurface->height,PX_ALIGN_LEFTBOTTOM,PX_NULL);

			if (pDesc->livex>psurface->width-pDesc->live.width)
			{
				pDesc->livex-=elpased;
			}
			if (pDesc->livex<psurface->width-pDesc->live.width)
			{
				pDesc->livex=psurface->width-pDesc->live.width;
			}
			//绘制右边人物
			PX_LiveFrameworkRender(psurface,&pDesc->live,pDesc->livex,psurface->height,PX_ALIGN_LEFTBOTTOM,elpased);

			if (pDesc->actorx==10)
			{
				if (pDesc->livex==psurface->width-pDesc->live.width)
				{
					if (pDesc->elpased>1000)
					{
						pDesc->state=PX_Galgame_Module_Stage_State_content;
						pDesc->elpased=0;
						pDesc->currenttalk=0;
						return;
					}
					pDesc->elpased+=elpased;
				}
			}
		}
		break;

	case PX_Galgame_Module_Stage_State_content:
		{
			//background
			PX_TextureRender(psurface,&pDesc->background,psurface->width/2,psurface->height/2,PX_ALIGN_CENTER,PX_NULL);

			//actor
			PX_TextureRender(psurface,&pDesc->actor,pDesc->actorx,psurface->height,PX_ALIGN_LEFTBOTTOM,PX_NULL);
			
			//live
			PX_LiveFrameworkRender(psurface,&pDesc->live,pDesc->livex,psurface->height,PX_ALIGN_LEFTBOTTOM,elpased);

			//画对话框
			if (pDesc->currenttalk>=pDesc->talks.size)
			{
				pDesc->currenttalk=pDesc->talks.size-1;
				pDesc->state=PX_Galgame_Module_Stage_State_optionentry;
				pDesc->elpased=0;
				return;
			};

			if (PX_strequ(PX_VECTORAT(PX_Galgame_Module_Stage_Talk,&pDesc->talks,pDesc->currenttalk)->t_content,"skip"))
			{
				pDesc->currenttalk++;
				pDesc->elpased=0;
				return;
			}

			if (pDesc->talks.size==0)
			{
				pDesc->state=PX_Galgame_Module_Stage_State_optionentry;
				pDesc->elpased=0;
				return;
			}
			pDesc->elpased+=elpased;

			//左边的人的对话
			if (pDesc->elpased<300)
			{
				//渐变效果
				PX_TEXTURERENDER_BLEND blend;
				blend.alpha=0.5f*pDesc->elpased/300;
				blend.hdr_R=1;
				blend.hdr_G=1;
				blend.hdr_B=1;
				if (pDesc->currenttalk%2==0)
				{
					PX_TextureRender(psurface,&pDesc->left_msgbox,psurface->width/2,psurface->height/2+30,PX_ALIGN_CENTER,&blend);
				}
				else
				{
					PX_TextureRender(psurface,&pDesc->right_msgbox,psurface->width/2,psurface->height/2+30,PX_ALIGN_CENTER,&blend);
				}
				
			}
			else
			{
				//绘制对话框和文本
				px_int step;
				PX_TEXTURERENDER_BLEND blend;
				blend.alpha=0.5f;
				blend.hdr_R=1;
				blend.hdr_G=1;
				blend.hdr_B=1;
				step=(pDesc->elpased-300)/80;
				
				if (pDesc->currenttalk%2==0)
				{
					PX_TextureRender(psurface,&pDesc->left_msgbox,psurface->width/2,psurface->height/2+30,PX_ALIGN_CENTER,&blend);
				}
				else
				{
					PX_TextureRender(psurface,&pDesc->right_msgbox,psurface->width/2,psurface->height/2+30,PX_ALIGN_CENTER,&blend);
				}

				PX_FontModuleDrawTextStep(psurface,pDesc->fm,\
					psurface->width/2-pDesc->left_msgbox.width/2+15,\
					psurface->height/2-pDesc->left_msgbox.height/2+42,\
					PX_ALIGN_LEFTTOP,\
					PX_VECTORAT(PX_Galgame_Module_Stage_Talk,&pDesc->talks,pDesc->currenttalk)->t_content,step,PX_COLOR(255,0,0,0)
					);
				if (pDesc->live.status==PX_LIVEFRAMEWORK_STATUS_STOP)
				{
					px_char *animationName=PX_VECTORAT(PX_Galgame_Module_Stage_Talk,&pDesc->talks,pDesc->currenttalk)->animation;
					if (animationName[0])
					{//播放painterengine live 动画
						PX_LiveFrameworkPlayAnimationByName(&pDesc->live,animationName);
					}
				}
			}
		}
		break;
	case PX_Galgame_Module_Stage_State_optionentry:
		{
			PX_TEXTURERENDER_BLEND blend;
			blend.alpha=0.5f;
			blend.hdr_R=1;
			blend.hdr_G=1;
			blend.hdr_B=1;

			//background
			PX_TextureRender(psurface,&pDesc->background,psurface->width/2,psurface->height/2,PX_ALIGN_CENTER,PX_NULL);
			//actor
			PX_TextureRender(psurface,&pDesc->actor,pDesc->actorx,psurface->height,PX_ALIGN_LEFTBOTTOM,PX_NULL);
			//live
			PX_LiveFrameworkRender(psurface,&pDesc->live,pDesc->livex,psurface->height,PX_ALIGN_LEFTBOTTOM,elpased);

			if (pDesc->option_count==0)
			{
				pDesc->state=PX_Galgame_Module_Stage_State_exit;
				pDesc->elpased=0;
				return;
			}

			//question

			if (pDesc->currenttalk%2==0)
			{
				PX_TextureRender(psurface,&pDesc->left_msgbox,psurface->width/2,psurface->height/2+30,PX_ALIGN_CENTER,&blend);
			}
			else
			{
				PX_TextureRender(psurface,&pDesc->right_msgbox,psurface->width/2,psurface->height/2+30,PX_ALIGN_CENTER,&blend);
			}

			PX_FontModuleDrawText(psurface,pDesc->fm,\
				psurface->width/2-pDesc->left_msgbox.width/2+15,\
				psurface->height/2-pDesc->left_msgbox.height/2+42,\
				PX_ALIGN_LEFTTOP,\
				PX_VECTORAT(PX_Galgame_Module_Stage_Talk,&pDesc->talks,pDesc->currenttalk)->t_content,PX_COLOR(255,0,0,0)
				);

			if (pDesc->live.status==PX_LIVEFRAMEWORK_STATUS_STOP)
			{
				switch(PX_rand()%3)
				{
				case 0:
					PX_LiveFrameworkPlayAnimationByName(&pDesc->live,"waiting1");
					break;
				case 1:
					PX_LiveFrameworkPlayAnimationByName(&pDesc->live,"waiting2");
					break;
				case 2:
					PX_LiveFrameworkPlayAnimationByName(&pDesc->live,"waiting3");
					break;
				}		
			}


			do 
			{
				//卡牌控件左边移动到中间
				px_bool arri;
				px_int i;
				arri=PX_TRUE;
				for (i=1;i<=pDesc->option_count;i++)
				{
					px_float targetx=1.0f*psurface->width/(pDesc->option_count+1)*(i);
					px_float distance;
					targetx-=pDesc->option[i].pushButton->Width/2;
					distance=targetx-pDesc->option[i].pushButton->x;
					if (distance>5)
					{
						distance=distance*elpased/380;
						pDesc->option[i].pushButton->x+=distance;
						arri=PX_FALSE;
					}
					else
					{
						pDesc->option[i].pushButton->x=targetx;
					}
				}
				PX_ObjectRender(psurface,pDesc->ui_root,elpased);
				if (arri)
				{
					pDesc->state=PX_Galgame_Module_Stage_State_optional;
					pDesc->elpased=0;
				}
			} while (0);


		}
		break;
	case PX_Galgame_Module_Stage_State_optional:
		{
			PX_TEXTURERENDER_BLEND blend;
			blend.alpha=0.5f;
			blend.hdr_R=1;
			blend.hdr_G=1;
			blend.hdr_B=1;

			//background
			PX_TextureRender(psurface,&pDesc->background,psurface->width/2,psurface->height/2,PX_ALIGN_CENTER,PX_NULL);
			//actor
			PX_TextureRender(psurface,&pDesc->actor,pDesc->actorx,psurface->height,PX_ALIGN_LEFTBOTTOM,PX_NULL);
			//live
			PX_LiveFrameworkRender(psurface,&pDesc->live,pDesc->livex,psurface->height,PX_ALIGN_LEFTBOTTOM,elpased);

			//question

			if (pDesc->currenttalk%2==0)
			{
				PX_TextureRender(psurface,&pDesc->left_msgbox,psurface->width/2,psurface->height/2+30,PX_ALIGN_CENTER,&blend);
			}
			else
			{
				PX_TextureRender(psurface,&pDesc->right_msgbox,psurface->width/2,psurface->height/2+30,PX_ALIGN_CENTER,&blend);
			}

			PX_FontModuleDrawText(psurface,pDesc->fm,\
				psurface->width/2-pDesc->left_msgbox.width/2+15,\
				psurface->height/2-pDesc->left_msgbox.height/2+42,\
				PX_ALIGN_LEFTTOP,\
				PX_VECTORAT(PX_Galgame_Module_Stage_Talk,&pDesc->talks,pDesc->currenttalk)->t_content,PX_COLOR(255,0,0,0)
				);

			if (pDesc->live.status==PX_LIVEFRAMEWORK_STATUS_STOP)
			{
				switch(PX_rand()%3)
				{
				case 0:
					PX_LiveFrameworkPlayAnimationByName(&pDesc->live,"waiting1");
					break;
				case 1:
					PX_LiveFrameworkPlayAnimationByName(&pDesc->live,"waiting2");
					break;
				case 2:
					PX_LiveFrameworkPlayAnimationByName(&pDesc->live,"waiting3");
					break;
				}		
			}

			PX_ObjectRender(psurface,pDesc->ui_root,elpased);
		}
		break;
	case PX_Galgame_Module_Stage_State_optionexit:
		{
			PX_TEXTURERENDER_BLEND blend;
			blend.alpha=0.5f;
			blend.hdr_R=1;
			blend.hdr_G=1;
			blend.hdr_B=1;

			//background
			PX_TextureRender(psurface,&pDesc->background,psurface->width/2,psurface->height/2,PX_ALIGN_CENTER,PX_NULL);
			//actor
			PX_TextureRender(psurface,&pDesc->actor,pDesc->actorx,psurface->height,PX_ALIGN_LEFTBOTTOM,PX_NULL);
			//live
			PX_LiveFrameworkRender(psurface,&pDesc->live,pDesc->livex,psurface->height,PX_ALIGN_LEFTBOTTOM,elpased);

			//question

			if (pDesc->currenttalk%2==0)
			{
				PX_TextureRender(psurface,&pDesc->left_msgbox,psurface->width/2,psurface->height/2+30,PX_ALIGN_CENTER,&blend);
			}
			else
			{
				PX_TextureRender(psurface,&pDesc->right_msgbox,psurface->width/2,psurface->height/2+30,PX_ALIGN_CENTER,&blend);
			}

			PX_FontModuleDrawText(psurface,pDesc->fm,\
				psurface->width/2-pDesc->left_msgbox.width/2+15,\
				psurface->height/2-pDesc->left_msgbox.height/2+42,\
				PX_ALIGN_LEFTTOP,\
				PX_VECTORAT(PX_Galgame_Module_Stage_Talk,&pDesc->talks,pDesc->currenttalk)->t_content,PX_COLOR(255,0,0,0)
				);

			if (pDesc->live.status==PX_LIVEFRAMEWORK_STATUS_STOP)
			{
				switch(PX_rand()%3)
				{
				case 0:
					PX_LiveFrameworkPlayAnimationByName(&pDesc->live,"waiting1");
					break;
				case 1:
					PX_LiveFrameworkPlayAnimationByName(&pDesc->live,"waiting2");
					break;
				case 2:
					PX_LiveFrameworkPlayAnimationByName(&pDesc->live,"waiting3");
					break;
				}		
			}


			do 
			{
				px_bool arri;
				px_int i;
				arri=PX_TRUE;
				for (i=1;i<=pDesc->option_count;i++)
				{
					//卡牌中间移到右边
					px_float targetx=psurface->width+20.f;
					px_float distance;
					distance=targetx-pDesc->option[i].pushButton->x;
					if (distance>5)
					{
						distance=distance*elpased/380;
						pDesc->option[i].pushButton->x+=distance;
						arri=PX_FALSE;
					}
					else
					{
						pDesc->option[i].pushButton->x=targetx;
					}
				}
				PX_ObjectRender(psurface,pDesc->ui_root,elpased);
				if (arri)
				{
					pDesc->state=PX_Galgame_Module_Stage_State_end;
					pDesc->elpased=0;
				}
			} while (0);
		}
		break;
	case PX_Galgame_Module_Stage_State_end:
		{
			//background
			PX_TextureRender(psurface,&pDesc->background,psurface->width/2,psurface->height/2,PX_ALIGN_CENTER,PX_NULL);

			//actor
			PX_TextureRender(psurface,&pDesc->actor,pDesc->actorx,psurface->height,PX_ALIGN_LEFTBOTTOM,PX_NULL);

			//live
			PX_LiveFrameworkRender(psurface,&pDesc->live,pDesc->livex,psurface->height,PX_ALIGN_LEFTBOTTOM,elpased);


			if (!pDesc->option[pDesc->choose].c_content[0])
			{
				pDesc->currenttalk=pDesc->talks.size-1;
				pDesc->state=PX_Galgame_Module_Stage_State_exit;
				pDesc->elpased=0;
				return;
			};

			pDesc->elpased+=elpased;

			//左边的人的对话
			if (pDesc->elpased<300)
			{
				PX_TEXTURERENDER_BLEND blend;
				blend.alpha=0.5f*pDesc->elpased/300;
				blend.hdr_R=1;
				blend.hdr_G=1;
				blend.hdr_B=1;
				if (pDesc->currenttalk%2==0)
				{
					PX_TextureRender(psurface,&pDesc->left_msgbox,psurface->width/2,psurface->height/2+30,PX_ALIGN_CENTER,&blend);
				}
				else
				{
					PX_TextureRender(psurface,&pDesc->right_msgbox,psurface->width/2,psurface->height/2+30,PX_ALIGN_CENTER,&blend);
				}

			}
			else
			{
				px_int step;
				PX_TEXTURERENDER_BLEND blend;
				blend.alpha=0.5f;
				blend.hdr_R=1;
				blend.hdr_G=1;
				blend.hdr_B=1;
				step=(pDesc->elpased-300)/80;

				if (pDesc->currenttalk%2==0)
				{
					PX_TextureRender(psurface,&pDesc->left_msgbox,psurface->width/2,psurface->height/2+30,PX_ALIGN_CENTER,&blend);
				}
				else
				{
					PX_TextureRender(psurface,&pDesc->right_msgbox,psurface->width/2,psurface->height/2+30,PX_ALIGN_CENTER,&blend);
				}

				PX_FontModuleDrawTextStep(psurface,pDesc->fm,\
					psurface->width/2-pDesc->left_msgbox.width/2+15,\
					psurface->height/2-pDesc->left_msgbox.height/2+42,\
					PX_ALIGN_LEFTTOP,\
					pDesc->option[pDesc->choose].c_content,step,PX_COLOR(255,0,0,0)
					);

				if (pDesc->live.status==PX_LIVEFRAMEWORK_STATUS_STOP)
				{
					px_char *animationName=pDesc->option[pDesc->choose].animtion;
					if (animationName[0])
					{
						PX_LiveFrameworkPlayAnimationByName(&pDesc->live,animationName);
					}
				}
			}
		}
		break;
	case PX_Galgame_Module_Stage_State_exit:
		{

			//如果没有背景图片,直接切换到下一个场景
			//计算经历时间
			if (pDesc->elpased>=PX_GALGAME_MODULE_STAGE_STATE_EXIT_DURATION)
			{
				//向调度器发送场景结束信息
				PX_ObjectExecuteEvent(pObject->pParent,PX_OBJECT_BUILD_EVENT(PX_GALGAME_MODULE_STAGE_EXIT));
				return;
			}

			//渲染背景图
			if (pDesc->background.surfaceBuffer)
			{
				PX_TEXTURERENDER_BLEND blend;//混合模式
				blend.alpha=1.0f-pDesc->elpased*1.0f/PX_GALGAME_MODULE_STAGE_STATE_EXIT_DURATION;
				blend.hdr_R=1;
				blend.hdr_G=1;
				blend.hdr_B=1;
				PX_TextureRender(psurface,&pDesc->background,psurface->width/2,psurface->height/2,PX_ALIGN_CENTER,&blend);
			}
		}
		break;
	}
}

px_void PX_Galgame_Module_StageFree(PX_Object *pObject)
{
	//释放资源
	px_int i;
	PX_Galgame_Module_Stage *pDesc=PX_ObjectGetDesc(PX_Galgame_Module_Stage,pObject);
	PX_TextureFree(&pDesc->background);
	PX_TextureFree(&pDesc->left_msgbox);
	PX_TextureFree(&pDesc->right_msgbox);
	PX_TextureFree(&pDesc->actor);
	PX_LiveFrameworkFree(&pDesc->live);
	for (i=0;i<9;i++)
	{
		if (pDesc->option[i].image.surfaceBuffer)
		{
			PX_TextureFree(&pDesc->option[i].image);
		}
	}
	if (pDesc->soundData.buffer)
	{
		PX_SoundPlayClear(pDesc->soundPlay);
		PX_SoundStaticDataFree(&pDesc->soundData);
	}

	PX_VectorFree(&pDesc->talks);
}

//当选择按钮时
px_void PX_Galgame_Module_OnSelectButton(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Galgame_Module_Stage *pDesc;
	px_int i;
	pDesc=PX_ObjectGetDesc(PX_Galgame_Module_Stage,(PX_Object *)ptr);
	//判断到底点击的是哪一个卡牌
	for(i=1;i<=pDesc->option_count;i++)
	{
		if (pDesc->option[i].pushButton==pObject)
		{
			pDesc->score=pDesc->option[i].score;
			pDesc->state=PX_Galgame_Module_Stage_State_optionexit;
			pDesc->choose=i;
			pDesc->elpased=0;
			return;
		}
	}
}

PX_Object *PX_Galgame_Module_StageInstall(PX_Object *pParent,PX_Runtime *pRuntime,PX_Json *pJson,const px_char payload[],PX_SoundPlay *soundPlay,PX_FontModule *fm)
{
	PX_Object *pObject;
	PX_Galgame_Module_Stage Desc,*pDesc;
	PX_memset(&Desc,0,sizeof(PX_Galgame_Module_Stage));
	pObject=PX_ObjectCreateEx(&pRuntime->mp_game,pParent,0,0,0,0,0,0,1,PX_NULL,PX_Galgame_Module_StageRender,PX_Galgame_Module_StageFree,&Desc,sizeof(Desc));
	pDesc=PX_ObjectGetDesc(PX_Galgame_Module_Stage,pObject);

	//初始化数据
	pDesc->pRuntime=pRuntime;
	pDesc->pJsonFramework=pJson;
	pDesc->soundPlay=soundPlay;
	pDesc->elpased=0;
	pDesc->fm=fm;

	//加载资源

	//加载背景图片
	do 
	{
		px_char spayload[256]={0};
		PX_strcat(spayload,payload);
		PX_strcat(spayload,".image");
		PX_LoadTextureFromFile(&pRuntime->mp_resources,&pDesc->background,PX_JsonGetString(pJson,spayload));
	} while (0);

	PX_LoadTextureFromFile(&pRuntime->mp_resources,&pDesc->left_msgbox,"assert/left_msgbox.traw");
	PX_LoadTextureFromFile(&pRuntime->mp_resources,&pDesc->right_msgbox,"assert/right_msgbox.traw");

	//加载左边人物
	do 
	{
		px_char spayload[256]={0};
		PX_strcat(spayload,payload);
		PX_strcat(spayload,".actor");
		PX_LoadTextureFromFile(&pRuntime->mp_resources,&pDesc->actor,PX_JsonGetString(pJson,spayload));
	} while (0);

	//加载PainterEngine Live
	do 
	{
		PX_IO_Data data;
		data=PX_LoadFileToIOData("assert/release.live");
		PX_LiveFrameworkImport(&pRuntime->mp_resources,&pDesc->live,data.buffer,data.size);
		PX_FreeIOData(&data);
	} while (0);
	

	//加载选项
	pDesc->option_count=0;
	PX_memset(&pDesc->option,0,sizeof(pDesc->option));
	do 
	{
		px_int i;
		px_char spayload[256]={0};
		for (i=1;i<=8;i++)
		{
			const px_char *content;
			PX_sprintf2(spayload,sizeof(spayload),"%1.c_image%2",PX_STRINGFORMAT_STRING(payload),PX_STRINGFORMAT_INT(i));
			content=PX_JsonGetString(pJson,spayload);
			if (content[0])
			{
				
				//开始加载选项数据

				//纹理
				PX_LoadTextureFromFile(&pRuntime->mp_resources,&pDesc->option[i].image,content);
				//选择动作
				PX_sprintf2(spayload,sizeof(spayload),"%1.c_animation%2",PX_STRINGFORMAT_STRING(payload),PX_STRINGFORMAT_INT(i));
				content=PX_JsonGetString(pJson,spayload);
				PX_strcat(pDesc->option[i].animtion,content);

				//选择分数
				PX_sprintf2(spayload,sizeof(spayload),"%1.c_score%2",PX_STRINGFORMAT_STRING(payload),PX_STRINGFORMAT_INT(i));
				pDesc->option[i].score=(px_int)PX_JsonGetNumber(pJson,spayload);
				
				//选择对话
				PX_sprintf2(spayload,sizeof(spayload),"%1.c_script%2",PX_STRINGFORMAT_STRING(payload),PX_STRINGFORMAT_INT(i));
				content=PX_JsonGetString(pJson,spayload);
				PX_strcat(pDesc->option[i].c_content,content);

				//创建按钮
				pDesc->option[i].pushButton=PX_Object_PushButtonCreate(&pRuntime->mp_ui,pObject,-300,30,pDesc->option[i].image.width,pDesc->option[i].image.height,"",PX_NULL);
				PX_Object_PushButtonSetTexture(pDesc->option[i].pushButton,&pDesc->option[i].image);
				PX_Object_PushButtonSetBackgroundColor(pDesc->option[i].pushButton,PX_COLOR(0,0,0,0));
				PX_ObjectRegisterEvent(pDesc->option[i].pushButton,PX_OBJECT_EVENT_EXECUTE,PX_Galgame_Module_OnSelectButton,pObject);
				pDesc->option_count++;
			}
			else
			{
				break;
			}
		}
	} while (0);


	//加载对话脚本
	PX_VectorInitialize(&pRuntime->mp_game,&pDesc->talks,sizeof(PX_Galgame_Module_Stage_Talk),32);
	do 
	{
		px_int i;
		px_char spayload[256]={0};
		for (i=1;i<128;i++)
		{
			const px_char *content;
			PX_Galgame_Module_Stage_Talk talk;
			PX_memset(&talk,0,sizeof(talk));

			PX_sprintf2(spayload,sizeof(spayload),"%1.t_script%2",PX_STRINGFORMAT_STRING(payload),PX_STRINGFORMAT_INT(i));
			content=PX_JsonGetString(pJson,spayload);
			if (content[0])
			{
				PX_strcat(talk.t_content,content);
				PX_sprintf2(spayload,sizeof(spayload),"%1.t_animation%2",PX_STRINGFORMAT_STRING(payload),PX_STRINGFORMAT_INT(i));
				content=PX_JsonGetString(pJson,spayload);
				PX_strcat(talk.animation,content);
				PX_VectorPushback(&pDesc->talks,&talk);
			}
			else
			{
				break;
			}
		}
	} while (0);

	//加载背景音乐
	do 
	{
		px_char spayload[256]={0};
		PX_strcat(spayload,payload);
		PX_strcat(spayload,".sound");
		PX_LoadSoundFromFile(&pRuntime->mp_resources,&pDesc->soundData,PX_JsonGetString(pJson,spayload));
	} while (0);

	//////////////////////////////////////////////////////////////////////////
	//如果有背景音乐,直接播放
	if (pDesc->soundData.size)
	{
		PX_SoundPlayAdd(pDesc->soundPlay,PX_SoundCreate(&pDesc->soundData,PX_TRUE));
	}

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Galgame_Module_StageOnCursorDown,pObject);

	//
	if (pDesc->background.width)
	{
		pDesc->actorx=-pDesc->background.width;
	}
	pDesc->livex=pRuntime->surface_width+1;
	
	

	return pObject;
}
