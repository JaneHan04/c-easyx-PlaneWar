#include <stdio.h>
#include <graphics.h> //图形库，可以帮助我们快速入门图形编程 easyx
#include <conio.h>
#include <time.h> //定时器要用到
#include <mmsystem.h> //导入声音头文件库
#pragma comment(lib,"winmm.lib")//导入声音的链接库
#include <Windows.h>

//枚举
enum My
{
	WIDTH = 591, //窗口宽度高度
	HEIGHT = 862,
	BULLET_NUM = 15, //玩家子弹数量
	ENEMY_NUM = 10,//敌机数量
	BIG,
	SMALL
};
struct Plane
{
	int x;
	int y;
	bool live;//是否存活
	int width;
	int height;
	int hp;
	int type;//敌机的类型  big small
	bool exploding = false;
}player, bull[BULLET_NUM],enemy[ENEMY_NUM];

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//定义所用到的图片
IMAGE home;//主菜单
IMAGE start;//开始游戏界面
IMAGE about; //关于游戏界面
IMAGE highscore;//最高得分界面
IMAGE Over;//游戏结束界面
IMAGE bk;//背景图片
IMAGE img_role[2];//玩家图片
IMAGE img_bull[2];//子弹图片
IMAGE img_enemy[2][2]; //两种敌机
IMAGE img_boom[6];//爆炸效果图

//全局变量
ExMessage msg;
int score = 0; //本局游戏得分
int totalTime = 60;//本局游戏时长
int startTime = 0;//游戏开始时间
bool GameStart = false;//游戏是否开始
bool GameOver = false;//游戏是否结束

////////////////////////////////////////////////////////////////////////////////////////////////

////用户交互模块
void Home(ExMessage* msg);//主菜单界面
void startGame();//开始游戏界面
void gameOver(ExMessage* msg);//游戏结束界面
void highScore(ExMessage* msg);//最高得分界面
void aboutGame(ExMessage* msg);//关于游戏界面
bool isInRect(ExMessage* msg, int x, int y, int w, int h);//鼠标是否在某个矩形区域

////游戏模块
void loadImg();//加载图片
void gameInit();//游戏初始化
void gameDraw();//游戏的绘制函数
bool CheckCollision(Plane& plane, Plane& enemy);//碰撞检测
void playerMove(int speed);//飞机移动，获取键盘消息，上下左右
void createBullet();//创建子弹
void BulletMove(int speed);//子弹移动
void createEnemy();//产生敌机
void enemyMove(int speed);//敌机的移动
void enemyHp(int i);//敌机血量
void playPlance();//打飞机
void Score(int score);//得分
bool Timer(int ms, int id);//定时器
void displayRemainingTime(int gameDuration, int startTime);//游戏剩余时长

////音乐
void menuMusic();//主菜单音乐
void closemenuMusic();//关闭主菜单音乐
void playMusic();//开始游戏音乐
void closeplayMusic();//关闭游戏音乐
void overYesMusic();//结算创造最高分音乐
void overNoMusic();//结算没创造最高分音乐

////文件操作模块
void highestscore(int score);//写入最高得分
int lookhighest();//读取最高分

////////////////////////////////////////////////////////////////////////////////////////////////

////用户交互模块///////////////////////////////////////////////////////////////////////////////////////////

//主菜单界面
void Home(ExMessage* msg)
{
	loadimage(&home, "images/option/home.jpg");
	putimage(0, 0, &home);
	menuMusic();
	while (true)
	{
		while (peekmessage(msg, EM_MOUSE))
		{
			//开始游戏
			if (isInRect(msg, 180, 510, 240, 60))
			{
				if (msg->message == WM_LBUTTONDOWN)
				{
					closemenuMusic();
					startGame();
				}
			}
			//最高得分
			else if (isInRect(msg, 180, 610, 240, 60))
			{
				if (msg->message == WM_LBUTTONDOWN)
				{
					highScore(msg);
				}
			}
			//关于游戏
			else if (isInRect(msg, 180, 710, 240, 60))
			{
				if (msg->message == WM_LBUTTONDOWN)
				{
					aboutGame(msg);
				}
			}
		}
	}
}
//开始游戏界面
void startGame()
{
	startTime = time(0); //游戏开始时间
	GameStart = true;
	gameInit();
	//双缓冲绘图
	BeginBatchDraw();
	while (!GameOver)
	{
		playMusic();//开始游戏音乐
		Sleep(10);
		gameDraw();
		if ((time(0) - startTime) > totalTime) {
			GameStart = false;
			GameOver = true;
			break;
		}
		displayRemainingTime(totalTime, startTime);
		FlushBatchDraw();
		playerMove(5);
		BulletMove(20);

		if (Timer(500, 0))
		{
			createEnemy();
		}
		if (Timer(10, 2))
		{
			enemyMove(2);
		}

		playPlance();

		//printf("移动成功\n");
	}
	if (GameOver)
	{
		gameOver(&msg);
		//关音乐
		closeplayMusic();
	}
	EndBatchDraw();

}
//最高得分界面
void highScore(ExMessage* msg)
{
	loadimage(&highscore, "images/option/highscore.jpg");
	putimage(0, 0, &highscore);

	int grade = lookhighest();

	char a[20]; // 用于存储最高得分的字符串

	setbkmode(TRANSPARENT); // 设置文本背景透明
	settextcolor(BLACK); // 设置文本颜色为黑色

	settextstyle(60, 0, _T("宋体"));
	sprintf_s(a, "最高得分：%d", grade); // 将得分转换为字符串
	outtextxy(130, 350, a); // 在屏幕左上角位置绘制得分

	while (true)
	{
		while (peekmessage(msg, EM_MOUSE))
		{
			if (isInRect(msg, 30, 30, 55, 55))
			{
				if (msg->message == WM_LBUTTONDOWN)
				{
					loadimage(&home, "images/option/home.jpg");
					putimage(0, 0, &home);
					Home(msg);
				}
			}
		}
	}
}
//关于游戏界面
void aboutGame(ExMessage* msg)
{
	loadimage(&about, "images/option/about.jpg");
	putimage(0, 0, &about);
	while (true)
	{
		while (peekmessage(msg, EM_MOUSE))
		{
			if (isInRect(msg, 30, 30, 55, 55))
			{
				if (msg->message == WM_LBUTTONDOWN)
				{
					loadimage(&home, "images/option/home.jpg");
					putimage(0, 0, &home);
					Home(msg);
				}
			}
		}
	}
}
//游戏结束界面
void gameOver(ExMessage* msg)
{
	setbkmode(TRANSPARENT); // 设置文本背景透明
	settextcolor(BLACK); // 设置文本颜色为黑色
	loadimage(&Over, "images/option/gameover.jpg");
	putimage(0, 0, &Over);

	int hs = lookhighest();
	if (score > hs)
	{
		overYesMusic();
		highestscore(score);//游戏结束存入最高得分
		settextstyle(50, 0, _T("宋体"));
		const char* message = "恭喜你!!!!";
		const char* message1 = "创造了最高记录！！！！";
		outtextxy(100, 500, message);
		outtextxy(100, 600, message1);
	}
	else
	{
		overNoMusic();
		settextstyle(50, 0, _T("宋体"));
		const char* message = "菜就多练";
		const char* message1 = "输不起别玩";
		outtextxy(100, 500, message);
		outtextxy(100, 600, message1);
	}
	char scoreText[20]; // 用于存储得分的字符串

	settextstyle(60, 0, _T("宋体"));
	sprintf_s(scoreText, "%d", score); // 将得分转换为字符串
	outtextxy(280, 385, scoreText); // 在屏幕左上角位置绘制得分

}
//鼠标是否在某个矩形区域（isInRect是左上角坐标，左上角到右下角截图分辨率）
bool isInRect(ExMessage* msg, int x, int y, int w, int h)
{
	if (msg->x > x && msg->x<x + w && msg->y>y && msg->y < y + h)
		return true;
	return false;
}

////游戏模块///////////////////////////////////////////////////////////////////////////////////////////

//加载图片
void loadImg()
{
	//加载背景图片 591*862
	//相对路径： ./表示当前文件夹下， ../表示文件夹的上一级目录  ./background.jpg
	//图片必须放在和cpp文件同一个文件夹下
	loadimage(&bk, "images/background.jpg");

	//加载玩家图片(制作掩码图）
	loadimage(&img_role[0], "images/me1_1.jpg");
	loadimage(&img_role[1], "images/me1_2.jpg");

	//加载子弹图片(制作掩码图）
	loadimage(&img_bull[0], "images/bullet1_1.jpg");
	loadimage(&img_bull[1], "images/bullet1_2.jpg");
	//加载敌机图片(制作掩码图）
	loadimage(&img_enemy[0][0], "images/enemy1_1.jpg");
	loadimage(&img_enemy[0][1], "images/enemy1.jpg");

	loadimage(&img_enemy[1][0], "images/enemy2_1.jpg");
	loadimage(&img_enemy[1][1], "images/enemy2.jpg");
	//加载敌机爆炸图片
	loadimage(&img_boom[0], "images/enemy_down1.jpg");
	loadimage(&img_boom[1], "images/enemy_down1.1.jpg");
	loadimage(&img_boom[2], "images/enemy2_down2.jpg");
	loadimage(&img_boom[3], "images/enemy2_down2.1.jpg");
	//我方飞机爆炸图片
	loadimage(&img_boom[4], "images/me_destroy_3.1.jpg");
	loadimage(&img_boom[5], "images/me_destroy_3.jpg");

}
//游戏初始化
void gameInit()
{
	loadImg();
	player.x = WIDTH / 2;
	player.y = HEIGHT - 126;
	player.live = true;

	//初始化子弹
	for (int i = 0; i < BULLET_NUM; i++)
	{
		bull[i].x = 0;
		bull[i].y = 0;
		bull[i].live = false;
	}
	//初始化敌机
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		enemy[i].live = false;
		enemyHp(i);
	}

}
//游戏的绘制函数
void gameDraw()
{
	//把背景图片贴到窗口上
	putimage(0, 0, &bk);
	Score(score);

	//先贴掩码图，再贴素材
	putimage(player.x, player.y, &img_role[0], NOTSRCERASE);
	putimage(player.x, player.y, &img_role[1], SRCINVERT);

	//绘制子弹
	for (int i = 0; i < BULLET_NUM; i++)
	{
		if (bull[i].live)
		{
			putimage(bull[i].x, bull[i].y, &img_bull[0], NOTSRCERASE);
			putimage(bull[i].x, bull[i].y, &img_bull[1], SRCINVERT);
		}
	}

	//绘制敌机
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (enemy[i].live)
		{
			if (enemy[i].type == BIG)
			{
				putimage(enemy[i].x, enemy[i].y, &img_enemy[1][0], NOTSRCERASE);
				putimage(enemy[i].x, enemy[i].y, &img_enemy[1][1], SRCINVERT);
			}
			else
			{
				putimage(enemy[i].x, enemy[i].y, &img_enemy[0][0], NOTSRCERASE);
				putimage(enemy[i].x, enemy[i].y, &img_enemy[0][1], SRCINVERT);
			}
		}
		if (enemy[i].hp <= 0)
		{
			if (enemy[i].type == BIG)
			{
				putimage(enemy[i].x, enemy[i].y, &img_boom[2], NOTSRCERASE);
				putimage(enemy[i].x, enemy[i].y, &img_boom[3], SRCINVERT);

			}
			else
			{
				putimage(enemy[i].x, enemy[i].y, &img_boom[0], NOTSRCERASE);
				putimage(enemy[i].x, enemy[i].y, &img_boom[1], SRCINVERT);
			}
		}
	}
}
//碰撞检测
bool CheckCollision(Plane& plane, Plane& enemy) {
	// 矩形碰撞框检测示例
	if (plane.x < enemy.x + enemy.width &&
		plane.x + plane.width > enemy.x &&
		plane.y < enemy.y + enemy.height &&
		plane.y + plane.height > enemy.y) {
		return true;
	}
	return false;
}
//飞机移动，获取键盘消息，上下左右
void playerMove(int speed)
{
#if 0 //WSAD 不顺畅
	//检测是否有键盘按下，如果有键按下，则返回真，否则返回假
	if (_kbhit())
	{
		//两种方式
		//1，_getch()  阻塞函数，和scanf一样，如果没有输入就会卡着程序，一直等待输入，不是C语言标准函数,需要包含头文件conio.h
		char key = _getch();
		switch (key)
		{
		case 'w':
		case 'W':
			player.y -= speed;
			break;
		case 's':
		case 'S':
			player.y += speed;
			break;
		case 'a':
		case 'A':
			player.x -= speed;
			break;
		case 'd':
		case 'D':
			player.x += speed;
			break;
		}
	}
#elif 1  //WSAD和小上下左右都可以，运动很顺畅
	//2，使用windows函数获取键盘输入 GetAsyncKeyState  非阻塞函数，会非常的流畅
	//检测键盘输入，字母必须用大写（大小写都能检测到）,小写什么都检测不到
	if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W'))
	{
		if (player.y > 0)
		{
			player.y -= speed;
		}
	}
	if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S'))
	{
		if (player.y + 126 < HEIGHT)
		{
			player.y += speed;
		}
	}
	if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A'))
	{
		if (player.x + 40 > 0)
		{
			player.x -= speed;
		}
	}
	if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D'))
	{
		if (player.x + 102 - 40 < WIDTH)
		{
			player.x += speed;
		}
	}

#endif // 0	
	if (GetAsyncKeyState(VK_SPACE) && Timer(200, 1))
	{
		//创建一个子弹
		createBullet();
	}
}
//创建子弹
void createBullet()
{
	for (int i = 0; i < BULLET_NUM; i++)
	{
		if (!bull[i].live)
		{
			bull[i].x = player.x + 51;
			bull[i].y = player.y;
			bull[i].live = true;
			break;
		}
	}
}
//子弹移动
void BulletMove(int speed)
{
	for (int i = 0; i < BULLET_NUM; i++)
	{
		if (bull[i].live)
		{
			bull[i].y -= speed;
			if (bull[i].y < 0)
			{
				bull[i].live = false;
			}
		}
	}
}
//产生敌机
void createEnemy()
{
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (!enemy[i].live)
		{
			enemy[i].live = true;
			enemy[i].x = rand() % (WIDTH - enemy[i].width);
			enemy[i].y = 0;
			enemyHp(i);
			break;
		}
	}

}
//敌机的移动
void enemyMove(int speed)
{
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (enemy[i].live)
		{
			enemy[i].y += speed;
			if (enemy[i].y > HEIGHT)
			{
				enemy[i].live = false;
			}
		}
	}
}
//敌机血量
void enemyHp(int i)
{
	int flag = rand() % 10;
	if (flag >= 0 && flag <= 2) //0-9
	{ //大敌机
		enemy[i].type = BIG;
		enemy[i].hp = 3;
		enemy[i].width = 69;
		enemy[i].height = 99;
	}
	else
	{ //小敌机
		enemy[i].type = SMALL;
		enemy[i].hp = 1;
		enemy[i].width = 57;
		enemy[i].height = 43;
	}
}
//打飞机
void playPlance()
{
	for (int i = 0; i < ENEMY_NUM; ++i) {
		if (CheckCollision(player, enemy[i])) {
			// 碰撞发生，处理飞机爆炸效果和游戏结束
			player.exploding = true;
			GameOver = true; // 设置游戏结束标志
			break; // 可以提前退出循环，因为游戏已经结束
		}
	}
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (!enemy[i].live) continue;
		for (int k = 0; k < BULLET_NUM; k++)
		{
			if (!bull[k].live) continue;
			if (bull[k].x > enemy[i].x && bull[k].x< enemy[i].x + enemy[i].width
				&& bull[k].y>enemy[i].y && bull[k].y < enemy[i].y + enemy[i].height)
			{
				bull[k].live = false;
				enemy[i].hp--;
			}
		}
		if (enemy[i].hp <= 0)
		{
			if (enemy[i].type == BIG) score += 2;
			else score += 1;
			Score(score);
			enemy[i].live = false;
		}
	}
}
//得分
void Score(int score)
{
	char scoreText[20]; // 用于存储得分的字符串

	setbkmode(TRANSPARENT); // 设置文本背景透明
	settextcolor(BLACK); // 设置文本颜色为黑色

	settextstyle(30, 0, _T("宋体"));
	sprintf_s(scoreText, "本局得分: %d", score); // 将得分转换为字符串
	outtextxy(30, 130, scoreText); // 在屏幕左上角位置绘制得分
}
//定时器
bool Timer(int ms, int id)
{
	if (GameStart)
	{
		static DWORD t[10];
		if (clock() - t[id] > ms)
		{
			t[id] = clock();
			return true;
		}
		return false;
	}
}
//游戏剩余时长
void displayRemainingTime(int gameDuration, int startTime) {
	setbkcolor(WHITE); // 设置背景色为白色
	settextcolor(BLACK); // 设置文本颜色为黑色
	// 计算剩余时间
	int currentTime = time(0);
	int elapsedTime = currentTime - startTime;
	int remainingTime = gameDuration - elapsedTime;

	// 绘制剩余时长
	char timeString[30]; // 确保缓冲区足够大
	settextstyle(30, 0, _T("宋体"));
	sprintf_s(timeString, "剩余时长: %d s", remainingTime);
	outtextxy(30, 100, timeString); // 在 (100, 100) 处绘制剩余时长文本

	// 如果剩余时间小于等于 0，显示游戏结束信息
	if (remainingTime <= 0) {
	}
}

////音乐///////////////////////////////////////////////////////////////////////////////////////////

//主菜单音乐
void menuMusic()//主菜单音乐
{
	mciSendString("open asset/major.mp3", NULL, 0, NULL);
	mciSendString("play asset/major.mp3", NULL, 0, NULL);
}
//关闭主菜单音乐
void closemenuMusic()
{
	mciSendString("stop asset/major.mp3", NULL, 0, NULL);
	mciSendString("close asset/major.mp3", NULL, 0, NULL);
}
//开始游戏音乐
void playMusic()//开始游戏音乐
{
	mciSendString("open asset/back.mp3", NULL, 0, NULL);
	mciSendString("play asset/back.mp3 repeat", NULL, 0, NULL); //循环播放
}
void overNoMusic()
{
	mciSendString("open asset/over2.mp3", NULL, 0, NULL);
	mciSendString("play asset/over2.mp3", NULL, 0, NULL);
}
//关闭游戏音乐
void closeplayMusic()
{
	mciSendString("stop asset/back.mp3", NULL, 0, NULL);
	mciSendString("close asset/back.mp3", NULL, 0, NULL);
}
//结算创造最高分音乐
void overYesMusic()
{
	mciSendString("open asset/over.mp3", NULL, 0, NULL);
	mciSendString("play asset/over.mp3", NULL, 0, NULL);
}
//结算没创造最高分音乐


////文件操作模块///////////////////////////////////////////////////////////////////////////////////////////

//写入最高分
void highestscore(int s)
{
	FILE* file;
	errno_t err;

	// 使用 fopen_s 安全地打开文件
	err = fopen_s(&file, "highest_score.txt", "w");
	if (err != 0) {
		printf("无法打开文件 highest_score.txt\n");
		return;
	}

	// 将最高得分写入文件
	fprintf(file, "%d", s);
	fclose(file);
}
//读取最高得分
int lookhighest()
{
	FILE* file;
	int hScore = 0;
	errno_t err;

	// 使用 fopen_s 安全地打开文件
	err = fopen_s(&file, "highest_score.txt", "r");
	if (err != 0) {
		printf("无法打开文件 highest_score.txt\n");
		return hScore;
	}

	// 从文件中读取最高得分
	fscanf_s(file, "%d", &hScore);
	fclose(file);

	printf("最高得分是: %d\n", hScore);

	return hScore;
}

////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
	
	initgraph(591, 862); // 初始化图形界面
	//initgraph(591, 862, SHOWCONSOLE);  
	Home(&msg);
	return 0;
}