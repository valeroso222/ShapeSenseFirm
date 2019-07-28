# ShapeSenseFirm

## bt_Triple_Motor_SW_test_0 (<- the file name should be changed.)
Firmware for ShapeSense to control positions of wings by serial commands.

The serial commands are below:

'a': Move the frontal wing by an intended value (mm) following 'a'. 

'b': Move the central wing by an intended value (mm) following 'b'. 

'c': Move the back wing by an intended value (mm) following 'a'. 

'i': Stop all wings.

'g': Monitor all wing positions. The shown values represent the rotation of the rotary encoders (not the distance from the initial positions).

'r': Reset all wing positions. Wings move down until they push the switches attatched at the top of the handle.

## auto (<- the file name should be changed.)
Firmware for Non-VR demo.

This program has the following additional serial commands:

'A': Enalbe Auto-mode, automatically change the positions of wings with the following order: 'initial shape' -> 'Kunai' -> 'Big fan' -> 'Hammer' -> 'Sword'.

'R': Enable Random-mode, randomly change the positions of wings.

'D': Disable auto/random-mode.
