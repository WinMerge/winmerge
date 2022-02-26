---
title: 変更履歴
---

### 2022/02/28 2.16.18-jp-2

#### 全般

- 不具合修正: [オプション]ウインドウの[メッセージボックス]カテゴリや[プラグイン]ウインドウのリストビューのツールチップが途中で切れることがあるのを修正。[(#1210)](https://github.com/WinMerge/winmerge/issues/1210)
- 「すべてを右側にコピー」、「すべてを左側にコピー」メニューに対応するツールバーのアイコンのツールチップが「右側すべて」、「左側すべて」となっていたのをメニューと同じになるように修正。

#### ファイル比較

- 不具合修正: [類似行をマッチさせる]オプションを有効にしたとき、空白の無視設定によっては期待通り動作しないことがあるのを修正。[(#1209)](https://github.com/WinMerge/winmerge/issues/1209)
- 不具合修正: F4キーによる行内差異選択が正しく動作しないことがあるのを修正。

#### フォルダ比較

- 不具合修正: フォルダ比較レポートにリンクしているファイル比較レポートがファイル名によっては別のものに置き換わってしまう可能性があるのを修正。 [(PR #1171)](https://github.com/WinMerge/winmerge/pull/1171) (PRいただきました。ありがとうございます。)
- 不具合修正: 同じ名前のフォルダとファイルを選択して「選択項目を最新に更新」メニューをクリックすると、比較結果が正しく表示されない問題を修正。 [(PR #1189)](https://github.com/WinMerge/winmerge/pull/1189) (PRいただきました。ありがとうございます。)
- 不具合修正: 複数ファイルやフォルダを選択しているにもかかわらず、ステータスバーに「0個の項目を選択」と表示されてしまうことがある問題を修正。
- 不具合修正: [オプション]ウインドウの[比較/画像]カテゴリの[フォルダー比較で画像比較を有効にする]にチェックしてフォルダ内の画像を比較した場合、複数ページある画像ファイルで差異があるにもかかわらず、最後のページに差異がないとそのファイルには差異がないとみなしてしまう問題を修正。
- 不具合修正: [オプション]ウインドウの[比較/画像]カテゴリの[フォルダー比較で画像比較を有効にする]にチェックしてフォルダ内の画像を比較するとクラッシュすることがある問題を修正。[(#1176)](https://github.com/WinMerge/winmerge/issues/1176)
- ファイルフィルタの拡張 [(PR #1179)](https://github.com/WinMerge/winmerge/pull/1179)
  - その1: [ファイルまたフォルダーの選択]ウインドウの[フォルダー:フィルター]で`!*.bak`のように前に「!」を付けた場合、パターンにマッチしたファイルを無視するようにした。また、`!.git\`のように末尾に「\」をつけた場合、フォルダにマッチするパターンとみなすようにした。この例では、`.git` フォルダを無視して比較します。
  - その2: ファイルフィルタファイル(.fltファイル)で `f:`や`d:`に指定したパターンにマッチしたファイルまたはフォルダの一部を除外する指定 `f!:` と `d!:` を追加した。

#### プラグイン

- 不具合修正: [プラグインの選択]ウインドウで[パイプを追加]ボタンを押すと指定したプラグイン引数が消えてしまうのを修正

### 2022/01/29 2.16.18-jp-1

#### 全般

- 不具合修正: Googleドライブのファイルを比較するとクラッシュすることがある。
- タブバーのツールチップにフルパス名、使用しているプラグイン、行フィルタ等で無視されている行があるかどうかを表示するようにした。 [(#960)](https://github.com/WinMerge/winmerge/issues/960)
- Contributors.txt に漏れていた人の名前を追加 [(PR #1094)](https://github.com/WinMerge/winmerge/pull/1094)
- View→Select Fontメニュー項目から表示されるフォント選択ダイアログを表示している間は、親ウィンドウを操作できないようにしました。

#### ファイル比較

- 不具合修正: オプションダイアログの[色/差異]カテゴリで単語の差異のテキストの色を指定しても反映されなかった。 [(#1116)](https://github.com/WinMerge/winmerge/issues/1116)
- [行を右端で折り返す]を有効にしている場合、非常に長い行があるとハングアップしているように見えるほど遅くなることがあるのを少し改善した。 [(#1111)](https://github.com/WinMerge/winmerge/issues/1111)
- ステータスバーのエンコーディングをダブルクリックした時、表示されるコードページウインドウの[反映先]がダブルクリックした側のペインだけチェックが付くようにした。 [(#793)](https://github.com/WinMerge/winmerge/issues/793)

#### クリップボード比較

- 新機能: クリップボード比較 [(PR #1147)](https://github.com/WinMerge/winmerge/pull/1147)
  - [ファイル]→[クリップボードを開く]メニューを選択すると、クリップボード履歴の直近の2つの内容を比較することができるようになりました。
  - 子MDIウィンドウが表示されていないときにCtrl+Vキーを押しても比較できます。
  - この機能は、Windows 10 version 1809 以降と WinMerge 64bit版でのみ有効です。

#### フォルダ比較

- 不具合修正: 3フォルダ比較時、比較結果列のソート順が2フォルダ比較と異なっていた。 [(#483)](https://github.com/WinMerge/winmerge/issues/483)
- 不具合修正: フォルダー比較ウィンドウで "選択された項目を更新 "を実行すると WinMerge がクラッシュする場合があった。 [(PR #1120)](https://github.com/WinMerge/winmerge/pull/1120) (PRいただきました。ありがとうございます。)
- 不具合修正: 非再帰的比較時、親フォルダへ移動するアイコンが表示されなかった。
- 不具合修正: 以前のバージョンと異なりソート順が逆になっていた。

#### プラグイン

- [その他のプラグイン]メニューの名前をを[すべてのプラグイン]に変更。 [(#1139,PR #1140)](https://github.com/WinMerge/winmerge/pull/1140) (PRいただきました。ありがとうございます。)
- プラグインの説明を翻訳可能にした。
- Apache Tika のバージョンを 2.2.1 に更新。

#### コマンドライン

- クリップボード比較のコマンドラインオプション(`/clipboard-compare`)を追加

#### アーカイブサポート

- 7-Zip 21.07 に更新。

#### インストーラ

- 不具合修正: ReadMe.txtのクイックスタートのリンクが間違っていた。 [(#1127)](https://github.com/WinMerge/winmerge/issues/1127)
- 不具合修正: 32bit版のWinMergeインストール後、64bit版のWinMergeを上書きインストールすると 7z.dll が上書きされなかった。
- Windows11ではWindows11向けシェルエクステンションをインストールするようにした。

#### マニュアル

- 不具合修正: 「4.8 クイック比較切替閾値」と「4.9 バイナリ比較切替閾値」の説明が間違っていたのを修正。 [(#1100)](https://github.com/WinMerge/winmerge/issues/1100)

#### 内部変更

- DirScan.cpp のタイポ修正 [(PR #1118)](https://github.com/WinMerge/winmerge/pull/1118) (PRいただきました。ありがとうございます。)

### 2021/12/29 2.16.16-jp-4
#### 全般

* INIファイル(WinMergeU.exeと同じ場所のwinmerge.ini)に[Defaults]セクションとキー=値を記載した場合、WinMerge終了前に設定変更していても次回起動時記載した設定値で上書きするようにした。[GitHub PR #1038](https://github.com/WinMerge/winmerge/pull/1071) (PRいただきました。ありがとうございます。)

#### ファイル比較

* 不具合修正: ファイルサイズによってはクラッシュすることがあった。[GitHub #1101](https://github.com/winmerge/winmerge/issues/1101)
* 不具合修正: [ウインドウ]→[分割]メニューでウインドウを分割した後、ペインの高さをドラッグ操作で０にしたとき、エラーが発生する。
* [表示]→[シンタックスハイライト]メニューをマルチカラムにした。
* 3ファイル比較: 隣接ペインだけでなく、直接左側ペインから右側ペインまたは逆方向にコピーできるようにした。(コンテキストメニューまたは[マージ]→[高度な操作]から)

#### 画像比較

* 不具合修正: [ツール]→[レポートの生成]メニューのアイコンが表示されないのを修正。
* 複数ページある画像ファイルの前ページを1つの比較レポートに出力できるようにした。(現在のページのみ出力したい場合は、名前を付けて保存ダイアログで「すべてのページ」のチェックを外してください)[osdn.net #43374](https://osdn.net/projects/winmerge-jp/ticket/43374)

#### バイナリ比較

* 不具合修正: [ツール]→[レポートの生成]メニューのアイコンが表示されないのを修正。

#### フォルダ比較

* ツリーモードが無効の時に大量のファイルを比較すると結果が表示されるまでかなり時間がかかるのを改善した。[GitHub PR #1069](https://github.com/WinMerge/winmerge/pull/1069)

#### プロジェクトファイル

* プロジェクトファイルに「数字を無視」の設定を保存できるようにした。[GitHub PR #1068](https://github.com/WinMerge/winmerge/pull/1068) (PRいただきました。ありがとうございます。)

#### 設定ダイアログ

* 「'Esc'キーでウインドウを閉じる」コンボボックスに「MDI子ウインドウが１つしかない場合、メインウインドウを閉じる」を追加。[GitHub PR #1052](https://github.com/WinMerge/winmerge/issues/1042)

#### シェルエクステンション

* Windows11のシェルエクステンション: 現在のWindows11Build22000とBuild22499で問題なそうなので無効にしていた拡張メニューを復活させた。

#### プラグイン

* 不具合修正: [プラグイン]→[プラグインの再読み込み]メニューでプラグインの再読み込みがされていなかった

#### アーカイブサポート

* 7-Zip 21.06 に更新。

### 2021/11/29 2.16.16-jp-3

#### 全般

* 64bit版をVisualStudio2022でビルドするようにした。
* タブバーのタブにタイトルが長すぎて表示しきれない場合、ツールチップで表示するようにした。[GitHub PR #1038](https://github.com/WinMerge/winmerge/pull/1038) (PRいただきました。ありがとうございます。)

#### ファイル比較

* 不具合修正: 64MBを超えるファイルを開くと表示されるウインドウで[はい]を選択するとクラッシュする。[GitHub #1036](https://github.com/winmerge/winmerge/issues/1036)
* 不具合修正: C#(Java, JavaScript)のキーワードハイライトを修正。[GitHub PR #1040](https://github.com/WinMerge/winmerge/pull/1040) (PRいただきました。ありがとうございます。)
* 不具合修正: WinMergeで編集中のファイルを他のアプリケーションで変更後、WinMergeに戻るとフォーカスしているペインが変わってしまう。[GitHub #1050](https://github.com/winmerge/winmerge/issues/1050)
* 不具合修正: PHPのシンタックスハイライトを修正。[GitHub PR #1055](https://github.com/WinMerge/winmerge/pull/1055) (PRいただきました。ありがとうございます。)
* 比較オプションとして[数字を無視]を追加。[GitHub PR #1024](https://github.com/WinMerge/winmerge/pull/1042) (PRいただきました。ありがとうございます。)
* D言語のシンタックスハイライトを追加。[GitHub PR #1042](https://github.com/WinMerge/winmerge/pull/1042) (PRいただきました。ありがとうございます。)
* [比較/一般]カテゴリの"類似行をマッチさせる"オプションの動作を若干改善
    * 高速化
    * 差異ブロックが15行以下の制限を4096バイトの制限に変更
* 3ファイル比較でも"類似行をマッチさせる"オプションを機能させるようにした。
* 行フィルタ、置換フィルタ等で差異を無視した行が存在する場合、タイトルに"(F)"を付加するようにした。
* 片方のファイルにコメントのみの行があり、他方のファイルには対応する行が存在しない場合、[コメントの違いを無視する]と[空行を無視する]オプション両方が有効になっていれば、この差異を無視するようにした。[GitHub #1057](https://github.com/winmerge/winmerge/issues/1057)

#### パッチ生成ウインドウ

* [クリップボードにコピー]チェックボックスを追加。 [GitHub #923](https://github.com/winmerge/winmerge/issues/923)

#### フォルダ比較

* 不具合修正: 追加プロパティがある場合に3フォルダ比較でクラッシュする

#### 設定ダイアログ

* 比較オプションとして[数字を無視]を追加。[GitHub PR #1024](https://github.com/WinMerge/winmerge/pull/1042) (PRいただきました。ありがとうございます。)

### 2021/10/29 2.16.16-jp-2

#### 全般

* 不具合修正: Windows コモンダイアログに表示されるテキストが、言語を切り替えたときに その言語に変更されない。

#### テーブル比較

* 不具合修正: 最後の列の幅を変更できないことがある。[GitHub #998](https://github.com/winmerge/winmerge/issues/998)

#### 画像比較

* 不具合修正: 画像比較ウインドウへのファイルのドラッグ＆ドロップが2回目以降 受け付けられない。

#### フォルダ比較

* 不具合修正: 1つのプロジェクトファイルに複数の比較設定がされ、それぞれ異なる ファイルフィルタが指定された場合、ファイルフィルタが正しく適用されない。[GitHub PR #995](https://github.com/WinMerge/winmerge/pull/995) (PRいただきました。ありがとうございます。)
* 画像サイズとファイルサイズの差異を列に表示できるようにした。 [GitHub #131](https://github.com/WinMerge/winmerge/issues/131)(以下のPRで実現)
* Windowsプロパティシステムから取得したファイルの各プロパティ値と ハッシュ値(MD5, SHA-1, SHA-256)を列に表示できるようにした。[GitHub PR #996](https://github.com/WinMerge/winmerge/pull/996)

#### 設定ダイアログ

* 不具合修正: [Windows 11 以降用シェル拡張を登録]ボタンを別ユーザが押した後、 このボタンが有効にならない。

#### プラグイン

* 不具合修正: `get_PluginUnpackedFileExtension` で指定した拡張子に対応する シンタックスハイライトがされない。[GitHub #983](https://github.com/WinMerge/winmerge/issues/983)
* 不具合修正: リンク先が存在しないショートカットのMS Officeドキュメントを 開くとエラーメッセージがWinMergeの後ろに隠れてしまう。[GitHub #1007](https://github.com/WinMerge/winmerge/issues/1007)
* 不具合修正: Apache Tika プラグイン: Apache tika を 2.1.0 に更新し、ダウンロード元URLを 変更。
* 不具合修正: CompareMSExcelFiles.sct: [ワークブックの情報を複数ファイルに展開する] が有効のとき、ファイル名に含まれるシート名の前の数値をゼロパッデングにした。(パッチをいただきました。ありがとうございます。)

#### シェルエクステンション

* 不具合修正: 65536以上ファイルを選択すると無限ループする。[GitHub PR #987](https://github.com/WinMerge/winmerge/pull/987) (PRいただきました。ありがとうございます。)
* Windows 11用シェルエクステンションから起動したWinMergeでおこなった設定変更が、 通常起動したWinMergeに反映されない。

### 2021/09/29 2.16.14-jp-6

#### 全般

* 不具合修正: Windows 11 で選択されたメニューの文字の色が標準の色(白色)にならない
* 不具合修正: WinMergeを閉じたのにプロセスが残ったままになるもう一つの問題を修正[GitHub #903](https://github.com/winmerge/winmerge/issues/903)
* Windows11用のShellExtensionに必要なためコード署名をするようした

#### ファイル比較

* 不具合修正: 左側ペインをクリック後Shiftキーを押しながらロケーションペインの左側バーをクリックしたとき、右側ペインも選択されてしまう[GitHub #970](https://github.com/winmerge/winmerge/issues/970)
* 不具合修正: Diffペインを再表示したときのスクロール位置が正しくない[osdn.net #42862](https://osdn.net/projects/winmerge-jp/ticket/42862)
* 不具合修正: ファイル保存に失敗したときのエラーメッセージが適切ではなかった
* 検索ダイアログの[このダイアログボックスを閉じない]の初期値をチェックありにしてほしい[GitHub #970](https://github.com/winmerge/winmerge/issues/970)

#### フォルダ比較

* 不具合修正: ファイルを大量にコピー後、操作できなくなる時間が長い

#### テーブル比較

* 外部のアプリケーション等が編集中のファイルを更新後、WinMergeが再読み込みしたとき、列の幅を再計算しないように変更[GitHub #951](https://github.com/winmerge/winmerge/issues/951)

#### 画像比較

* 不具合修正: ロケーションペインのコントロールの配置、サイズ変更時の動作を改善[GitHub winimerge#24](https://github.com/WinMerge/winimerge/issues/24)

#### 設定ダイアログ

* プロジェクトファイルに保存する項目や復元する項目の除外/包含を指定できるようにした。[GitHub PR #953](https://github.com/WinMerge/winmerge/pull/953) (PRいただきました。ありがとうございます。)
* 不具合修正: シェルカテゴリでARM64版のShellExtensionの登録/解除ができない
* Windows 11 用のShellExtension 登録/解除用ボタンを追加

#### プラグイン

* 以下のプラグインがうまく動かないのを修正
    * DisassembleIL
    * DisassembleNative

#### シェルエクステンション

* 不具合修正: ナビケーションペインでWinMergeの拡張メニューが二重に表示される。[osdn.net #42702](https://osdn.net/projects/winmerge-jp/ticket/42702)
* Windows 11 のエクスプローラのコンテキストメニュー用ShellExtension(WinMergeContextMenu.dll)を追加。
    * まだインストーラはこのシェルエクステンションを登録するようにはしていません。
    * 登録するには、設定ダイアログの[シェル統合]カテゴリ→[Windows 11以降用シェル拡張を登録]ボタンを押下します。
    * また「拡張メニューを有効にする」にチェックを入れても安定性の問題により拡張メニューを表示しないようにしています。

### 2021/08/29 2.16.14-jp-3

#### 全般

* 不具合修正: INIファイルに不正な値が含まれているときにクラッシュする。
* 不具合修正: 特定の環境でWinMergeが起動してすぐにクラッシュする。[GitHub #808](https://github.com/winmerge/winmerge/issues/808), [GitHub #905](https://github.com/winmerge/winmerge/issues/905), [GitHub #908](https://github.com/winmerge/winmerge/issues/908)
* 不具合修正: プラグインメニューの表示等で時間がかかっている場合にマウスカーソルが砂時計になっていなかった。

#### ファイル比較

* 不具合修正: 3ペイン表示時、Diffペインの高さの計算が間違っていた。
* 不具合修正: 一部のUnicode文字を含む文字列を正規表現で検索するとハイライトが正しく行われない。[GitHub #914](https://github.com/winmerge/winmerge/issues/914)
* 不具合修正: Shift+F4キー押下でクラッシュすることがあった。

#### テーブル比較

* 不具合修正: [ファイル]→[新規(3ペイン)]→[テーブル]メニューを選択すると、2ペインのウインドウが表示されてしまう。

#### バイナリ比較

* 不具合修正: 3ファイル比較時、[中央のファイルを保存]、[名前をつけて中央のファイルを保存]メニューが有効にならなかった。

#### 画像比較

* 不具合修正: コンテキストメニューのキーボードアクセラレーターが被っていた。[GitHub #905](https://github.com/winmerge/winmerge/issues/905)

#### プロジェクトファイル

* プロジェクトファイルに比較オプションを保存するようにした。[GitHub PR #915](https://github.com/winmerge/winmerge/pull/915) (PRいただきました。ありがとうございます。)

#### プラグイン

* q, yq, Apache Tika等のファイルをダウンロードするとき、ハッシュ値を確認するようにした。
* 以下のプラグインを追加
    * DisassembleJVM
    * DisassembleIL
    * DisassembleNative

#### シェルエクステンション

* ~~不具合修正: ナビケーションペインでWinMergeの拡張メニューが二重に表示される。[osdn.net #42702](https://osdn.net/projects/winmerge-jp/ticket/42702)~~
* 拡張メニューで中央として開くファイルを選択できるようにした。[GitHub #249](https://github.com/winmerge/winmerge/issues/249)

#### コマンドラインオプション

* `/c 列位置`を追加
* `/EnableExitCode` を追加(プロセスの終了コードに比較結果が格納されます。(0:ファイルが一致している場合、1:差異がある場合、2:何らかのエラー)

### 2021/07/29 2.16.14-jp-1

#### 全般

* WinMergeのウインドウを閉じたにもかかわらず、WinMergeのプロセスが終了しないことがある問題の対策。

#### ファイル比較

* 不具合修正: 置換フィルタ:「検索する文字列」を空にしたとき、ファイル比較時ハングする。

#### フォルダ比較

* 不具合修正: フォルダ比較ウィンドウでファイルをダブルクリックしてファイル比較ウィンドウを開いた後、フォルダ比較ウィンドウに戻るとファイルの選択が解除される。 [GitHub PR #857](https://github.com/winmerge/winmerge/pull/857) (PRいただきました。ありがとうございます。)
* コンテキストメニューに「新しいウインドウで比較」メニューを追加。 [GitHub #232](https://github.com/winmerge/winmerge/issues/232), [\#277](https://github.com/winmerge/winmerge/issues/277)

#### バイナリ比較

* 不具合修正: タイトルバーが更新されないことがある。

#### 画像比較

* EXIF情報で回転表示を指示している画像ファイルが回転して表示されない。 [GitHub winmerge/winimerge #20](https://github.com/winmerge/winimerge/issues/20)
* コンテキストメニューに次のメニューを追加。

    * 右へ90度回転
    * 左へ90度回転
    * 上下反転
    * 左右反転

#### オプションダイアログ

* 「メッセージボックス」カテゴリを追加し、メッセージボックスの「再びこの質問をしない」チェックボックスにチェックして表示されなくなったメッセージボックスを再表示できるようにした。  [GitHub #772](https://github.com/winmerge/winmerge/issues/772), [GitHub PR #859](https://github.com/winmerge/winmerge/issues/859)

#### ファイルまたはフォルダーの選択ウインドウ

* 不具合修正: 「プロジェクトを保存」ボタンのドロップダウンメニューに不要なプラグインメニューが表示される。 [GitHub PR #882](https://github.com/winmerge/winmerge/pull/882), [\#892](https://github.com/winmerge/winmerge/pull/892) (PRいただきました。ありがとうございます。)

#### プラグイン

* 不具合修正: CompareMSExcelFiles.sct: 図形が含まれるExcelファイルを比較すると「この図にはビットマップしか含まれていません」メッセージが表示されることがある。(パッチをいただきました。ありがとうございます。)
* 不具合修正: IgnoreColumns、IgnoreFieldsComma, IgnoreFieldsTabプラグインが正常に動作しない。 [GitHub #853](https://github.com/winmerge/winmerge/issues/853)
* エディタスクリプトプラグインでもプラグイン引数指定とパイプによる連結ができるようにした。[GitHub PR #871](https://github.com/winmerge/winmerge/pull/871)
* 以下の展開プラグインを追加:

    * PrettifyHTML
    * PrettifyYAML
    * ValidateHTML
    * QueryYAML
* 以下のエディタスクリプトプラグインを追加:

    * PrettifyHTML
    * PrettifyYAML
* Apache Tika をバージョン 2.0.0 に更新。
* yq コマンドをバージョン 4.11.1 に更新。

#### コマンドライン

* /l コマンドラインオプション(行番号指定)を追加。 [osdn.net #41528](https://osdn.net/projects/winmerge-jp/ticket/41528)
* /t (ウインドウタイプ)、 /table-delimiter (テーブルファイルの区切り文字)、 /new(新規作成), /fileext(シンタックスハイライト用拡張子指定)、 /inifile(INIファイル指定) コマンドラインオプションを追加。

#### インストーラ

* 「TortoiseGit との連携」や 「TortoiseSVN との連携」にチェックしていないにもかかわらず、WinMergeが3-wayマージツールとして登録されてしまう。 [GitHub #878](https://github.com/winmerge/winmerge/issues/878)

### 2021/07/05 PluginPack

* `IgnoreColumns.dll` を `IgnoreColumns_1-5.dll`にリネームすると指定したファイル名の一部の列範囲を無視する機能が動作しなくなっていたのを修正

### 2021/06/29 2.16.12-jp-3

#### 全般

* WinMergeU.exe と同じフォルダに winmerge.ini を置いた場合、設定情報の読み込みと書き込みをそのファイルに対して行うようにした[GitHub PR #750](https://github.com/winmerge/winmerge/pull/750) (PRいただきました。ありがとうございます。)
* ツールバーの設定アイコンのドロップダウンメニューに「空行を無視する」「コメントの違いを無視する」を追加[GitHub #804](https://github.com/winmerge/winmerge/issues/804)

#### ファイル比較

* 不具合修正: PHPファイルのシンタックスハイライトが正常に行われない[GitHub PR #782](https://github.com/winmerge/winmerge/pull/782)[GitHub PR #802](https://github.com/winmerge/winmerge/pull/802) (PRいただきました。ありがとうございます。)
* 不具合修正: 設定ダイアログを閉じた後などに行頭でBSキーを押しても何も起こらなくなる
* 不具合修正: ファイルを保存する前に「形式を指定して再比較」メニューを選択すると編集した内容が破棄されてしまう
* 不具合修正: 「空行を無視する」と「無視された差異の行を同一行の色と同じにする」の設定を有効にしたとき、差異の色付けがずれてしまうことがある
* Smarty シンタックスハイライトを追加[GitHub PR #821](https://github.com/winmerge/winmerge/pull/821) (PRいただきました。ありがとうございます。)
* 上書きモードでカレットの幅を太くするようにした[osdn.net #42179](https://osdn.net/projects/winmerge-jp/ticket/42179)

#### フォルダ比較

* 不具合修正: BOMのみで中身がないファイルを比較すると比較結果が不安定になる[GitHub #768](https://github.com/winmerge/winmerge/issues/768)
* 不具合修正: フォルダの再比較でクラッシュすることがある[osdn.net #42219](https://osdn.net/projects/winmerge-jp/ticket/42219)

#### バイナリ比較

* 不具合修正: 新規作成したあとファイルが保存できなかった
* 展開プラグインを使用できるようにした

#### 画像比較

* 不具合修正: Windows XP で画像を比較するとクラッシュすることがある[GitHub #784](https://github.com/winmerge/winmerge/issues/784)
* 展開プラグインを使用できるようにした

#### シェルエクステンション

* 不具合修正: 中国語版 WindowsXP で Register.bat が正常に動作しない[GitHub #780](https://github.com/winmerge/winmerge/issues/780)

#### プラグイン

* 展開プラグインをメニューから選択できるようにした
* 展開プラグインや比較前処理プラグインに引数を指定できるようにし、シェルやコマンドプロンプトの様にプラグインを | で連結して複数指定できるようにした
* フォルダ比較ウインドウで複数のファイルに対して一括で展開プラグインや比較前処理プラグインを指定できるようにした
* 展開プラグインを使用してファイルを開いた場合、タブのタイトルに「(U)」,比較前処理プラグインを使用している場合は、「(P)」を付加するようにした
* プラグイン設定ダイアログでプラグイン引数が省略されたときのデフォルト引数を指定できるようにした
* 自動展開/自動比較前処理が適用されるプラグインをプラグイン設定ダイアログで適用外にできるようにした
* 以下の展開プラグインを追加

    * ApacheTika
    * PrettifyJSON
    * PrettifyXML
    * QueryCSV
    * QueryTSV
    * QueryJSON
    * MakeUpper
    * MakeLower
    * RemoveDuplicates
    * CountDuplicates
    * SortAscending
    * SortDescending
    * ExecFilterCommand
    * Tokenize
    * Trim
    * SelectColumns
    * SelectLines
    * ReverseColumns
    * ReverseLines
    * Replace
    * VisualizeGraphviz
* 以下のエディタスクリプトを追加

    * RemoveDuplicates
    * CountDuplicates
    * Tokenize
    * Trim
    * SelectColumns
    * SelectLines
    * ReverseColumns
    * ReverseLines
    * Replace
    * ApacheTika
    * PrettifyJSON
    * PrettifyXML

### 2021/04/29 2.16.12-jp-1

#### 全般

* ARM64版を追加

#### ファイル比較

* 不具合修正: PHPファイルのシンタックスハイライトが正常に行われない[GitHub PR #751](https://github.com/winmerge/winmerge/pull/751)[GitHub PR #724](https://github.com/winmerge/winmerge/pull/724) (PRいただきました。ありがとうございます。)
* 不具合修正: 1つのDiffブロックに複数行ある場合、"^"を含む正規表現の置換フィルタが機能しない
* 不具合修正: Ctrl+0キーで拡大縮小していた文字のサイズが正確に元に戻らない
* 不具合修正: シンタックスハイライトが有効の場合にクラッシュすることがある
* 不具合修正: HTMLファイル内のスクリプトのシンタックスハイライトが正常に行われないことがある
* 不具合修正: 文字列検索ダイアログの横幅をもう少し広く[osdn.net #42063](https://osdn.net/projects/winmerge-jp/ticket/42063)
* 不具合修正: 一部の画面でキーボードアクセラレータが重複[osdn.net #42064](https://osdn.net/projects/winmerge-jp/ticket/42064)
* ステータスバーに選択した行数と文字数を表示するようにした[GitHub #135](https://github.com/winmerge/winmerge/issues/135)
* 次のファイル、前のファイルボタンにショートカットキーCtrl+F8, Ctrl+F7を割りつけた[GitHub #721](https://github.com/winmerge/winmerge/issues/721)
* C#のシンタックスハイライト用キーワードを最新化[GitHub PR #719](https://github.com/winmerge/winmerge/pull/719) (PRいただきました。ありがとうございます。)
* PHPのシンタックスハイライト用キーワードを最新化[GitHub PR #724](https://github.com/winmerge/winmerge/pull/724) (PRいただきました。ありがとうございます。)
* AutoItのシンタックスハイライトを改善[GitHub PR #749](https://github.com/winmerge/winmerge/pull/749)[GitHub PR #753](https://github.com/winmerge/winmerge/pull/753) (PRいただきました。ありがとうございます。)
* 行内差異の処理に時間がかかりすぎた場合、処理を中断するようにした
* [表示]→[空白を表示]メニューを選択したときに、表示されるタブやスペースを表す文字の色薄くした
* コンテキストメニューに[開く]→[親フォルダーを開く]メニューを追加

#### フォルダ比較

* 不具合修正: コピーの確認ダイアログウインドウの「はい」「いいえ」ボタンが重なっていることがある[GitHub #739](https://github.com/winmerge/winmerge/issues/739)
* 不具合修正: 3フォルダ比較時、[表示]→[ペインの交換]メニューを選択したとき比較結果が更新されない[osdn.net #41901](https://osdn.net/projects/winmerge-jp/ticket/41901)

#### バイナリ比較

* 不具合修正: ツールバーの[次のファイル]、[前のファイル]ボタンが押せない
* 不具合修正: Ctrl+マウスホイールによるフォントサイズの拡大縮小が1つのペインのみしか行われない

#### 画像比較

* 不具合修正: ツールバーの[次のファイル]、[前のファイル]ボタンが押せない
* 不具合修正: [表示]→[垂直分割]メニューのチェックを外した時のヘッダーバーの幅が適切ではなかった
* 不具合修正: ペインの幅を変えるときマウスポインタのアイコンが変化しない
* 左側ペインの垂直スクロールバーを表示しないようにした

#### ファイルまたはフォルダの選択ウインドウ

* ZIPファイル等の圧縮ファイルで[比較]ボタンのプルダウンメニューの[バイナリ]を選択したとき、バイナリファイルとして開けるようにした

#### アーカイブサポート

* 不具合修正: ZIPファイル等を開くとたまにクラッシュする

#### プラグイン

* 不具合修正: インタフェースの解放漏れがあった[GitHub #755](https://github.com/winmerge/winmerge/issues/755)

### 2021/03/29 2.16.10-jp-6

#### 全般

* タブバー: アクティブなタブと非アクティブなタブが区別しづらいといわれたので少し目立つようにタブに影をつけてみた
* ツールバー: 保存アイコンにドロップダウンメニューを追加
* なぜか2.16.10+-jp-5からFirefoxやChromeで64bit版zipをダウンロードすると、「このファイルを開くのは危険です」といわれるようになってしまったため、
                            いろいろ試行錯誤すると、日本語以外の翻訳ファイル(.po)を削除するとなぜか不平をいわれなくなったので、問題が解消するまで
                            日本語以外の翻訳ファイルを含めないようにした

#### ファイル比較

* 不具合修正: ファイルの文字コードがUTF-8以外で、コメントに日本語等のASCII以外の文字が含まれている場合、コメントフィルタが正しく動作しないことがあった
* コードページダイアログに「BOM」チェックボックスを追加

#### フォルダ比較

* 不具合修正: コードページダイアログでコードページを変更しても、開くファイルに反映されなかった

#### 画像比較

* 不具合修正: フォルダ比較ウインドウから右側しか存在しない画像ファイルを開くと画像が表示されない[osdn.net #41721](https://osdn.net/projects/winmerge-jp/ticket/41721)

#### ファイルまたはフォルダの選択ウインドウ

* 比較ボタンにバイナリ/画像等のファイルの種類を指定できるドロップダウンメニューを追加

#### アーカイブサポート

* 不具合修正: 上書きインストールでMerge7z.dllが更新されない
                            [osdn.net #41682](https://osdn.net/projects/winmerge-jp/ticket/41682)

### 2021/03/02 2.16.10-jp-4

#### ファイル比較

* 不具合修正: 空白比較オプションで「変更を無視」または「すべて無視」が選択され、かつ、「空行を無視する」が有効の場合、空行ではない行が無視されることがある。(バージョン 2.16.10+-jp-3 で発生)[osdn.net #41668](https://osdn.net/projects/winmerge-jp/ticket/41668)

### 2021/03/01 2.16.10-jp-3

#### 全般

* 不具合修正: フォルダ比較ウインドウからファイル比較ウインドウを開いた後、ファイル比較ウインドウを閉じずにフォルダ比較ウインドウを閉じるとクラッシュする。[GitHub #645](https://github.com/WinMerge/winmerge/issues/645)
* ツールバーの開くアイコンの右から最近開いたファイルやフォルダを選択できるようにした。

#### ファイル比較

* 不具合修正: VBのシンタックスハイライトで、一部のキーワードが着色しない。[osdn.net #41440](https://osdn.net/projects/winmerge-jp/ticket/41440)
* 不具合修正: Zipファイルの中身を比較しているときに、[表示]→[ペインの交換]でファイルを入れ替えてもタイトルが入れ替わらない。
* 不具合修正: オプションダイアログの[コードページ]カテゴリ、[mlang.dll...]のコンボボックスで[自動選択]を選択した場合、ファイルの文字コードをUTF-7とみなしてしまうことがある。[GitHub #607](https://github.com/WinMerge/winmerge/issues/607)
* 不具合修正: Shiftキーを押しながらロケーションペインでカーソルを移動するか、移動ダイアログで移動したとき、選択範囲が正しく広がらない。[sf.net #2237](https://sourceforge.net/p/winmerge/bugs/2237/)
* 不具合修正: AutoItのファイルを開いたとき、' でくくられた文字列が文字列の色にならない。[GitHub #610](https://github.com/WinMerge/winmerge/issues/610)
* 不具合修正: ウインドウを分割しているときにシンタックスハイライトのファイルタイプを変更したとき、2つ目のペインに変更が反映されない。[GitHub PR #624](https://github.com/winmerge/winmerge/pull/624) (PRいただきました。ありがとうございます。)
* シンタックスハイライト用に各ファイル種別の拡張子を追加設定できるページをオプションダイアログに追加。[GitHub PR #627](https://github.com/winmerge/winmerge/pull/627) (PRいただきました。ありがとうございます。)
* オプションダイアログの[一般]カテゴリ→[単一インスタンスモード]に「1つのみインスタンスを起動し、既起動インスタンスの終了を待つ」を追加。これは、SourcetreeやVisual Studioのような、起動した外部比較ツールの終了を検出したら比較に使用していた一時ファイルを削除してしまうタイプのバージョン管理ツールで単一インスタンスモードが正常に機能していなかったもの対策になっています。[GitHub #622](https://github.com/WinMerge/winmerge/issues/622)
* 空白無視、かつ空行無視が設定されているときは、半角スペースとタブしか含まない行を差分から除外。[osdn.net #41355](https://osdn.net/projects/winmerge-jp/ticket/41355)(パッチをいただきました。ありがとうございます。)
* 右クリックメニューに[選択した行を左/右側にコピー]メニューを追加[GitHub PR #642](https://github.com/winmerge/winmerge/pull/642) (PRいただきました。ありがとうございます。)

#### ファイルまたはフォルダの選択ウインドウ

* 不具合修正: このウインドウでは保存ボタンが押せるが、押しても空のファイルしか保存できなかったため、現在の指定内容をプロジェクトファイルとして保存するようにした。
* ファイルを1つだけ指定して比較ボタンを押した場合に、そのファイルのコピーと比較するようにした。

#### オプションダイアログ

* 不具合修正: 色の設定ダイアログの「色の作成」で作成した色が保存されないことがある。[GitHub PR #624](https://github.com/winmerge/winmerge/pull/624) (PRいただきました。ありがとうございます。)

#### アーカイブサポート

* RAR5形式の圧縮ファイルを展開できるようにした。[GitHub #644](https://github.com/WinMerge/winmerge/issues/644)

#### コマンドライン

* /sw オプションを追加。(オプションダイアログの[一般]カテゴリ→[単一インスタンスモード]の「1つのみインスタンスを起動し、既起動インスタンスの終了を待つ」と同じ動作をする)[GitHub #622](https://github.com/WinMerge/winmerge/issues/622)
* /self-compare オプションを追加。(指定した1つのファイルとそのファイルのコピーを比較する)

### 2021/01/30 2.16.10-jp-1

#### 全般

* 不具合修正: [最近使用したファイルやフォルダー]メニューになにも表示されなくなった。
* 3ファイル/フォルダ比較時でも任意のペインの入れ替えができるようにした。[GitHub PR #533](https://github.com/winmerge/winmerge/pull/533) (PRいただきました。ありがとうございます。)

#### ファイル比較

* 不具合修正: ステータスバーの右側をダブルクリックするとクラッシュする。[GitHub #531](https://github.com/WinMerge/winmerge/issues/531)
* Java と JavaScript のシンタックスハイライトのルールを分離[osdn.net #41083](https://osdn.net/projects/winmerge-jp/ticket/41083)(JavaScriptのキーワード一覧をいただきました。ありがとうございます。)
* [表示]->[Diffコンテキスト]->[反転]メニューを追加。差異行を逆に表示したくないときや一致する行のみを選択してコピーするのに使用します。
* AutoItのシンタックスハイライトを追加
* [ファイル]→[新規]→[テーブル]メニューを追加
* 置換フィルター機能を追加[GitHub PR #544](https://github.com/winmerge/winmerge/pull/544) (PRいただきました。ありがとうございます。)
* ツールバーに次のファイル、前のファイル、最後のファイル、最初のファイルアイコンを追加[GitHub PR #561](https://github.com/winmerge/winmerge/pull/561) (PRいただきました。ありがとうございます。)

#### フォルダ比較

* 不具合修正: フォルダを選択してコピーするとフォルダ内のファイルの状態が更新されない。[GitHub #537](https://github.com/WinMerge/winmerge/issues/537)

#### 画像比較

* 不具合修正: 32bit未満の画像をクリップボードから画像の貼り付けると崩れた画像が張り付けられてしまっていた。
* [画像]メニュー→[画像から抽出したテキストを比較]メニューを追加した。

#### コマンドライン

* 不具合修正: コマンドラインオプション /x がバイナリ比較時や画像比較時にきかない。
* フォルダ比較方法を指定するコマンドラインオプション /m を追加。[GitHub #530](https://github.com/winmerge/winmerge/issues/530)


    次のキーワードが指定できます。

    `Full`(フルコンテンツ), `Quick`(クイックコンテンツ), `Binary`(バイナリコンテンツ), `Date`(更新日時), `SizeDate`(更新日時とサイズ), `Size`(サイズ)
    
    例: フォルダ比較方法=更新日時とサイズ で比較
    ~~~
    WinMergeU /m SizeDate folder1 folder2
    ~~~

### 2020/12/29 2.16.8-jp-11

#### 全般

* 不具合修正: 言語を変更してもタブバーのコンテキストメニューに反映されない。[GitHub PR #528](https://github.com/winmerge/winmerge/pull/528) (PRいただきました。ありがとうございます。)
* メニューなどの翻訳の改善[GitHub-jp PR #8](https://github.com/sdottaka/winmerge-v2-jp/pull/8) (PRいただきました。ありがとうございます。)

#### ファイル比較

* 不具合修正: 直前にControlキーやShiftキーを押して戻しただけにもかかわらず、ドラッグなどの操作が、ControlキーやShiftキーを押しながらの操作とみなされてしまう。[GitHub PR #505](https://github.com/WinMerge/winmerge/pull/505) (PRいただきました。ありがとうございます。)
* 不具合修正: HTMLレポートの作成に失敗したときも「レポートの生成に成功しました」と表示されてしまう。
* 移動ブロック検出を有効にしたとき、ロケーションペインで描画される移動行の接続線がデフォルトで表示されないのを改善。[GitHub #498](https://github.com/WinMerge/winmerge/issues/498)
* 新規作成で画像やバイナリを選べるようにした。

#### パッチの生成ダイアログ

* 不具合修正: 「改行文字の違いを無視する」チェックボックスにチェックを入れてパッチを生成するとクラッシュすることがある。[GitHub #521](https://github.com/WinMerge/winmerge/issues/521)

#### 画像比較

* クリップボードから画像の貼り付け、クリップボードへの画像のコピーができるようにした。
* [画像]メニュー→[ドラッグモード]→[矩形選択]メニューを追加した。

#### コマンドライン

* /s- コマンドラインオプションの追加: このオプションを指定すると、「複数のインスタンスを起動しない」の設定を無視して、別プロセスのWinMergeを起動します。

### 2020/11/29 2.16.8-jp-8

#### ファイル比較

* 不具合修正: コメントの違いを無視するオプションを有効したとき、コメント以外も無視してしまうことがある。(シンタックスハイライトベースのコメントフィルタ実装に変更)[osdn.net #40488](https://osdn.net/projects/winmerge-jp/ticket/40488)
* 不具合修正: ペイン幅変更後、別タブに切り替えて再度ペイン幅を変更したタブに戻るとペイン幅が元に戻ってしまう。[GitHub #403](https://github.com/WinMerge/winmerge/issues/403)
* 不具合修正: Diff ペインがアクティブな場合、「移動...」メニューやロケーションペインの左クリックによる移動が正しく動作しない。[GitHub PR #476](https://github.com/WinMerge/winmerge/pull/476) (PRいただきました。ありがとうございます。)
* HTMLファイル内の`<style>`タグ内のCSSや`<script>`タグ内のJavascriptがシンタックスハイライトされるようにした。
* [移動行に移動]等のメニューを追加。他方のペインの移動行へ移動できるようになりました。(シュートカットキー Ctrl+Shift+G)[GitHub PR #484](https://github.com/WinMerge/winmerge/pull/484) (PRいただきました。ありがとうございます。)
* 非常に長い行の行内差異表示が非常に遅かったのを若干改善。
* [自動的に最初の行内差異へスクロールする]オプションを追加。

#### バイナリファイル比較

* 不具合修正: フォーカスがあたっていないとマウスホイールでスクロールしない。[GitHub #456](https://github.com/WinMerge/winmerge/issues/456)

#### フォルダ比較

* 不具合修正: 大きなサイズのファイルを比較している時に[一時停止]や[停止]ボタンを押してもすぐに停止しない。[GitHub #342](https://github.com/WinMerge/winmerge/issues/342)
* 比較結果列でソートしたとき、左のみまたは右のみのファイルとフォルダが連続で表示されるようにした。[GitHub #483](https://github.com/WinMerge/winmerge/issues/483)

### 2020/10/29 2.16.8-jp-5

#### 全般

* 不具合修正: メニューのアイコンが別のアイコンで表示されてしまう問題を修正
* 起動速度の高速化

#### ファイル比較

* 不具合修正: 3ファイル比較で行フィルタ、コメントフィルタ、[空行を無視する]オプションが機能しない問題を修正[GitHub #395](https://github.com/WinMerge/winmerge/issues/395) [GitHub #450](https://github.com/WinMerge/winmerge/issues/450)
* ファイルの最終行が削除ブロックの場合、そのブロックに同期ポイントは設定できないため、エラーを表示するようにした[GitHub PR #457](https://github.com/WinMerge/winmerge/pull/457) (PRいただきました。ありがとうございます。)
* WinMergeで開いているファイルを外部エディタでUTF-8からBOM付きUTF-8または逆に変更した後、WinMergeでリロードしてもステータスバーのエンコーディング名が変わらない問題を修正[GitHub #466](https://github.com/WinMerge/winmerge/issues/466)
* ステータスバーダブルクリックで関連するダイアログ・メニューを表示するようにした
* Alt+Down/Upキーやツールバーアイコン等で次/前の差異に移動時、行内差異が横にスクロールせずに見えるようするため、最初の行内差異にカーソルを移動するようにした[GitHub #452](https://github.com/WinMerge/winmerge/issues/452)

#### フォルダ比較

* コピーの確認ダイアログで「再びこの質問をしない」チェックボックスを追加[GitHub PR #445](https://github.com/WinMerge/winmerge/pull/445) (PRいただきました。ありがとうございます。)

### 2020/9/29 2.16.8-jp-2

#### 全般

* メモリリークの修正[GitHub PR #432](https://github.com/WinMerge/winmerge/pull/432) (PRいただきました。ありがとうございます。)
* ツールバーのアイコンとファイル比較ウインドウのブックマークアイコン等をDPIに応じて拡大表示するようにした

#### ファイル比較

* 1行目を選択した状態で同期ポイントを追加した場合、ファイル比較ウィンドウで比較結果が1行ずれて表示される問題を修正[GitHub PR #435](https://github.com/WinMerge/winmerge/pull/435) (PRいただきました。ありがとうございます。)
* [表示]メニュー→[空白を表示]を選択すると行末の改行文字まで表示するようになっていたが、タブまたは空白文字のみの表示とし、改行の表示/非表示は新たに追加した[改行を表示]メニューで行うようにした

#### フォルダ比較

* コマンドライン引数でフォルダ比較ウインドウに表示する列を指定できるようにした。
                            ただし、わかりやすくはなく、バージョン列を表示するには、以下のように指定します。

    ~~~
    -cfg "DirView/ColumnOrders=0 1 2 3 4 -1 -1 5 -1 -1 -1 -1 -1 6 7 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1"
    ~~~
    どのような値を設定したらよいかを知るには、フォルダ比較ウインドウの[ツール]→[列の設定]で表示したい列を指定した後、レジストリキー `HKEY_CURRENT_USER\SOFTWARE\Thingamahoochie\WinMerge\DirView\ColumnOrders` の値を参照して下さい。


#### パッチの生成ダイアログ

* 複数のファイルを選択してパッチの生成ダイアログを開いたときに、ダイアログのFile1またはFile2のコンボボックスへの入力が適用されない問題を修正[GitHub PR #421](https://github.com/WinMerge/winmerge/pull/421) (PRいただきました。ありがとうございます。)

### 2020/8/29 2.16.8-jp-1

#### ファイル比較

* 不具合修正: Diffアルゴリズムがdefault以外の場合、[大文字と小文字の違いを区別しない]が機能しない
* 不具合修正: Diffアルゴリズムがdefault以外の場合、WindowsXP上で差異が表示されない[GitHub #420](https://github.com/WinMerge/winmerge/issues/420)
* 不具合修正: 選択範囲領域に白い縦線が表示されることがある
* 不具合修正: 水平マウスホイールが適切に動作していなかった
* F4キーで移動する範囲をファイル全体に拡張
* 再スキャン時、マージ時、Undo時等で、水平スクロール位置があまり変わらないようにした
* [コードページの違いを無視する]が無効の場合、BOM付きUTF-8とBOMなしUTF-8を区別するようにした

#### テーブル比較

* 不具合修正: [右端で行を折り返す]が有効の時、一行目のダブルクォーテーションで括られた範囲でEnterキーを押下すると異常終了する
* 不具合修正: [右端で行を折り返す]が有効の時、行内差異が適切に表示されないことがあった

#### フォルダ比較

* 不具合修正: 削除しようとしたファイルが存在しなかった場合のエラーメッセージが適切でなかった
* 不具合修正: 3-wayフォルダー比較で、[中央/右のみ存在する項目を表示]メニューが正しく動作しない[osdn.net #40672](https://osdn.net/projects/winmerge-jp/ticket/40672)
* Ctrlキーを押したままフォルダをWinMergeにドラッグしたときの動作を変更。いままでは、常に[サブフォルダを含める]にチェックが入った時と同じ動作となっていましたが、現在の[サブフォルダを含める]チェックボックスのチェック状態と反対の動作をするようにしました。

#### 画像比較

* 不具合修正: スクロールするとき、ウインドウ枠にゴミが残る[GitHub winimerge PR #8](https://github.com/WinMerge/winimerge/pull/8) (PRいただきました。ありがとうございます。)
* 不具合修正: リサイズするときのちらつきを低減[GitHub winimerge PR #9](https://github.com/WinMerge/winimerge/pull/9) (PRいただきました。ありがとうございます。)

#### オプションダイアログ

* [一般/画像]カテゴリの[画像ファイルとして扱うファイルパターン]等でドロップダウンメニューでパターンの有効・無効を選択できるようにした[GitHub PR #391](https://github.com/WinMerge/winmerge/pull/391)[GitHub PR #397](https://github.com/WinMerge/winmerge/pull/397) (PRいただきました。ありがとうございます。)
* ウインドウサイズを変更できるようにした
* 以下のデフォルト値を変更

    * 比較/一般: コードページの違いを無視する: 無効
    * 比較/フォルダー: 片方にしか存在しないサブフォルダー内も含める: 有効

#### [WinMergeについて]ダイアログ

* GitHub issue #316の再修正[GitHub PR #392](https://github.com/WinMerge/winmerge/pull/392) (PRいただきました。ありがとうございます。)
* 古くて現状を反映していない開発者リストをアスキーアートに変更[GitHub PR #394](https://github.com/WinMerge/winmerge/pull/394) (PRいただきました。ありがとうございます。)

#### インストーラ

* 一般のGUIユーザではないユーザでサイレントインストールしたとき、「Internal error: Failed to expand shell folder constant "userdesktop"」エラーが発生する問題の修正の試み[GitHubPR #354](https://github.com/WinMerge/winmerge/issues/354)
* ユーザ毎インストール用インストーラ(管理者権限不要)を追加(WinMerge-2.16.8-jp-1-PerUser-Setup.exe)

### 2020/7/29 2.16.6-jp-11

#### ファイル比較

* 不具合修正: 過去に検索した文字列(例:abcd)を含む文字列(例:ab)を選択して検索ダイアログを表示すると過去に検索した文字列のほうが検索文字列になってしまう[GitHub #368](https://github.com/WinMerge/winmerge/issues/368)
* 不具合修正: 選択範囲の置換が機能していない

#### フォルダ比較

* 不具合修正: フォルダが選択されている状態で、フォルダに対して実行できないメニュー項目がある場合に、そのメニュー項目を無効にするように変更[GitHub PR #366](https://github.com/WinMerge/winmerge/pull/366) (PRいただきました。ありがとうございます。)
* 不具合修正: パスワード付きの7zファイルを比較するとフリーズする[GitHub #367](https://github.com/WinMerge/winmerge/issues/367)
* 不具合修正: 3つフォルダの比較ウインドウで、1項目選択し、[形式を指定して比較]のXML等を選択すると、2つのファイルしかファイルが比較されない[GitHub PR #381](https://github.com/WinMerge/winmerge/pull/381) (PRいただきました。ありがとうございます。)

#### テーブル比較

* CSVファイルやTSVファイルの内容を表計算ソフト風に表示する機能を追加。  
  * ※まだデフォルトで拡張子.csvファイルや.tsvファイルをこの機能で表示するようにしていません。様子を見てデフォルトにするつもりです。  
    この機能を使用したい場合、.csvファイル等のファイル比較ウインドウ表示後、[ファイル]→[形式を指定して再比較]→[テーブル]メニューを選択してください。  
    または、[編集]→[設定]メニュー→[比較/テーブル]カテゴリで[CSVファイルとして扱うファイルパターン]に `*.csv` を入力すると、今後すぐに.csvファイルがこの機能で比較・表示されます。

#### プラグイン

* IgnoreCommentsC.dllプラグイン: C# と TypeScriptの拡張子に対応[GitHub PR #382](https://github.com/WinMerge/winmerge/pull/382) (PRいただきました。ありがとうございます。)

#### ドラッグ＆ドロップ

* 不具合修正: 秀丸ファイラーClassic等からファイルをドラッグ＆ドロップすると元のファイルが比較されるのではなく、Tempフォルダにコピーされたファイルフォルダが比較されてしまう。(以前WinSCPからのドラッグ＆ドロップに対応した修正に問題があったため元に戻しています)

### 2020/6/29 2.16.6-jp-10

#### ファイル比較

* 不具合修正: CompareMSExcelFilesプラグインの画像比較時にエラーが発生[osdn.net #40472](https://osdn.net/projects/winmerge-jp/ticket/40472)
* 不具合修正: ファイル選択ダイアログを閉じた後、MDI親Windowの内側が再描画されない[osdn.net #40487](https://osdn.net/projects/winmerge-jp/ticket/40487)
* 不具合修正: 複数行を選択しTABキーを押したとき、選択範囲に含まれる削除行はインデントしないようにした[GitHub #356](https://github.com/WinMerge/winmerge/issues/356)
* 不具合修正: 移動ブロック(削除)が間違った色で描画される[GitHub #358](https://github.com/WinMerge/winmerge/issues/358)
* 不具合修正: 同じファイルパスを指定して比較したとき警告されなくなっている[GitHub #362](https://github.com/WinMerge/winmerge/issues/362)
* 不具合修正: 単語単位の移動・選択で"."が単語の境界として扱われない
* HTMLファイルの文字コード判定でHTML5のcharsetを参照するようにした[osdn.net #40476](https://osdn.net/projects/winmerge-jp/ticket/40476)
* 同じファイルパスを指定して比較したとき警告されなくなっている[GitHub #362](https://github.com/WinMerge/winmerge/issues/362)

#### プラグイン

* Visioの比較プラグインを追加[osdn.net #40473](https://osdn.net/projects/winmerge-jp/ticket/40473) (PRいただきました。ありがとうございます。)

#### フィルタ

* Visual C# loose フィルタの無視するフォルダに bin と obj フォルダを追加[GitHub PR #356](https://github.com/WinMerge/winmerge/issues/356)

### 2020/5/29 2.16.6-jp-6

#### ファイル比較

* 不具合修正: Diffペインで通常編集できないにもかかわらず、D&D、BSキー、Delキーで編集できてしまう[osdn.net #40390](https://osdn.net/projects/winmerge-jp/ticket/40390)
* 不具合修正: 現在の差異ではない箇所に、検索マーカやユーザ定義マーカがあると、 Diffペインに検索マーカやユーザ定義マーカが表示される[osdn.net #40407](https://osdn.net/projects/winmerge-jp/ticket/40407)
* 不具合修正: ファイル比較後、別アプリケーションで一方のファイルを削除またファイル名を変更し、他方はファイルの内容を変更した場合、WinMergeに戻るとクラッシュする[GitHub #351](https://github.com/WinMerge/winmerge/issues/351)
* 不具合修正: 制御文字が含まれている行の折り返し位置がおかしかった
* 不具合修正: NUL文字が含まれている行に検索対象文字列があっても検索されない
* 不具合修正: 横スクロールすると結合文字が含まれている行にゴミが表示されることがある
* 不具合修正: 矩形選択の範囲外にマウスポインタを移動したときにマウスポインタのアイコンが変わらないことがある
* 非常に大きなファイルを比較するとメモリ使用量を著しく消費したり、比較時間が長くなるため、64MBを超えるファイルを比較する場合、ファイルの内容は表示せず比較結果だけ表示するか問うようにした

#### フォルダ比較

* 不具合修正: フォルダウインドウからファイル名に大文字小文字のみの違いがあるファイルを開き、編集した結果、ファイル比較結果が変わったにもかかわらず、フォルダウインドウに反映されない[GitHub #332](https://github.com/WinMerge/winmerge/issues/332)
* 不具合修正: 非常に長いファイル名のファイルの削除操作を行っても削除されないことがある[GitHub #339](https://github.com/WinMerge/winmerge/issues/339)
* 不具合修正: 表示されるファイル数が非常に多い場合にファイル選択操作が著しく遅くなる[GitHub #348](https://github.com/WinMerge/winmerge/issues/348)
* フォルダを選択してパッチが生成できるようになった(PullRequestありがとうございます)[GitHub PR #331](https://github.com/WinMerge/winmerge/pull/331)

#### ファイルまたはフォルダ選択ウインドウ

* ファイルまたはフォルダのパス入力テキストボックスに存在しないパスが指定されている状態で参照ボタンを押すと、まったく関係ないフォルダを初期表示してしまうのを、親フォルダが存在していれば、親フォルダを初期表示にするようにした[GitHub #346](https://github.com/WinMerge/winmerge/issues/346)

#### その他

* 64bit版のみASLR(アドレス空間配置のランダム化)、CFG(制御フローガード)を有効にした[GitHub #315](https://github.com/WinMerge/winmerge/issues/315)
* コマンド引数で展開プラグインを指定できるようにした。(例. WinMergeU.exe file1.xlsx file2.xlsx /unpacker CompareMSExcelFiles.sct)

### 2020/4/29 2.16.6-jp-4

#### ファイル比較

* 不具合修正: バージョン2.16.4より文字幅が広くなっている
* 不具合修正: editor addin.sct プラグイン: SortAscending, SortDescending での改行の扱いを修正[osdn.net #40266](https://osdn.net/projects/winmerge-jp/ticket/40266)
* 不具合修正: [改行文字の違いを無視する]が有効になっているにもかかわらず、差分ブロックで改行文字の違いが強調表示される
* 不具合修正: Diffペインで[ウインドウ]→[分割]メニューをクリックするとクラッシュする
* 不具合修正: Diffペインで一部の編集操作ができてしまう[GitHub #307](https://github.com/WinMerge/winmerge/issues/307)
* 不具合修正: 外部アプリケーションによるファイルの文字コードの変更が反映されない[GitHub #320](https://github.com/WinMerge/winmerge/issues/320)
* ロケーションペインで、移動ブロックどうしを結ぶ斜め線の表示を改善 [osdn.net #39913](https://osdn.net/projects/winmerge-jp/ticket/39913)
* 単語区切りとみなす記号の初期値に「=」もほしい [osdn.net #40224](https://osdn.net/projects/winmerge-jp/ticket/40224)

#### フォルダ比較

* 不具合修正: ファイル名変更中にファイルを削除するとWinMergeが異常終了し、親フォルダが消えてしまう(親フォルダが別名に変更される)。[Twitter](https://twitter.com/yamato_kakeru_/status/1253924284503691264)
* 不具合修正: テキストファイルがバイナリファイルとみなされてしまう [osdn.net #40296](https://osdn.net/projects/winmerge-jp/ticket/40296)

#### 画像比較

* 設定ウインドウ→[比較/画像]カテゴリに[フォルダ比較で画像比較を有効にする]チェックボックスを追加。このチェックボックスがチェックされている場合、フォルダ比較時に画像ファイルの内容が(メタデータが違う等で)異なっていても画像として一致していれば、同一とみなすようになります。また、色距離閾値の設定も適用されます。
* [画像]→[ドラッグモード]メニューに[垂直ワイプ]、[水平ワイプ]メニューを追加

#### バイナリ比較

* 不具合修正: オプションFrhed設定のボタン操作で異常終了[osdn.net #40308](https://osdn.net/projects/winmerge-jp/ticket/40308)

#### HTMLレポート

* 不具合修正: HTML レポートにマージンの色とテキストの色が適用されない
* 不具合修正: 行頭の空白文字が表示されない

#### GUI全般

* メニューアイコンをDPIに合わせて拡大表示するようにした
* Solarized Light/Dark カラースキームを追加

### 2020/3/1 2.16.6-jp-1

#### ファイル比較

* ロケーションペインの背景色を現在のエディタウインドウの背景色に近い色になるようにした

#### 画像比較

* 不具合修正: 画像ファイルの比較で左ドラッグ時のスクロール動作がおかしい[osdn.net #40205](https://osdn.net/projects/winmerge-jp/ticket/40205)
* 不具合修正: 16bitグレースケール画像が正しく表示できない[GitHub #6](https://github.com/WinMerge/winimerge/issues/6)
* 不具合修正: ベクタ画像スケーリングメニューで拡大・縮小率を選択しても、選択された拡大・縮小率のメニュー項目が選択状態にならない問題を修正

#### インストーラ

* ウィルス誤検出が解決しないため、64bit版のインストーラの作成用InnoSetupのバージョンを6から5に一時的に戻しています。このため、ユーザ毎のインストールができなくなっています。

#### マニュアル

* 「オプションとコンフィグレーション」の修正[GitHub PR #262](https://github.com/WinMerge/winmerge/pull/262)

### 2020/1/29 2.16.4-jp-13

#### ファイル比較

* 不具合修正: Diffペインで Enter キーを押すとエラーダイアログが表示される[osdn.net #39924](https://osdn.net/projects/winmerge-jp/ticket/39924)
* 不具合修正: ファイルサイズが 4294967296 またはその倍数のファイルを比較するとWinMergeが終了してしまう[GitHub PR #257](https://github.com/WinMerge/winmerge/pull/257)
* 不具合修正: Redo操作で予期されない文字が入力されてしまうことがある
* 不具合修正: 置換ダイアログで[すべて置換]ボタンを押しても、カーソルの位置によっては、すべて置換されないことがある
* タッチパッド 横スクロールに対応[GitHub #254](https://github.com/WinMerge/winmerge/issue/254)

#### パッチ作成

* 不具合修正: フォルダ比較ウインドウから片方が存在しないファイルを選択してパッチを作成するとWinMergeがクラッシュする[](https://github.com/WinMerge/winmerge/issues/252)

#### プロジェクトファイル

* 不具合修正: ドライブ名なしの\から始まるパスがカレントディレクトリからの相対パスとして扱われてしまう

#### 画像比較

* SVG, EMF, WMF, PDF ファイルを画像比較できるようにした。(※64bit版WinMergeかつWindows10のみ対応)。デフォルトではテキストファイルとして開くため、SVGファイルなどを画像比較したい場合は、[ファイル]→[形式を指定して再比較]→[画像]を選択してください。また、[オプション]ウインドウ→[比較/画像]カテゴリ→[画像ファイルとして扱うパターン]に ;`*.svg` を追加すると、SVGファイルがすぐに画像比較されるようになります。

#### インストーラ

* 不具合修正: スペルミス修正(TortoiseGIT→TortoiseGit)[GitHub PR #246](https://github.com/WinMerge/winmerge/pull/246)

#### その他

* Ctrl+Tabキーでウインドウを切り替える際に切り替え先がリストから選べるようになった[GitHub PR #247](https://github.com/WinMerge/winmerge/pull/247)
