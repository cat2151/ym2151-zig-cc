Last updated: 2025-11-02

# Project Overview

## プロジェクト概要
- Nuked-OPM（YM2151エミュレータ）を利用し、Zig CCでビルドされる簡易シーケンサの実験プロジェクトです。
- C言語を用いてFM音源の音を生成し、リアルタイム再生やWAVファイルへの出力が可能です。
- 音楽マクロ言語（MML）の処理や、開発フェーズごとに機能を段階的に実装しています。

## 技術スタック
- フロントエンド: 該当なし（Web Audio APIはバックエンド/ビルド時に使用される可能性のある技術スタックとして、Tone.jsとの関連で記載）
- 音楽・オーディオ:
    - Tone.js: Web Audio APIを抽象化し、ブラウザ上で高度な音声処理を行うためのJavaScriptライブラリ。（プロジェクト情報の記述から、MMLパーサーなどと組み合わせて、最終的な音源生成や再生に利用される可能性があるが、コアはC言語とNuked-OPMであるため、Webフロントエンド要素が直接存在しない場合は、関連技術として解釈。）
    - Web Audio API: Webブラウザ上で音声処理を行うためのAPI。（Tone.js経由で使用）
    - MML (Music Macro Language): 音楽をテキストで記述するための記法。このプロジェクトでは、MMLで記述された音楽データをパースして音源生成に利用する。
- 開発ツール:
    - Node.js runtime: JavaScriptコードを実行するための環境。主に自動化スクリプトや開発支援ツール（MMLパーサーなど）で使用される。
- テスト: 該当なし
- ビルドツール: 
    - zig cc: Zig言語のコンパイラが提供するC言語コンパイラ機能。このプロジェクトのC言語ソースコードをビルドするために使用される。
- 言語機能: 該当なし (プロジェクトの主要言語はC言語とZig (ビルドツールとして))
- 自動化・CI/CD:
    - GitHub Actions: コードの変更を検知し、自動的にテスト、ビルド、デプロイなどのワークフローを実行するCI/CDサービス。
        - プロジェクト要約自動生成: プロジェクトの概要ドキュメントを自動で生成するワークフロー。
        - Issue自動管理: 課題追跡（Issue）の管理を自動化するワークフロー。
        - README多言語翻訳: READMEファイルを複数の言語に自動翻訳するワークフロー。
        - i18n automation: 国際化（i18n）に関連する自動翻訳処理を指す。
- 開発標準:
    - ruff.toml: Pythonの高速LinterおよびFormatterであるRuffの設定ファイル。Pythonスクリプト（例: `build.py`）のコード品質を保つために使用される。

## ファイル階層ツリー
```
.
├── .gitignore
├── .vscode/
│   └── settings.json
├── LICENSE
├── README.ja.md
├── README.md
├── build.py
├── generated-docs/
│   └── development-status-generated-prompt.md
├── issue-notes/
│   └── (開発中のメモファイル群)
├── opm.c
├── opm.h
├── ruff.toml
└── src/
    ├── phase1/
    │   └── test_opm.c
    ├── phase2/
    │   └── wav_output.c
    ├── phase3/
    │   ├── miniaudio.h
    │   └── real_time_audio.c
    └── phase4/
        ├── core.h
        ├── events.h
        ├── miniaudio.h
        ├── player.c
        ├── types.h
        └── wav_writer.h
```

## ファイル詳細説明
- **.gitignore**: Gitがバージョン管理の対象から除外するファイルやディレクトリを指定する設定ファイルです。
- **.vscode/settings.json**: Visual Studio Codeエディタのプロジェクト固有の設定ファイルです。
- **LICENSE**: プロジェクトのライセンス情報が記載されています。
- **README.ja.md**: プロジェクトの概要、使い方、技術スタックなどを日本語で説明するファイルです。
- **README.md**: プロジェクトの概要、使い方、技術スタックなどを英語で説明するファイルです。
- **build.py**: プロジェクトをビルドするためのPythonスクリプトです。主にZig CCコンパイラを呼び出してC言語のソースコードをコンパイルする役割を担います。
- **generated-docs/development-status-generated-prompt.md**: GitHub Actionsによって自動生成された開発状況に関するドキュメントです。
- **issue-notes/**: 開発中の課題やメモが記述されたMarkdownファイルが格納されているディレクトリです。
- **opm.c**: Nuked-OPM（Yamaha YM2151エミュレータ）のC言語実装が含まれています。FM音源の合成処理を担当します。
- **opm.h**: `opm.c`で定義されている関数やデータ構造の宣言が含まれるヘッダーファイルです。
- **ruff.toml**: PythonのLinter/FormatterであるRuffの設定ファイルです。`build.py`などのPythonコードの品質を維持するために使用されます。
- **src/phase1/test_opm.c**: プロジェクトの初期段階（フェーズ1）におけるNuked-OPMエミュレータの基本機能テスト用のC言語ソースファイルです。
- **src/phase2/wav_output.c**: プロジェクトのフェーズ2で実装されたWAVファイル出力機能を提供するC言語ソースファイルです。生成された音源データをWAV形式で保存します。
- **src/phase3/miniaudio.h**: リアルタイムオーディオ処理ライブラリであるMiniaudioのヘッダーファイルです。音声デバイスへの低レイテンシ出力に利用されます。
- **src/phase3/real_time_audio.c**: プロジェクトのフェーズ3で実装された、Miniaudioライブラリを用いたリアルタイムオーディオ出力処理を行うC言語ソースファイルです。
- **src/phase4/core.h**: シーケンサのコアロジックに関する共通の定義や関数プロトタイプをまとめたヘッダーファイルです。
- **src/phase4/events.h**: シーケンサが扱うイベント（音符、テンポ変更など）の定義や関連する関数プロトタイプをまとめたヘッダーファイルです。
- **src/phase4/miniaudio.h**: (再掲の可能性あり、または`phase3`と共通のヘッダー)。
- **src/phase4/player.c**: プロジェクトの最終フェーズ（フェーズ4）におけるシーケンサの再生ロジックを実装したC言語ソースファイルです。MMLデータを解釈し、Nuked-OPMを通じて音源を生成・再生します。
- **src/phase4/types.h**: プロジェクト全体で利用されるカスタムデータ型や構造体の定義をまとめたヘッダーファイルです。
- **src/phase4/wav_writer.h**: WAVファイルへの書き込み機能に関する宣言をまとめたヘッダーファイルです。`src/phase2/wav_output.c`と連携する可能性があります。

## 関数詳細説明
提供された情報から具体的な関数の詳細は検出されませんでした。しかし、各ファイルの役割から以下の主要な機能を持つ関数が存在すると推測されます。

- **`opm.c`**:
    - `opm_init(YM2151 *chip, int clock, int rate)`: YM2151エミュレータを初期化します。クロック周波数とサンプリングレートを設定します。
    - `opm_write(YM2151 *chip, int addr, int data)`: YM2151レジスタにデータを書き込みます。
    - `opm_update(YM2151 *chip, int32_t *buffer, int samples)`: 指定されたサンプル数のオーディオデータを生成し、バッファに書き込みます。

- **`src/phase1/test_opm.c`**:
    - `main()`: プログラムのエントリポイントであり、Nuked-OPMエミュレータのテストシーケンスを実行します。
    - `run_opm_test(void)`: OPMの各種機能を検証するテスト関数群を呼び出します。

- **`src/phase2/wav_output.c`**:
    - `write_wav_header(FILE *fp, int num_samples, int sample_rate, int num_channels, int bits_per_sample)`: WAVファイルのヘッダーをファイルに書き込みます。
    - `write_wav_data(FILE *fp, const int16_t *data, int num_samples)`: オーディオサンプルデータをWAVファイルに書き込みます。
    - `save_to_wav(const char *filename, const int16_t *buffer, int num_samples, int sample_rate)`: 指定されたバッファの内容をWAVファイルとして保存します。

- **`src/phase3/real_time_audio.c`**:
    - `audio_callback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)`: Miniaudioデバイスからのコールバック関数で、リアルタイムでオーディオデータを生成し、出力バッファに書き込みます。
    - `init_audio_device(int sample_rate, int num_channels)`: Miniaudioデバイスを初期化し、オーディオストリームを開始します。
    - `deinit_audio_device(void)`: Miniaudioデバイスを停止し、リソースを解放します。

- **`src/phase4/player.c`**:
    - `player_init(Player *pPlayer, int sample_rate)`: シーケンサプレイヤーを初期化します。
    - `player_load_mml(Player *pPlayer, const char *mml_string)`: MML文字列をロードし、内部イベントキューを構築します。
    - `player_process_frame(Player *pPlayer, int32_t *output_buffer, int num_samples)`: 1フレーム分のオーディオデータを生成し、次のイベントに進めます。
    - `player_is_playing(Player *pPlayer)`: プレイヤーが現在再生中であるかを確認します。

## 関数呼び出し階層ツリー
```
関数呼び出し階層は提供された情報からは分析できませんでした。

---
Generated at: 2025-11-02 07:05:57 JST
