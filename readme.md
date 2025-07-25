# WSP

## 概要

WSP(White Sauce Pasta) はWindows、Mac、Linuxなど様々なOS上で動作する[HSP](https://hsp.tv/)風のインタプリタ言語です。HSPのような手軽さと堅牢な型システムを持ち、より安全なプログラミングを可能にします。

## 特徴

- **型安全**: HSPのような動的型付けに加え、静的型付けもサポート。型が異なる場合は自動的に元の型に変換されます。
- **変数のスコープ**: {}ブロック内で宣言された変数はそのブロック内でのみ有効です。
- **クロスプラットフォーム**: Windows、Mac、Linuxで動作します。
- **簡単な構文**: HSPに似た命令とC++のような構文を組み合わせた、直感的なコード記述が可能です。
- **GUI**:HSPのような簡単なGUI操作が可能で、ゲームやアプリケーションの開発が容易です。
- **安心・安全の日本产**:纯国产の语言で、ドキュメソト(本MD)も日语で提供されます。
- **オープンソース**: インタプリタのソースコードは[GitHub](https://github.com/UnABC/WSP)で公開されています。

## インストール方法

### Windows

1. [リリースページ](https://www.google.com/teapot)から最新のリリースをダウンロードします。
2. ダウンロードしたZIPファイルを解凍し、任意のディレクトリに配置します。
3. `wsp.exe`を実行することで、WSPインタプリタが起動します。
4. コマンドラインから`wsp test.wsp`のようにWSPファイルを指定して実行できます。

### Windows/Mac/Linux

1. src/以下のソースコードをダウンロードします。
2. 以下のコマンドを実行してビルドします。

   ```bash
   g++ -o wsp src/*.cpp -I include -lmingw32 -lSDL3 -lopengl32 -lfreetype -lglew32
   ```

3. ビルドが成功したら、`wsp`コマンドで実行できます。
4. `wsp test.wsp`のように、WSPファイルを指定して実行します。

### 依存関係

- SDL3
- OpenGL
- FreeType
- GLEW
- C++23以上のコンパイラ

## 使い方

WSPの基本的な使い方は以下の通りです。

### 変数の宣言と代入

```wsp
int a = 10
double b = 3.14
string c = "Hello, WSP!"
// dは動的型付けの変数
d = 2
d = "Dynamic type"
int e = d // dは自動的にint(ここでは0)に変換される
// 明示的な型変換
f = int(b) // doubleからintへの変換
g = string(a) // intからstringへの変換
// 変数のスコープ
{
    string localVar = "Titech" // この変数はこのブロック内でのみ有効
    print("Local variable: " + localVar)
}
print("Outside local variable: " + localVar) //localVarはこのスコープ外なので0(int型)で初期化される
```

### 条件分岐

```wsp
// if文
if a > 5 {
    print("a is greater than 5")
} else if a == 5 {
    print("a is equal to 5")
} else {
    print("a is less than 5")
}
if (b == 0) print("b is zero")
// if文の条件部分での変数宣言
if ((int f = a*b) == 10) {
    print("f is 10")
} else {
    print("f is" + f)
}
// 三項演算子
result = (a > 5) ? "Greater" : "Not greater"
```

### ループ

```wsp
while a < 20 {
    print(a)
    a++
}

repeat 5 {
    print("This will repeat 5 times")
    // 自動的に変数cntが宣言され、0から4まで増加する
    print("cnt:"+cnt)
}
```

### 関数の定義と呼び出し

```wsp
// 通常の関数の定義
int add(int x, int y) {
    return x + y
}
// 参照渡しの関数の定義
void changeLiteral(var str) {
    str = "Changed"
}
// デフォルト引数を持つ関数の定義
double multiply(double x, double y,double z = 1) {
    return x * y * times
}
// void型の関数の定義
void printMessage(string msg) {
    print(msg)
}
result = add(5, 10)
printMessage("Result is " + result)
string literal = "Original"
changeLiteral(literal)
print("Literal after change: " + literal)
print("Multiply result: " + multiply(2.0, 3.0, 4.0))
print("Multiply result with default: " + multiply(2.0, 3.0))
print(add_string("Hello, ", "World!"))

// 関数は後で定義しても呼び出せる
string add_string(string a, string b) {
    return a + b
}
```

### 配列

```wsp
// 配列の宣言と初期化
int arr = [1, 2, 3, 4, 5]
double arr2[10][10]
// 配列の要素へのアクセス
print("First element: " + arr[0])
// 配列の要素の変更
arr[0] = 10
// 配列の長さを取得
print("Array length: " + size(arr))
// 二次元配列の要素へのアクセス
arr2[0][0] = 1.0
arr2[1][1] = 2.0
print("2D Array element: " + arr2[0][0])
// 二次元配列の長さを取得
print("2D Array size: " + size(arr2[0]))
```

### 文字列操作

```wsp
// 文字列の宣言と初期化
string str = "Hello, WSP!"
// 文字列の長さを取得
print("String length: " + strlen(str))
// 文字列の連結
str += " How are you?"
// 文字列の部分文字列を取得
print("Substring: " + strmid(str, 0, 5)) // "Hello"
print("Substring: " + strmid(str, 5)) // "WSP!"
// 文字列の検索
print("Index of 'WSP': " + findstr(str, "WSP")) // 7
// 文字列の繰り返し
print("Repeated string: " + (str*2)) // "Hello, WSP!Hello, WSP!"
```

### その他便利な組み込み関数等

```wsp
// 数値の絶対値
print("Absolute value: " + abs(-10)) // 10
// 数値の最大値と最小値
print("Max value: " + max(10, 20)) // 20
print("Min value: " + min(10, 20)) // 10
// 数値を範囲内に制限
print("Clamp value: " + limit(15, 10, 20)) // 15
// 乱数の生成
randomize(0) // 乱数の初期化(引数はシード値:省略時は現在の時間)
print("Random number: " + rnd(100)) // 0から99までの乱数
// 時間の取得
print("Current time: " + gettime()) // 現在の時間をms単位で取得
print("Current time: " + unixtime()) // 現在の時間をUnixタイムスタンプで取得
// 各種定数
print("PI: " + M_PI) // 円周率
print("E: " + M_E) // 自然対数の底
print("Current directory: " + dir_cur) // 現在の作業ディレクトリ
```

## 描画について

WSPでは、2Dグラフィックスの描画が可能です。以下の関数を使用して、図形や画像を描画できます。

```wsp
// 描画色の設定
color(255, 0, 0) // 赤色
// 画面のクリア
cls(255, 255, 255) // 白色で画面をクリア
// 座標の設定
pos(100, 100) // 座標を(100, 100)に設定
print("Hello, WSP!") // 画面に文字列を描画
// 三角形の描画
triangle(100, 100, 150, 50, 200, 100)
// 四角形の描画
quad(100, 100, 200, 100, 200, 200, 100, 200)
// 角丸四角形の描画
roundrect(100, 100, 200, 100, 20) // 角の半径は20
// 円の描画
circle(300, 300, 50) // 中心座標(300, 300)、半径50の円を描画
// 楕円の描画
ellipse(400, 400, 100, 50) // 中心座標(400, 400)、x方向の半径100、y方向の半径50の楕円を描画
// 線分の描画
line(100, 100, 200, 200) // (100, 100)から(200, 200)までの線分を描画
```

### color関数について

`color`関数は、描画色を設定するための関数です。引数にはRGBカラー値とオプションでアルファ値を指定できます。また、`n`引数を指定することで、より細かい色の設定が可能です。
ここで、`n`引数は各図形、画像、テキストの頂点の色を指定するために使用されます。`n`が0の場合は、デフォルトの描画色が使用されます。1以上の値を指定すると、各頂点ごとに異なる色を設定できます。

```wsp
color(255, 0, 0) // 赤色
print("This text is red") // 赤色のテキストを描画
color(0, 255, 0, 128) // 緑色、半透明
quad(100, 100, 200, 100, 200, 200, 100, 200) // 緑色の四角形を描画
color(0, 0, 255, 255, 1) // 青色、アルファ値255、n=1
color(255, 255, 0, 255, 2) // 黄色、アルファ値255、n=2
color(255, 0, 255, 255, 3) // マゼンタ、アルファ値255、n=3
// n引数を使用して、各頂点の色を指定
triangle(100, 100, 150, 50, 200, 100) // カラフルな三角形を描画
```

### gmodeについて

`gmode`は描画モードを指定するための変数です。以下の値を設定できます。

- `0`: 通常の描画モード
- `1`: 画像のcolor関数適用有効化
- `2`: 加算合成モード
- `4`: 減算合成モード
- `6`: 乗算合成モード
ここで、mode`1`は画像の描画時にcolor関数で設定した色を適用するモードです。これにより、画像に色を乗算することができます。また、このモードは`1+2`などとすることで他の描画モードと組み合わせて使用することも可能です。

```wsp
LoadImage("image.png", 1) // 画像を読み込み、ID=1で保存
cls(0, 0, 0) // 画面を黒色でクリア
color(255, 0, 0) // 描画色を赤色に設定
circle(100, 100, 50) // 赤色の円を描画
gmode(3) // 加算合成モードに設定
color(255,0,0,255,0) // 左上を赤色で指定
color(0,255,0,255,1) // 右上を緑色で指定
color(0,0,255,255,2) // 左下を青色で指定
color(255,255,0,255,3) // 右下を黄色で指定
pos(200, 200) // 描画位置を(200, 200)に設定
DrawImage(1) // 画像を描画
```

## 各種組み込み関数一覧

iは整数型、dは浮動小数点型、sは文字列型を表します。引数の型は省略可能で、引数がない場合は空欄にしています。また、引数の(=...)は省略時のデフォルト値を示します。

| 関数名       | 型    | 引数                                                                                                                                     | 説明                                                                                                                                                                                                                                                                                                                                             |
| ------------ | ----- | ---------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| abs          | i/d   | i/d x                                                                                                                                    | xの絶対値を返す                                                                                                                                                                                                                                                                                                                                  |
| sqrt         | i/d   | i/d x                                                                                                                                    | xの平方根を返す                                                                                                                                                                                                                                                                                                                                  |
| sin          | i/d   | i/d x                                                                                                                                    | xの正弦を返す                                                                                                                                                                                                                                                                                                                                    |
| cos          | i/d   | i/d x                                                                                                                                    | xの余弦を返す                                                                                                                                                                                                                                                                                                                                    |
| tan          | i/d   | i/d x                                                                                                                                    | xの正接を返す                                                                                                                                                                                                                                                                                                                                    |
| asin         | i/d   | i/d x                                                                                                                                    | xの逆正弦を返す                                                                                                                                                                                                                                                                                                                                  |
| acos         | i/d   | i/d x                                                                                                                                    | xの逆余弦を返す                                                                                                                                                                                                                                                                                                                                  |
| atan         | i/d   | i/d x                                                                                                                                    | xの逆正接を返す                                                                                                                                                                                                                                                                                                                                  |
| atan2        | i/d   | i/d y, i/d x                                                                                                                             | y/xの逆正接を返す                                                                                                                                                                                                                                                                                                                                |
| log          | i/d   | i/d x                                                                                                                                    | xの自然対数を返す                                                                                                                                                                                                                                                                                                                                |
| log10        | i/d   | i/d x                                                                                                                                    | xの常用対数を返す                                                                                                                                                                                                                                                                                                                                |
| ceil         | i/d   | i/d x                                                                                                                                    | xの切り上げ整数を返す                                                                                                                                                                                                                                                                                                                            |
| floor        | i/d   | i/d x                                                                                                                                    | xの切り下げ整数を返す                                                                                                                                                                                                                                                                                                                            |
| round        | i/d   | i/d x                                                                                                                                    | xを四捨五入した整数を返す                                                                                                                                                                                                                                                                                                                        |
| trunc        | i/d   | i/d x                                                                                                                                    | xの整数部分を返す                                                                                                                                                                                                                                                                                                                                |
| sinh         | i/d   | i/d x                                                                                                                                    | xの双曲正弦を返す                                                                                                                                                                                                                                                                                                                                |
| cosh         | i/d   | i/d x                                                                                                                                    | xの双曲余弦を返す                                                                                                                                                                                                                                                                                                                                |
| tanh         | i/d   | i/d x                                                                                                                                    | xの双曲正接を返す                                                                                                                                                                                                                                                                                                                                |
| asinh        | i/d   | i/d x                                                                                                                                    | xの逆双曲正弦を返す                                                                                                                                                                                                                                                                                                                              |
| acosh        | i/d   | i/d x                                                                                                                                    | xの逆双曲余弦を返す                                                                                                                                                                                                                                                                                                                              |
| atanh        | i/d   | i/d x                                                                                                                                    | xの逆双曲正接を返す                                                                                                                                                                                                                                                                                                                              |
| exp          | i/d   | i/d x                                                                                                                                    | eのx乗を返す                                                                                                                                                                                                                                                                                                                                     |
| pow          | i/d   | i/d x, i/d y                                                                                                                             | xのy乗を返す                                                                                                                                                                                                                                                                                                                                     |
| fmod         | i/d   | i/d x, i/d y                                                                                                                             | xをyで割った余りを返す。実数に対しても有効です。                                                                                                                                                                                                                                                                                                 |
| hypot        | i/d   | i/d x, i/d y                                                                                                                             | xとyのユークリッド距離を返す。√(x^2 + y^2)                                                                                                                                                                                                                                                                                                       |
| max          | i/d/s | i/d/s x, i/d/s y                                                                                                                         | xとyの最大値を返す                                                                                                                                                                                                                                                                                                                               |
| min          | i/d/s | i/d/s x, i/d/s y                                                                                                                         | xとyの最小値を返す                                                                                                                                                                                                                                                                                                                               |
| limit        | i/d/s | i/d/s x, i/d/s min, i/d/s max                                                                                                            | xをminとmaxの範囲に制限                                                                                                                                                                                                                                                                                                                          |
| rnd          | i/d   | i/d max                                                                                                                                  | 0からmax-1までの乱数を返す                                                                                                                                                                                                                                                                                                                       |
| randomize    | void  | i/d seed (=gettime())                                                                                                                    | 乱数のシード値を設定                                                                                                                                                                                                                                                                                                                             |
| gettime      | i/d   |                                                                                                                                          | 現在の時間をミリ秒単位で返す                                                                                                                                                                                                                                                                                                                     |
| unixtime     | i/d   |                                                                                                                                          | 現在のUnixタイムスタンプを返す                                                                                                                                                                                                                                                                                                                   |
| int          | i/d/s | i/d/s x                                                                                                                                  | xを整数に変換して返す                                                                                                                                                                                                                                                                                                                            |
| double       | i/d/s | i/d/s x                                                                                                                                  | xを浮動小数点数に変換して返す                                                                                                                                                                                                                                                                                                                    |
| string       | i/d/s | i/d/s x                                                                                                                                  | xを文字列に変換して返す                                                                                                                                                                                                                                                                                                                          |
| size         | i/d/s | i/d/s x                                                                                                                                  | xの要素数を返す                                                                                                                                                                                                                                                                                                                                  |
| findstr      | i/d/s | i/d/s str, i/d/s substr                                                                                                                  | str内でsubstrの位置を返す                                                                                                                                                                                                                                                                                                                        |
| strmid       | i/d/s | i/d/s str, i/d/s start, i/d/s len (=size(str))                                                                                           | strのstart位置からlen文字を返す                                                                                                                                                                                                                                                                                                                  |
| getkey       | i     | s keyname                                                                                                                                | キーボード入力を取得する。keynameはキー名を指定。例: "A", "B", "C", "ENTER" , "UP"など。keynameは下記表を参照。                                                                                                                                                                                                                                  |
| mousepos     | d     | i/d                                                                                                                                      | マウスカーソルの座標を取得する。引数が0のときはx座標を返し、引数が1のときはy座標を返す。                                                                                                                                                                                                                                                         |
| mousegpos    | d     | i/d                                                                                                                                      | マウスカーソルの座標(ウィンドウ全体)を取得する。引数が0のときはx座標を返し、引数が1のときはy座標を返す。                                                                                                                                                                                                                                         |
| mouseclick   | i     |                                                                                                                                          | マウスのクリック状態を取得する。左、中、右マウスボタン、サイドマウスボタン1,2が押されたとき、それぞれ1,2,4,8,16,32を返す。                                                                                                                                                                                                                       |
| HideMouse    | void  |                                                                                                                                          | マウスカーソルを非表示にする。                                                                                                                                                                                                                                                                                                                   |
| ShowMouse    | void  |                                                                                                                                          | マウスカーソルを表示する。                                                                                                                                                                                                                                                                                                                       |
| console      | void  | i/d/s msg                                                                                                                                | msgをコンソールに出力                                                                                                                                                                                                                                                                                                                            |
| print        | void  | i/d/s msg                                                                                                                                | msgを画面に出力                                                                                                                                                                                                                                                                                                                                  |
| pos          | void  | i/d x, i/d y                                                                                                                             | 画面上の座標を(x, y)に設定                                                                                                                                                                                                                                                                                                                       |
| color        | void  | i/d r, i/d g, i/d b, i/d a (=255),i/d n (= 0)                                                                                            | 描画色を(r, g, b, a)に設定                                                                                                                                                                                                                                                                                                                       |
| hsvcolor     | void  | i/d h, i/d s, i/d v, i/d a (=255),i/d n (= 0)                                                                                            | HSV色空間で描画色を設定する。hは色相、sは彩度、vは明度。aはアルファ値。nは頂点のインデックス。                                                                                                                                                                                                                                                   |
| cls          | void  | i/d r, i/d g, i/d b                                                                                                                      | 画面を(r, g, b)でクリア                                                                                                                                                                                                                                                                                                                          |
| dialog       | void  | i/d/s msg, i/d/s title(="") i/d mode (=0)                                                                                                | ダイアログを表示する。                                                                                                                                                                                                                                                                                                                           |
| wait         | void  | i/d ms (=1)                                                                                                                              | msミリ秒待機する。                                                                                                                                                                                                                                                                                                                               |
| redraw       | void  | i/d mode                                                                                                                                 | 描画の有効/無効を指定する。                                                                                                                                                                                                                                                                                                                      |
| font         | void  | i/d size, s path(=default)                                                                                                               | fontのサイズやスタイルを設定する。pathはフォントファイルのパス。                                                                                                                                                                                                                                                                                 |
| triangle     | void  | i/d x1, i/d y1, i/d x2, i/d y2, i/d x3, i/d y3                                                                                           | 三角形を描画する。座標は画面の左上が(0, 0)となる。                                                                                                                                                                                                                                                                                               |
| quad         | void  | i/d x1, i/d y1, i/d x2, i/d y2, i/d x3, i/d y3, i/d x4, i/d y4                                                                           | 四角形を描画する。                                                                                                                                                                                                                                                                                                                               |
| rect         | void  | i/d x, i/d y, i/d w, i/d h                                                                                                               | 四角形を描画する。左上の座標(x, y)と幅w、高さhを指定。                                                                                                                                                                                                                                                                                           |
| roundrect    | void  | i/d x, i/d y, i/d w, i/d h, i/d r (=0)                                                                                                   | 角丸四角形を描画する。rは角の半径。                                                                                                                                                                                                                                                                                                              |
| circle       | void  | i/d x, i/d y, i/d r                                                                                                                      | 円を描画する。rは円の半径。                                                                                                                                                                                                                                                                                                                      |
| ellipse      | void  | i/d x, i/d y, i/d rx, i/d ry ,i/d angle(= 0.0)                                                                                           | 楕円を描画する。rxはx方向の半径、ryはy方向の半径。angleは回転角度(rad)。                                                                                                                                                                                                                                                                         |
| line         | void  | i/d x1, i/d y1, i/d x2, i/d y2                                                                                                           | 線分を描画する。                                                                                                                                                                                                                                                                                                                                 |
| LoadImage    | void  | s path,i id, i/d centerX (=0), i/d centerY (=0)                                                                                          | 画像を読み込み、描画する。centerX, centerYは画像の中心座標。                                                                                                                                                                                                                                                                                     |
| DrawImage    | void  | i id, i/d x_size(=1.0), i/d y_size(=1.0), i/d angle(=0.0), i/d tex_x(=0.0),i/d tex_y(=0.0),i/d tex_width(=WIDTH),i/d tex_height(=HEIGHT) | 画像を描画する。                                                                                                                                                                                                                                                                                                                                 |
| gmode        | void  | i/d mode                                                                                                                                 | 描画モードを設定する。                                                                                                                                                                                                                                                                                                                           |
| texture      | void  | i/d id, i/d tex_x(=0.0),i/d tex_y(=0.0),i/d tex_width(=WIDTH),i/d tex_height(=HEIGHT)                                                    | 図形を描画するときにテクスチャを適用する。                                                                                                                                                                                                                                                                                                       |
| screen       | void  | i id,s title(="WSP"), i/d width(=640), i/d height(=480), i/d mode(=0)                                                                    | 画面のサイズを設定する。titleはウィンドウのタイトル。widthとheightは画面の幅と高さ。modeは描画モード(0:通常,1:フルスクリーン,2:縁なしウィンドウ,4:最大ウィンドウ,8:最小ウィンドウ,16:非表示ウィンドウ,32:最前面ウィンドウ,64:サイズ変更可能なウィンドウ)                                                                                         |
| ScreenPos    | void  | i/d x, i/d y                                                                                                                             | 画面の位置を(x, y)に設定する。                                                                                                                                                                                                                                                                                                                   |
| gsel         | void  | i id                                                                                                                                     | 描画対象の画面を切り替える。idはscreen関数で設定した画面ID。                                                                                                                                                                                                                                                                                     |
| gcopy        | void  | i src_id, i/d x(=0), i/d y(=0), i/d w(=WIDTH), i/d h(=HEIGHT),i/d dst_x(=0), i/d dst_y(=0),i/dst_w(=WIDTH), i/dst_h(=HEIGHT)             | src_idの画面から現在の画面にコピーする。x, yはコピー開始位置、w, hはコピーサイズ。dst_x, dst_y, dst_w, dst_hはコピー先の位置とサイズ。                                                                                                                                                                                                           |
| ResizeScreen | void  | i/d width, i/d height                                                                                                                    | 画面のサイズを変更する。widthとheightは新しい画面の幅と高さ。                                                                                                                                                                                                                                                                                    |
| title        | void  | s title                                                                                                                                  | ウィンドウのタイトルを設定する。                                                                                                                                                                                                                                                                                                                 |
| HideScreen   | void  | i id(=0)                                                                                                                                 | 指定した画面を非表示にする。idはscreen関数で設定した画面ID。0の場合は現在の画面を非表示にする。                                                                                                                                                                                                                                                  |
| ShowScreen   | void  | i id(=0)                                                                                                                                 | 指定した画面を表示する。idはscreen関数で設定した画面ID。0の場合は現在の画面を表示する。                                                                                                                                                                                                                                                          |
| mmload       | void  | s filename, i channel(=0)                                                                                                                | 音楽ファイル(mp3,ogg等)を読み込む。channelは音楽のチャンネル番号。                                                                                                                                                                                                                                                                               |
| dmmload      | void  | s filename, i channel(=0)                                                                                                                | 効果音ファイル(wav等)を読み込む。channelは効果音のチャンネル番号。                                                                                                                                                                                                                                                                               |
| mmplay       | void  | i channel, i loops(=0)                                                                                                                   | 音楽を再生する。channelは音楽のチャンネル番号。loopsはループ回数(0は1回のみ-1は無限ループ)。                                                                                                                                                                                                                                                     |
| dmmplay      | void  | i channel, i loops(=0)                                                                                                                   | 効果音を再生する。channelは効果音のチャンネル番号。loopsはループ回数(0は1回のみ-1は無限ループ)。                                                                                                                                                                                                                                                 |
| mmpos        | void  | i channel, d position                                                                                                                    | 音楽の再生位置を設定する。channelは音楽のチャンネル番号。positionは秒単位で指定。この関数を呼び出す前にmmplayであらかじめ音楽を再生しておく必要があります。                                                                                                                                                                                      |
| mmstop       | void  | i channel(=0)                                                                                                                            | 音楽を停止する。channelは音楽のチャンネル番号。                                                                                                                                                                                                                                                                                                  |
| dmmstop      | void  | i channel(=0)                                                                                                                            | 効果音を停止する。channelは効果音のチャンネル番号。                                                                                                                                                                                                                                                                                              |
| mmpause      | void  | i channel(=0)                                                                                                                            | 音楽を一時停止する。channelは音楽のチャンネル番号。                                                                                                                                                                                                                                                                                              |
| mmresume     | void  | i channel(=0)                                                                                                                            | 音楽を再開する。channelは音楽のチャンネル番号。                                                                                                                                                                                                                                                                                                  |
| mmvol        | void  | i channel, i volume(0-128)                                                                                                               | 音楽/効果音の音量を設定する。channelは音楽のチャンネル番号。volumeは0から128までの音量。                                                                                                                                                                                                                                                         |
| mmseek       | d     | i channel(=0)                                                                                                                            | 音楽の再生位置を取得する。channelは音楽のチャンネル番号。再生位置は秒単位で返される。                                                                                                                                                                                                                                                            |
| mmplaying    | i     | i channel(=0)                                                                                                                            | 音楽が再生中かどうかを返す。channelは音楽のチャンネル番号。                                                                                                                                                                                                                                                                                      |
| mmvolinfo    | i     | i channel(=0)                                                                                                                            | 音量情報を取得する。channelは音楽のチャンネル番号。音量は0から128までの値で返される。                                                                                                                                                                                                                                                            |
| end          | void  |                                                                                                                                          | プログラムを終了する。                                                                                                                                                                                                                                                                                                                           |
| exist        | i     | s filename                                                                                                                               | ファイルが存在するかどうかを確認する。filenameはファイルのパス。存在する場合は1、存在しない場合は0を返す。                                                                                                                                                                                                                                       |
| noteload     | s     | s filename                                                                                                                               | ファイルからテキストを読み込む。filenameはファイルのパス。ファイルの内容を文字列として返す。                                                                                                                                                                                                                                                     |
| notesave     | void  | s filename, s content                                                                                                                    | ファイルにテキストを書き込む。filenameはファイルのパス、contentは書き込む内容。                                                                                                                                                                                                                                                                  |
| makedir      | void  | s path                                                                                                                                   | ディレクトリを作成する。pathは作成するディレクトリのパス。既に存在する場合は何もしない。                                                                                                                                                                                                                                                         |
| rmfile       | void  | s path                                                                                                                                   | ファイル/ディレクトリを削除する。pathは削除するファイルのパス。存在しない場合は何もしない。また、ディレクトリが空でない場合は削除できない。                                                                                                                                                                                                      |
| rmdir        | void  | s path                                                                                                                                   | ディレクトリを削除する。pathは削除するディレクトリのパス。                                                                                                                                                                                                                                                                                       |
| rename       | void  | s oldname, s newname                                                                                                                     | ファイル/ディレクトリの名前/pathを変更する。oldnameは変更前の名前/path、newnameは変更後の名前/path。存在しない場合は何もしない。                                                                                                                                                                                                                 |
| cpfile       | void  | s src, s dest                                                                                                                            | ファイルをコピーする。srcはコピー元のファイルのパス、destはコピー先のファイルのパス。存在しない場合は何もしない。                                                                                                                                                                                                                                |
| cpdir        | void  | s src, s dest                                                                                                                            | ディレクトリをコピーする。srcはコピー元のディレクトリのパス、destはコピー先のディレクトリのパス。存在しない場合は何もしない。                                                                                                                                                                                                                    |
| system       | void  | s command                                                                                                                                | システムコマンドを実行する。commandは実行するコマンドの文字列。                                                                                                                                                                                                                                                                                  |
| Set3DCamera  | void  | i/d x(=0.0), i/d y(=0.0), i/d z(=0.0), i/d target_x(=0.0), i/d target_y(=0.0), i/d target_z(=0.0)                                        | 3Dカメラの位置とターゲットを設定する。x, y, zはカメラの位置、target_x, target_y, target_zはカメラが向くターゲットの座標。この関数が呼び出された時点でそのウィンドウは3D描画モードになります。                                                                                                                                                    |
| Reset3D      | void  |                                                                                                                                          | 3D描画モードをリセットします。ビュー行列とプロジェクション行列を初期化し、3D描画モードを解除します。これにより、次回の描画は2Dモードになります。                                                                                                                                                                                                 |
| mkparticle   | void  | i id, i/d r, i/d g, i/d b, array arr(=[20,1,100\])                                                                                       | 色や形状を指定してパーティクルを作成し、IDを割り当てます。最後の引数arrはパーティクルの形状を指定する配列で、デフォルトは[20,1,100\](円形)です。配列は[繰り返し数,シーケンス長,半径シーケンス0,半径シーケンス1,...\]の順で格納してください。なお、半径シーケンスにはパーティクルを多角形とみなした時の各頂点の中心からの半径を指定してください。 |
| ldparticle   | void  | i id, i img_id                                                                                                                           | あらかじめLoadImageで読み込んだ画像に対し、パーティクルIDを割り当てます。                                                                                                                                                                                                                                                                        |
| particle     | void  | i id, i/d x, i/d y, i/d z, i/d r                                                                                                         | パーティクルを描画します。x, y, zはパーティクルの位置、rはパーティクルの半径です。                                                                                                                                                                                                                                                               |
| particlem    | void  | i id, i/d r                                                                                                                              | パーティクルを描画します。rはパーティクルの半径です。パーティクルの位置は前回のパーティクル描画時の位置が使用されます。                                                                                                                                                                                                                          |
| particler    | void  | i id, i/d x, i/d y, i/d z, i/d  r,i/d angle                                                                                              | 角度を指定してパーティクルを描画します。                                                                                                                                                                                                                                                                                                         |
| particlemr   | void  | i id, i/d r, i/d angle                                                                                                                   | 角度を指定してパーティクルを描画します。パーティクルの位置は前回のパーティクル描画時の位置が使用されます。                                                                                                                                                                                                                                       |
| line3D       | void  | i/d x1, i/d y1, i/d z1, i/d x2, i/d y2, i/d z2                                                                                           | 3D空間上に線分を描画します。使用する前に必ずSet3DCameraを呼び出してください。                                                                                                                                                                                                                                                                    |
| cube         | void  | i/d x1, i/d y1, i/d z1, i/d x2, i/d y2, i/d z2                                                                                           | 3D空間上に立方体を描画します。x1, y1, z1は立方体の一つの頂点、x2, y2, z2は対角の頂点です。使用する前に必ずSet3DCameraを呼び出してください。                                                                                                                                                                                                      |
| texture3D    | void  | i/d x1,i/d y1 , i/d z1,i/d x2,i/d y2 , i/d z2, i/d x3, i/d y3, i/d z3, i/d x4, i/d y4, i/d z4                                            | quadの3D版です。quadと同様、textureを用いて画像を張り付けることができます。使用する前に必ずSet3DCameraを呼び出してください。                                                                                                                                                                                                                     |

### 各種定数

以下は、WSPで使用可能な定数の一覧です。これらの定数は、数学的な計算や描画に役立ちます。

| 定数名      | 型  | 説明                   |
| ----------- | --- | ---------------------- |
| M_PI        | d   | 円周率                 |
| M_DOUBLE_PI | d   | $2\pi$                 |
| M_THREE_PI  | d   | $3\pi$                 |
| M_FOUR_PI   | d   | $4\pi$                 |
| M_FIVE_PI   | d   | $5\pi$                 |
| M_SIX_PI    | d   | $6\pi$                 |
| M_SEVEN_PI  | d   | $7\pi$                 |
| M_EIGHT_PI  | d   | $8\pi$                 |
| M_NINE_PI   | d   | $9\pi$                 |
| M_PI_2      | d   | $\frac{\pi}{2}$        |
| M_PI_4      | d   | $\frac{\pi}{4}$        |
| M_1_PI      | d   | $\frac{1}{\pi}$        |
| M_2_PI      | d   | $\frac{2}{\pi}$        |
| M_2_SQRTPI  | d   | $\frac{2}{\sqrt{\pi}}$ |
| M_E         | d   | 自然対数の底           |
| M_LOG2E     | d   | 2の常用対数            |
| M_LOG10E    | d   | 10の常用対数           |
| M_LN2       | d   | 2の自然対数            |
| M_LN10      | d   | 10の自然対数           |
| M_SQRT2     | d   | $\sqrt{2}$             |
| M_SQRT1_2   | d   | $\frac{1}{\sqrt{2}}$   |
| M_SQRT3     | d   | $\sqrt{3}$             |
| true        | i   | 1                      |
| false       | i   | 0                      |
| dir_cur     | s   | 現在の作業ディレクトリ |
| ginfo_r     | i   | 現在の描画色の赤成分   |
| ginfo_g     | i   | 現在の描画色の緑成分   |
| ginfo_b     | i   | 現在の描画色の青成分   |

### キーコード一覧

| キー名               | キーコード             |
| -------------------- | ---------------------- |
| A~Z                  | "A", "B", ..., "Z"     |
| 0~9                  | "0", "1", ..., "9"     |
| ENTER                | "ENTER"                |
| ESC                  | "ESC"                  |
| ↑                    | "UP"                   |
| ↓                    | "DOWN"                 |
| ←                    | "LEFT"                 |
| →                    | "RIGHT"                |
| RETURN               | "RETURN"               |
| BACKSPACE            | "BACKSPACE"            |
| TAB                  | "TAB"                  |
| PAGEUP               | "PAGEUP"               |
| PAGEDOWN             | "PAGEDOWN"             |
| HOME                 | "HOME"                 |
| END                  | "END"                  |
| INSERT               | "INSERT"               |
| DELETE               | "DELETE"               |
| F1~F12               | "F1", "F2", ..., "F12" |
| CAPSLOCK             | "CAPSLOCK"             |
| NUMLOCK              | "NUMLOCK"              |
| SCROLLLOCK           | "SCROLLLOCK"           |
| PRINTSCREEN          | "PRINTSCREEN"          |
| PAUSE                | "PAUSE"                |
| 左SHIFT              | "LSHIFT"               |
| 右SHIFT              | "RSHIFT"               |
| 左CTRL               | "LCTRL"                |
| 右CTRL               | "RCTRL"                |
| 左ALT                | "LALT"                 |
| 右ALT                | "RALT"                 |
| 左Windowsキー        | "LMETA"                |
| 右Windowsキー        | "RMETA"                |
| アプリケーションキー | "MENU"                 |
