#include <usbh_midi.h>
#include <usbhub.h>
#include <XInput.h>

// USBとMIDIのオブジェクト
USB Usb;
USBH_MIDI Midi(&Usb);

// XInputコントローラーの状態を管理する構造体
struct XInputState {
  int16_t joyX = 0;
  int16_t joyY = 0;
};

// グローバルな状態変数
XInputState xinputState;
static bool currentNoteOn[128] = {false};

// MIDIノートからXInputボタンへのマッピングテーブル
const struct {
  uint8_t midiNote;
  XInputControl control;
} buttonMappings[] = {
  {49, XInputControl::TRIGGER_RIGHT},
  {53, XInputControl::BUTTON_LB},
  {55, XInputControl::TRIGGER_LEFT},
  {57, XInputControl::BUTTON_L3},
  {59, XInputControl::BUTTON_R3},
  {60, XInputControl::BUTTON_START},
  {62, XInputControl::BUTTON_BACK},
  {67, XInputControl::BUTTON_RB},
  {69, XInputControl::BUTTON_X},
  {70, XInputControl::BUTTON_A},
  {71, XInputControl::BUTTON_Y},
  {72, XInputControl::BUTTON_B},
  // Dパッドマッピング
  {56, XInputControl::DPAD_LEFT},
  {58, XInputControl::DPAD_RIGHT},
  {61, XInputControl::DPAD_DOWN},
  {63, XInputControl::DPAD_UP},
};

// 関数プロトタイプ
void updateJoystickState();
void updateButtonState(uint8_t midiNote, bool noteOn);

void setup() {
  Serial.begin(250000);
  delay(200);

  // USB初期化の堅牢な処理
  int retryCount = 0;
  while (Usb.Init() != 0) {
    Serial.print("USB初期化に失敗。再試行... ");
    Serial.println(++retryCount);
    if (retryCount > 5) {
      Serial.println("複数回の試行後も失敗。ハードウェアを確認してください。");
      while (1);
    }
    delay(1000);
  }
  Serial.println("USB初期化に成功。");

  XInput.begin();
  delay(10);
  Serial.println("XInput初期化に成功。");
}

void loop() {
  // USBタスクを常時実行
  Usb.Task();

  uint8_t outBuf[3];
  uint8_t size = Midi.RecvData(outBuf);

  // MIDIメッセージが届き次第、即座に処理
  if (size > 0) {
    uint8_t midiNote = outBuf[1];
    bool noteOn = (outBuf[0] & 0xF0) == 0x90 && outBuf[2] > 0;
    bool noteOff = (outBuf[0] & 0xF0) == 0x80 || ((outBuf[0] & 0xF0) == 0x90 && outBuf[2] == 0);

    if (noteOn) {
      currentNoteOn[midiNote] = true;
    } else if (noteOff) {
      currentNoteOn[midiNote] = false;
    }

    // コントローラーの状態を更新
    updateJoystickState();
    updateButtonState(midiNote, noteOn);
    
    // 更新された状態をXInputコントローラーに送信
    XInput.setJoystick(XInputControl::JOY_LEFT, xinputState.joyX, xinputState.joyY);
    XInput.send();
  }
}

// ジョイスティックの状態を更新する関数
void updateJoystickState() {
  int16_t tempX = 0;
  int16_t tempY = 0;

  // 左右の動き
  if (currentNoteOn[48]) tempX -= 32767; // C4 (左)
  if (currentNoteOn[52]) tempX += 32767; // E4 (右)

  // 上下の動き
  if (currentNoteOn[68]) tempY += 32767; // G5 (上)
  if (currentNoteOn[50]) tempY -= 32767; // D4 (下)

  // 特殊な斜め入力のロジック
  if (currentNoteOn[50] && currentNoteOn[48] && currentNoteOn[68]) {
    tempX = -32767;
    tempY = 32767;
  }

  xinputState.joyX = tempX;
  xinputState.joyY = tempY;
}

// ボタンの状態を更新する関数
void updateButtonState(uint8_t midiNote, bool noteOn) {
  for (const auto& mapping : buttonMappings) {
    if (mapping.midiNote == midiNote) {
      if (noteOn) {
        XInput.press(mapping.control);
      } else {
        XInput.release(mapping.control);
      }
      return; // マッピングが見つかったら終了
    }
  }
}
