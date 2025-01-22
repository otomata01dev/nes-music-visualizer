#include "../lib/neslib.h"
#include "../chr/main_char.h"

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

    pal_bg(main_pal);

    vram_adr(NAMETABLE_A);
    vram_write((unsigned char *)main_nam, 1024);

    put_str(NTADR_A(6, 3), "Music Player for NES!"); // 画面に文字列を表示

    ppu_on_all(); // PPUをオンにする
}

// メイン関数
void main() {
    setup_graphics();
    while (1) {
        ppu_wait_nmi();
    }
}
