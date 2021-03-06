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
void Initialfield(void);   //fieldの初期化
void drawingBlock(void);   //ブロックの描画
void moveBlock(int nextx, int nexty); //ブロックの移動
void control(int key);  //ブロックのコントロール
bool collision(int nextx, int nexty); //ブロックの衝突(移動先が空白でなければ移動できないなど)
void SetBLock(void);    //ブロックの固定
void TurnBlock(void);   //ブロックの回転
void checkLine(void);   //ラインの判定
void deleteLines(int line); //下の行を消す
void StrageBlock(void); //ブロックの保存

int field[WIDTH][HEIGHT];
int board[WIDTH][HEIGHT];
int blockx = FIRST_X; //ブロックの初期x座標
int blocky = FIRST_Y; //ブロックの初期y座標
int block[BLOCK_SIZE][BLOCK_SIZE];
int sumline = 0;
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
doubleLayer layers;  // ダブルバッファ用のデータ
int windowID;  // ウィンドウの番号
int layerID;  // レイヤーの番号


/// main　関数 ///
int main() {
	hgevent *event;
	int key;

	windowID = HgOpen(600,750);
	layers = HgWAddDoubleLayer(windowID);
	HgWSetEventMask(windowID, HG_KEY_DOWN);

	GameField();
	Initialfield();
	create_block();
	drawingBlock();
	HgSleep(1.0);

	while (1) {
		layerID = HgLSwitch(&layers);
        HgLClear(layerID);
		GameField();
		drawingBlock();
		event = HgEventNonBlocking();

		if (event != NULL) {  //キー入力
            key = event->ch;
		    control(key);
	    }
		if (collision(blockx, blocky-1)) {
			moveBlock(blockx, blocky-1);
			//blocky--;
			HgSleep(0.5);
		}else{
			SetBLock();
			checkLine();
			blockx = FIRST_X;
		 	blocky = FIRST_Y;
		 	create_block();
		 	HgSleep(1.0);
		}
	}
	HgGetChar();
    HgClose();
	return 0;
}

//フィールドの描写
void GameField(void){
	HgWSetFillColor(layerID,HG_SKYBLUE);
	HgWBoxFill(layerID,0,0,600,750,0);
	HgWSetFillColor(layerID,HG_BLACK);
	HgWBoxFill(layerID,0,0,360,750,0);
	HgWSetFillColor(layerID,HG_LGRAY);
	HgWBoxFill(layerID,0,0,360,30,0);
	HgWBoxFill(layerID,0,0,30,750,0);
	HgWBoxFill(layerID,330,0,30,750,0);
}

//画面の初期化
void Initialfield(void){
	int x,y;
	for (x = 0; x < WIDTH; x++) {
		for (y = 0; y < HEIGHT; y++) {
			field[x][y] = board[x][y] = 0;
			if (x == 0 || x == 11 || y == 0) {
				field[x][y] = board[x][y] = WALL;
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
						HgWSetFillColor(layerID,HG_SKYBLUE);
						break;
					case 2:
						HgWSetFillColor(layerID,HG_RED);
						break;
					case 3:
						HgWSetFillColor(layerID,HG_PURPLE);
						break;
					case 4:
						HgWSetFillColor(layerID,HG_GREEN);
						break;
					case 5:
						HgWSetFillColor(layerID,HG_BLUE);
						break;
					case 6:
						HgWSetFillColor(layerID,HG_PINK);
						break;
					case 7:
						HgWSetFillColor(layerID,HG_YELLOW);
						break;
					case 8:
						HgWSetFillColor(layerID,HG_ORANGE);
						break;
				}
				HgWBoxFill(layerID, 30 * x, 30 * y, 30, 30, 1);
			}
		}
	}
}
//ブロック生成関数
void create_block(void) {
	int typeB;   //ブロックの種類
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
			//ブロックの初期座標に入力
			field[i+blockx][j+blocky] = board[i+blockx][j+blocky] + block[i][j];
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
			field[x+blockx][y+blocky] -= block[x][y];
		}
	}

	//座標更新
	blockx = nextx;
	blocky = nexty;

	//次の場所の描画
	for (x = 0; x < BLOCK_SIZE; x++) {
		for (y = 0; y < BLOCK_SIZE; y++) {
			field[x+blockx][y+blocky] += block[x][y];
		}
	}
}

//衝突判定
bool collision(int nextx, int nexty) {
	int x,y;

	for (x = 0; x < BLOCK_SIZE; x++) {
		for (y = 0; y < BLOCK_SIZE; y++) {
			//ブロックであるとき
			if (block[x][y] >= 1) {
				//移動した後の座標が 0 以外なら移動できない
				if (board[x+nextx][y+nexty] != 0) {
					return false;  //移動できない
				}else{
					break;
				}
			}
		}
	}
	return true;  //移動した後が0なら移動できる
}

//固定用配列
void SetBLock(void) {
	int x,y;

	for (x = 0; x < BLOCK_SIZE; x++) {
		for (y = 0; y < BLOCK_SIZE; y++) {
			board[x][y] = field[x][y];
		}
	}
}

//回転用関数
void TurnBlock(void) {
	int strage[BLOCK_SIZE][BLOCK_SIZE];  //ブロック保存用配列
	int x,y;

	//回転前を保存
	for (x = 0; x < BLOCK_SIZE; x++) {
		for (y = 0; y < BLOCK_SIZE; y++) {
			strage[x][y] = block[x][y];
		}
	}
	//ブロックの回転
	for (x = 0; x < BLOCK_SIZE; x++) {
		for (y = 0; y < BLOCK_SIZE; y++) {
			block[x][y] = strage[BLOCK_SIZE-1-y][x];
		}
	}
	//回転後が衝突するなら、回転前に戻して中止する
	if (collision(blockx, blocky) == false) {
		for (x = 0; x < BLOCK_SIZE; x++) {
			for (y = 0; y < BLOCK_SIZE; y++) {
				block[x][y] = strage[x][y];
			}
		}
		return;
	}
	//回転後のブロックを field に読み込む
	for (x = 0; x < BLOCK_SIZE; x++) {
		for (y = 0; y < BLOCK_SIZE; y++) {
			field[x+blockx][y+blocky] = block[x][y];
		}
	}
}

//ブロック操作関数
void control(int key) {
	if (key == HG_U_ARROW) { //回転
		TurnBlock();
	}else if (key == HG_L_ARROW) {
		if (collision(blockx-1, blocky)) { //移動可能なら左に移動
			printf("debag\n");
			moveBlock(blockx-1, blocky);
			//blockx--;
		}
	}else if (key == HG_R_ARROW) {
		if (collision(blockx+1, blocky)) { //移動可能なら右に移動
			moveBlock(blockx+1, blocky);
			//blockx++;
		}
	}else if (key == HG_D_ARROW) {
		if (collision(blockx, blocky-1)) { //移動可能なら下に移動
			moveBlock(blockx, blocky-1);
			//blocky--;
		}
	}
}

//列の消去判定
void checkLine(void) {
	int line = 0;
	bool check;
	int x,y;

	for (y = 1; y < HEIGHT-1; y++) {
		check = true;
		for (x = 1; x < WIDTH-1; x++) {
			if (board[x][y] == 0) {
				check = false;
				break;
			}
		}
		if (check == true) {  //横一列が揃っていたら
			deleteLines(y);
			line++;
		}
	}
	linesum += line; //消したラインを加算
}

//列の消去と１段下にずらす
void deleteLines(int line) {
	int x,y;

	//横１列を消去
	for (x = 1; x < WIDTH-1; x++) {
		board[x][line] = 0;
	}

	//全体を一段ずらす
	for (y = line; y > 0; y--) {
		for (x = 1; x < WIDTH-1; x++) {
			board[x][y] = board[x][y-1];
		}
	}

	//boardをfieldに反映する
	for (x = 0; x < WIDTH; x++) {
		for (y = 0; y < HEIGHT; y++) {
			field[x][y] = board[x][y];
		}
	}
}
