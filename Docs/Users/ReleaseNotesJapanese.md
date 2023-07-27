# WinMerge 2.16.32 リリースノート

- [このリリースについて](#about-this-release)
- [2.16.32の新機能](#what-is-new-in-21632)
- [2.16.31 beta の新機能](#what-is-new-in-21631-beta)
- [既知の問題](#known-issues)

2023年7月

## このリリースについて

WinMerge の 2.16.32 安定版リリースです。
このリリースは、以前の WinMerge 安定版リリースに代わる推奨リリースです。

不具合は <a href="http://github.com/WinMerge/winmerge/issues">bug-tracker</a> で報告してください。
日本語での報告は、<a href="https://osdn.net/projects/winmerge-jp/ticket/">こちら</a>でお願いします。

## 2.16.32 の新機能

### 全般

- 不具合修正: INIファイルをインポートしたとき置換フィルタなどが正しく復元されないことがある問題を修正した。[(#1925)](https://github.com/WinMerge/winmerge/issues/1925)

### ファイル比較

- 不具合修正: 比較するファイルのパスが長すぎると保存できない問題を修正した。[(#1923)](https://github.com/WinMerge/winmerge/issues/1923)
- 不具合修正: 外部のアプリで比較中の2つのファイルを一致させた後WinMergeに戻った時、「選択されたファイルは同一です。」が二度表示されるのを修正した。[(#1902)](https://github.com/WinMerge/winmerge/issues/1902)
- Pythonの最近のバージョンのキーワードがシンタックスハイライトされるようにした。[(PR #1938)](https://github.com/WinMerge/winmerge/pull/1938)

### フォルダ比較

- 不具合修正: フォルダアイコンの左の ＜ をクリックすると不必要なスクロールが発生することがある問題を修正した。 [(#1915)](https://github.com/WinMerge/winmerge/issues/1915)
- フォルダ比較中に使用するCPUコアの数を変更できるようにした。[(#1945)](https://github.com/WinMerge/winmerge/pull/1945)

### Webページ比較

- 比較結果をレポートに出力できるようにした。[(#1941)](https://github.com/WinMerge/winmerge/pull/1941)

### コマンドライン

- 比較結果をレポートに出力できるようにした。[(#1941)](https://github.com/WinMerge/winmerge/pull/1941)

### アーカイブサポート

- 7-Zip を 23.01 に更新した。[(PR #1913)](https://github.com/WinMerge/winmerge/pull/1913)

### 翻訳

- 新規翻訳: Tamil (PR #1946)
- 翻訳の更新:
  - Brazilian (PR #1948)
  - Chinese Traditional (PR #1940)
  - Corsican (PR #1933)
  - French (PR #1927,#1928,#1951)
  - Korean (PR #1908)
  - Lithuanian (PR #1949)
  - Portuguese (PR #1930)
  - Slovenian
  - Turkish (#1931)

## 2.16.31 Beta の新機能

### 全般

- 不具合修正: 置換フィルタの[置換後の文字列]に\rや\nを指定しても改行文字に置換されなかった。[(#1861)](https://github.com/WinMerge/winmerge/issues/1861)
- タスクバーのWinMergeアイコンのジャンプリストに[クリップボード比較]等のメニューを追加した。[(PR #1828)](https://github.com/WinMerge/winmerge/pull/1828)
- HTMLレポートのタイトル部分をスクロールしても表示し続ける様にした。[(PR #1892)](https://github.com/WinMerge/winmerge/pull/1892)

### ファイル比較

- 不具合修正: 移動ダイアログで行数を超える行番号を指定した場合、意図しない行に移動することがある問題を修正した。[(PR #1826)](https://github.com/WinMerge/winmerge/pull/1826)
- 不具合修正: [改行文字の違いを無視する]を指定しているにもかかわらず、改行文字が異なる行の行末がハイライトされるのを修正した。[(#1838, PR #1849)](https://github.com/WinMerge/winmerge/pull/1849)
- [すべてを右側/左側にコピー]でメッセージボックスを表示するようにした。[(PR #1827)](https://github.com/WinMerge/winmerge/pull/1827)
- 移動ダイアログに行番号の下限と上限を表示するようにした。[(PR #1896)](https://github.com/WinMerge/winmerge/pull/1896)

## フォルダ比較

- 不具合修正: 下に表示される比較状況表示ウインドウの全項目数が、全桁表示されないことがあるのを修正した。 [(#1840)](https://github.com/WinMerge/winmerge/issues/1840)
- 不具合修正: クイックコンテンツ比較方法で比較したとき、フォルダ比較の結果とファイル比較結果の結果が異なることがある問題を修正した。[(#1882)](https://github.com/WinMerge/winmerge/issues/1882)

### プラグイン

- PrettifyJSON: jq のバージョンを 1.6 に更新。[(#1871)](https://github.com/WinMerge/winmerge/issues/1871)
- プラグイン関連のエラーメッセージを翻訳可能にした。[(#1873)](https://github.com/WinMerge/winmerge/issues/1873)
- ApplyPatch: GNU patch のバージョンを 2.7.6-1 に更新。[(#1897)](https://github.com/WinMerge/winmerge/pull/1897)

### インストーラー

- /noicons コマンドラインオプションを指定すると表示されることがあるメッセージボックスを抑制可能なメッセージボックスに変更した。[(#1852)](https://github.com/WinMerge/winmerge/issues/1852)

### 翻訳

- 翻訳の更新:
  - Brazilian (PR #1829,#1837,#1876,#1903)
  - Bulgarian (PR #1822)
  - Chinese Simplified (PR #1835,#1846,#1885,#1906)
  - Dutch (PR #1831)
  - French (PR #1841,#1842,#1894)
  - Galician (PR #1833)
  - German (PR #1850,#1875,#1907)
  - Hungarian (PR #1832,#1839,#1845,#1878,#1905)
  - Japanese
  - Korean (PR #1820,#1821,#1877)
  - Lithuanian (PR #1847,#1889,#1904)
  - Polish (PR #1869,#1870,#1884)
  - Portuguese (PR #1843,#1895)
  - Spanish (PR #1834)
  - Russian (PR #1824,#1825,#1862)

## 既知の問題

 - フォルダ比較で画像比較を有効にした場合、比較結果が安定しない。 [(#1391)](https://github.com/WinMerge/winmerge/issues/1391)
 - 巨大ファイルの比較でクラッシュすることがある。[(#325)](https://github.com/WinMerge/winmerge/issues/325)
 - 大幅に異なるフォルダの比較が非常に時間がかかる。 [(#322)](https://github.com/WinMerge/winmerge/issues/322)
 - 新規作成で表示範囲を超えるテキストを貼り付けたとき、垂直スクロールバーでスクロールできない。 [(#296)](https://github.com/WinMerge/winmerge/issues/296)
