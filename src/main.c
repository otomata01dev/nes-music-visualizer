#include "../lib/neslib.h"
#include "../lib/famistudio.h"
#include "../chr/main_char.h"
#include "../lib/visualizer.h"

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

// VRAMバッファ
unsigned char volume_vram_buffer[11*5+1] = {
    MSB(NTADR_A(8, 13))|NT_UPD_HORZ, LSB(NTADR_A(8, 13)), 8,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,

    MSB(NTADR_A(8, 17))|NT_UPD_HORZ, LSB(NTADR_A(8, 17)), 8,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,

    MSB(NTADR_A(8, 21))|NT_UPD_HORZ, LSB(NTADR_A(8, 21)), 8,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,

    MSB(NTADR_A(8, 25))|NT_UPD_HORZ, LSB(NTADR_A(8, 25)), 8,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,

    MSB(NTADR_A(8, 27))|NT_UPD_HORZ, LSB(NTADR_A(8, 27)), 8,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    
    NT_UPD_EOF
};

unsigned char prev_volume_square1 = 0;
unsigned char prev_volume_square2 = 0;
unsigned char prev_volume_triangle = 0;
unsigned char prev_volume_noise = 0;
unsigned char prev_volume_dpcm = 0;

void volume_bar_update() {
    unsigned char volume_square1 = visualizer_get_volume(0);
    unsigned char volume_square2 = visualizer_get_volume(1);
    unsigned char volume_triangle = visualizer_get_volume(2);
    unsigned char volume_noise = visualizer_get_volume(3);
    unsigned char volume_dpcm = visualizer_get_volume(4);
    int i;

    for (i = 0; i < 8; i++) {
        if (prev_volume_square1 == volume_square1) break;
        volume_vram_buffer[3 + i] = i < volume_square1 ? 0x0E : 0x0F;
    }

    for (i = 0; i < 8; i++) {
        if (prev_volume_square2 == volume_square2) break;
        volume_vram_buffer[14 + i] = i < volume_square2 ? 0x0E : 0x0F;
    }

    for (i = 0; i < 8; i++) {
        if (prev_volume_triangle == volume_triangle) break;
        volume_vram_buffer[25 + i] = i < volume_triangle ? 0x0E : 0x0F;
    }

    for (i = 0; i < 8; i++) {
        if (prev_volume_noise == volume_noise) break;
        volume_vram_buffer[36 + i] = i < volume_noise ? 0x0E : 0x0F;
    }

    for (i = 0; i < 8; i++) {
        if (prev_volume_dpcm == volume_dpcm) break;
        volume_vram_buffer[47 + i] = i < volume_dpcm ? 0x0E : 0x0F;
    }

    if (volume_square1 != prev_volume_square1) {
        prev_volume_square1 = volume_square1;
    }
    if (volume_square2 != prev_volume_square2) {
        prev_volume_square2 = volume_square2;
    }
    if (volume_triangle != prev_volume_triangle) {
        prev_volume_triangle = volume_triangle;
    }
    if (volume_noise != prev_volume_noise) {
        prev_volume_noise = volume_noise;
    }
    if (volume_dpcm != prev_volume_dpcm) {
        prev_volume_dpcm = volume_dpcm;
    }
}

// 画面表示のセットアップ関数
void setup_graphics() {
    ppu_off(); // PPUをオフにする

    pal_bg((const char *)main_pal);

    vram_adr(NAMETABLE_A);
    vram_write((unsigned char *)main_nam, 1024);

    put_str(NTADR_A(6, 6), "Music Player for NES!"); // 画面に文字列を表示

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

    // VRAMバッファを設定。引数のVRAMバッファの変更を次のフレームで反映
    set_vram_update(volume_vram_buffer);
    while (1) {
        ppu_wait_nmi();
        check_input();
        famistudio_update();
        volume_bar_update();
    }
}
