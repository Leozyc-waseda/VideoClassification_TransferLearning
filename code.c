for read code
#pragma sfr
#pragma EI
#pragma DI
#pragma NOP

#define ON 0
#define OFF 1

#define C 382
#define D 341
#define E 303
#define F 286
#define G 255
#define A 227
#define B 202

#define RANGE 137		//音階の範囲設定　単位はmm 　 
#define mode2_range 280		//みんなで音階の範囲設定

#define WIDTH 10		//音階の範囲のヒステリシス範囲設定　単位 7はmm

#pragma interrupt INTTM04 channnel4_interrupt RB1
#pragma interrupt INTTM03 channnel3_interrupt RB0
#pragma interrupt INTTM00 tau0_ch0_intr RB2

//-----------------------------------------------
//インクルード
//-----------------------------------------------
#include <math.h>

//-----------------------------------------------
//プロトタイプ宣言
//-----------------------------------------------
unsigned long distance(void);
void hdinit(void);
void channel4_start(void);
void channel3_start(void);

void init_tmp(void);
double get_tmp(double vs);

void beep_init(void);
void beep_start(int freq,int oct);
void beep_end(void);
void beep_setFrequency(int freq,int oct);
void channel0_start(void);
//-----------------------------------------------
//グローバル変数定義
//-----------------------------------------------
static unsigned int tdr = 0;

static unsigned int i=0;
static unsigned int sonic_flag=0;


//デバッグ用
static unsigned long distance_watch=0;
static unsigned long distance_watch2=0;
double tmp_watch=0.0;
static sound_watch=0;

void main(void){
	
	
	unsigned long d = 0;
	static int sound_new = 0;
	static int sound_old = 0;
	static int range_min = 0;
	static int range_max = 0;
	long bb;
	
	DI();			//割り込み禁止
	
	init_tmp();		//温度センサ初期化
	hdinit();		//超音波センサ初期化
	beep_init();		//ブザー初期化
	channel4_start();	//超音波センサ送信トリガ開始
	channel3_start();	//超音波センサ受信開始
	channel0_start();	//ブザー開始
			
	EI();			//割り込み許可
	
	
	while(1){
		//フラグが立った時に距離を更新
		if(sonic_flag){
			d = distance();
		}
		distance_watch = d;
		//一人で mode
		//RANGE 137		//音階の範囲設定　単位はmm
		// WIDTH 10		//音階の範囲のヒステリシス範囲設定　単位 7はmm
		range_min = (40 + RANGE * sound_old - WIDTH);
		if (range_min < 40) range_min = 40;
		range_max = (40 + RANGE * (sound_old + 1) + WIDTH);
		
		if(range_min < d && d < range_max){
			beep_start(sound_old + 1, 4);
		}else{
			if (40 <= d){
				sound_new = (d - 40) / RANGE;
				if (0 <= sound_new && sound_new <= 6){
					sound_old = sound_new;
				}
				beep_start(sound_new + 1, 4);
			} else {
				beep_start(0, 4);
			}
		}

		// みんなで
}

}


//-----------------------------------------------
//概要:初期設定
//-----------------------------------------------
void hdinit(void){
//I/O割り当て
	PIOR0 = 0x00;		/*周辺I/Oリダイレクション・レジスタ0　タイマアレイユニット０の入六端子をP125に割り当て*/
	
//CPUクロック設定
	CMC = 0x00;		/*システムクロック動作モードレジスタ*/
	MSTOP = 1;		/*高速システム・クロックの動作制御　X1発振回路停止*/
	MCM0 = 0;		/*メイン・システム・クロックに高速オンチップ・オシレータ・クロック（f_IH）を選択*/
	XTSTOP = 1;		/*XT1発振回路停止*/
	OSMC = 0x10;		/*OSMCレジスタ　低速オンチップ・オシレータ・クロックを選択*/
	CSS = 0;		/*CPU/周辺ハードウェア・クロックにメイン・システム・クロックを選択*/
	HIOSTOP = 0;		/*高速オンチップ・オシレータ・クロックの動作制御　0:動作　　1:停止*/	
	
	
//ポート設定
	P0.1 = 0;		/*ポートレジスタ初期化*/
	PM0.1 = 0;		/*ポートP01　0:出力、1:入力*/
	PMC12.5=0;		/*ポートP125　デジタル入出力*/
	PM12.5 = 1;		/*ポートP125　0:出力、1:入力*/
	
	
	

//タイマ・アレイ・ユニット初期設定
	TAU0EN = 1;		/*周辺イネーブル・レジスタ　クロック供給*/
	TPS0 = 0x0050;		/*タイマ・クロック選択レジスタ　CK00：f_CLK    CK01:f_CLK/2^5   を使用*/
	TT0 = 0x0AFF;		/*タイマ・チャンネル停止レジスタ　ユニット0の全タイマ停止*/
	TMIF00 = 0;		/*割り込み要因INTTM00　割り込み要求フラグ*/
	TMMK00 = 1;		/*割り込み要因INTTM00　割り込みマスク・フラグ*/
	TMIF01 = 0;		/*割り込み要因INTTM01　割り込み要求フラグ*/
	TMMK01 = 1;		/*割り込み要因INTTM01　割り込みマスク・フラグ*/
	TMMK01H = 1;
	TMIF01H = 0;
	TMMK02 = 1;		/*割り込み要因INTTM02　割り込みマスク・フラグ*/
	TMIF02 = 0;		/*割り込み要因INTTM02　割り込み要求フラグ*/
	TMMK03 = 1;		/*割り込み要因INTTM03　割り込みマスク・フラグ*/
	TMIF03 = 0;		/*割り込み要因INTTM03　割り込み要求フラグ*/
	TMMK03H = 1;
	TMIF03H = 0;
	TMMK04 = 1;		/*割り込み要因INTTM04　割り込みマスク・フラグ*/
	TMIF04 = 0;		/*割り込み要因INTTM04　割り込み要求フラグ*/
	TMMK05 = 1;		/*割り込み要因INTTM05　割り込みマスク・フラグ*/
	TMIF05 = 0;		/*割り込み要因INTTM05　割り込み要求フラグ*/
	TMMK06 = 1;		/*割り込み要因INTTM06　割り込みマスク・フラグ*/
	TMIF06 = 0;		/*割り込み要因INTTM06　割り込み要求フラグ*/
	TMMK07 = 1;		/*割り込み要因INTTM07　割り込みマスク・フラグ*/
	TMIF07 = 0;		/*割り込み要因INTTM07　割り込み要求フラグ*/
	
//割り込み優先順位設定
	TMPR104 = 1;		/*INTTM04　優先順位指定フラグ*/
	TMPR004 = 1;		/*INTTM04　優先順位指定フラグ*/
	TMPR103 = 1;		/*INTTM03　優先順位指定フラグ*/
	TMPR003 = 1;		/*INTTM03　優先順位指定フラグ*/

//チャンネル４　CMU→超音波センサ　100us間隔で割り込み
	TMR04 = 0x0001;		/*タイマ・モード・レジスタ*/
	TDR04 = 0x0C7F;		/*1 / 32MHz = 0.0315us  100us = 0.0315 * 3200　3200 - 1 = 3199.  */
	TO0 = 0;		/*タイマ出力レジスタ　初期化*/
	TOE0 = 0x0000;		/*タイマ出力レジスタ書き込み許可　TO04を許可*/
	
	
	
//チャンネル3初期設定　超音波センサ→SMU
	TMR03 = 0x82CC;		/*タイマ・モード・レジスタ*/
	TIS0 = 0x00;		/*タイマ入力選択レジスタ*/
	TOM0 = 0;		/*タイマ出力モード・レジスタ　マスタ・チャンネル出力モード*/
	TOL0 = 0;		/*タイマ出力レベル・レジスタ　アクティブ・ハイに設定*/

//チャンネル0初期設定　ブザー用
	TMR00 = 0x8000;		/*タイマ・モード・レジスタ CK01：１MHｚを指定*/
	TMPR100 =1;		/*INTTM00　優先順位指定フラグ*/
	TMPR000 = 1;		/*INTTM00　優先順位指定フラグ*/
	TDR00 = 10U-1U;		/*1us *　10　= 10usごとに割り込み*/
	
	
	NFEN1 = 0x08;		/*ノイズ・フィルタ許可レジスタ　TNFEN03を許可*/
	
}
//-----------------------------------------------
//概要:チャンネル0動作開始関数
//説明：
//-----------------------------------------------
void channel0_start(void){
	TMIF00 = 0U;
	TMMK00 = 0U;
		
	TS0 |= 0x0001U;	/*タイマアレイユニットチャンネル0の動作を許可*/
	
}
//-----------------------------------------------
//概要:チャンネル4動作開始関数
//説明：
//-----------------------------------------------
void channel4_start(void){
	TMIF04 = 0;
	TMMK04 = 0;
	
	TS0 =0x0010;	/*タイマアレイユニットチャンネル4の動作を許可*/
	
}

//-----------------------------------------------
//概要:チャンネル3動作開始関数
//説明：
//-----------------------------------------------
void channel3_start(void){
	TMIF03 = 0;
	TMMK03 = 0;
	
	TS0 =0x0008;	/*タイマアレイユニットチャンネル3の動作を許可*/
	
}


//-----------------------------------------------
//概要:チャンネル4割り込み関数
//説明：超音波センサへの出力信号トリガ発生
//-----------------------------------------------
void channnel4_interrupt(void){

	static unsigned int count_60ms = 0;
	
	count_60ms++;		/*カウントアップ*/
	
	if(count_60ms == 600)
	{
		P0.1 = 1;	/*ポートP01をHigh*/
		count_60ms = 0;	/*カウントリセット*/
		
	}else{
		P0.1 = 0;	/*ポートP01をLow*/
	}
	
}

//-----------------------------------------------
//概要:チャンネル3割り込み関数
//説明：超音波センサからの受信信号
//-----------------------------------------------
void channnel3_interrupt(void){

	tdr = TDR03;	
	
	sonic_flag=1;//超音波受信フラグ
	//デバッグ用
	P6.6 = 0;//センサからのパルス幅計測完了割り込みでLED66点灯
	
}

//-----------------------------------------------
//概要:距離計算
//説明：
//-----------------------------------------------
unsigned long distance(void){
	double t_echo;
	unsigned long distance;
	
	t_echo = tdr/1000.0;// + 0.2;//32000.0;
	
	//distance = (long)(t_echo * 340.29 / 2);//音素による音速補正しない
	distance = (long)(t_echo * (331.5+0.61*get_tmp(3.3)) / 2);//温度取得し音速を補正
	
	//デバッグ用
	tmp_watch=get_tmp(3.3);
	distance_watch2=(long)(t_echo * 340.29 / 2);
	
	sonic_flag=0;//超音波受信フラグクリア
	
	return distance;
}
//-----------------------------------------------
//概要:温度センサ用端子・A/DC初期化
//説明：
//-----------------------------------------------
void init_tmp(void){
	ADCEN = 1U;// PER0.5 = 1 、 入力クロック供給、 A/D コンバータ
	ADM0 = 0x18U;
	// 変換動作停止、 A/D 電圧コンパレータ停止、セレクト・モード、変換速度設定
	// b7:ADCS:変換動作停止 ,b6: セレクト・モード
	/* b53:FR2 FR0: 変換クロック fclk /8,b2,1:LV1, 標準モード 1, */
	// b0:ADCE:A/D電圧コンパレータ動作禁止
	ADM1 = 0x20;
	/* ソフトウェア・トリガ・モード、ワンショット変換モード ,
	/* b7,b6:ADTMD1,ADTMD0:
	ソフトウェア・トリガ・モード ,b5: ワンショット変換
	モード ,b1,b0:ADTRS1,ADTRS0:TAU0 チャネル 1,,,(INTTM01) */
	ADM2 = 0x60U;
	// A/D コンバータ電源、 AVREFP,AVREFM から供給 ,10bit 分解能
	// b7,b6:ADREFP1,ADREFP0:P33/AVREFP/ANI0から供給
	// b5,b4:ADREFM:P34/AVREFM/ANI1から供給
	// b3:ADRCK:変換結果上限 下限チェック なし
	// b2:AWC:SNOOZEモード機能を使用しない
	// b0:ADTYP:10bit分解能
	ADS = 0x03U;//P81をアナログ入力に指定
	// アナログ入力チャネル :ANI2 を指定 セレクト・モード (ADMD=0)
	
	return;
}
//-----------------------------------------------
//概要:温度センサ取得・計算
//説明：温度センサへ電源電圧vsを引数
//-----------------------------------------------
double get_tmp(double vs){
	unsigned long counta=0;
	unsigned short int tmp_ad=0;
	double tmp_v=0.0;
	double tmp_c=0.0;
		
	ADCE = 1U;/* A/D 電圧コンパレータの動作許可
	/*安定待ち時間 B 1us */
	//安定待ち時間 ( をソフトウェアでカウントする。
	
	for (counta = 0U ; counta < 0x100U ; counta++){
			NOP();NOP();NOP();NOP();
			NOP();NOP();NOP();NOP();
			NOP();NOP();
	};
	ADIF = 0U;// A/D 変換割り込み要求クリア
		ADCS = 1U;// A/D 電圧コンパレータの動作許可

		do{// A/D 変換終了？
			NOP();
		}while (ADIF == 0U);

		ADIF = 0U;// A/D 変換割り込み要求クリア
		//tmp_ad_d=(double)(ADCR>>6);
		tmp_ad= (unsigned short int )(ADCR >> 6);//センサ出力電圧1024段階
		tmp_v=(tmp_ad)*vs/1024.0;//センサ出力(V)
		tmp_c=tmp_v*1000/6.25-67.84;
		
		ADCE = 0U;// A/D 電圧コンパレータの動作停止
		
		return tmp_c;
}
//ブザー用割り込み10us周期
__interrupt void tau0_ch0_intr(void){
	i++;
	P6.6=0;
}
//ブザーの初期化
void beep_init(void)
{
   	P14 &= 0xFEU;
 	PM14 &= 0xFEU;

	//EI();
}

//ブザーの鳴動開始
void beep_start(int freq,int oct)
{
	beep_setFrequency(freq,oct);
}

//ブザーの鳴動終了
void beep_end(void)
{
	beep_setFrequency(0,1);
}
//ブザーの周波数設定(freq;ド:1、レ:2、〜シ:7、それ以外:無音　oct：オクターブ「低:1〜高くなるにつれて+1( ´∀｀ )」）
void beep_setFrequency(int freq, int oct)
{
		switch(freq){
		case 1: if(i >= C/oct){
				P14.0 ^= 1;
				i = 0;
				break;
			}
			else 
			break;
		case 2: if(i >= D/oct){
				P14.0 ^= 1;
				i = 0;
				break;
			}
			else 
			break;
		case 3: if(i >= E/oct){
				P14.0 ^= 1;
				i = 0;
				break;
			}
			else 
			break;
		case 4: if(i >= F/oct){
				P14.0 ^= 1;
				i = 0;
				break;
			}
			else 
			break;
		case 5: if(i >= G/oct){
				P14.0 ^= 1;
				i = 0;
				break;
			}
			else 
			break;
		case 6: if(i >= A/oct){
				P14.0 ^= 1;
				i = 0;
				break;
			}
			else 
			break;
		case 7: if(i >= B/oct){
				P14.0 ^= 1;
				i = 0;
				break;
			}
			else 
			break;
		default: 
			P14.0 = 0;
			break;
	}
}




