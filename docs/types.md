# 型一覧

## 整数型

```wsp
int a = 10
int b = "10" // 自動でint型に変換される

// エラー!
int c = "Hello, World"
```

## 倍精度浮動小数点型

```wsp
double a = 3.14
double b = "3.14" // 自動でdouble型に変換される

// エラー!
double c = "Hello, World"
```

## 文字列型

```wsp
string a = "Hello, World!"
string b = 10 // 自動でstring型に変換される => "10"
string c = 3.14 // 自動でstring型に変換される => "3.14"
```

## 配列

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
