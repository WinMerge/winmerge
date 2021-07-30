# WinMerge 2.16.14 リリースノート

- [このリリースについて](#about-this-release)
- [2.16.14の新機能](#what-is-new-in-21614)
- [2.16.13 beta の新機能](#what-is-new-in-21613-beta)
- [既知の問題](#known-issues)

2021年7月

## このリリースについて

WinMerge の 2.16.14 安定版リリースです。
このリリースは、以前の WinMerge 安定版リリースに代わる推奨リリースです。

不具合は <a href="http://github.com/winmerge/winmerge/issues">bug-tracker</a> で報告してください。
日本語での報告は、<a href="https://osdn.net/projects/winmerge-jp/ticket/">こちら</a>でお願いします。

## 2.16.14 の新機能

### 全般

- WinMergeのウインドウを閉じたにもかかわらず、WinMergeのプロセスが終了しないことがある問題の対策。

### ファイル比較

- 不具合修正: 置換フィルター:「検索する文字列」を空にしたとき、ファイル比較時ハングする。

### フォルダ比較

- 不具合修正: フォルダ比較ウィンドウでファイルをダブルクリックしてファイル比較ウィンドウを開いた後、フォルダ比較ウィンドウに戻るとファイルの選択が解除される。 (PR #857)
- コンテキストメニューに「新しいウインドウで比較」メニューを追加。 (#232,#277)

### バイナリ比較

- 不具合修正: タイトルバーが更新されないことがある。

### 画像比較

- EXIF情報で回転表示を指示している画像ファイルが回転して表示されない。 (winmerge/winimerge #20)
- コンテキストメニューに次のメニューを追加。
  - 右へ90度回転
  - 左へ90度回転
  - 上下反転
  - 左右反転

### オプションダイアログ

- 「メッセージボックス」カテゴリを追加し、メッセージボックスの「再びこの質問をしない」チェックボックスにチェックして表示されなくなったメッセージボックスを再表示できるようにした。 (#772, PR #859)

### ファイルまたはフォルダーの選択ウインドウ

- 不具合修正: 「プロジェクトを保存」ボタンのドロップダウンメニューに不要なプラグインメニューが表示される。 (PR #882,#892)

### プラグイン

- 不具合修正: CompareMSExcelFiles.sct: 図形が含まれるExcelファイルを比較すると「この図にはビットマップしか含まれていません」メッセージが表示されることがある。
- 不具合修正: IgnoreColumns、IgnoreFieldsComma, IgnoreFieldsTabプラグインが正常に動作しない。 (#853)
- エディタスクリプトプラグインでもプラグイン引数指定とパイプによる連結ができるようにした。(PR #871)
- 以下の展開プラグインを追加:
  - PrettifyHTML
  - PrettifyYAML
  - ValidateHTML
  - QueryYAML
  - SelectColumns
  - SelectLines
  - ReverseColumns
  - ReverseLines
  - Replace
- 以下のエディタスクリプトプラグインを追加:
  - PrettifyHTML
  - PrettifyYAML
  - SelectColumns
  - SelectLines
  - ReverseColumns
  - ReverseLines
  - Replace
- Apache Tika をバージョン 2.0.0 に更新。
- yq コマンドをバージョン 4.11.1 に更新。

### コマンドライン

- /l コマンドラインオプション(行番号指定)を追加。 (osdn.net #41528)
- /t (ウインドウタイプ)、 /table-delimiter (テーブルファイルの区切り文字)、 /new(新規作成), /fileext(シンタックスハイライト用拡張子指定)、 /inifile(INIファイル指定) コマンドラインオプションを追加。

### インストーラ

- 「TortoiseGit との連携」や 「TortoiseSVN との連携」にチェックしていないにもかかわらず、WinMergeが3-wayマージツールとして登録されてしまう。 (#878)

### 翻訳

- 翻訳の更新:
  - Bulgarian (PR #850)
  - Dutch (PR #842,#893)
  - Galician (PR #869)
  - German (PR #860,#870,#883,#890)
  - Hungarian (PR #845,#856,#897)
  - Japanese
  - Lithuanian (PR #840,#849,#866,#875,#879,#894)
  - Portuguese (PR #846,#872,#898)
  - Slovenian (#858)
  - Russian (PR #847)
  - Turkish (PR #848)

### 内部変更

- 不具合修正: WinMerge doesn't build under Visual Studio 16.10.2 (#841)
- 不具合修正: x64: LINK : warning LNK4010: invalid subsystem version number 5.01;
    default subsystem version assumed (#855)
- 不具合修正: Project: heksedit cl : command line warning D9002: ignoring unknown
    option '/arch:SSE' (#861)
- 不具合修正:ALL.vs2019.sln cl : command line warning D9035: option 'Gm' has been
    deprecated and will be removed in a future release (#862)
- Resolves: Add continuous code security and quality analysis (PR #844, #843)
- Improvement: Add check and error mesage in DownloadDeps.cmd that path to
    7-zip exists (#864)

## 2.16.13 Beta の新機能

### 全般

- 不具合修正: 中国語版 WindowsXP で Register.bat が正常に動作しない。 (#780)
- WinMergeU.exe と同じフォルダに winmerge.ini を置いた場合、設定情報の読み込みと書き込みをそのファイルに対して行うようにした。 (#248) (PR #750)
- ツールバーの設定アイコンのドロップダウンメニューに「空行を無視する」「コメントの違いを無視する」を追加。 (#804)

### ファイル比較

- 不具合修正: PHPファイルのシンタックスハイライトが正常に行われない。 (PR #782, PR #802)
- 不具合修正: 設定ダイアログを閉じた後などに行頭でBSキーを押しても何も起こらなくなる。
- 不具合修正: ファイルを保存する前に「形式を指定して再比較」メニューを選択すると編集した内容が破棄されてしまう。
- 不具合修正: 「空行を無視する」と「無視された差異の行を同一行の色と同じにする」の設定を有効にしたとき、差異の色付けがずれてしまうことがある。(#834)
- Smarty シンタックスハイライトを追加。 (PR #821)
- 上書きモードでカレットの幅を太くするようにした。 (osdn.net #42179)

### フォルダ比較

- 不具合修正: BOMのみで中身がないファイルを比較すると比較結果が不安定になる。 (#768)
- 不具合修正: フォルダの再比較でクラッシュすることがある。 (osdn.net #42219)

### バイナリ比較

- 不具合修正: 新規作成したあとファイルが保存できなかった。
- 展開プラグインを使用できるようにした。

### 画像比較

- 不具合修正: Windows XP で画像を比較するとクラッシュすることがある。 (#784)
- 展開プラグインを使用できるようにした。

### プラグイン

- プラグイン機能の機能追加 (PR #797)
  - 展開プラグインをメニューから選択できるようにした。
  - 展開プラグインや比較前処理プラグインに引数を指定できるようにし、シェルやコマンドプロンプトの様にプラグインを | で連結して複数指定できるようにした。
  - フォルダ比較ウインドウで複数のファイルに対して一括で展開プラグインや比較前処理プラグインを指定できるようにした。指定したプラグインは、展開プラグイン、比較前処理プラグインの列で表示できます。
  - 展開プラグインを使用してファイルを開いた場合、タブのタイトルに「(U)」,比較前処理プラグインを使用している場合は、「(P)」を付加するようにした。
  - プラグイン設定ダイアログでプラグイン引数が省略されたときのデフォルト引数を指定できるようにした。
  - 自動展開/自動比較前処理が適用されるプラグインをプラグイン設定ダイアログで適用外にできるようにした。
  - 以下の展開プラグインを追加:
    - ApacheTika
    - PrettifyJSON
    - PrettifyXML
    - QueryCSV
    - QueryTSV
    - QueryJSON
    - MakeUpper
    - MakeLower
    - RemoveDuplicates
    - CountDuplicates
    - SortAscending
    - SortDescending
    - ExecFilterCommand
    - Tokenize
    - Trim
    - VisualizeGraphviz
  - 以下のエディタスクリプトプラグインを追加:
    - RemoveDuplicates
    - CountDuplicates
    - Tokenize
    - Trim
    - ApacheTika
    - PrettifyJSON
    - PrettifyXML

### 翻訳

- 不具合修正: "The number of automatically merged changes: %1\nThe number of unresolved conflicts: %2"が翻訳されない。(PR #763)
- 翻訳の更新:
  - French (PR #762)
  - Japanese
  - Polish (PR #769)
  - Turkish (PR #803)
  - Russian (PR #761)

### 内部変更

- Update CWindowsManagerDialog (PR #811)
- Update CWindowsManagerDialog - check some pointers for null and made safe
    casts (PR #824)


## 既知の問題

 - 巨大ファイルの比較でクラッシュすることがある。(GitHub #325)
 - 大幅に異なるフォルダの比較が非常に時間がかかる。 (GitHub #322)
 - 新規作成で表示範囲を超えるテキストを貼り付けたとき、垂直スクロールバーでスクロールできない。 (GitHub #296)
