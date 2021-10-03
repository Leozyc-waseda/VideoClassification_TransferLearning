録音の最後にまとめてフラッシュに書き込む(こっちでいきます)
	• 演奏
		○ 構文: void perform(unsigned char mode)
		○ 引数: unsigned char mode: 0->ヒトリデ, 1->ミンナデ
		○ 戻り値: void
		○ 概要: 演奏モード
		○ 詳細:
			§ //宣言と初期化
			§ double dist;
			§ int doremi;
			§ int octave;
			§ enum State state;
			§ state = PLAY;
		
			§ int sw_array[5][2];
			§ init_sw_array(sw_array);
		
			§ char sound_data[1024];
			§ char array_idx = 6;
		
			§ while(1) 
				□ //距離読んで、音鳴らして、LCD・LED表示するルーチン
				□ dist = cal_distance()
				□ doremi = dist2doremi(mode,dist)
				□ sound(doremi, octave)
				□ lcd_string = gen_lcd_string(mode, state, doremi, octave, dist)
				□ lcd_display(lcd_string)
				□ if state == PLAY || state == RECORD
					® rgb = doremi2rgb(doremi)
				□ else if ELAPSED_TIME % 500 == 0
					® rgb ^= 0xff0000
				□ color_led(rgb)
			
				□ //ボタントリガの処理ルーチン
				□ get_switch(sw_array)
				□ if sw_array[2][0] > sw_array[2][1]  //戻るボタン
					® if state == PLAY
						◊ stop_buzzer()
						◊ return;
					® else if state == STANDBY
						◊ state = PLAY
					® else
						◊ state = PLAY
						◊ finish_rec(sound_data, idx, number, rec_time, addr)
				□ if (sw_array[1][0] > sw_array[1][1]) && state == PLAY  //録音ボタン
					® state = STANDBY
					® ELAPSED_TIME = 0  (10ms単位)
					® rgb = 0x000000
				□ if (sw_array[3][0] > sw_array[3][1]) && state != STANDBY  //上ボタン
					® if octave < 7
						◊ octave++
				□ if (sw_array[4][0] > sw_array[4][1]) && state != STANDBY  //下ボタン
					® if octave > 0
						◊ octave--
			
				□ //状態管理の処理ルーチン
				□ if state == STANDBY
					® if ELAPSED_TIME > 300 (3000ms)
						◊ state = RECORD
						◊ ELAPSED_TIME = 0
				□ else if state == RECORD
					® DURATION = 0
					® record(doremi, octave, sound_data, idx)
					® if ELAPSED_TIME > 1000 (10,000ms 仮です)
						◊ state = FINE
						◊ ELAPSED_TIME = 0
						◊ rgb = 0x000000
				□ else if state == FINE
					® record(doremi, octave, sound_data, idx)
					® if ELAPSED_TIME > 3000
						◊ state = PLAY
						◊ finish_rec(sound_data, idx, number, rec_time, addr)


その他使用関数
	• 距離->ドレミ 
		○ 構文: unsigned int dist2doremi(unsign char mode,double dist)
		○ 引数:unsign char mode：0->ヒトリデ, 1->ミンナデ
		○           double dist：超音波センサからもらった距離
		○ 戻り値: int型のdoremi index：
		○ 000 – ド
		○ 001 – レ
		○ 010 – ミ
		○ 011 - ファ
		○ 100 – ソ
		○ 101 – ラ
		○ 110 – シ
		○ 111 – 無音
		○ 概要:距離→doremiの変換
		○ 詳細:
			
	• LCD入力文字列生成
		○ 構文: char* gen_lcd_string(unsigned char mode, unsigned char state, unsigned char doremi, unsigned char octave, double dist)
		○ 引数:unsigned char mode： 0->ヒトリデ, 1->ミンナデ
		○ 　　 unsigned char state：PLAY, STANDBY, RECORD, FINE
		○          unsigned char doremi, unsigned char octave, double dist
		○ 戻り値:LCD文字列情報
		○ 概要:LCD入力文字列用意関数
		○ 詳細:
		char* gen_lcd_string(unsigned char mode, unsigned char state, unsigned char doremi, unsigned char octave, double dist)
		{		// 戻り値:LCD文字列情報
				// 概要:LCD入力文字列用意関数
				char lcd_string[5][32]
				//mode： 0->ヒトリデ
				// state：PLAY, STANDBY, RECORD, FINE
				if (mode == 0){
					if state == 0    //ヒトリデPLAY、通常時
						lcd_string[0] = {'ﾋ', 'ﾄ', 'ﾘ', 'ﾃ', '゛'};
						lcd_string[1]=state
						lcd_string[2]=doremi
						lcd_string[3]=octave
						lcd_string[4]=dist 
						else if state == 1    //ヒトリデSTANDBY、録音ボタンを押したとき(カウントダウン)
							char lcd_string[32] = {}
						else    //ヒトリデRECORD or FINE　録音中
							char lcd_string[32] = {}
				}else{
						//mode：1->ミンナデ
						if state == 0    //ミンナデPLAY、通常時
							char lcd_string[32] = {}
						else if state == 1    //ミンナデSTANDBY、録音ボタンを押したとき(カウントダウン)
							char lcd_string[32] = {}
						else    //ミンナデRECORD or FINE、録音中
							char lcd_string[32] = {}
					}
				return lcd_string
		}
		
	• ドレミ->RGB
		○ 構文: int* doremi2rgb(unsigned int doremi)
		○ 引数: unsigned char doremi
		○ 戻り値: unsigned int rgb：フルカラーLEDに表示のrgb値
		○ 概要: doremi→rgb値の変換
		○ 詳細:
		○ 
	• ドレミ、オクターブ、デュレーション->2Bデータに格納
		○ 構文: unsigned short gen_data2b(unsinged char doremi, unsigned char octave, unsigned int duration)
		○ 引数:unsinged char doremi, unsigned char octave, 
			§ unsigned int duration：デュレーション
		○ 戻り値:unsigned short gen_data2b：2bytesの音源データ
		○ 概要:doremi, octave, durationの音源データ→2bytesデータに格納
		○ 詳細:
			§ return doremi << 13 | octave << 10 | (0x03ff & duration)
	• スイッチ入力を読み取る
		○ 構文: void get_switch(int sw_array[][2])
		○ 引数: sw_array = {{決定old, 決定new}, {録音old, 録音new}, {戻るold, 戻るnew}, {上old, 上new}, {下old, 下new}}
		○ 戻り値: void
		○ 概要: 5ボタンの新旧値を保持するsw_arrayを読み書きする。
		○ 詳細: 
			§ int sum = 0;
			§ for int i = 0; i < 5; i++
				□ t = switch_get(i)
				□ sum += t
				□ sw_array[i][0] = sw_array[i][1]
				□ sw_array[i][1] = t
			§ if sum > 1  //同時押しなら
				□ init_sw_array(sw_array)  //配列初期化する
	• スイッチ配列初期化
		○ void init_sw_array(int sw_array[][2])
		○ for (int i = 0; i < 5; i++){
			§ for (int j = 0; j < 2; j++){
				□ sw_array[i][j] = 0;
			§ }
		○ }
		○ 
		


//演奏モード Main flow
//0->ヒトリデ, 1->ミンナデ
void perform(unsigned char mode)
{
 	//宣言と初期化
 	double dist;//距離測定からもらった距離
 	int doremi;
 	int octave;
 	enum State state;//state : 0 Play, 1 Standby, 2 Record.
 	state = PLAY;
 	int sw_array[5][2];//5ボタンの新旧を保持スイッチ
 	init_sw_array(sw_array);//スイッチ配列初期化
 	char sound_data[1024];//音源データ
 	char array_idx = 6;

 	while(1){ 
	//距離読んで、音鳴らして、LCD・LED表示するルーチン
	dist = cal_distance();
	doremi = dist2doremi(mode,dist);
	sound(doremi, octave);//音鳴す
	lcd_string = gen_lcd_string(mode, state, doremi, octave, dist);//LCD入力文字列生成
	lcd_display(lcd_string);//show LCD文字列 

	if (state == PLAY || state == RECORD)
 		rgb = doremi2rgb(doremi);
	else if (ELAPSED_TIME % 500 == 0)//fullcolor led 点滅 rgb_value
	 	rgb ^= 0xff0000
	
	color_led(rgb);//show color to led

	//ボタントリガの処理ルーチン

	get_switch(sw_array);//スイッチ入力を読み取る
	if (sw_array[2][0] > sw_array[2][1])  //戻るボタン
 		{
 			if (state == PLAY){
				stop_buzzer();
 				return;
 			}
 		}else if (state == STANDBY)
 			state = PLAY;
		else
		{
 			state = PLAY;
			finish_rec(sound_data, idx, number, rec_time, addr);
		}
	if (sw_array[1][0] > sw_array[1][1]) && state == PLAY  //録音ボタン
 		state = STANDBY
 		ELAPSED_TIME = 0  (10ms単位)
 		rgb = 0x000000
	if (sw_array[3][0] > sw_array[3][1]) && state != STANDBY  //上ボタン
 	if octave < 7
	 	octave++
	if (sw_array[4][0] > sw_array[4][1]) && state != STANDBY  //下ボタン
 	if octave > 0
		octave--
	//状態管理の処理ルーチン
	if state == STANDBY
 	if ELAPSED_TIME > 300 (3000ms)
		state = RECORD
		ELAPSED_TIME = 0
	else if state == RECORD
 		DURATION = 0
 		record(doremi, octave, sound_data, idx)
 	if ELAPSED_TIME > 1000 (10,000ms 仮です)
		state = FINE
		ELAPSED_TIME = 0
		rgb = 0x000000
	else if state == FINE
 		record(doremi, octave, sound_data, idx)
 	if ELAPSED_TIME > 3000
		state = PLAY
		finish_rec(sound_data, idx, number, rec_time, addr)
}

