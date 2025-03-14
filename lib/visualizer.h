#ifndef VISUALIZER_H
#define VISUALIZER_H

// FamiStudio Sound Engine経由で指定されたチャンネルの情報を取得する関数
//
// 引数: track - チャンネル番号(sq1:0, sq2:1, tri:2, noise:3, dpcm:4)
//
// ノート番号(音程)を取得
extern unsigned char visualizer_get_note(unsigned char track);

// 音量(0-8)を取得
extern unsigned char visualizer_get_volume(unsigned char track);

#endif
