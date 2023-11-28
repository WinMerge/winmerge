# WinMerge 2.16.36 リリースノート

- [このリリースについて](#about-this-release)
- [2.16.36の新機能](#what-is-new-in-21636)
- [2.16.35 beta の新機能](#what-is-new-in-21635-beta)
- [既知の問題](#known-issues)

2023年11月

## このリリースについて

WinMerge の 2.16.36 安定版リリースです。
このリリースは、以前の WinMerge 安定版リリースに代わる推奨リリースです。

不具合は <a href="http://github.com/WinMerge/winmerge/issues">bug-tracker</a> で報告してください。
日本語での報告は、<a href="https://sourceforge.net/p/winmerge-v2-jp/tickets/">こちら</a>でお願いします。

## <a name="what-is-new-in-21636"></a>2.16.36 の新機能

### 画像比較

- 不具合修正: 複数ページある画像ファイルが読み取り専用の場合、単一ページの画像として扱われる問題を修正した。 [(winmerge/winimerge#32)](https://github.com/winmerge/winimerge/issues/32))

### Webページ比較

- 差異が多い場合に極端に遅くなるのを改善

### ファイルフィルター

- 不具合修正: ファイルフィルター ダイアログで&lt;なし&gt;が選択されているとき等ボタンを押しても効果がないときにボタンが押せないようにした。 [(PR #2118)](https://github.com/WinMerge/winmerge/pull/2118)(PRをいただきました。ありがとうございます。)

### 翻訳

- 翻訳の更新:
  - Corsican (PR #2123)
  - Galician (PR #2120)
  - Hungarian (PR #2122)
  - Portuguese (PR #2119)
  - Spanish (PR #2120)
  - Slovenian
  - Turkish (PR #2116)

## <a name="what-is-new-in-21635-beta"></a>2.16.35 Beta の新機能

### ファイル比較

- 不具合修正: 3つのファイルを比較する際に、"左側/右側にコピーして次に進む"のクリックで、1つ差異をスキップすることがある問題を修正した。 [(#1234)](https://github.com/WinMerge/winmerge/issues/1234)
- 不具合修正: 印刷プレビューのボタンが翻訳できない問題を修正した。 [(#2083)](https://github.com/WinMerge/winmerge/issues/2083), [(#2079)](https://github.com/WinMerge/winmerge/issues/2079)
- 不具合修正: ウィンドウがリサイズされたときにヘッダーバーに設定されたキャプションが復元される問題を修正した。

### バイナリ比較

- 不具合修正: 32ビット版WinMergeで次の差異に移動するとファイルの末尾に移動してしまうことがあった。[(#2081)](https://github.com/WinMerge/winmerge/issues/2081))

### 画像比較

- 不具合修正: [MDI子ウィンドウが１つしかない場合、メインウインドウを閉じる]が選択されているにもかかわらず、'Esc' キーで子ウインドウのみが閉じるのを修正した。 [(#2084)](https://github.com/WinMerge/winmerge/issues/2084)

### Webページ比較

- 不具合修正: [MDI子ウィンドウが１つしかない場合、メインウインドウを閉じる]が選択されているにもかかわらず、'Esc' キーで子ウインドウのみが閉じるのを修正した。 [(#2084)](https://github.com/WinMerge/winmerge/issues/2084)
- 片方のペインでスクロールやクリック、入力をしたとき、他方のペインでも同様に行われるようにイベント同期機能を追加 [(PR #2111)](https://github.com/WinMerge/winmerge/pull/2111)

### フォルダ比較

- 不具合修正: Diffアルゴリズムがdefaultの場合、「改行文字の違いを無視する」等の設定が無視されていた問題を修正した。 [(#2080)](https://github.com/WinMerge/winmerge/issues/2080), [(#2099)](https://github.com/WinMerge/winmerge/issues/2099)

### ファイルフィルター

- 不具合修正: フィルターダイアログでファイルフィルターが選択されていない場合に、"[F]" がファイルフィルターとして設定される問題を修正した。

### インストーラ

- 不具合修正: ユーザ毎インストーラ(WinMerge-x.x.x-x64-PerUser-Setup.exe)でインストールするとき、一部誤ったレジストリパスに書き込みが行われていたのを修正した [(PR #2086)](https://github.com/WinMerge/winmerge/pull/2086)(PRをいただきました。ありがとうございます。)
- 不具合修正: インストール先フォルダ名にシングルクォーテーション(')が含まれると、Windows11用のシェルエクステンションの登録に失敗する問題を修正した。 [(#2109)](https://github.com/WinMerge/winmerge/issues/2109)

### 翻訳

- 翻訳の更新:
  - Brazilian (PR #2088,#2112)
  - Bulgarian (PR #2105)
  - Chinese Simplified (PR #2091,#2113)
  - French (PR #2106)
  - Galician (PR #2085,#2107)
  - Hungarian (PR #2093)
  - Japanese
  - Korean (PR #2092)
  - Lithuanian (PR #2090,#2114)
  - Polish (PR #2087)
  - Romanian (PR #2089,#2095)
  - Slovenian (#2096)
  - Spanish (PR #2085,#2107)
  - Turkish (PR #2076)

## <a name="known-issues"></a>既知の問題

 - フォルダ比較で画像比較を有効にした場合、比較結果が安定しない。 [(#1391)](https://github.com/WinMerge/winmerge/issues/1391)
 - 巨大ファイルの比較でクラッシュすることがある。[(#325)](https://github.com/WinMerge/winmerge/issues/325)
 - 大幅に異なるフォルダの比較が非常に時間がかかる。 [(#322)](https://github.com/WinMerge/winmerge/issues/322)
 - 新規作成で表示範囲を超えるテキストを貼り付けたとき、垂直スクロールバーでスクロールできない。 [(#296)](https://github.com/WinMerge/winmerge/issues/296)
