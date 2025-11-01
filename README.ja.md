# ym2151-zig-cc

Nuked-OPM を利用した 440Hz 3秒の音声レンダリングテストを zig cc でビルドするプロジェクト。

## 状況
### issueに入れたまま寝かせると混乱するのでこっちにおいておくコーナー
- 課題、ゴールがふわっとしている
  - 候補1
    - 440Hz、誤差+-1Hz、で鳴ること、を目指す
  - 候補2
    - ピッチは問わないものとし、鳴るので完了とする
  - 候補3
    - oscsyncのように、mouse連動でピッチ変化すること、を目指す
  - 候補4
    - cat-play-mmlの実装ヒントのために、
      - cycle単位のレジスタ書き込みログ（read側が最もシンプルに処理できるログ）
      - のreadと演奏ができること、を目指す
      - 前提として、候補1か候補4の実現が必須となる
  - 候補5
    - 内部55KHz（あるいは4MHzで内部62.5KHz）、
    - ダウンサンプリングして48000Hz、
    - を実現する
    - 結果的に候補1も実現される
    - 方式は、requestされたsample数ぶんを内部62.5KHzでレンダリングのちダウンサンプリングする
      - リサンプラーはstateを持つので、buffer境界での破綻はない

- 課題、ピッチがおかしいはず
  - 分析、レンダリング処理のハルシネーションによるミス
  - 分析、内部周波数約55KHzでレンダリングしてダウンサンプリングすべき
    - 現状はダウンサンプリングがないからピッチがおかしいはず
      - 「requestされたsample数ぶんレンダリングしてるだけ」なので低ピッチのはず
  - どうする？
    - 後回し
- 課題、44100から48000に変更したり、4MHzに変更したりすると、ピッチがおかしくなる
  - 前述のピッチ問題も関連するし、KC/KFの算出が4MHz対応してないのもある
  - どうする？
    - 後回し

## 概要

このプロジェクトは、Nuked-OPM（Yamaha YM2151エミュレータ）を使用して440Hzのトーンを3秒間レンダリングし、出力バッファが無音でないことを検証するC言語プログラムです。ビルドには zig cc を使用します。

## 特徴

- ✅ Nuked-OPMライブラリを使用した正確なYM2151エミュレーション
- ✅ OPM_SetICを使用した適切な初期化（無音回避）
- ✅ 初期化に100ms、各レジスタ書き込み後に10msの遅延
- ✅ zig cc を使用したクロスプラットフォームビルド（Linux、Windows）
- ✅ GitHub Actions による自動テスト
- ✅ バッファの非サイレント検証
- ✅ WAVファイル出力機能（src/phase2/）
- ✅ リアルタイム音声再生機能（src/phase3/）

## クイックスタート

### Linux

```bash
# zig cc を使ってビルド
zig cc -o test_opm test_opm.c opm.c -lm

# 実行
./test_opm
```

### Windows

```cmd
zig cc -o test_opm.exe test_opm.c opm.c -lm
test_opm.exe
```

詳細なビルド手順と環境設定については、[BUILD.md](BUILD.md) を参照してください。

## テスト結果の例

```
✅ SUCCESS: Buffer is NON-SILENT!
The audio buffer contains audio data.
  Total samples: 264600
  Non-zero samples: 264600 (100.00%)
  Max absolute value: 8080
```

## WAVファイル出力（Phase2）

440Hzの音を3秒間のWAVファイルとして出力するプログラムが `src/phase2/` に実装されています。

```bash
# ビルド
python3 build.py build-phase2-gcc

# 実行（output.wavを生成）
./wav_output

# カスタムファイル名で実行
./wav_output my_sound.wav
```

詳細については、[src/phase2/README.md](src/phase2/README.md) を参照してください。

## リアルタイム音声再生（Phase3）

440Hzの音を3秒間リアルタイムで再生するプログラムが `src/phase3/` に実装されています。MiniAudioを使用したdata_callbackベースのリアルタイム音声生成です。

```bash
# ビルド
python3 build.py build-phase3-gcc

# 実行（スピーカーから音が出ます）
./real_time_audio
```

詳細については、[src/phase3/README.md](src/phase3/README.md) を参照してください。

## ドキュメント

- [BUILD.md](BUILD.md) - 詳細なビルド手順とトラブルシューティング
- [src/phase2/README.md](src/phase2/README.md) - WAV出力プログラムの説明
- [src/phase3/README.md](src/phase3/README.md) - リアルタイム音声再生プログラムの説明
- [.github/workflows/test.yml](.github/workflows/test.yml) - CI/CD設定

## ライセンス

- このプロジェクト: MIT License
- Nuked-OPM: LGPL 2.1
- MiniAudio: Public Domain OR MIT-0 (dual license)
