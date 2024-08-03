#include <usbh_midi.h>
#include <usbhub.h>
#include <XInput.h>

USB Usb;
USBH_MIDI Midi(&Usb);

void MIDI_poll();
void convertToXInput(char midiNote, bool noteOn);
void releaseAllXInput();

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
  // ノートに対するXInputへのマッピングを行う
  // 以下はサンプルであり、必要に応じて変更してください。

  // Xbox 360コントローラーを基にしたサンプルマッピング
  // Dパッドの代わりに左アナログスティックを使用

  // 左アナログスティックで2つの方向キーが入力されたときに、その両方の入力が加算されるような形
  // 左アナログスティックで2つの方向キーが入力されているときに、片方の入力がなくなるとその方向への入力が減算されるような形

  XInputController& xinput = XInput;  // ここでxinputを宣言

  switch (midiNote) {
    case 48:  // 左
      xInputX -= noteOn ? 32767 : 0;
      if (!noteOn && (currentNoteOn[80] || currentNoteOn[50])) {
        xInputX += currentNoteOn[80] ? 32767 : 0;  // 上が残っていれば加算
        xInputX += currentNoteOn[50] ? -32767 : 0; // 下が残っていれば減算
      }
      break;
    case 80:  // 上
      xInputY += noteOn ? 32767 : 0;
      if  (!noteOn && (currentNoteOn[48] || currentNoteOn[52])) {
        xInputY += currentNoteOn[48] ? -32767 : 0;  // 上が残っていれば加算
        xInputY += currentNoteOn[52] ? 32767 : 0; // 下が残っていれば減算
      }
      break;
    case 50:  // 下
      xInputY -= noteOn ? 32767 : 0;
      if  (!noteOn && (currentNoteOn[48] || currentNoteOn[52])) {
        xInputY += currentNoteOn[48] ? -32767 : 0;  // 上が残っていれば加算
        xInputY += currentNoteOn[52] ? 32767 : 0; // 下が残っていれば減算
      }
      break;
    case 52:  // 右
      xInputX += noteOn ? 32767 : 0;
      if (!noteOn && (currentNoteOn[80] || currentNoteOn[50])) {
        xInputX += currentNoteOn[80] ? 32767 : 0;  // 上が残っていれば減算
        xInputX += currentNoteOn[50] ? -32767 : 0; // 下が残っていれば減算
      }
      break;
    // 他のノートに対するケースを追加してください。
    case 49:
      xinput.press(XInputControl::BUTTON_RB);
      if (!noteOn) xinput.release(XInputControl::BUTTON_RB);
      break;
    case 53:
      xinput.press(XInputControl::TRIGGER_LEFT);
      if (!noteOn) xinput.release(XInputControl::TRIGGER_LEFT);
      break;
    case 55:
      xinput.press(XInputControl::BUTTON_LB);
      if (!noteOn) xinput.release(XInputControl::BUTTON_LB);
      break;
    case 60:
      xinput.press(XInputControl::BUTTON_START);
      if (!noteOn) xinput.release(XInputControl::BUTTON_START);
      break;
    case 62:
      xinput.press(XInputControl::BUTTON_BACK);
      if (!noteOn) xinput.release(XInputControl::BUTTON_BACK);
      break;
    case 79:
      xinput.press(XInputControl::TRIGGER_RIGHT);
      if (!noteOn) xinput.release(XInputControl::TRIGGER_RIGHT);
      break;
    case 81:
      xinput.press(XInputControl::BUTTON_X);
      if (!noteOn) xinput.release(XInputControl::BUTTON_X);
      break;
    case 82:
      xinput.press(XInputControl::BUTTON_A);
      if (!noteOn) xinput.release(XInputControl::BUTTON_A);
      break;
    case 83:
      xinput.press(XInputControl::BUTTON_Y);
      if (!noteOn) xinput.release(XInputControl::BUTTON_Y);
      break;
    case 84:
      xinput.press(XInputControl::BUTTON_B);
      if (!noteOn) xinput.release(XInputControl::BUTTON_B);
      break;
  }

  // ノートオフの場合はすぐにリリース
  if (!noteOn) {
    currentNoteOn[midiNote] = false;
    releaseAllXInput();
  } else {
    currentNoteOn[midiNote] = true;
  }
}

void releaseAllXInput()
{
  // 離れた入力の分だけ減算
  if (!currentNoteOn[48] && !currentNoteOn[50] && !currentNoteOn[52] && !currentNoteOn[80]) {
    xInputX = 0;
    xInputY = 0;
  }
}
