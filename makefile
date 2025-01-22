# プロジェクト名
TITLE = visualizer

# ツールチェイン
CC65 = cc65
CA65 = ca65
LD65 = ld65

# レイアウトファイル
CFG = layout.cfg

# ディレクトリ
BUILDDIR = build
SYSDIR = sys
SRCDIR = src
RESDIR = res

# 出力ファイル
ROMFILE = $(BUILDDIR)/$(TITLE).nes

# 必要なオブジェクトファイル
OBJS = $(BUILDDIR)/crt.o $(BUILDDIR)/main.o $(BUILDDIR)/main_res.o

# デフォルトターゲット
.PHONY: default clean
default: $(ROMFILE)

# ROM作成 (obj to rom)
$(ROMFILE): $(OBJS)
	$(LD65) -C $(CFG) -o $@ $(OBJS) nes.lib
	@echo "ビルド完了: $(ROMFILE)"

# crt.oをビルド (asm to obj)
$(BUILDDIR)/crt.o: $(SYSDIR)/crt.s | $(BUILDDIR)
	$(CA65) -o $@ $<
	@echo "作成完了: crt.o"

# main.oをビルド (c to asm to obj)
$(BUILDDIR)/main.o: $(SRCDIR)/main.c | $(BUILDDIR)
	$(CC65) -Oi -t nes -o $(BUILDDIR)/main.s $<
	$(CA65) -o $@ $(BUILDDIR)/main.s
	@echo "作成完了: main.o"

# main_res.oをビルド (asm to obj)
$(BUILDDIR)/main_res.o: $(RESDIR)/main.s | $(BUILDDIR)
	$(CA65) -o $@ $<
	@echo "作成完了: main_res.o"

# 必要なディレクトリを作成
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# クリーン
clean:
	rm $(BUILDDIR)/*
	@echo "ビルドディレクトリのクリーン完了"
