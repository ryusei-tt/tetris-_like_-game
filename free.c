/*
	blockgame.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <handy.h>
#include <time.h>

#define WIDTH 12
#define HEIGHT 25
#define TYPE 8
#define WALL -1
#define BLOCK 1
#define BLOCK_SIZE 4
#define FIRST_X 4
#define FIRST_Y 20


void GameField(void);
void create_block(void);  //落下するブロック生成
void InitialScreen(void);   //画面の初期化
void drawingBlock(void);   //ブロックの描画
void moveBlock(int nextx, int nexty); //ブロックの移動
void control(void);  //ブロックのコントロール
bool collision(int nextx, int nexty); //ブロックの衝突(移動先が空白でなければ移動できないなど)
void SetBLock(void);    //ブロックの固定
void TurnBlock(int key);   //ブロックの回転
void DeletBottom(void); //下の行を消す
void StrageBlock(void); //ブロックの保存

int field[WIDTH][HEIGHT];
int blockx = FIRST_X; //ブロックの初期x座標
int blocky = FIRST_Y; //ブロックの初期y座標
int block[BLOCK_SIZE][BLOCK_SIZE];
int blockform[TYPE][BLOCK_SIZE][BLOCK_SIZE] = {
	//長方形
	{{0, 1, 0, 0},
	 {0, 1, 0, 0},
	 {0, 1, 0, 0},
	 {0, 1, 0, 0}},
	 //稲妻型
	{{0, 2, 0, 0},
	 {0, 2, 2, 0},
	 {0, 0, 2, 0},
	 {0, 0, 0, 0}},
	 //逆稲妻型
	{{0, 0, 3, 0},
	 {0, 3, 3, 0},
	 {0, 3, 0, 0},
	 {0, 0, 0, 0}},
	 //L字型
	{{0, 4, 0, 0},
	 {0, 4, 0, 0},
	 {0, 4, 4, 0},
	 {0, 0, 0, 0}},
	 //逆L字型
	{{0, 0, 5, 0},
	 {0, 0, 5, 0},
	 {0, 5, 5, 0},
	 {0, 0, 0, 0}},
	 //凸型
	{{0, 6, 0, 0},
	 {6, 6, 6, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},
	 //凹型
	{{0, 0, 0, 0},
	 {7, 0, 7, 0},
	 {7, 7, 7, 0},
	 {0, 0, 0, 0}},
	 //階段型
	{{0, 0, 0, 0},
	 {0, 8, 0, 0},
	 {0, 8, 8, 0},
	 {0, 0, 0, 0}}
};
int linesum = 0; //消したラインの数
int gameover = false; //ゲームオーバーのフラグ

/// main　関数 ///
int main() {
	HgOpen(600,750);
	GameField();
	InitialScreen();
	create_block();
	drawingBlock();
	HgSleep(1.0);

	while (1) {
		HgClear();
		GameField();
		if (collision(blockx, blocky-1)) {
			moveBlock(blockx, blocky-1);
			drawingBlock();
			blocky--;
			HgSleep(0.5);
		}else{
			 blockx = FIRST_X;
			 blocky = FIRST_Y;
			 create_block();
			 drawingBlock();
			 HgSleep(2.0);
		 }
	 }
	HgGetChar();
    HgClose();
	return 0;
}

//フィールドの描写
void GameField(void){
	HgSetFillColor(HG_SKYBLUE);
	HgBoxFill(0,0,600,750,0);
	HgSetFillColor(HG_BLACK);
	HgBoxFill(0,0,360,750,0);
	HgSetFillColor(HG_LGRAY);
	HgBoxFill(0,0,360,30,0);
	HgBoxFill(0,0,30,750,0);
	HgBoxFill(330,0,30,750,0);
}

//画面の初期化
void InitialScreen(void){
	int x,y;
	for (x = 0; x < WIDTH; x++) {
		for (y = 0; y < HEIGHT; y++) {
			field[x][y] = 0;
			if (x == 0 || x == 11 || y == 0) {
				field[x][y] = WALL;
			}
		}
	}
}

//ブロックの描画関数
void drawingBlock(void) {
	int x,y;

	for (x = 0; x < WIDTH; x++) {
		for (y = 0; y < HEIGHT; y++) {
			if (field[x][y] >= 1) {
				switch (field[x][y]) {
					case 1:
						HgSetFillColor(HG_SKYBLUE);
						break;
					case 2:
						HgSetFillColor(HG_RED);
						break;
					case 3:
						HgSetFillColor(HG_PURPLE);
						break;
					case 4:
						HgSetFillColor(HG_GREEN);
						break;
					case 5:
						HgSetFillColor(HG_BLUE);
						break;
					case 6:
						HgSetFillColor(HG_PINK);
						break;
					case 7:
						HgSetFillColor(HG_YELLOW);
						break;
					case 8:
						HgSetFillColor(HG_ORANGE);
						break;
				}
				HgBoxFill(30 * (x+1), 30 * (y+1), 30, 30, 1);
			}
		}
	}
}
//ブロック生成関数
void create_block(void) {
	int typeB;
	int i,j;

	srand(time(NULL));
	typeB = rand() % TYPE;

	//生成するブロックを　blockform　から読み込む
	for (i = 0; i < BLOCK_SIZE; i++) {
		for (j = 0; j < BLOCK_SIZE; j++) {
			block[i][j] = blockform[typeB][i][j];
		}
	}

	//field にブロックを読み込む
	for (i = 0; i < BLOCK_SIZE; i++) {
		for (j = 0; j < BLOCK_SIZE; j++) { //ブロックを定義した形に出力
			field[i+FIRST_X][j+FIRST_Y] =  block[i][j]; //ブロックの初期座標に入力
		}
	}
	for (i = 0; i < WIDTH; i++) {    // 配列確認用
		for (j = 0; j < HEIGHT; j++) {
			printf("%d   ", field[i][j]);
		}
		printf("\n");
	}
}

//ブロックの移動関数
void moveBlock(int nextx, int nexty) {
	int x,y;

	//現在の位置を消去
	for (x = 0; x < BLOCK_SIZE; x++) {
		for (y = 0; y < BLOCK_SIZE; y++) {
			field[x+FIRST_X][y+FIRST_Y] = 0;
		}
	}
	//次の場所の描画
	for (x = 0; x < BLOCK_SIZE; x++) {
		for (y = 0; y < BLOCK_SIZE; y++) {
			field[x+nextx][y+nexty] = block[x][y];
		}
	}
}



//衝突判定
bool collision(int nextx, int nexty) {
	int x,y;
	for (x = 0; x < BLOCK_SIZE; x++) {
		for (y = 0; y < BLOCK_SIZE; y++) {
			//ブロックである
			if (block[x][y] >= 1) {
				//移動した後の座標が 0 以外なら移動できない
				if (field[x+nextx][y+nexty] != 0) {
					return false;
				}else{
					break;
				}
			}
		}
	}
	return true;  //移動できる
}

void TurnBlock(int key) {
	//hgevent *event;

}
