# WinMerge 2.16.22 リリースノート

- [このリリースについて](#about-this-release)
- [2.16.22の新機能](#what-is-new-in-21622)
- [2.16.21 beta の新機能](#what-is-new-in-21621-beta)
- [既知の問題](#known-issues)

2022年7月

## このリリースについて

WinMerge の 2.16.22 安定版リリースです。
このリリースは、以前の WinMerge 安定版リリースに代わる推奨リリースです。

不具合は <a href="http://github.com/WinMerge/winmerge/issues">bug-tracker</a> で報告してください。
日本語での報告は、<a href="https://osdn.net/projects/winmerge-jp/ticket/">こちら</a>でお願いします。

## 2.16.22 の新機能

### 全般

- ヘッダーバーのキャプションを変更できるようにした。[(#1395)](https://github.com/WinMerge/winmerge/issues/1395)

### ファイル比較

- 不具合修正: [置換]ダイアログで置換後の文字列に検索する文字列が含まれ、[選択範囲]を選択した場合、[すべて置換]を押下すると、1つしか置換されないのを修正。[(#1376)](https://github.com/WinMerge/winmerge/issues/1376)
- 不具合修正: 検索ダイアログで「大文字と小文字を区別する」のチェックを外しているにもかかわらず、キリル文字等の大文字小文字を無視して検索できない問題を修正。 [(#1380)](https://github.com/WinMerge/winmerge/issues/1380)
- 不具合修正: 1行が長い場合、矩形選択に時間がかかりすぎる。 [(#1386)](https://github.com/WinMerge/winmerge/issues/1386)
- 不具合修正: 同じ行に複数置換対象がある場合、正規表現による文字列置換が正常に行われない。([#1387](https://github.com/WinMerge/winmerge/issues/1387), [PR #1388](https://github.com/WinMerge/winmerge/pull/1388))
- 行内差異処理の高速化 [(PR #1411)](https://github.com/WinMerge/winmerge/pull/1411)

### フォルダ比較

- 不具合修正: 3フォルダ比較時、ファイル名が大文字/小文字のみ異なる場合に正しく表示されないのを修正。[(#1372)](https://github.com/WinMerge/winmerge/pull/1372)
- 不具合修正: 片側にしか存在しないファイルまたはフォルダーを、同名のファイルまたはフォルダが存在する名前にリネームした場合、比較結果が正しく表示されない問題を修正。[(PR #1392)](https://github.com/WinMerge/winmerge/pull/1392)
- 不具合修正: フォルダ比較完了後ステータスバーに「経過時間」が表示されなくなっていたのを修正。
- 不具合修正: フォルダ比較ウィンドウでフォルダの名前を変更した後、フォルダ内の項目のフォルダ列が更新されない問題を修正。[(PR #1408)](https://github.com/WinMerge/winmerge/pull/1408)

### プラグイン

- 不具合修正: CompareMSExcelFilesプラグイン: 設定ダイアログの[画像分割サイズ]でCtrl+V等のクリップボード操作ができるようにした。また数値以外が入力できてしまうことがあったのを修正。[(#1374)](https://github.com/WinMerge/winmerge/pull/1374)
- wsc拡張子のプラグインに対応 [(PR #1390)](https://github.com/WinMerge/winmerge/pull/1390)

### アーカイブサポート

- 7-Zip 22.00 に更新

### プロジェクトファイル

- プロジェクトファイルに非表示項目を保存できるようにした。[(PR #1377)](https://github.com/WinMerge/winmerge/pull/1377)

### オプションダイアログ

- プロジェクトカテゴリに非表示の項目を追加。[(PR #1377)](https://github.com/WinMerge/winmerge/pull/1377)

### 翻訳

- 翻訳の更新:
  - Bulgarian (PR #1375)
  - French (PR #1418)
  - Galician (PR #1400)
  - German (PR #1396,#1399)
  - Hungarian (PR #1393,#1398)
  - Japanese
  - Lithuanian (PR #1394)
  - Portuguese (PR #1416)

### 内部変更

- SuperComboBox.cpp 内のタイポ修正。[(PR #1379)](https://github.com/WinMerge/winmerge/pull/1379)

## 2.16.21 Beta の新機能

### 全般

- 不具合修正: Event ID 78 のサイドバイサイドエラーが発生する可能性があるのを修正。[(#1312)](https://github.com/WinMerge/winmerge/issues/1312)

### ファイル比較

- 不具合修正: 行内差異単位の[左側/右側にコピー]が想定通りに動作しないのを修正。[(#1334)](https://github.com/WinMerge/winmerge/issues/1334)
- 不具合修正: オプションダイアログで[行内差異の色付け]で[単語単位]を選んでいるにもかかわらず、キリル文字を含む行の行内差異が単語単位でハイライトされないのを修正。[(#1362)](https://github.com/WinMerge/winmerge/issues/1362)
- 不具合修正: .sql ファイル内のコメント(--)がコメントの色にならないのを修正。[(#1354)](https://github.com/WinMerge/winmerge/issues/1354)
- ABAP の言語のシンタックスハイライトができるようにした。[(#1340)](https://github.com/WinMerge/winmerge/issues/1340)
- diff アルゴリズムに none を追加。(行の挿入・削除検出を行わず、行を上から順に愚直に比較するアルゴリズムです。)
- ロケーションペインでマウスホイール操作したとき、エディタウインドウをスクロールするようにした。
- ファイルのバックアップに失敗したとき、バックアップ先のファイル名も表示するようにした。 [(#1326)](https://github.com/WinMerge/winmerge/issues/1326)

### バイナリ比較

- [表示]→[垂直分割]メニューのチェックを外して上下分割表示ができるようにした。

### 画像比較

- [表示]→[垂直分割]メニューのチェックを外して上下分割表示にしたにもかかわらず、次回比較時元に戻ってしまうのを修正。
- 挿入/削除検出を有効にした時に、patience や histogram diff アルゴリズムを適用できるようにした。	

### Webページ比較

- 不具合修正: アドレスバーでKキーを押すとテキストが消えてしまうのを修正
- [表示]→[垂直分割]メニューのチェックを外して上下分割表示にしたにもかかわらず、次回比較時元に戻ってしまうのを修正。
- リソースツリー比較で展開した各ファイルの更新日時を展開した日時ではなく、そのリソースの日時になるようにした。
- Ctrl+L キーでアドレスバーに移動するようにした。

### フォルダ比較

- 不具合修正: Alt+↓キーで差異のある項目に移動後、Shift+矢印キーで選択範囲を広げようとすると、想定外の範囲となってしまうのを修正。[(#1335)](https://github.com/WinMerge/winmerge/issues/1335)
- 不具合修正: フォルダ比較を比較途中で停止した後、[選択項目を最新に更新]メニューで選択項目を更新すると、比較結果が確定していないフォルダのアイコンが同一または差異のアイコンになってしまうのを修正。[(#1349)](https://github.com/WinMerge/winmerge/issues/1349)
- 不具合修正: フォルダ比較後、外部アプリケーションでファイル名を大文字↔小文字に変更した後、[選択項目を最新に更新]メニューで該当ファイルを更新してもファイル名の変更が反映されないのを修正。[(#1358)](https://github.com/WinMerge/winmerge/pull/1358)
- 不具合修正: 3フォルダ比較時、ファイル名が大文字/小文字のみ異なる場合に正しく表示されないのを修正。[(#1372)](https://github.com/WinMerge/winmerge/pull/1372)(PRいただきました。ありがとうございます。)
- フォルダを複数選択して[非水平的に比較する]メニューを選択したとき、[サブフォルダーを含める]オプションが有効ならば別タブでフォルダを比較するようにした。[(#1367)](https://github.com/WinMerge/winmerge/issues/1367)
- エクスプローラと同様に番号を含むファイル名のソート順が数値の大きさ順になるようにした。[osdn.net #44557](https://osdn.net/projects/winmerge-jp/ticket/44557)

### レポート

- 不具合修正: 3つのフォルダのフォルダ比較レポートのタイトルに、中央フォルダが出力されない問題を修正。[(PR #1324)](https://github.com/WinMerge/winmerge/pull/1324)
- 不具合修正: フォルダ比較レポートでファイル名フィールドが "TEST.TXT|test.txt|\<None\>"のように"\<None\>"を含む場合、"\<None\>" の部分が正しく生成されない問題を修正。[(PR #1344)](https://github.com/WinMerge/winmerge/pull/1344)
- 不具合修正: ファイル比較レポートで行内の文字数が少ないときに、行番号列の幅が広くなりすぎるのを修正。[(PR #1333)](https://github.com/WinMerge/winmerge/pull/1333)
- 不具合修正: ファイル比較レポートの生成時に、ファイル名によって WinMerge がクラッシュする問題を修正。[(PR #1319)](https://github.com/WinMerge/winmerge/pull/1319) (PRいただきました。ありがとうございます。)

### 設定ログ

- 不具合修正: Windows 11 にもかかわらず、[ヘルプ]→[設定ログ]メニューで表示される内容に Windows 10 と表示されるのを修正。[(#1192)](https://github.com/WinMerge/winmerge/issues/1192)

### プラグイン

- 不具合修正: CompareMSExcelFiles プラグインのプラグイン設定で「ワークブックの情報を複数ファイルに展開する 」オプションが有効になっているにもかかわらず、メニュー項目「プラグイン]→[展開プラグインで開く]から .xlsx ファイルを開いたとき、複数ファイルに展開されない問題を修正。[osdn.net #44522](https://osdn.net/projects/winmerge-jp/ticket/44522)
- 不具合修正: プラグインの選択ダイアログ: [展開プラグイン適用後、ファイルを同じウインドウタイプで開く]チェックボックスをチェックしても、次にダイアログを開いた時にはチェックされていない不具合を修正。
- プラグイン選択ダイアログのサイズを変更できるようにした。[(#1308)](https://github.com/WinMerge/winmerge/issues/1308)

### コマンドライン

- 不具合修正: ファイルを3つ指定しないと機能しないコマンドラインオプション(`/al`, `/am`, `/ar`)を2つのファイル指定と共に使用するとクラッシュするのを修正。[(#1363)](https://github.com/WinMerge/winmerge/issues/1363)

### シェルエクステンション

- 不具合修正: Windows11のシェルエクステンション: [その他のオプションを表示]で表示されるWinMergeのメニューが[拡張メニューを有効にする]にチェックをしているにもかかわらず、拡張メニューにならないのを修正。
- 不具合修正: Windows11のシェルエクステンション: エクスプローラのコンテキストメニューからWinMergeを起動したとき、タスクバーにアイコンが表示されないのを修正。

### 翻訳

- 翻訳の更新:
  - Chinese Simplified (PR #1330)
  - Corsican (PR #1331,#1347)
  - German (PR #1311,#1329)
  - Hungarian (PR #1325)
  - Italian (PR #1355)
  - Japanese (PR #1338)
  - Lithuanian (PR #1318,#1327)
  - Polish (#1323)
  - Portuguese (PR #1317,#1345)
  - Slovenian
  - Turkish (#1332)
  - Russian (PR #1310)

### 内部変更

- タイポ修正。[(PR #1313)](https://github.com/WinMerge/winmerge/pull/1313) 
- ソースコード内のコメントで「GNU General Public License」 が 「GNU DIFF General Public License」になっていたのを修正。[(PR #1342)](https://github.com/WinMerge/winmerge/pull/1342)
- diffutils で islower()+toupper() の代わりに isupper()+tolower() を使用。 [(PR #1351)](https://github.com/WinMerge/winmerge/pull/1351)
- diff.h で定義されている変数の初期化を追加。[(PR #1360)](https://github.com/WinMerge/winmerge/pull/1360)

## 既知の問題

 - フォルダ比較で画像比較を有効にした場合、比較結果が安定しない。 (#1391)
 - 巨大ファイルの比較でクラッシュすることがある。(#325)
 - 大幅に異なるフォルダの比較が非常に時間がかかる。 (#322)
 - 新規作成で表示範囲を超えるテキストを貼り付けたとき、垂直スクロールバーでスクロールできない。 (#296)
