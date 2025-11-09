# ym2151-zig-cc

Nuked-OPM を利用した簡易シーケンサを zig cc でビルドするプロジェクト

<p align="left">
  <a href="README.ja.md"><img src="https://img.shields.io/badge/🇯🇵-Japanese-red.svg" alt="Japanese"></a>
  <a href="README.md"><img src="https://img.shields.io/badge/🇺🇸-English-blue.svg" alt="English"></a>
</p>

## Quick Links
| 項目 | リンク |
|------|--------|
| 📊 開発状況 | [generated-docs/development-status](generated-docs/development-status.md) |

# 状況

アーカイブします。後継のRust版を開発します。

このリポジトリの用途は、C言語による最小限のコードを提供する用です。

## 概要

このプロジェクトは、Nuked-OPM（Yamaha YM2151エミュレータ）を使用してシンプルな音を鳴らす実験用のC言語プログラムです。ビルドには zig cc を使用します。

## 特徴

- ✅ Nuked-OPMライブラリを使用した正確なYM2151エミュレーション
- ✅ zig cc を使用したWindowsビルド
- ✅ WAVファイル出力機能（src/phase2/）
- ✅ リアルタイム音声再生機能（src/phase3/）
- ✅ 簡易シーケンサ機能（src/phase4/）
- ✅ Windowsで簡単にYM2151の音を鳴らす体験を提供
- ✅ シンプルなcodeで参考にしやすい（かもしれない）

## ビルド

```powershell
python build.py build-phase4 && ./player.exe
```

## 対象プラットフォーム

Windows

Linuxは動作確認していません。agentが自発的にLinux実装をしてtestをしているのを、全体最適の観点から黙認している程度です

## メンテナンス

- もし鳴らなくなった場合には、できるだけ優先して鳴らせる状態に回復するよう行動するつもり

## 今後の展望

- もしcodeやコメント、ドキュメントに明らかな不備があった場合には修正を検討

## スコープ外

このprojectはcodeのシンプルさを優先するため、以下のような、codeを複雑にする機能はスコープ外とします。

- userによる積極的なLinux対応ほか、マルチプラットフォーム対応
- ライブラリとして直接ソースファイルを利用するための汎用性向上ほか、codeを複雑にするあらゆる機能追加
- インタラクティブな演奏
- X680x0で演奏したかのようなclockやtimingの再現
- 既存曲の再現

## ライセンス

- このプロジェクト: MIT License
- Nuked-OPM: LGPL 2.1
- MiniAudio: Public Domain OR MIT-0 (dual license)

※英語版README.mdは、README.ja.mdを元にGeminiの翻訳でGitHub Actionsにより自動生成しています
