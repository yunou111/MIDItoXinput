#include <usbh_midi.h>
#include <usbhub.h>
#include <XInput.h>

USB Usb;
USBH_MIDI Midi(&Usb);

static bool currentNoteOn[128] = {false};
static int16_t xInputX = 0;
static int16_t xInputY = 0;

unsigned long lastPollTime = 0;

//ArduinoがUSBデバイスと安定して通信できる状態を確保し、XInputによる入力の準備を整える//
void setup() {
  Serial.begin(250000);  // 250000bpsでシリアル通信を開始
  delay(200);

  int retryCount = 0;
  while (Usb.Init() != 0) {
    Serial.print("USB initialization failed, retrying... Attempt: ");
    Serial.println(retryCount + 1);
    retryCount++;
    if (retryCount > 5) {
      Serial.println("Failed to initialize USB after multiple attempts. Check hardware.");
      while (1);  // 5回リトライしても失敗したら停止
    }
    delay(1000);
  }
  Serial.println("USB initialized successfully.");

  XInput.begin();
  delay(10);
}

void loop() {
  unsigned long currentMicros = micros();

  // 1000Hz (1ms) ごとにポーリング処理を実行
  if (currentMicros - lastPollTime >= 1) {  // 1msごとに
    lastPollTime = currentMicros;

    // 非同期的にUSBとMIDIデータを処理
    Usb.Task();
    MIDI_poll();
  }
}

//MIDIキーボードのノート（鍵盤の音）を XInputのボタンとしてマッピングしつつ、左スティックの値を更新する関数//
void MIDI_poll() {
  uint8_t outBuf[3];
  uint8_t size = Midi.RecvData(outBuf);  // MIDIデータの受信サイズを取得

  // MIDIメッセージがある場合のみ処理
  if (size > 0) {
    // MIDIメッセージタイプの判定と処理
    switch (outBuf[0] & 0xf0) {
      case 0x80:  // note off
        convertToXInput(outBuf[1], false);
        break;
      case 0x90:  // note on
        convertToXInput(outBuf[1], outBuf[2] != 0);
        break;
    }
  }

  // 左アナログスティックの値をXInputに送信
  XInputController& xinput = XInput;
  xinput.setJoystick(XInputControl::JOY_LEFT, xInputX, xInputY);
  xinput.send();
}

void convertToXInput(char midiNote, bool noteOn) {
  // MIDIノートの状態を更新
  currentNoteOn[midiNote] = noteOn;

  // 入力状態に基づいてスティック位置を再計算
  int16_t tempX = 0;
  int16_t tempY = 0;

  // 左右の計算
  if (currentNoteOn[48]) tempX -= 32767;  // 左
  if (currentNoteOn[52]) tempX += 32767;  // 右

  // 上下の計算
  if (currentNoteOn[80]) tempY += 32767;  // 上
  if (currentNoteOn[50]) tempY -= 32767;  // 下

  // 下方向から左斜め上への特殊ロジック
  if (currentNoteOn[50] && currentNoteOn[48] && currentNoteOn[80]) {
    tempX = -32767;  // 左
    tempY = 32767;   // 上
  }

  // 計算結果を更新
  xInputX = tempX;
  xInputY = tempY;

  // その他のボタン処理(37鍵盤)
  XInputController& xinput = XInput;

  switch (midiNote) {
    case 49:
      if (noteOn) xinput.press(XInputControl::TRIGGER_RIGHT);
      else xinput.release(XInputControl::TRIGGER_RIGHT);
      break;
    case 53:
      if (noteOn) xinput.press(XInputControl::BUTTON_LB);
      else xinput.release(XInputControl::BUTTON_LB);
      break;
    case 55:
      if (noteOn) xinput.press(XInputControl::TRIGGER_LEFT);
      else xinput.release(XInputControl::TRIGGER_LEFT);
      break;
    case 57:
      if (noteOn) xinput.press(XInputControl::BUTTON_L3);
      else xinput.release(XInputControl::BUTTON_L3);
      break;
    case 59:
      if (noteOn) xinput.press(XInputControl::BUTTON_R3);
      else xinput.release(XInputControl::BUTTON_R3);
      break;
    case 60:
      if (noteOn) xinput.press(XInputControl::BUTTON_START);
      else xinput.release(XInputControl::BUTTON_START);
      break;
    case 62:
      if (noteOn) xinput.press(XInputControl::BUTTON_BACK);
      else xinput.release(XInputControl::BUTTON_BACK);
      break;
    case 79:
      if (noteOn) xinput.press(XInputControl::BUTTON_RB);
      else xinput.release(XInputControl::BUTTON_RB);
      break;
    case 81:
      if (noteOn) xinput.press(XInputControl::BUTTON_X);
      else xinput.release(XInputControl::BUTTON_X);
      break;
    case 82:
      if (noteOn) xinput.press(XInputControl::BUTTON_A);
      else xinput.release(XInputControl::BUTTON_A);
      break;
    case 83:
      if (noteOn) xinput.press(XInputControl::BUTTON_Y);
      else xinput.release(XInputControl::BUTTON_Y);
      break;
    case 84:
      if (noteOn) xinput.press(XInputControl::BUTTON_B);
      else xinput.release(XInputControl::BUTTON_B);
      break;
  // Dパッド(競技用にするなら削除)
    case 56:
      if (noteOn) xinput.press(XInputControl::DPAD_LEFT);
      else xinput.release(XInputControl::DPAD_LEFT);
      break;
    case 58:
      if (noteOn) xinput.press(XInputControl::DPAD_RIGHT);
      else xinput.release(XInputControl::DPAD_RIGHT);
      break;
    case 61:
      if (noteOn) xinput.press(XInputControl::DPAD_DOWN);
      else xinput.release(XInputControl::DPAD_DOWN);
      break;
    case 63:
      if (noteOn) xinput.press(XInputControl::DPAD_UP);
      else xinput.release(XInputControl::DPAD_UP);
      break;
  }
}
