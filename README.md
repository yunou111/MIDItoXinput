MIDIキーボードからの入力をXinputの入力へ変換します。<br>
【追記】下方向から左斜め上への入力ができなかったのを修正しました。（2024/11/24）<br>
<br>
### 必要なハードウェア  

・Arduino Leonardo (ATmega32u4)  
・Arduino USB Host Shield<br>
<br>
### 使い方  

1．USB Host Shield のピンを、Arduino Leonardo のソケットに差し込んでください。  

2．次のサイトから Arduino IDE をダウンロードして PC にインストールしてください。 https://www.arduino.cc/en/Main/Software  

3．次のサイトの How to include the library の Manual Installation を参考にして、USB Host Library Rev.2.0 を Arduino IDE にインストールしてください。 https://github.com/felis/USB_Host_Shield_2.0  

4．次のサイトを参考に Arduino XInput Library を Arduino IDE にインストールしてください。 https://qiita.com/sh00t/items/fee21d8d893bf134ac4f  

5．このサイトの MIDItoXinput.ino をダウンロードし、Arduino IDE で、MIDItoXinput.ino を開いてください。  

6．Leonardo の USB micro-B コネクタと PC を USB ケーブルで接続してください。  

7．Arduino IDE のメニューの [ツール] -> [ボード] -> [Xinput AVR Boards] で [Arduino Leonardo w/ Xinput] を選択してください。同様に [ツール] -> [シリアルポート] で、[COM*(Arduino Leonardo)] を選択してください（*は数字）。  

8．Arduino IDE のツールバーの (✓) アイコンをクリックしてください。下の方に「スケッチをコンパイルしています…」と表示され、しばらく待つと「コンパイルが完了しました。」と表示されます。  

9．Arduino IDE のツールバーの (->) アイコンをクリックしてください。コンパイルの後、「マイコンボードに書き込んでいます…」「ボードへの書き込みが完了しました。」と表示されれば完了です。
（書き込み中にデバイスの外れる音がしたら、すぐに Arduino Leonardo のリセットボタンを押してください。※）

※ボードへの書き込み中にXInputのゲームコントローラーとして認識されるとArduino IDE上でポートを認識できなくなるため再接続する。

10．Arduino Leonardo のUSB A の方に MIDI キーボードを接続し、コントロールパネルのデバイスとプリンターで　[Xbox 360 Controller for Windows]　と認識されていることと、ゲームコントローラーの設定 -> プロパティ で正しく入力できているかを確認してください。  
<br>
そのほか詳細はこちら<br>
https://note.com/yunou_111/n/na6df6258635c
