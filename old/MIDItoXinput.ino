#include <usbh_midi.h>
#include <usbhub.h>
#include <XInput.h>

USB Usb;
USBH_MIDI Midi(&Usb);

void MIDI_poll();
void convertToXInput(char midiNote, bool noteOn);

static bool currentNoteOn[128] = {false};
static int16_t xInputX = 0;
static int16_t xInputY = 0;

void setup()
{
  if (Usb.Init() == -1) {
    while (1);  // 初期化失敗時は停止
  }
  delay(10);
  XInput.begin();
}

void loop()
{
  Usb.Task();
  MIDI_poll();
}

void MIDI_poll()
{
  uint8_t outBuf[3];
  uint8_t size;

  do {
    if ((size = Midi.RecvData(outBuf)) > 0) {
      switch (outBuf[0] & 0xf0) {
        case 0x80:  // note off
          convertToXInput(outBuf[1], false);
          break;
        case 0x90:  // note on
          convertToXInput(outBuf[1], outBuf[2] != 0);
          break;
      }
    }
  } while (size > 0);

  // 左アナログスティックの値をXInputに送信
  XInputController& xinput = XInput;
  xinput.setJoystick(XInputControl::JOY_LEFT, xInputX, xInputY);
  xinput.send();
}

void convertToXInput(char midiNote, bool noteOn)
{
  // MIDIノートの状態を更新
  currentNoteOn[midiNote] = noteOn;

  // 入力状態に基づいてスティック位置を再計算
  int16_t tempX = 0;
  int16_t tempY = 0;

  // 左右の計算
  if (currentNoteOn[48]) tempX -= 32767;  // 左
  if (currentNoteOn[52]) tempX += 32767;  // 右

  // 上下の計算
  if (currentNoteOn[68]) tempY += 32767;  // 上
  if (currentNoteOn[50]) tempY -= 32767;  // 下

  // 下方向から左斜め上への特殊ロジック
  if (currentNoteOn[50] && currentNoteOn[48] && currentNoteOn[68]) {
    tempX = -32767;  // 左
    tempY = 32767;   // 上
  }

  // 計算結果を更新
  xInputX = tempX;
  xInputY = tempY;

  // その他のボタン処理
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
    case 67:
      if (noteOn) xinput.press(XInputControl::BUTTON_RB);
      else xinput.release(XInputControl::BUTTON_RB);
      break;
    case 69:
      if (noteOn) xinput.press(XInputControl::BUTTON_X);
      else xinput.release(XInputControl::BUTTON_X);
      break;
    case 70:
      if (noteOn) xinput.press(XInputControl::BUTTON_A);
      else xinput.release(XInputControl::BUTTON_A);
      break;
    case 71:
      if (noteOn) xinput.press(XInputControl::BUTTON_Y);
      else xinput.release(XInputControl::BUTTON_Y);
      break;
    case 72:
      if (noteOn) xinput.press(XInputControl::BUTTON_B);
      else xinput.release(XInputControl::BUTTON_B);
      break;
  }
}
