# Nuked-OPM 440Hz Test Program

このプログラムは、Nuked-OPMライブラリを使用して440Hzのトーンを3秒間レンダリングし、出力バッファが無音でないことを確認します。

## プログラムの説明

`test_opm.c` は以下の処理を実行します：

1. Nuked-OPMチップの初期化（OPM_SetICを使用）
2. 100msの初期化遅延
3. 440Hz（A4音）のFMトーンの設定
4. 各レジスタ書き込み後に10ms相当のサイクルを消費
5. 3秒間のオーディオバッファのレンダリング
6. バッファが無音でないことの確認とレポート

## Linux での zig cc を使ったビルドと実行

### 前提条件

Zig をインストールする必要があります。以下のいずれかの方法でインストールできます：

#### オプション1: Snap を使用（Ubuntu/Debian）
```bash
sudo snap install zig --classic --beta
```

#### オプション2: 公式バイナリをダウンロード
1. https://ziglang.org/download/ から最新版をダウンロード
2. アーカイブを展開
3. PATHに追加

```bash
# 例：
wget https://ziglang.org/download/0.13.0/zig-linux-x86_64-0.13.0.tar.xz
tar xf zig-linux-x86_64-0.13.0.tar.xz
export PATH=$PATH:$(pwd)/zig-linux-x86_64-0.13.0
```

### ビルドと実行

```bash
# ビルド
zig cc -o test_opm test_opm.c opm.c -lm

# 実行
./test_opm
```

### 期待される出力

```
Nuked-OPM 440Hz Test Program
=============================

Configuring OPM for 440Hz tone...
  Setting IC=1...
  Clocking 357954 cycles (100ms)...
  Setting IC=0...
  Clocking 357954 cycles (100ms)...
  Writing registers...
  Key ON channel 0, all operators...
  Configuration complete.
Rendering 3 seconds of audio...
  First non-zero sample at index 0: L=-7952 R=-7952

Analyzing rendered audio buffer...
  Total samples: 264600
  Non-zero samples: 264600 (100.00%)
  Max absolute value: 8080
  Average absolute value: 2326.68

✅ SUCCESS: Buffer is NON-SILENT!
The audio buffer contains audio data.
```

## Windows での zig cc を使ったビルドと実行

### 前提条件

Zig for Windows をインストールする必要があります：

1. https://ziglang.org/download/ から Windows版をダウンロード
2. ZIPを展開
3. Zigのbinディレクトリをシステムの環境変数PATHに追加

### ビルド（Windows ネイティブ）

```cmd
# Windowsでコンパイル
zig cc -o test_opm.exe test_opm.c opm.c -lm

# 実行
test_opm.exe
```

### クロスコンパイル（Linux から Windows バイナリをビルド）

Linux 環境で Windows バイナリをビルドすることもできます：

```bash
# Linux上でWindowsバイナリをビルド
zig cc -target x86_64-windows -o test_opm.exe test_opm.c opm.c -lm

# このバイナリをWindows PCにコピーして実行
```

## WSL (Windows Subsystem for Linux) でのビルドと実行

WSLでは、Linuxと同じ手順でビルド・実行できます：

```bash
# WSL Ubuntu等で
zig cc -o test_opm test_opm.c opm.c -lm
./test_opm
```

## GitHub Actions での自動テスト

このリポジトリには GitHub Actions ワークフローが含まれており、プッシュ時に自動的に：

1. Linux 環境で zig cc を使ってビルド・テストを実行
2. Windows バイナリをクロスコンパイル
3. Windows バイナリをアーティファクトとしてアップロード（7日間保存）

ワークフローは `.github/workflows/test.yml` で定義されています。

## 技術的な詳細

### Nuked-OPM について

- Nuked-OPMは、Yamaha YM2151（OPM）チップのサイクル精度エミュレータです
- LGPL 2.1 ライセンスで公開されています
- https://github.com/nukeykt/Nuked-OPM

### 重要な設定

- サンプルレート: 44100 Hz
- OPM クロック: 3579545 Hz
- サンプルあたりのサイクル数: 64
- 音の長さ: 3秒
- 周波数: 440Hz (A4音)

### OPM_SetIC の重要性

**注意**: Nuked-OPMでは、`OPM_SetIC` を使用してICピンを制御しないと、チップが無音を出力し続けます。これは実際のハードウェアの動作を正確にエミュレートしているためです。

初期化シーケンス：
1. `OPM_SetIC(chip, 1)` - ICをアクティブにして初期化
2. 100ms 相当のクロックサイクルを実行
3. `OPM_SetIC(chip, 0)` - ICを解除
4. さらに100ms相当のクロックサイクルを実行
5. レジスタの設定を開始

### レジスタ書き込みの遅延

各レジスタ書き込み後に10ms相当のサイクルを消費することで、ハードウェアの動作により近い動作を再現しています。

## トラブルシューティング

### バッファが無音の場合

以下を確認してください：

1. OPM_SetIC が正しく呼ばれているか
2. 初期化遅延が十分か
3. レジスタ値が正しいか
4. Key ONコマンドが正しく送信されているか

### zig が見つからない

PATHにzigが追加されているか確認してください：

```bash
which zig
zig version
```

## ライセンス

- このテストプログラム: MIT License
- Nuked-OPM: LGPL 2.1
