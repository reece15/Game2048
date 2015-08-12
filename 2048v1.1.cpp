\\
\\	2048pc��
\\
\\	vc6.0 Easyx������
\\
\\	2015 1 25	
\\      
\\	by 0xcc



#include <stdio.h>
#include <graphics.h>
#include <conio.h>
#include <time.h>
#pragma comment(lib, "winmm.lib")

#define MAP_XY 132	//������
#define BLACK_B  1	//��ɫ����߽���
#define BLACK_M 10	//�����߽���

IMAGE block_rel;	//����ͼ
IMAGE block;	//��Ĥͼ
IMAGE draw_block;	//��Ҫ���Ƶķ���
IMAGE bg;	//����ͼ��
FILE *fp;	//ȫ��   ������־�ļ�ָ��

void init_map(int map_check[4][4]);	//��ʼ����ͼ��Ϣ
void welcome();
int rand_put(int map_check[4][4], int *full_num, int mirror[4][4]);	//���������һ������
int flush_map(int map_check[4][4], int rand_flag);			//ˢ�µ�ͼ
int get_use(IMAGE *buffer, int flag);		//�õ���Ҫ�õ��ķ���
void move(int map_check[4][4]);			//�ƶ����񲢴������
void check_key(int map_check[4][4], int mirror[4][4], int *full_num, int *useful_key);	//�������ļ��Ƿ��ı�ԭͼ
int over_check(int map_check[4][4]);		//��Ϸʧ�ܼ��
void winner_check(int value);			//��Ϸʤ�����
void close_game();				//�ر���Ϸ

int main(void)
{
	int full_num = 0;
	int map_check[4][4]; //0 �հ�  ������ֵ 2 4 8 16 32������
	int useful_key = 1;  //�Ƿ��ǻ�����ͼ�θı�ļ�ֵ��־
	int mirror[4][4];	//ԭͼ����
	
	if (!(fp = fopen("debug_info.log", "wt")))
	{
		exit(1);
	}
	initgraph(580, 580);
	loadimage(&bg, "images/bg.bmp");
	loadimage(&block, "images/block_1.jpg");
	loadimage(&block_rel, "images/block_rel.bmp");
	welcome();
	init_map(map_check);
	srand((unsigned int)time(NULL));
	BeginBatchDraw();		//������ͼ
	while (1)
	{
		if (useful_key == 1)		//������ļ�ֵ������ͼ���ƶ�ʱ  
		{
			mciSendString("open audio/move.mp3 alias move", NULL, 0, NULL);
			mciSendString("play move", NULL, 0, NULL);
			Sleep(100);	
			rand_put(map_check, &full_num, mirror);
			flush_map(map_check, 1);		//ˢ�µ�ͼ
		}
		if (full_num == 16)			//����ͼȫ��ʱ
		{
			if(!over_check(map_check))	//��ͼȫ�� ��û�����ڵ���ͬ��ֵʱ
			{
				close_game();
			}
		}
		move(map_check);			//�ı�ͼ��
		flush_map(map_check, 0);	//ˢ�µ�ͼ			
		if (useful_key == 1)
		{
			useful_key = 0;
			mciSendString("stop move", NULL, 0, NULL);
			mciSendString("close move", NULL, 0, NULL);
		}
		check_key(map_check, mirror, &full_num, &useful_key);
	}
	return 0;
}

void welcome()
{
	MOUSEMSG m;

	loadimage(NULL, "images/welcome_5.jpg");
	while (1)
	{
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN && m.x >= 200 && m.x <= 390 && m.y <= 530 && m.y >= 460)
		{
			break;
		}
	}
	putimage(0, 0, &bg);
}

void init_map(int map_check[4][4])
{
	int i, j;
	
	fprintf(fp, "API-init_map:\n");
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			map_check[i][j] = 0;
			fprintf(fp, "%4d", map_check[i][j]);
		}
		fprintf(fp, "\n");
	}
}

int rand_put(int map_check[4][4], int *full_num, int mirror[4][4])
{
	int stop;		
	int i, j, count = 0;
	int rand_num = 16 - *full_num - 1;	//�������Χ
	int flag_chance = 0;	//��ѡ����
	
	fprintf(fp, "\n\nAPI--rand_put:");
	fflush(fp);
	if (*full_num == 16)
	{
		fprintf(fp, "\nreturn: 1");
		fflush(fp);
		return 1;
	}
	else if (*full_num == 15)
	{
		stop = 0;
	}
	else
	{
		stop = rand()%rand_num;	//��������� ȡʣ��ո��еĵ�stop���ո�
	}
	fprintf(fp, "\nstop:%d", stop);
	fflush(fp);
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (!map_check[i][j] && !flag_chance)
			{
				if (count == stop)
				{
					map_check[i][j] = ((rand()%100 > 3 ) ? 2 : 4);	//�������2����4
					fprintf(fp, "\nrand_xy:%d %d\nrand_result:%d", i, j, map_check[i][j]);
					fflush(fp);
					*full_num += 1;
					flag_chance = 1;
				}
				count += 1;
			}
			mirror[i][j] = map_check[i][j];	//����δ�ı��ͼ
		}
	}
	return 0;
}

int flush_map(int map_check[4][4], int rand_flag)
{
	int i, j, count = 0, tmp_x = 0, tmp_y = 44;
	IMAGE buffer;
	LOGFONT f;
	char string[5];

	fprintf(fp, "\n\nAPI--flush_map:");
	fflush(fp);
	setbkmode(TRANSPARENT);
	setcolor(BLACK);
	getfont(&f);                          
	f.lfHeight = 42;                     
	strcpy(f.lfFaceName, "����");        
	f.lfQuality = ANTIALIASED_QUALITY;    
	setfont(&f);                          // ����������ʽ
	if (!rand_flag)
	{
		putimage(0, 0, &bg);
	}
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (map_check[i][j] != 0)
			{
				switch (map_check[i][j])
				{
				case 2: get_use(&buffer, 8);break;
				case 4: get_use(&buffer, 7);break;
				case 8: get_use(&buffer, 6);break;
				case 16: get_use(&buffer, 5);break;
				case 32: get_use(&buffer, 4);break;
				case 64: get_use(&buffer, 3);break;
				case 128: get_use(&buffer, 2);break;
				case 256: get_use(&buffer, 1);break;
				default: get_use(&buffer, 0);break;
				}
				if (map_check[i][j] < 10)
				{
					tmp_x = 54; 
				}
				else if (map_check[i][j] < 100)
				{
					tmp_x = 44;
				}
				else if(map_check[i][j] < 1000)
				{
					tmp_x = 34;
				}
				else
				{
					tmp_x = 24;
				}
				putimage(BLACK_M + (BLACK_M + MAP_XY) * j, BLACK_M + (BLACK_M + MAP_XY) * i, &block, SRCAND);
				putimage(BLACK_M + (BLACK_M + MAP_XY) * j, BLACK_M + (BLACK_M + MAP_XY) * i, &buffer, SRCPAINT);	//ʹ����Ԫ��դ��ӡ��ͼ
				sprintf(string, "%d", map_check[i][j]);
				outtextxy(BLACK_M + (BLACK_M + MAP_XY) * j + tmp_x, BLACK_M + (BLACK_M + MAP_XY) * i + tmp_y, string);
			}
		}
	}
	FlushBatchDraw();
	fprintf(fp, "\nreturn:0");
	fflush(fp);
	return 0;
}

int get_use(IMAGE *buffer, int flag)
{
	SetWorkingImage(&block_rel);
	getimage(buffer, BLACK_B + (BLACK_B + MAP_XY) * (flag%3), BLACK_B + (BLACK_B + MAP_XY) * (flag/3), MAP_XY, MAP_XY);
	SetWorkingImage();
	return 0;
}

void move(int map_check[4][4])
{
	int i, j, k;
	int swap_space[4];
	char key;
	
	fprintf(fp, "\n\nAPI--MOVE:");
	fflush(fp);
	key = getch();
	fflush(stdin);
	fprintf(fp, "\n\nkey:%c", key);
	fflush(fp);
	if (key == 'S' || key == 's')
	{
		for (j = 0; j < 4; j++)
		{
			memset(swap_space, 0, sizeof(swap_space));
			k = 0;
			for (i = 3; i >= 0; i--)	//����һ�У��У�
			{
				if (map_check[i][j] != 0)	//������ֵ����swap_space  
				{
					swap_space[k] = map_check[i][j];
					k++;
				}
			}
			for (i = 0; i < k - 1; i++)
			{
				if (swap_space[i] == swap_space[i+1])	//�ж�����Ԫ���Ƿ���ͬ
				{
					swap_space[i] += swap_space[i+1];
					swap_space[i+1] = 0;
					if (i+2 < k-1)		//�ж��Ƿ������ ��aabb����ʽ
					{
						if(swap_space[i+2] == swap_space[i+3])
						{
							swap_space[i+1] += swap_space[i+2] + swap_space[i+2];
							swap_space[i+2] = swap_space[i+3] = 0;
							break;
						}
					}
				}
				else if (swap_space[i] == 0) //�Ƿ�����
				{
					swap_space[i] += swap_space[i+1];
					swap_space[i+1] = 0;
				}
			}
			for (i = 3, k = 0; i >= 0 ; i--)	//���У��У����¸�ֵ
			{
				map_check[i][j] = swap_space[k];
				winner_check(swap_space[k]);	//ʤ�����
				k++;
			}
		}
	}
	else if (key == 'W' || key == 'w')
	{
		for (j = 0; j < 4; j++)
		{
			memset(swap_space, 0, sizeof(swap_space));
			k = 0;
			for (i = 0; i < 4; i++)
			{
				if (map_check[i][j] != 0)
				{
					swap_space[k] = map_check[i][j];
					k++;
				}
			}
			for (i = 0; i < k - 1; i++)
			{
				if (swap_space[i] == swap_space[i+1])
				{
					swap_space[i] += swap_space[i+1];
					swap_space[i+1] = 0;
					if (i+2 < k-1)
					{
						if(swap_space[i+2] == swap_space[i+3])
						{
							swap_space[i+1] += swap_space[i+2] + swap_space[i+2];
							swap_space[i+2] = swap_space[i+3] = 0;
							break;
						}
					}
				}
				else if (swap_space[i] == 0)
				{
					swap_space[i] += swap_space[i+1];
					swap_space[i+1] = 0;
				}
			}
			for (i = 0, k = 0; i < 4; i++)
			{
				map_check[i][j] = swap_space[k];
				winner_check(swap_space[k]);
				k++;
			}
		}
	}
	else if (key == 'A' || key == 'a')
	{
		for (i = 0; i < 4; i++)
		{
			memset(swap_space, 0, sizeof(swap_space));
			k = 0;
			for (j = 0; j < 4; j++)
			{
				if (map_check[i][j] != 0)
				{
					swap_space[k] = map_check[i][j];
					k++;
				}
			}
			for (j = 0; j < k - 1; j++)
			{
				if (swap_space[j] == swap_space[j+1])
				{
					swap_space[j] += swap_space[j+1];
					swap_space[j+1] = 0;
					if (j+2 < k-1)
					{
						if(swap_space[j+2] == swap_space[j+3])
						{
							swap_space[j+1] += swap_space[j+2] + swap_space[j+2];
							swap_space[j+2] = swap_space[j+3] = 0;
							break;
						}
					}
				}
				else if (swap_space[j] == 0)
				{
					swap_space[j] += swap_space[j+1];
					swap_space[j+1] = 0;
				}
			}
			for (j = 0, k = 0; j < 4; j++)
			{
				map_check[i][j] = swap_space[k];
				winner_check(swap_space[k]);
				k++;
			}
		}	
	}
	else if (key == 'D' || key == 'd')
	{
		for (i = 0; i < 4; i++)
		{
			memset(swap_space, 0, sizeof(swap_space));
			k = 0;
			for (j = 3; j >= 0; j--)
			{
				if (map_check[i][j] != 0)
				{
					swap_space[k] = map_check[i][j];
					k++;
				}
			}
			for (j = 0; j < k - 1; j++)
			{
				if (swap_space[j] == swap_space[j+1])
				{
					swap_space[j] += swap_space[j+1];
					swap_space[j+1] = 0;
					if (j+2 < k-1)
					{
						if(swap_space[j+2] == swap_space[j+3])
						{
							swap_space[j+1] += swap_space[j+2] + swap_space[j+2];
							swap_space[j+2] = swap_space[j+3] = 0;
							break;
						}
					}
				}
				else if (swap_space[j] == 0)
				{
					swap_space[j] += swap_space[j+1];
					swap_space[j+1] = 0;
				}
			}
			for (j = 3, k = 0; j >= 0; j--)
			{
				map_check[i][j] = swap_space[k];
				winner_check(swap_space[k]);
				k++;
			}
		}
	}
	else if (key == 27)
	{
		exit(1);
	}
	else
	{
		fprintf(fp, "\nreturn:void");
		fflush(fp);
		return;
	}
}

void check_key(int map_check[4][4], int mirror[4][4], int *full_num, int *useful_key)
{
	int i, j;

	*full_num = 0;
	fprintf(fp, "\n\nAPI--check_key:\n");
	fflush(fp);
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (map_check[i][j] != 0)	
			{
				*full_num += 1;		//ͳ�Ʒ�����ֵ����
			}
			if(mirror[i][j] != map_check[i][j])
			{
				*useful_key = 1;	//ͳ���ƶ���ͼ���Ƿ��ԭ����һ�� ��һ����֤�������KEY�ɸı�ͼ��
			}
			fprintf(fp, "%4d", map_check[i][j]);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "\nfull_num:%d\nuseful_key:%d\n", *full_num, *useful_key);
	fflush(fp);
}
int over_check(int map_check[4][4])
{
	int i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)	//�κ�һ����ֵ�������ϣ��£�����ֻҪ����ͬ����ͼ�λ��ܸı�
		{
			if (j-1 >= 0)
			{
				if (map_check[i][j] == map_check[i][j-1])
				{
					return 1;
				}
			}
			if (i-1 >= 0)
			{
				if (map_check[i][j] == map_check[i-1][j])
				{
					return 1;
				}
			}
			if (i+1 < 4)
			{
				if (map_check[i][j] == map_check[i+1][j])
				{
					return 1;
				}
			}
			if (j+1 < 4)
			{
				if (map_check[i][j] == map_check[i][j+1])
				{
					return 1;
				}
			}
		}
	}
	return 0;
}

void winner_check(int value)
{
	LOGFONT f;

	EndBatchDraw();
	fprintf(fp, "\n\nAPI--winner-check:\n");
	fflush(fp);
	if (value == 2048)	//�ۼӳ���2048��ʤ��
	{
		setcolor(RGB(0, 0, 0));
		getfont(&f);                          
		f.lfHeight = 72;                     
		strcpy(f.lfFaceName, "����");        
		f.lfQuality = ANTIALIASED_QUALITY;                            // ����������ʽ
		setfont(&f);
		outtextxy(140, 250, "��ϲͨ��!");
		getfont(&f);
		f.lfHeight = 12;
		setfont(&f);
		outtextxy(200, 200, "��ϲͨ�أ������ڴ���������!");
		Sleep(5000);
		closegraph();
		fprintf(fp, "\n\nAPI--CLSOE\nEXIT:PASSED");
		fclose(fp);
		exit(1);
	}
	fprintf(fp, "return:void");
	fflush(fp);
}

void close_game()
{
	LOGFONT f;
	
	EndBatchDraw();
	getch();
	getfont(&f);                          
	f.lfHeight = 72;                     
	strcpy(f.lfFaceName, "����");        
	f.lfQuality = ANTIALIASED_QUALITY;    
	setfont(&f);                          // ����������ʽ
	setcolor(RGB(0, 0, 0));
	outtextxy(140, 240, "����ʧ��!");
	getfont(&f); 
	f.lfHeight = 12;
	setfont(&f);
	outtextxy(240, 200, "����ʧ�ܣ����������!");
	Sleep(5000);
	fprintf(fp, "\n\nAPI--CLSOE\nEXIT:FAILED");
	fflush(fp);
	fclose(fp);
	closegraph();
	exit(1);
}