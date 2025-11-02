Last updated: 2025-11-03

# Project Overview

## プロジェクト概要
- Yamaha YM2151エミュレータ「Nuked-OPM」を使用した簡易シーケンサプロジェクトです。
- C言語で実装されており、Zigコンパイラの`zig cc`コマンドを用いてビルドされます。
- シンプルな音の生成、リアルタイム再生、およびWAVファイル出力の実験目的で開発されています。

## 技術スタック
- フロントエンド: [該当する技術はありません]
- 音楽・オーディオ: 
    - Tone.js: Web Audio APIを抽象化し、ブラウザ上で高度な音声処理を可能にするJavaScriptライブラリ。
    - Web Audio API: ウェブブラウザで複雑な音声処理を可能にするJavaScript API（Tone.js経由で利用）。
    - MML (Music Macro Language): 音楽をテキストで記述するための簡易的な記法を解析する技術。
- 開発ツール: 
    - Node.js runtime: JavaScriptコードを実行するための環境。
- テスト: [該当する技術はありません]
- ビルドツール: 
    - zig cc: Zigコンパイラに含まれるC言語コンパイラ機能で、プロジェクトのビルドに使用されます。
    - Python: `build.py`スクリプトの実行環境として利用されます。
- 言語機能: 
    - C言語: プロジェクトの主要なコードがC言語で記述されています。
- 自動化・CI/CD: 
    - GitHub Actions: コードの変更を検知して自動で様々なタスクを実行するCI/CDプラットフォーム。
        - プロジェクト要約自動生成: プロジェクトの概要ドキュメントを自動生成します。
        - Issue自動管理: GitHub Issueのライフサイクル管理を自動化します。
        - README多言語翻訳: READMEファイルを複数の言語に自動翻訳します。
        - i18n automation: 国際化（i18n）関連の自動化ワークフロー。
- 開発標準: [該当する技術はありません]

## ファイル階層ツリー
```
📄 .gitignore
📁 .vscode/
  📊 settings.json
📄 LICENSE
📖 README.ja.md
📖 README.md
📄 build.py
📁 generated-docs/
  📖 development-status-generated-prompt.md
📁 issue-notes/
  📖 12.md
  📖 13.md
  📖 14.md
  📖 17.md
  📖 20.md
  📖 21.md
  📖 22.md
  📖 29.md
  📖 30.md
  📖 31.md
  📖 33.md
  📖 5.md
  📖 6.md
  📖 7.md
📄 opm.c
📄 opm.h
📄 ruff.toml
📁 src/
  📁 phase1/
    📄 test_opm.c
  📁 phase2/
    📄 wav_output.c
  📁 phase3/
    📄 miniaudio.h
    📄 real_time_audio.c
  📁 phase4/
    📄 core.h
    📄 events.h
    📄 miniaudio.h
    📄 player.c
    📄 types.h
    📄 wav_writer.h
```

## ファイル詳細説明
- **`.gitignore`**: Gitによるバージョン管理から除外するファイルやディレクトリを指定します。
- **`.vscode/settings.json`**: Visual Studio Codeエディタのワークスペース固有の設定を定義します。
- **`LICENSE`**: プロジェクトのソースコードの使用、配布、変更に関するライセンス情報を提供します。
- **`README.ja.md`**: プロジェクトの概要、使い方、目的などを日本語で説明するメインのドキュメントです。
- **`README.md`**: プロジェクトの概要、使い方、目的などを英語で説明するメインのドキュメントです。
- **`build.py`**: プロジェクトのビルドプロセスを自動化するためのPythonスクリプトです。
- **`generated-docs/development-status-generated-prompt.md`**: 自動化されたプロセスによって生成される、プロジェクトの開発状況に関するドキュメントです。
- **`issue-notes/`**: 開発中の特定のIssueに関する詳細なメモや考察が格納されるディレクトリです。
    - **`issue-notes/12.md` ... `issue-notes/7.md`**: 個々のIssueに関する具体的なメモファイル。
- **`opm.c`**: Yamaha YM2151エミュレータ「Nuked-OPM」の主要なC言語実装ファイルで、FM音源のコアロジックを含みます。
- **`opm.h`**: `opm.c`に対応するヘッダファイルで、Nuked-OPMエミュレータの公開インターフェースやデータ構造を定義します。
- **`ruff.toml`**: PythonコードのリンティングとフォーマットをRuffツールで行うための設定ファイルです。
- **`src/phase1/test_opm.c`**: Nuked-OPMエミュレータの基本的な機能や特定の挙動を確認するためのテストコードです。
- **`src/phase2/wav_output.c`**: 生成されたオーディオデータを標準的なWAV形式のファイルに出力する機能の実装です。
- **`src/phase3/miniaudio.h`**: 軽量かつポータブルなオーディオI/Oライブラリであるminiaudioのヘッダファイルです。リアルタイムオーディオ再生に利用されます。
- **`src/phase3/real_time_audio.c`**: miniaudioライブラリを利用して、生成された音声をリアルタイムでシステムオーディオデバイスに出力する機能の実装です。
- **`src/phase4/core.h`**: シーケンサの基盤となるデータ構造、定数、および共通ユーティリティ関数の定義を含むヘッダファイルです。
- **`src/phase4/events.h`**: シーケンス内の音楽イベント（音符オン/オフ、パラメータ変更など）の定義と、それらの処理に関連する構造体やマクロを含むヘッダファイルです。
- **`src/phase4/miniaudio.h`**: `src/phase3/miniaudio.h`と同様、miniaudioライブラリのヘッダファイル（プロジェクト内で複数箇所から参照される可能性があります）。
- **`src/phase4/player.c`**: シーケンスデータを読み込み、MMLなどを解釈してNuked-OPMエミュレータを制御し、音楽を生成・再生する主要なプレイヤーロジックを実装しています。
- **`src/phase4/types.h`**: プロジェクト全体で一貫して使用されるカスタムデータ型や列挙型を定義するヘッダファイルです。
- **`src/phase4/wav_writer.h`**: WAVファイルへのデータ書き込み機能のインターフェースを定義するヘッダファイルです。

## 関数詳細説明
関数の情報は提供されていません。

## 関数呼び出し階層ツリー
```
関数呼び出し階層の情報は提供されていません。
```

---
Generated at: 2025-11-03 07:06:23 JST
