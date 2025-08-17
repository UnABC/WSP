# 基本構文

## 変数宣言 / 代入

型を指定して変数を宣言します。

```wsp
int a = 10
double b = 3.14
string c = "Hello, World!"
```

型を指定しないHSPのような変数宣言も可能です。

```wsp
a = 10
b = 3.14
c = "Hello, World!"
```

型変換関数を用いた明示的な型キャスト。

```wsp
f = int(b) // doubleからintへの変換
g = string(a) // intからstringへの変換
```

## 演算子

```wsp
int x = 10
int y = 5

// 基本的な四則演算
print("加算: " + (x + y))
print("減算: " + (x - y))
print("乗算: " + (x * y))
print("除算: " + (x / y))

// インクリメント / デクリメント
print("インクリメント: " + x++)
print("デクリメント: " + y--)
```

## スコープ

変数のスコープは、宣言されたブロック内に限定されます。

```wsp
{
    int x = 10
    {
        int y = 20
        print(x + y) // 30
    }
    print(x + y) // yが宣言されたスコープの外にあるため、intの初期値で初期化される。
}
```

## 条件分岐

if文や三項演算子を使用して条件分岐を行います。

```wsp
// if文
if a > 5 {
    print("a is greater than 5")
} else if a == 5 {
    print("a is equal to 5")
} else {
    print("a is less than 5")
}

// ブロック内の文が一つならば
// 波括弧を省くことができる。
if (b == 0)
    print("b is zero")

// if文の条件部分での変数宣言
if ((int f = a * b) == 10) {
    print("f is 10")
} else {
    print("f is" + f)
}

// 三項演算子
result = (a > 5) ? "Greater" : "Not greater"

```

## ループ

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

## 関数定義と呼び出し

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

## コメント

HSPと同様です。
