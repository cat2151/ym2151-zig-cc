Last updated: 2025-11-03

# Development Status

## 現在のIssues
オープン中のIssueはありません。

## 次の一手候補
1.  OPM音源の新しい音色パラメータを追加する [Issue #100](../issue-notes/100.md)
    -   最初の小さな一歩: `opm.c`に新しい波形やエンベロープ設定を定義するための構造体メンバーを追加する。
    -   Agent実行プロンプ:
        ```
        対象ファイル: `opm.c`, `opm.h`, `src/phase1/test_opm.c`

        実行内容: `opm.c`と`opm.h`の既存のOPM音源構造を分析し、新しい音色パラメータ（例: 追加のオペレータ設定、変調タイプ）を安全に追加できる拡張ポイントを特定してください。その後、その新しいパラメータを初期化し、既存の音源生成ロジックに組み込むための最小限のコード変更案を提案してください。

        確認事項: 既存のデータ構造や関数シグネチャとの互換性、および`src/phase1/test_opm.c`でのテストのしやすさを確認してください。

        期待する出力: 新しい音色パラメータを追加するための`opm.h`と`opm.c`へのコード変更案をMarkdown形式で出力し、その変更が既存の機能に与える影響について説明してください。
        ```

2.  `opm.c`のユニットテストをGitHub Actionsで自動化する [Issue #101](../issue-notes/101.md)
    -   最初の小さな一歩: `src/phase1/test_opm.c`をコンパイルし実行するための最小限のGitHub Actionsワークフローを作成する。
    -   Agent実行プロンプ:
        ```
        対象ファイル: `.github/workflows/`ディレクトリ内の既存のワークフローファイル, `src/phase1/test_opm.c`, `opm.c`, `opm.h`

        実行内容: `src/phase1/test_opm.c`が`opm.c`と`opm.h`に依存していることを考慮し、GitHub ActionsでC言語プロジェクトをビルド・テストする一般的な方法を調査してください。その後、`test_opm.c`をコンパイルし、実行する新しいGitHub Actionsワークフローファイル（例: `.github/workflows/test-opm.yml`）を作成する手順と内容を記述してください。

        確認事項: ビルド環境（例: GCCのバージョン）の指定、テストの成功/失敗を適切にレポートする方法、および既存のCI/CDワークフローとの競合がないことを確認してください。

        期待する出力: `test-opm.yml`のYAMLコード例と、そのワークフローの導入手順をMarkdown形式で生成してください。
        ```

3.  OPM音源の出力をリアルタイムオーディオ再生に繋ぎ込む [Issue #102](../issue-notes/102.md)
    -   最初の小さな一歩: `src/phase3/real_time_audio.c`内のオーディオコールバック関数で、`opm.c`から生成された短いサンプルを再生するよう修正する。
    -   Agent実行プロンプ:
        ```
        対象ファイル: `opm.c`, `opm.h`, `src/phase3/real_time_audio.c`, `src/phase3/miniaudio.h`

        実行内容: `opm.c`からオーディオサンプルを生成する方法と、`src/phase3/real_time_audio.c`が`miniaudio.h`を使用してどのようにリアルタイム再生を行っているかを分析してください。その後、`opm.c`で生成されたサウンドデータを`src/phase3/real_time_audio.c`のオーディオ再生ループに供給するための、初期的な結合方法を提案してください。

        確認事項: OPM音源のサンプルレートと`miniaudio`の再生レートの整合性、バッファリングの考慮事項、および既存のオーディオ再生処理に大きな変更を加えないで統合できるかを確認してください。

        期待する出力: OPM音源とリアルタイムオーディオ再生を統合するための、`src/phase3/real_time_audio.c`への具体的なコード変更の提案と、関連する`opm.c`の変更点の概要をMarkdown形式で出力してください。

---
Generated at: 2025-11-03 07:05:56 JST
