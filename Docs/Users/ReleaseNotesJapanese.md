# WinMerge 2.16.26 リリースノート

- [このリリースについて](#about-this-release)
- [2.16.26の新機能](#what-is-new-in-21626)
- [2.16.25 beta の新機能](#what-is-new-in-21625-beta)
- [既知の問題](#known-issues)

2023年1月

## このリリースについて

WinMerge の 2.16.26 安定版リリースです。
このリリースは、以前の WinMerge 安定版リリースに代わる推奨リリースです。

不具合は <a href="http://github.com/WinMerge/winmerge/issues">bug-tracker</a> で報告してください。
日本語での報告は、<a href="https://osdn.net/projects/winmerge-jp/ticket/">こちら</a>でお願いします。

## 2.16.26 の新機能

### 全般

- ヘッダーバーに特定のパス名が設定されるとクラッシュすることがある問題を修正。

### ファイル比較

- ファイルパスバーで編集する時、Ctr+Vキーでテキストを貼り付けられるようにした。[(#1651)](https://github.com/WinMerge/winmerge/issues/1651)

### フォルダ比較

- 不具合修正: エクスポートされた.iniファイルをインポートしても行フィルタや置換フィルタが復元されない問題を修正。(正常に行フィルタや置換フィルタを復元するには、最新バージョンでエクスポートした.iniファイルを使用する必要があります) [(#1638)](https://github.com/WinMerge/winmerge/issues/1638)
- 不具合修正: フィルヘッダーバーのメニューの[開く]が無効化されていたのを修正。
- 3フォルダ比較時、差異があるが2つのフォルダは一致していた場合、比較結果列に2つが一致していることを表示するようにした。[(PR #1649)](https://github.com/WinMerge/winmerge/pull/1649)
- コンテキストメニュー→[左/右側を開く]→[親フォルダを開く]を選択したとき、WinMergeで選択していたファイルがExplorerでも選択されるようにした。[(#1662)](https://github.com/WinMerge/winmerge/issues/1662)
- 3フォルダ比較で[表示]→[3方向比較]メニューがフォルダに対してできるようにした。[(PR #1683)](https://github.com/WinMerge/winmerge/pull/1683)

### バイナリ比較

- 不具合修正: フィルヘッダーバーのメニューの[開く]が無効化されていたのを修正。

### Webページ比較

- 不具合修正: オプションダイアログの単語の差異の削除の色が使われていなかったのを修正。
- [数字を無視する]オプションが機能するようにした。

### オプションダイアログ

- オプションダイアログの[比較>フォルダ]カテゴリの改善。[(PR #1645)](https://github.com/WinMerge/winmerge/pull/1645)

### プラグイン

- MarkdownファイルをHTMLに変換するプラグイン(PreviewMarkdown)を追加。[(PR #1641)](https://github.com/WinMerge/winmerge/pull/1641)
- PlantUMLのファイルを画像に変換するプラグイン(PreviewPlantUML)を追加。[(PR #1666)](https://github.com/WinMerge/winmerge/pull/1666)
- CompareMSExcelFiles: プラグインオプションウインドウに"ワークシートをHTMLとして保存して比較する"を追加。
- ApacheTika: Apache Tika のバージョンを 2.6.0 に更新。
- ApacheTika: もし、Java がインストールされていない場合、OpenJDK 19.0.2 をダウンロードして使用するようにした。

### 翻訳

- 翻訳の更新:
  - Brazilian (PR #1656,#1670)
  - Chinese Simplified (PR #1635,#1665,#1667,#1677,#1681)
  - Corsican (PR #1674)
  - French (PR #1640,#1679)
  - German (PR #1660,#1671)
  - Hungarian (PR #1664)
  - Japanese
  - Lithuanian (PR #1657,#1673)
  - Polish (PR #1648)
  - Portuguese (PR #1669)
  - Russian (PR #1676)
  - Slovenian
  - Swedish (PR #1655,#1663,#1682)

## 2.16.25 Beta の新機能

### ファイル比較

- 不具合修正: 範囲選択して置換するとすべて置換されないことがある問題を修正。[(#1556)](https://github.com/WinMerge/winmerge/issues/1556)
- 不具合修正: 正規表現を使用した置換でハングすることがある問題を修正。[(#1575)](https://github.com/WinMerge/winmerge/issues/1575)
- SQLファイルのシンタックスハイライトのキーワード追加。[(PR #1493)](https://github.com/WinMerge/winmerge/pull/1493)
- htmlファイルのシンタックスハイライトキーワードを最新化。[(PR #1565)](https://github.com/WinMerge/winmerge/pull/1565)
- cssファイルのシンタックスハイライトキーワードを最新化。[(PR #1572)](https://github.com/WinMerge/winmerge/pull/1572)
- サイズが大きいファイルの表示速度を改善。[(PR #1574)](https://github.com/WinMerge/winmerge/pull/1574)
- Ctrl+Nキーで空のファイル比較ウインドウを表示した時、左側のペインにフォーカスが当たるようにした。 [(#1598)](https://github.com/WinMerge/winmerge/issues/1598)

### バイナリ比較

- 64bit版ではファイルサイズが2GB以上のファイルも開けるようにした。[(PR #1549)](https://github.com/WinMerge/winmerge/pull/1549)

### フォルダ比較

- 不具合修正: [列の表示]ダイアログでOKボタンを押すとクラッシュすることがある問題を修正。[(PR #1568)](https://github.com/WinMerge/winmerge/pull/1568)
- 不具合修正: ファイルやフォルダをコピーするときクラッシュすることがあるのを修正。[(#1558)](https://github.com/WinMerge/winmerge/issues/1558)
- 不具合修正: フォルダとフォルダ内のファイルを選択して圧縮ファイルを作成すると、圧縮ファイルに同じファイルが含まれてしまう問題を修正。[(#1588)](https://github.com/WinMerge/winmerge/issues/1588)
- 不具合修正: ツリーモードでフォルダを展開したとき、想定外の位置にスクロールすることがあるのを修正。[(osdn.net #46061)](https://osdn.net/projects/winmerge-jp/ticket/46061)
- 不具合修正: フォルダ比較レポート内の#や%を含むファイル名のファイル比較レポートへのリンクが正しくない問題を修正。[(osdn.net #46082)](https://osdn.net/projects/winmerge-jp/ticket/46082)
- 3フォルダー比較で3つ存在しないファイルの「比較結果」列の表示を改善。[(PR #1545)](https://github.com/WinMerge/winmerge/pull/1545)
- コンテキストメニューに "すべての表示列をコピー" を追加 [(PR #1615)](https://github.com/WinMerge/winmerge/issues/1615)
    
### オプションダイアログ

- 変更されたファイルの自動再読み込みオプションを追加 [(PR #1611)](https://github.com/WinMerge/winmerge/pull/1611)

### 翻訳

- 翻訳の更新:
  - Brazilian (PR #1617,#1630)
  - Chinese Simplified (PR #1614)
  - Corsican (PR #1628,#1629)
  - Galician (#1581)
  - German (PR #1616,#1633)
  - Hungarian (PR #1618,#1631)
  - Lithuanian (PR #1621,#1632)
  - Japanese
  - Polish (PR #1566)
  - Russian (PR #1553,#1554,#1555)
  - Slovenian
  - Swedish (PR #1594)
  - Turkish (PR #1563)

### その他

- pngファイルのサイズを縮小。[(PR #1541)](https://github.com/WinMerge/winmerge/pull/1541)
- サイズ最適化のためフィールドを再配置。[(PR #1546)](https://github.com/WinMerge/winmerge/pull/1546)
- Diff3.h のリファクタリング。[(PR #1577)](https://github.com/WinMerge/winmerge/pull/1577)
- コメントの誤りの修正。[(PR #1578)](https://github.com/WinMerge/winmerge/pull/1578)
- 同じファイルを開く場合の処理の最適化。[(PR #1579)](https://github.com/WinMerge/winmerge/pull/1579)
- SubeditList.cpp のリファクタリング。[(PR #1580)](https://github.com/WinMerge/winmerge/pull/1580)
- unicoder.cpp のリファクタリング。[(PR #1582)](https://github.com/WinMerge/winmerge/pull/1582), [(PR #1584)](https://github.com/WinMerge/winmerge/pull/1584), [(PR #1586)](https://github.com/WinMerge/winmerge/pull/1586)
- markdown.cpp のリファクタリング。[(PR #1590)](https://github.com/WinMerge/winmerge/pull/1590)
- デバッグ用の機能を追加 [(PR #1595)](https://github.com/WinMerge/winmerge/pull/1595)

## 既知の問題

 - Windows11でExplorerの右クリックメニューのWinMergeのメニューが機能しないことがある。[(#1619)](https://github.com/WinMerge/winmerge/issues/1619)
 - フォルダ比較で画像比較を有効にした場合、比較結果が安定しない。 [(#1391)](https://github.com/WinMerge/winmerge/issues/1391)
 - 巨大ファイルの比較でクラッシュすることがある。[(#325)](https://github.com/WinMerge/winmerge/issues/325)
 - 大幅に異なるフォルダの比較が非常に時間がかかる。 [(#322)](https://github.com/WinMerge/winmerge/issues/322)
 - 新規作成で表示範囲を超えるテキストを貼り付けたとき、垂直スクロールバーでスクロールできない。 [(#296)](https://github.com/WinMerge/winmerge/issues/296)
