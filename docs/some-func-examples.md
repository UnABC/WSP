# 一部関数の使用例

## 文字列操作

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

## その他便利な組み込み関数等

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
