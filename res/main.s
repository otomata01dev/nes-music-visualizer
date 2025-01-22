.segment "RODATA"
; 音源データの読み込み
; music_data:

; sounds_data:
	
.segment "SAMPLES"
; DPCM関連のデータ読み込み

.segment "CHARS"
; グラフィックデータの読み込み
	.incbin "../chr/main.chr"
