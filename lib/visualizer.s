; ビジュアライザー実装向けの関数
; FamiStudio Sound Engine経由で指定されたチャンネルの情報を取得する
; 引数 x: チャンネル番号 (Sq1=0, Sq2=1, Tri=2, Noise=3, DPCM=4)
;
; 該当チャンネルで発音されているノート番号(音程)を取得
; 戻り値: ノート番号(音程): 1-96、未発音時は0
.export _visualizer_get_note_pitch_num
.proc _visualizer_get_note_pitch_num
    tax
    lda famistudio_chn_note, x
    rts
.endproc

; 該当チャンネルのボリューム(0-8)を取得
.export _visualizer_get_volume
.proc _visualizer_get_volume
    tax
    lda famistudio_chn_note_counter, x
    rts
.endproc
