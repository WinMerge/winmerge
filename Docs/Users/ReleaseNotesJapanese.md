# WinMerge 2.16.30 リリースノート

- [このリリースについて](#about-this-release)
- [2.16.30の新機能](#what-is-new-in-21630)
- [2.16.29 beta の新機能](#what-is-new-in-21629-beta)
- [既知の問題](#known-issues)

2023年4月

## このリリースについて

WinMerge の 2.16.30 安定版リリースです。
このリリースは、以前の WinMerge 安定版リリースに代わる推奨リリースです。

不具合は <a href="http://github.com/WinMerge/winmerge/issues">bug-tracker</a> で報告してください。
日本語での報告は、<a href="https://osdn.net/projects/winmerge-jp/ticket/">こちら</a>でお願いします。

## 2.16.30 の新機能

### 全般

- 不具合修正: エクスポートされた ini ファイルを /inifile コマンドラインオプション等で使用する場合、差異等の色が黒になってしまう問題を修正した。[(#1799)](https://github.com/WinMerge/winmerge/issues/1799)
- 32ビット版のWinMergeU.exeのファイルサイズを削減した。

### ファイル比較

- 不具合修正: vcxprojなどのXML形式のファイルがシンタックスハイライトされない問題を修正した。
- 不具合修正: ARM64版でEOL文字が表示されない問題を修正した。
- 2つのファイルを比較する場合、移動ダイアログの[中央]ラジオボタンを無効にするようにした。[(PR #1779)](https://github.com/WinMerge/winmerge/pull/1779)(PRいただきました。ありがとうございます。)

### 画像比較

- 不具合修正: タブを中ボタンでクリックするとクラッシュすることがある問題を修正した。[(#1785)](https://github.com/WinMerge/winmerge/issues/1785)
- 不具合修正: 変更した色距離閾値が次回WinMerge起動で0に戻ってしまう問題を修正した。
- 色距離閾値を442まで変更できるように修正した。[(winmerge/winimerge #29)](https://github.com/WinMerge/winimerge/issues/29)

### ファイルフィルタ

- UTF-8で保存されているファイルフィルタでも機能するようにした。

### オプションダイアログ

- 不具合修正: バージョン2.16.29で、オプションダイアログのシェル統合カテゴリページが正しく表示されない問題を修正した。
- 不具合修正: エクスポートした設定情報にプラグインの設定も含められるようにした。

### プラグイン

- 不具合修正: プラグイン -> 手動展開メニューがチェックされている場合でもApplyPatchプラグインが機能するようにした。
- 不具合修正: 32ビットOSでCompareMSExcelFilesプラグイン等の設定ウィンドウが開けない問題を修正した。
- [プラグインの設定]ボタンを[プラグインの選択]ウィンドウに追加した。
- プラグインの設定ダイアログを翻訳可能にし、INIファイルにプラグインの設定を保存できるようにした。[(PR #1783)](https://github.com/WinMerge/winmerge/pull/1783)

### コマンドライン

- 不具合修正: `/new` コマンドラインオプションを指定した場合、`/fr` オプションが無視される問題を修正した。
- 不具合修正: 自動マージ後に競合する行に移動しない問題を修正した。

### プロジェクトファイル

- 不具合修正: プロジェクトファイル内のパスに環境変数が含まれるとプロジェクトファイルが開けない問題を修正した。[(#1793)](https://github.com/WinMerge/winmerge/issues/1793)

### インストーラ

- 不具合修正: インストーラの「プログラムグループの指定」ページで、プログラムグループの名前を空にし、[プログラムグループを作成しない]のチェックを外すと、インストールに失敗する問題を修正した。[(#1802)](https://github.com/WinMerge/winmerge/issues/1802)

### 翻訳

- 翻訳の更新:
  - Brazilian (PR #1806)
  - Chinese Simplified (PR #1808)
  - German (PR #1807)
  - Hungarian (PR #1809)
  - Japanese
  - Korean (PR #1774,#1788,#1790,#1791)
  - Lithuanian (PR #1817)
  - Polish (PR #1815)
  - Portuguese (PR #1787)
  - Slovenian (#1812)

## 2.16.29 Beta の新機能

### ファイル比較

- 不具合修正: 3方向比較で[類似行をマッチさせる]の動作を改善した。[(#1696)](https://github.com/WinMerge/winmerge/issues/1696)
- 不具合修正: 選択範囲の終点の位置よっては正しくインデントされないことがあるのを修正。[(#1740)](https://github.com/WinMerge/winmerge/issues/1740)
- デフォルト以外のDiffアルゴリズムの選択時の以下の問題を修正:
  - [改行文字の違いを無視する]が無効でも、空白の差分を無視するオプションが有効な場合、改行文字の差分が無視される。
  - [改行文字の違いを無視する]が有効でも、CRとCR+LFの差異が無視されない。
- 正規表現のチェックを外し、[単語単位で探す]にチェックを入れた場合、正しく動作しないのを修正。
- MATLAB のシンタックスハイライトを追加。[(PR #1766)](https://github.com/WinMerge/winmerge/pull/1766)(PRいただきました。ありがとうございます。)

### テーブル比較

- テーブル比較ウィンドウの[表示]→[行を右端で折り返す]メニューを[折り返して全体を表示]に変更し、そのチェック状態がテキスト比較ウィンドウの同じメニュー項目とは別に保存されるようにした。[(osdn.net #47553)](https://osdn.net/projects/winmerge-jp/ticket/47553)
- CSVファイルのセパレーターをカンマからセミコロン等他の文字に変更できるようにした。

### バイナリ比較

- 不具合修正: ウインドウサイズを変更すると、スクロール位置が適切な位置にならない問題を修正した。[(WinMerge/frhed #13)](https://github.com/WinMerge/frhed/issues/13)
- 不具合修正: ファイル読み込み時にファイルサイズが切り捨てられると、無限ループが発生する問題を修正した。

### 画像比較

- 不具合修正: 画像比較レポートを共有フォルダに保存できない問題を修正した。

### Webページ比較

- 不具合修正: ヘッダーバーにファイルパスが表示されない問題を修正した。

### フォルダ比較

- 不具合修正: ツリーモードでフォルダを折りたたんだ後、その横のアイコンが下向きのママになる問題を修正した。[(#1747)](https://github.com/WinMerge/winmerge/issues/1747)
- 不具合修正: 片方のファイルを削除しても日付やサイズ等の列が更新されないのを修正した。
- コンテキストメニューの[移動]に[左側から右側]、または[右側から左側]を追加した。[(PR #1732)](https://github.com/WinMerge/winmerge/pull/1732)
- F2キーまたは[名前の変更]メニュー選択時、拡張子部分が選択されないようにした。

### オプションダイアログ

- 「形式を指定して比較」メニューをオプションダイアログの[シェル統合]カテゴリから追加できるようにした。

### プラグイン

- IgnoreLeadingLineNumbers プラグインの処理速度改善。[(#1715)](https://github.com/WinMerge/winmerge/issues/1715)
   (IgnoreLeadingLineNumbers.dll から IgnoreLeadingLineNumbers.sct にファイル名を変更しました。)

### コマンドライン

- 不具合修正: コマンドラインオプション /self-compare を使用して file.zip:Zone.Identifier:$DATA などの代替データストリームを比較した場合、クラッシュするのを修正。
- 不具合修正: パスを1つだけ指定して/tコマンドラインオプションを指定した場合にクラッシュする問題を修正。

### シェルエクステンション

- 不具合修正: Windows 11 エクスプローラーの2つ目以降のタブでファイルやフォルダを選択して比較しても、選択したファイルやフォルダが比較されない問題を修正した。[(#1733)](https://github.com/WinMerge/winmerge/issues/1733)
- 不具合修正: Windows 11 エクスプローラーでファイル以外の領域を右クリックすると、WinMerge メニューが表示されない問題を修正した。
- 「形式を指定して比較」メニューをオプションダイアログの[シェル統合]カテゴリから追加できるようにした。

### インストーラ

- Frhed/WinIMerge/Merge7z/WinWebDiffが常にインストールするようにした。また、[コンポーネントの選択]ページでプラグインを選択した場合、GnuWin32 Patch for Windows/HTML Tidy/jq/md4c もインストールされるようにした。
- ExplorerがシェルエクステンションのDLLを使用中の時、インストーラからExplorerを再起動してもインストールに成功しないことがある問題を修正した。
- 64ビット版インストーラーにIgnoreLeadingLineNumbers.sctプラグインを追加。

### 翻訳

- 不具合修正: 一部のメッセージが翻訳されないのを修正した。[(PR #1712)](https://github.com/WinMerge/winmerge/pull/1712)(PRいただきました。ありがとうございます。)
- 翻訳の更新:
  - Brazilian (PR #1711,#1713,#1727,#1728,#1756)
  - Chinese Simplified (PR #1714,#1719,#1722,#1731,#1765)
  - Corsican (PR #1717,#1723,#1752)
  - Dutch (PR #1757)
  - German (PR #1730,#1754)
  - Hungarian (PR #1725,#1726,#1758)
  - Japanese
  - Korean (PR #1689,#1690,#1692,#1693,#1694,#1718)
  - Lithuanian (PR #1729,#1755)
  - Polish (PR #1763,#1764)
  - Portuguese (PR #1737)
  - Russian (PR #1710,#1751)
  - Swedish (#1706,PR #1707)

## 既知の問題

 - フォルダ比較で画像比較を有効にした場合、比較結果が安定しない。 [(#1391)](https://github.com/WinMerge/winmerge/issues/1391)
 - 巨大ファイルの比較でクラッシュすることがある。[(#325)](https://github.com/WinMerge/winmerge/issues/325)
 - 大幅に異なるフォルダの比較が非常に時間がかかる。 [(#322)](https://github.com/WinMerge/winmerge/issues/322)
 - 新規作成で表示範囲を超えるテキストを貼り付けたとき、垂直スクロールバーでスクロールできない。 [(#296)](https://github.com/WinMerge/winmerge/issues/296)
