#include "../lib/neslib.h"
#include "../lib/famistudio.h"
#include "../chr/main_char.h"

// 曲データ
extern const unsigned char music_data[];

// 文字列をタイルセット番号に変換してタイルを描画する関数
void put_str(unsigned int adr, const char *str) {
    vram_adr(adr);
    while(1) {
        if(!*str) break;
        vram_put((*str++));
    }
}

// 画面表示のセットアップ関数
void setup_graphics() {
    ppu_off(); // PPUをオフにする

    pal_bg((const char *)main_pal);

    vram_adr(NAMETABLE_A);
    vram_write((unsigned char *)main_nam, 1024);

    put_str(NTADR_A(6, 3), "Music Player for NES!"); // 画面に文字列を表示

    ppu_on_all(); // PPUをオンにする
}

// 再生フラグ
unsigned char is_playing = 0;

// 入力チェック関数
void check_input() {
    // パッドの入力を取得
    unsigned char pad = pad_trigger(0);

    // STARTボタンが押されたら曲を再生
    if (pad & PAD_START) {
        if (!is_playing) {
            famistudio_music_play(0);
            is_playing = 1;
        }
    }

    // SELECTボタンが押されたら曲を停止
    if (pad & PAD_SELECT) {
        if (is_playing) {
            famistudio_music_stop();
            is_playing = 0;
        }
    }
}

// メイン関数
void main() {
    setup_graphics();

    // FamiStudioの初期化
    famistudio_init(FAMISTUDIO_PLATFORM_NTSC, &music_data);
    while (1) {
        check_input();
        famistudio_update();
        ppu_wait_nmi();
    }
}
