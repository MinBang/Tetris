#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <conio.h>

#define BOARD_WIDTH 12
#define BOARD_HEIGHT 20
#define BOARD_X 5
#define BOARD_Y 1

#define LOSE 200

enum { 
		BLACK,      
		DARK_BLUE,   
		DARK_GREEN,   
		DARK_SKY_BLUE,   
		DARK_RED,   
		DARK_VIOLET, 
		DARK_YELLOW,  
		GRAY,       
		DARK_GRAY,   
		BLUE,      
		GREEN,      
		SKY_BLUE,   
		RED,     
		VIOLET,    
		YELLOW,      	
		WHITE,      	
}; 

enum {LEFT=75,RIGHT=77,UP=72,DOWN=80,SPACE=32};

struct Stage
{
	int speed;
	int stick_rate;
	int clear_line;
	int is_clear;
};

struct u_Block
{
	int x;
	int y;
	int shape;
	int angle;
	int color;
	bool exist;
};
Stage g_Stage[3]; // easy normal hard 3개의 난이도 구성이 있다
u_Block using_Block; // 현재 사용중 블록
u_Block Next_using_Block; // 다음 사용할 블록
int g_Map[BOARD_HEIGHT+2][BOARD_WIDTH+2] = {0,}; // 게입맵
int block[7][4][4][4]={
	1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,	1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,	1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,	1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,

	1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,	1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,	1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,	1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,

	0,1,0,0,1,1,0,0,0,1,0,0,0,0,0,0,	1,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,	1,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,	0,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,

	1,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,	1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,	1,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,	0,0,1,0,1,1,1,0,0,0,0,0,0,0,0,0,

	1,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0,	1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,	0,1,0,0,0,1,0,0,1,1,0,0,0,0,0,0,	1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,

	1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,	0,1,0,0,1,1,0,0,1,0,0,0,0,0,0,0,	1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,	0,1,0,0,1,1,0,0,1,0,0,0,0,0,0,0,

	0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,	1,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0,	0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,	1,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0

};

void removeCursor(void); // 커서 깜빡임을 제거한다
void gotoxy(int x, int y); // 원하는 좌표로 이동한다
void SetColor(int color); // 색상을 변경한다

u_Block CreateBlock(int diff); // 블록데이터 초기화
void Init(); // 필요한 기초 데이터들을 초기화한다
void Change_Block(int diff); // 다음 블록이 사용중 블록으로 바뀐뒤 재생성한다
void DrawMap(); // 맵을 그린다
void Show_Block(); // 사용중 블럭을 그린다.
void Move_Block(int diff); // 벽돌을 밑으로 떨어트린다
void Change_Angle(int diff);
void Erase(); // 잔상들을 
void Combine();
void Check_Clearline(int diff);
int Check_Collision(int shape,int angle, int x, int y);

int main()
{
	int key;
	
	int g_Diff;	

	srand(time(NULL));

	removeCursor();

	while(1)
	{
		SetColor(WHITE);
		system("cls");
		printf("게임 난이도를 골라주세요(0=쉬움,1=노말,2=하드)");
		scanf("%d",&g_Diff);
		system("cls");

		////////////////////////////////////////////

		Init();
		using_Block = CreateBlock(g_Diff);
		Next_using_Block = CreateBlock(g_Diff);
		DrawMap();
		Show_Block();
		
		for(int i=0;1;i++)
		{	
			//클리어 라인조건이 완족되면 true가 되고 모든 변수들이 초기화된다
			if(g_Stage[g_Diff].clear_line == 0)
			{
				system("cls");
				printf("당신이 게임에서 이겼습니다!!");
				Sleep(1000);
				break;
			}
			else if(g_Stage[g_Diff].is_clear == LOSE)
			{
				system("cls");
				printf("당신은 게임에서 졋습니�?ㅜ__ㅜ");
				Sleep(1000);
				break;
			}
			//사용중인 블록이 충돌조건에 의해 false상태가 됬을경�?교체작업
			if(using_Block.exist == FALSE)
			{
				Change_Block(g_Diff);	
			}
			
			if(kbhit())
			{
				key = _getch();

				if(key == 0xE0 || key == 0)
				{
					key = _getch();
					
					if(using_Block.y > -1)
					{
						switch(key) 
						{
						case UP:
							if(Check_Collision(using_Block.shape,using_Block.angle+1%4,using_Block.x,using_Block.y) == 0)
								Change_Angle(g_Diff);
							break;
						case LEFT:
							Erase();
							if(Check_Collision(using_Block.shape,using_Block.angle,using_Block.x-1,using_Block.y) == 0)
								using_Block.x--;
							Show_Block();
							break;
						case RIGHT:
							Erase();
							if(Check_Collision(using_Block.shape,using_Block.angle,using_Block.x+1,using_Block.y) == 0)
								using_Block.x++;						
							Show_Block();
							break;
						case DOWN:
							Move_Block(g_Diff);
							Show_Block();
							break;						
						}
					}
				}
				if(key == SPACE)
				{
					while(using_Block.exist != FALSE)
						Move_Block(g_Diff);
					Show_Block();						
				}
				if(key == 'q' || key == 'Q')
				{
					break;
				}
			}

			if(i%g_Stage[g_Diff].speed == 0)
			{				
				Move_Block(g_Diff);
				Show_Block();
				i=0;
			}
			Sleep(10);
		}
	}

	return 0;
}

void Check_Clearline(int diff)
{
	int count=0;
	int bomb=1;

	for(int i=1;i<BOARD_HEIGHT+1;i++)
	{
		for(int j=1;j<BOARD_WIDTH+1;j++)
		{
			if(g_Map[0][j] == 2)
			{
				g_Stage[diff].is_clear = 200;
				return;
			}
			else if(g_Map[i][j])
				count++;
			if(count == 12)
			{
				for(int z=i;z>0;z--)
				{
					for(int v=1;v<BOARD_WIDTH+1;v++)
					{
						g_Map[z][v] = g_Map[z-1][v];
					}
				}
				bomb++;
				g_Stage[diff].clear_line--;		
			}			
		}
		count = 0;
	}
}

void Combine()
{
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			if(block[using_Block.shape][using_Block.angle][i][j] == 1)
			{
				g_Map[using_Block.y+i][using_Block.x+j] = 2;
			}
		}
	}
}

void Erase()
{
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			if(block[using_Block.shape][using_Block.angle][i][j] == 1)
			{
				gotoxy(BOARD_X+(using_Block.x+j)*2,BOARD_Y+using_Block.y+i);
				printf("  ");
			}
			if(block[using_Block.shape][using_Block.angle][i][j] == 1)
			{
				gotoxy(Next_using_Block.x+j*2,Next_using_Block.y+i);
				printf("  ");
			}
		}
	}
}

void Change_Angle(int Diff)
{
	Erase();
	using_Block.angle=(using_Block.angle+1)%4;
	Show_Block();
}

int Check_Collision(int shape, int angle, int x, int y)
{
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			if(block[shape][angle][i][j] == 1 && g_Map[y+i][x+j] == 2)
			{
				return 1;
			}
		}
	}

	return 0;
}

void Move_Block(int diff)
{
	Erase();

	if(Check_Collision(using_Block.shape,using_Block.angle,using_Block.x,using_Block.y+1) == 0)
		using_Block.y++;
	else if(Check_Collision(using_Block.shape,using_Block.angle,using_Block.x,using_Block.y+1) == 1)
	{
		Combine();
		Check_Clearline(diff);
		using_Block.exist = FALSE;
		DrawMap();
	}	
}

void Show_Block()
{
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			if(BOARD_Y+using_Block.y+i >= 1 && using_Block.exist == TRUE)
			{				
				if(block[using_Block.shape][using_Block.angle][i][j] == 1)
				{
					SetColor(using_Block.color);
					gotoxy(BOARD_X+(using_Block.x+j)*2,BOARD_Y+using_Block.y+i);
					printf("■");
				}
			}
		}
	}

	//Next_using_Block을 표시
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			SetColor(Next_using_Block.color);
			gotoxy(Next_using_Block.x+j*2,Next_using_Block.y+i);

			if(block[Next_using_Block.shape][Next_using_Block.angle][i][j] == 1)
			printf("■");
		}
	}
}

void DrawMap()
{
	for(int i=0;i<BOARD_HEIGHT+2;i++)
	{
		for(int j=0;j<BOARD_WIDTH+2;j++)
		{
			if(i == BOARD_HEIGHT+1 || j == 0 || j == BOARD_WIDTH+1)
				SetColor(DARK_VIOLET);
			else
				SetColor(GRAY);

			gotoxy(BOARD_X+j*2,BOARD_Y+i);

			if(g_Map[i][j] == 2)
				printf("■");
			else
				printf("  ");
		}
	}

	SetColor(DARK_GRAY);
	gotoxy(40,7);
	printf("┌───────────┐");
	gotoxy(40,8);
	printf("│ ▲ UP    : 각도바꾸기│");
	gotoxy(40,9);
	printf("│ ◀ LEFT  : 좌측이동  │");
	gotoxy(40,10);
	printf("│ ▶ RIGHT : 우측이동  │");
	gotoxy(40,11);
	printf("│ ▼ DOWN  : 하단이동  │");
	gotoxy(40,12);
	printf("│ q 게임종료           │");
	gotoxy(40,13);
	printf("└───────────┘");
}

void Change_Block(int diff)
{
	using_Block.angle = Next_using_Block.angle;
	using_Block.exist = Next_using_Block.exist;
	using_Block.shape = Next_using_Block.shape;
	using_Block.color = Next_using_Block.color;
	using_Block.x=5;
	using_Block.y=-3;
	using_Block.exist = TRUE;

	Next_using_Block = CreateBlock(diff);

	Erase();
	Show_Block();
}

u_Block CreateBlock(int diff)
{
	u_Block t_Block;
	int tmp = rand()%100;
	int term = 4;

	if(tmp <= g_Stage[diff].stick_rate)
		t_Block.shape = 0;
	else
		t_Block.shape = rand()%6+1;

	t_Block.x = BOARD_X+((BOARD_WIDTH+2+term)*2);
	t_Block.y = BOARD_Y + 1;
	t_Block.angle = 0;
	t_Block.color = rand()%6+9;

	if(using_Block.exist == FALSE)
	{
		t_Block.x = (BOARD_WIDTH/2)-1;
		t_Block.y = -3;
		t_Block.exist = TRUE;
	}
	
	return t_Block;
}

void Init()
{
	//맵 데이터 초기화
	for(int i=0;i<BOARD_HEIGHT+2;i++)
	{
		for(int j=0;j<BOARD_WIDTH+2;j++)
		{
			if(i == BOARD_HEIGHT+1 || j == 0 || j == BOARD_WIDTH+1)
				g_Map[i][j] = 2;
			else
				g_Map[i][j] = 0;
		}
	}

	//현재 사용중인 블록에 대한 데이터값 초기화
	using_Block.angle = 0;
	using_Block.exist = 0;
	using_Block.shape = 0;
	using_Block.color = 0;
	using_Block.exist=FALSE;

	//대기중인 블록에 대한 데이터값 초기화
	Next_using_Block.angle = 0;
	Next_using_Block.exist = 0;
	Next_using_Block.shape = 0;
	Next_using_Block.color = 0;
	Next_using_Block.exist=FALSE;

	//g_Stage 데이터 초기화
	for(int i=0;i<3;i++)
	{
		g_Stage[i].is_clear = 0;
		g_Stage[i].clear_line = 20;
		g_Stage[i].speed = 35;
		g_Stage[i].stick_rate = 20;
		if(i > 0)
		{
			g_Stage[i].is_clear = FALSE;
			g_Stage[i].clear_line = g_Stage[i-1].clear_line + 20;
			g_Stage[i].speed = g_Stage[i-1].speed - 13;
			g_Stage[i].stick_rate = g_Stage[i-1].stick_rate - 6;
		}
	}
}

void removeCursor(void)
{ 
	CONSOLE_CURSOR_INFO curInfo; 
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo); 
	curInfo.bVisible=0; 
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo); 
} 

void gotoxy(int x, int y) 
{ 
	COORD pos={x,y}; 
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),pos); 
}

void SetColor(int color) 
{ 
	static HANDLE std_output_handle=GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(std_output_handle, color); 	
} 