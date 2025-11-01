# ym2151-zig-cc

Nuked-OPM を利用した簡易シーケンサを zig cc でビルドするプロジェクト

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

## スコープ外

このprojectはcodeのシンプルさを優先するため、以下のような、codeを複雑にする機能はスコープ外とします。

- X680x0で演奏したかのようなclockやtimingの再現
- 既存曲の再現
- インタラクティブな演奏

## ライセンス

- このプロジェクト: MIT License
- Nuked-OPM: LGPL 2.1
- MiniAudio: Public Domain OR MIT-0 (dual license)
