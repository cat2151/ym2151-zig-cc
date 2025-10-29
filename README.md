# ym2151-zig-cc

Nuked-OPM を利用した 440Hz 3秒の音声レンダリングテストを zig cc でビルドするプロジェクト。

## 概要

このプロジェクトは、Nuked-OPM（Yamaha YM2151エミュレータ）を使用して440Hzのトーンを3秒間レンダリングし、出力バッファが無音でないことを検証するC言語プログラムです。ビルドには zig cc を使用します。

## 特徴

- ✅ Nuked-OPMライブラリを使用した正確なYM2151エミュレーション
- ✅ OPM_SetICを使用した適切な初期化（無音回避）
- ✅ 初期化に100ms、各レジスタ書き込み後に10msの遅延
- ✅ zig cc を使用したクロスプラットフォームビルド（Linux、Windows）
- ✅ GitHub Actions による自動テスト
- ✅ バッファの非サイレント検証

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

## ドキュメント

- [BUILD.md](BUILD.md) - 詳細なビルド手順とトラブルシューティング
- [.github/workflows/test.yml](.github/workflows/test.yml) - CI/CD設定

## ライセンス

- このプロジェクト: MIT License
- Nuked-OPM: LGPL 2.1
