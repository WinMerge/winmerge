# WinMerge 2.16.34 リリースノート

- [このリリースについて](#about-this-release)
- [2.16.34の新機能](#what-is-new-in-21634)
- [2.16.32 beta の新機能](#what-is-new-in-21632-beta)
- [既知の問題](#known-issues)

2023年10月

## このリリースについて

WinMerge の 2.16.34 安定版リリースです。
このリリースは、以前の WinMerge 安定版リリースに代わる推奨リリースです。

不具合は <a href="http://github.com/WinMerge/winmerge/issues">bug-tracker</a> で報告してください。
日本語での報告は、<a href="https://osdn.net/projects/winmerge-jp/ticket/">こちら</a>でお願いします。

## 2.16.34 の新機能

### ファイル比較

- コマンドライン引数にパスとして NUL や \\.\NUL を指定しても比較できるようにした。 [(PR #2056)](https://github.com/WinMerge/winmerge/pull/2056)
- ツールバーアイコン[現在の行内差異]の説明に「(F4)」を追加した。 [(#2050)](https://github.com/WinMerge/winmerge/issues/2050)
- Diff アルゴリズムが default 以外に設定され、最終行の EOL が片方のファイルには存在するが他方のファイルには存在する場合にクラッシュすることがある問題を修正した。

### 画像比較

- 不具合修正:  ファイルを保存する際、ファイル名に .png が付加されてしまっていたのを修正した。

### フォルダー比較

- 不具合修正: ツリーモードが無効の場合、片方しか存在しないフォルダが表示されないことがある。 [(#2046)](https://github.com/WinMerge/winmerge/issues/2046)

### 行フィルター

- 行フィルターのウインドウのボタン制御を追加した。[(PR #2047)](https://github.com/WinMerge/winmerge/pull/2047)

### 置換フィルター

- 置換フィルターのウインドウのボタン制御を追加した。 [(PR #2068)](https://github.com/WinMerge/winmerge/pull/2068)

### 翻訳

- 翻訳の更新:
  - French (PR #2055,#2058)
  - Korean (PR #2057,#2059,#2060)

### 内部処理

- CMDIChildWnd のハンドリングに関する軽微な修正。 (PR #2043)

##  2.16.33 Beta の新機能

### 一般

- 起動直後のメモリ使用量とユーザオブジェクトの使用量を微量だが削減した。

### ファイル比較

- 不具合修正: 一行しかないファイルではAlt+Downキー等で差異に移動できなかった問題を修正した。[(#1972)](https://github.com/WinMerge/winmerge/issues/1972)
- 不具合修正: ダブルクリックで現在の差異を変更したとき、「右側へコピー」または「左側コピー」アイコン等が有効にならないことがある問題を修正した。[(#1980)](https://github.com/WinMerge/winmerge/issues/1980)
- 不具合修正: コマンドラインオプション `/o` (保存先ファイルの指定) を使用して起動したした場合、起動したウインドウの別タブで開いたファイルを保存すると別タブにもかかわらず `/o` で指定したファイルに保存されてしまう問題を修正した。[(#1988)](https://github.com/WinMerge/winmerge/issues/1988)
- 不具合修正: "コメントの違いを無視する" を有効にしても、片側のみ存在するインラインコメントが無視できないことがある。[(#2008)](https://github.com/WinMerge/winmerge/issues/2008)
- Rust の最近のバージョンのキーワードがシンタックスハイライトされるようにした。[(PR #1998)](https://github.com/WinMerge/winmerge/pull/1998)
- コンテキストメニューに[両側シェルメニュー]を追加した。[(#1986)](https://github.com/WinMerge/winmerge/issues/1986)[(PR #2021)](https://github.com/WinMerge/winmerge/pull/2021)

### テーブル比較

- ヘッダー部のコンテキストメニューの[最初の行をヘッダーとして使用する] がチェックされている場合、最初の行が非表示の場合でも、ヘッダーは最初の行を表示するようにした。
- テーブル比較ウインドウからレポート作成した場合、表形式でレポートが作成されるようにした。[(PR #1983)](https://github.com/WinMerge/winmerge/pull/1983)

### フォルダー比較

- 不具合修正: ファイル比較方法が "フルコンテンツ" または "クイックコンテンツ" でファイルサイズが64MBを超える場合、差異、左/右EOL列などが未定義の値として表示される問題を修正した。
- 不具合修正: バイナリコンテンツ比較方法でシンボリックリンクが正しく比較できない問題を修正した。[(#1976)](https://github.com/WinMerge/winmerge/issues/1976)
- 不具合修正: クイックコンテンツ比較方法を使用している場合、左/右EOL列の値が正しく表示されない場合がある問題を修正した。
- [表示]→[サブフォルダーの展開]メニューから差異があるフォルダまたは同一のフォルダのみ展開できるようにした。また、オプションウインドウの[比較/フォルダ]カテゴリに比較後、差異があるフォルダまたは同一のフォルダのみ自動的に展開できるようにする設定を追加した。[(PR #1964)](https://github.com/WinMerge/winmerge/issues/1382)[(PR #1382)](https://github.com/WinMerge/winmerge/pull/1964)
- フォルダ比較にデフォルト以外のDiffアルゴリズム(patience、histogramなど)を適用できるようにした。[(PR #2015)](https://github.com/WinMerge/winmerge/pull/2015)[(#2002)](https://github.com/WinMerge/winmerge/issues/2002)
- フォルダ比較に30秒以上時間がかかった場合、そのウインドウを閉じるとき確認メッセージを表示するようにした。

### 行フィルター

- 行フィルターや置換フィルターでフィルタされるはずの行とフィルタされない行が隣接している場合、フィルタされるはずの行がフィルタされないのを改善した。[(PR #2032)](https://github.com/WinMerge/winmerge/pull/2000)[(#796)](https://github.com/WinMerge/winmerge/issues/796)[(#1620)](https://github.com/WinMerge/winmerge/issues/1620)

### 置換フィルター

- 不具合修正: 空文字列にマッチするパターンを指定された場合、ハングアップしてしまう問題を修正した。
- 行フィルターや置換フィルターでフィルタされるはずの行とフィルタされない行が隣接している場合、フィルタされるはずの行がフィルタされないのを改善した。[(PR #2032)](https://github.com/WinMerge/winmerge/pull/2000)[(#796)](https://github.com/WinMerge/winmerge/issues/796)[(#1620)](https://github.com/WinMerge/winmerge/issues/1620)

### オプションダイアログ

- [シェル統合]カテゴリで[Windows 11 以降用シェル拡張を登録]ボタン押下で登録に失敗した場合、エラーメッセージを表示するウインドウを閉じないようにした。
  (インターネットに接続されていない Windows 11 環境ではCertumのルート証明書がないため、登録に失敗することがあります。その場合は、Certum Trusted Network CA2 のルート証明書をインストールしてみてください。)

### プラグイン

- 不具合修正: プラグインウインドウで Apache Tikaプラグインを無効にしても無効にならない問題を修正した。 [(#2012)](https://github.com/WinMerge/winmerge/issues/2012)
- jqをバージョン1.7に更新した。

### マニュアル

- po4a を使用してマニュアルを作成するようにした。 [(PR #1994)](https://github.com/WinMerge/winmerge/pull/1994)[(#499)](https://github.com/WinMerge/winmerge/pull/499)

### 翻訳

- 翻訳の更新:
  - Brazilian (PR #1969,#2001,#2025)
  - Chinese Traditional (PR #1953,#1971,#2017,#2026)
  - Corsican (PR #2022)
  - German (PR #1952,#1977,#1989)
  - Hungarian (PR #1968,#1999)
  - Japanese
  - Korean (PR #1979,#2030)
  - Lithuanian (PR #1974,#2018,#2027)
  - Polish (PR #1990)
  - Portuguese (PR #1973,#2014)
  - Slovenian
  - Ukrainian (PR #1955)

### 内部処理

- stdコンテナ内の挿入をreserveを使用して最適化した[(PR #2000)](https://github.com/WinMerge/winmerge/pull/2000)(PRをいただきました。ありがとうございます。)


## 既知の問題

 - フォルダ比較で画像比較を有効にした場合、比較結果が安定しない。 [(#1391)](https://github.com/WinMerge/winmerge/issues/1391)
 - 巨大ファイルの比較でクラッシュすることがある。[(#325)](https://github.com/WinMerge/winmerge/issues/325)
 - 大幅に異なるフォルダの比較が非常に時間がかかる。 [(#322)](https://github.com/WinMerge/winmerge/issues/322)
 - 新規作成で表示範囲を超えるテキストを貼り付けたとき、垂直スクロールバーでスクロールできない。 [(#296)](https://github.com/WinMerge/winmerge/issues/296)
