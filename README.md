# M5S_WxBeacon2


https://user-images.githubusercontent.com/26270227/186464005-bfc6b1c4-3f5c-4e7f-b2bc-fb4b9d4b0df4.mp4

定期的に WxBeacon2 から情報を取得し、ティッカーに表示、ポン子ちゃんアバターが喋ります。


## 概要
[株式会社ウェザーニューズ](https://ja.wikipedia.org/wiki/%E3%82%A6%E3%82%A7%E3%82%B6%E3%83%BC%E3%83%8B%E3%83%A5%E3%83%BC%E3%82%BA)(以下 WNI )による 24 時間お天気番組[ウェザーニュースLiVE](https://weathernews.jp/web910/) とリンクしたアプリケーション [ウェザーニュース](https://weathernews.jp/app/) では サンクスポイント 2000 pt以上取得すると[ウェザービーコン2](https://weathernews.jp/smart/wxbeacon2/)(以下 WxBeacon2 ) をゲットできます。(別途[有償購入](https://weathernews.jp/sorashop/)も可能)

<img src="https://weathernews.jp/smart/wxbeacon2/images/mainV.jpg" alt="WxBeacon2" title="WxBeacon2" width="375" height="196">

M5Stack Basic / Gray / Core2 を使って WxBeacon2 から情報を取得し、 [AquesTalk ESP32](https://www.a-quest.com/products/aquestalk_esp32.html) にて喋らせるアプリケーションです。<br>
どうせならウェザーニュースLiVE のマスコット、ポン子こと [WEATHEROID TypeA Airi](https://twitter.com/typea_airi) ちゃんに喋らせようという事で、アバターはポン子ちゃんとなっております。

<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/d/d7/WEATHEROID_TypeA_Airi.jpg/419px-WEATHEROID_TypeA_Airi.jpg" alt="ponko" title="ponko" width="210" height="424">

## 必要なもの

### M5Stack
M5Stack Basic,Gray,Core2 のいずれか。

### WxBeacon2
実体は [OMRON 2JCIE-BL01](https://components.omron.com/jp-ja/products/sensors/2JCIE-BL) です。ですので、WxBeacon2 でなく当該製品でも動作すると思われます(手元に無いので未確認)。

## コンパイル

### 依存ライブラリ
* [M5Unified](https://github.com/m5stack/M5Unified) 0.0.7 or later
* [M5GFX](https://github.com/m5stack/M5GFX) 0.0.20 or later
* [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino) 1.4.0 or later
* [AquesTalk ESP32](https://www.a-quest.com/products/aquestalk_esp32.html) 2.21 or later

### 画像ソース
**権利上の問題でポン子ちゃんの画像などは当リポジトリには含まれません。**<br>
自分で以下の条件を満たす画像をソースファイルとして共にコンパイルする必要があります。

#### フォーマット
Basic,Gray での動作の都合上、メモリ使用料削減の為に 4bpp,1bpp を扱います。
[Windows Bitmap](https://ja.wikipedia.org/wiki/Windows_bitmap) Version 3 (BITMAPINFOHEADER.biSize == 40) format であること。
画像は作成の際の位置イメージです。

* 顔<br>
Windows bitmap 320 px x 240 px 4 bpp (16色)  パレット 0 番は透過色。口とパレット構成を合わせる為、顔にも透過色が必要。
![face](./doc/ponko_face.png)
* 閉じた口<br>
Windows bitmap 48 px x 32 px 4 bpp (16色) パレット 0 番は透過色。顔に透過色を用いて上書きされる。
![close](./doc/ponko_mouth_close.png)
* 開いた口<br>
Windows bitmap 48 px x 32 px 4 bpp (16色) パレット 0 番は透過色。顔に透過色を用いて上書きされる。
![open](./doc/ponko_mouth_open.png)
* WNI ロゴ<br>
Windows bitmap 32 px x 32 px 1 bpp (2色) パレット 0 番は透過色。ティッカーに透過色を用いて上書きされる。
![logo](./doc/logo.png)

**顔、閉じた口、開いた口は同じパレット配列でなければなりません。**<BR>
16色 Bitmap のパレットの並び替えは、拙作 [reorder_palette.py](https://github.com/GOB52/bitmap_tools/blob/master/reorder_palette.py) が使用できます。


#### ソースファイル化
全てのリソースについて行ってください。 ponko_face.bmp における例を示します。

```
xxd -i ponko_face.bmp ./src/res/ponko_face.cpp
```

xxd で C形式に変換します。出力場所は ./src/res が良いでしょう。
出力したソースを修正します。

```cpp

#include "resource.h"  // 宣言

//unsigned char ponko_face_bmp[] = {
const uint8_t ponko_face_bmp[] = {
  0x42, 0x4d, 0x76, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00,
  0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x40, 0x01, 0x00, 0x00, 0xf0, 0x00,
.
.
.
};
// unsigned int ponko_face_bmp_len = 38518;
const size_t ponko_face_bmp_len = 38518;

```

#### 参考 : 各リソースの長さ(フォーマット他条件が一致していれば違っても良い)

|xxd -i source| \_bmp\_len|
|---|---|
|ponk\_face.cpp| 38518 |
|ponko\_mouth\_close.cpp| 886 |
|ponko\_mouth\_open.cpp| 886 |
|logo.cpp| 190 |


### 動作制御
./src/main.cpp の上部にいくつかの設定項目があります。

```cpp

// Settings
namespace
{
// Auto talking interval time (Unit:second) [自動で WxBeacon2 からデータを取得して喋る頻度]
constexpr uint16_t TALKING_INTERVAL_SEC = 60 * 3;
// Measurement interval time (Unit:second) for broadcast mode [WxBeacon2 のデータ更新頻度]
constexpr uint16_t MEASUREMENT_INTERVAL_BROADCAST_SEC = 60 * 3;
// turnon LED when got advertise data. [WxBeacon2 のデータを取得できたら LED を光らせるか?]
constexpr bool TURNON_LED = false;
}

```

### WiFi SSID password

時刻取得の為、起動時に WiFi 接続で NTP サーバーへの接続を行います。<br>
時刻の表示並びに、 WxBeacon2 のモードを出荷時に戻す際に、蓄積データの基準時刻を書き込む必要がある為です。<BR>
前もって接続する WiFi の SSID とパスワードをハードに書き込む必要があります。(Wifi.begin() を引数なしで呼ぶと保存されている情報を元に接続します)<br>
SmartConfig 等を利用して SSID とパスワード書き込むか、 WiFi.begin(ssid, password); として一旦接続させるかして、ハードに情報を保存してください。<br>
@tnkmasayuki さんによる記事 [Lang-ship](https://lang-ship.com/blog/work/esp32-wi-fi-setting/) が参考になるでしょう。


## 動作

WxBeacon2 は通常(出荷時状態)、計測データを内部に蓄積しますが、このアプリケーションでは蓄積モードではなく、ブロードキャストモードを利用します。後述の操作方法(\*1)より、ブロードキャストモードに変更する必要があります。<BR>
**なおモードを変更した場合、蓄積したデータは全て失われる点、ウェザーニュースアプリとの連携ができなくなる点には注意です。**<br>
**後述の操作方法(\*2)によって出荷時に状態を戻し、再びウェザーニュースアプリとの連携を取ることができるようになります。**
モードがブロードキャストモードではない場合は、ティッカーが紫色となり、ティッカーにその旨表示されます。。<BR>
ブロードキャストモードであれば、起動時にデータを取得、ティッカーに表示してポン子ちゃんが喋ります。その際のティッカーの色は WBGT (熱中症危険度) のレベルに応じて青、黄、赤のいずれかの色となります。以降は定期的にデータを取得して表示と喋りを行います。


## 操作方法

|ボタン| 長押し | 短押し |
|---|---|---|
|A | | 強制的にWxBeacon2のデータを取得して喋る|
|B | (\*2)WxBeacon2 を出荷時の状態へ戻す | |
|C | (\*1)WxBeacon2 をブロードキャストモードに変更する | ティッカー表示の ON/OFF |

C 長押しでブロードキャストモードに上手くならなかった場合は、 B 長押しで出荷時に戻して再度試してください。

## 謝辞

* 各ライブラリの作成者関係者の方々、助言や示唆を頂いた方々には毎度大変お世話になっております。<BR>
* 毎日楽しい番組を送ってくださる WNI のスタッフ、[気象予報士、キャスター](https://weathernews.jp/wnl/caster/index.html)他の皆様、作業中ラジオ的に楽しんでおります。<BR>
* 元祖 Vtuber ポン子ちゃんを生み出した皆様、マネージャーの[山岸キャスター](https://twitter.com/airin0609)、木曜日の深夜がいつも楽しみです。<BR>
