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

// 音量バーの前回の音量
unsigned char prev_volume[5] = {0, 0, 0, 0, 0};

// 音量バーのVRAMバッファのオフセット
const unsigned char volume_vram_buffer_index_offset[5] = {3, 14, 25, 36, 47};

// 音量バーのVRAMバッファの更新関数
void update_volume_vram_buffer(unsigned char channel, unsigned char volume) {
    unsigned char i;
    unsigned char buffer_index = volume_vram_buffer_index_offset[channel];
    
    if (prev_volume[channel] == volume) return;

    for (i = 0; i < 8; i++) {
        volume_vram_buffer[buffer_index + i] = i < volume ? 0x0E : 0x0F;
    }

    prev_volume[channel] = volume;
}

// 音量バーの更新関数
void volume_bar_update() {
    unsigned char volume;
    unsigned char i;

    // 各チャンネルの音量を取得し、VRAMバッファを更新
    for (i = 0; i < 5; i++) {
        volume = visualizer_get_volume(i);
        update_volume_vram_buffer(i, volume);
    }
}

// 鍵盤スプライトの描画に関する定数
const unsigned char channel_keyboard_y[3] = {112, 144, 176}; // 各チャンネルの鍵盤スプライトのY座標
const unsigned char channel_sprite_id[3] = {0, 4, 8}; // 各チャンネルのスプライトID
const unsigned char key_tile[12] = { // 鍵盤スプライトのタイル番号
    0xC0, 0xC8, 0xC2, 0xC8, 0xC4, 0xC6, 0xC8, 0xC2, 0xC8, 0xC2, 0xC8, 0xC4
};
const unsigned char key_offset[12] = { // 鍵盤スプライトのX座標のオフセット
    0, 1, 2, 4, 5, 8, 10, 11, 13, 14, 16, 17
};

// 入力されたノート番号を元にスプライトを表示
void highlight_key_sprite(unsigned char channel) {
    unsigned char pitch_num = visualizer_get_note_pitch_num(channel); // 再生中ノートの音程番号を取得
    unsigned char octave = (pitch_num - 1) / 12; // オクターブ番号を取得
    unsigned char key_index = (pitch_num - 1) % 12; // キー番号を取得

    // 各チャンネルのスプライトを描画する座標を計算
    // x座標
    unsigned char sprite_x = 17 + (octave * 32) + key_index + key_offset[key_index] - 32;
    // y座標、ファミコンのスプライト描画の仕様上y座標が1ピクセルずれるため-1をする
    unsigned char sprite_y = channel_keyboard_y[channel] - 1;

    // スプライトの描画
    if (pitch_num == 0) {
        // 音程番号が0 = 音がなっていない場合はスプライトを非表示(画面外描画)にする
        oam_spr(sprite_x, -1, key_tile[key_index], 2&3, channel_sprite_id[channel]);
    } else {
        // 音程番号が0以外 = 音がなっている場合はスプライトを表示
        oam_spr(sprite_x, sprite_y, key_tile[key_index], 2&3, channel_sprite_id[channel]);
    }
}

// スプライトの更新関数
void sprite_update() {
    int i;
    for (i = 0; i < 3; i++) {
        highlight_key_sprite(i);
    }
}

// 画面表示のセットアップ関数
void setup_graphics() {
    ppu_off(); // PPUをオフにする

    // パレットの設定
    pal_bg((const char *)main_pal);
    pal_spr((const char *)main_pal);

    // タイルの設定
    vram_adr(NAMETABLE_A);
    vram_write((unsigned char *)main_nam, 1024);

    put_str(NTADR_A(6, 6), "Music Player for NES!"); // 画面に文字列を表示

    // スプライトの設定
    oam_clear();
    oam_size(1);

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
        sprite_update();
    }
}
