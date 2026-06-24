[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT) 

![alt text](pics/vidbitsnap.png) 

# nitrologic microPAL VidBit 0.0.6 🤖

An RP2350 microcontroller video game harness.

## tools

* resetcoms 0.1 - send reboot command to all com ports with path "USB\\VID_2E8" 

## documentation

this document - https://github.com/nitrologic/vidbit

# CONNECTORS

# PS/2 Mini DIN 6
```
   ◎   ◎
  ◎     ◎
    ◎ ◎

6 4 2  1 3 5
```

| pin | GPIO | name |
| --- | ---- | ----------- |
| 1   | GP0  | +Data
| 2   | GP2  | rx0
| 3   |      | GND
| 4   |      | VCC
| 5   | GP1  | +Clock
| 6   | GP3  | tx0

# ATARI 9 PIN
```
 ◎ ◎ ◎ ◎ ◎
  ◎ ◎ ◎ ◎

  1 2 3 4 5
   6 7 8 9
```

| pin | name |
| --- | ----------- |
| 1   | Up
| 2   | Down
| 3   | 
| 4   | 
| 5   | Paddle B
| 6   | Paddle A
| 7   | VCC
| 8   | GND
| 9   | Button

# VGA 15 PIN

| pin | name |
| --- | ----------- |
| 1   | Red
| 2   | Green
| 3   | Blue
| 4   | ID2
| 5   | GND
| 6   | Red GND
| 7   | Green GND
| 8   | Blue GND
| 9   | * 5V KEY
| 10  | * GND SYNC
| 11  | NC ID0 RES
| 12  | * ID1 SDA   | GP0
| 13  | HSYNC
| 14  | VSYNC
| 15  | * ID3 SCL   | GP1

## DDC2B / I²C


# 74165

"◎ 1  _LOAD    VCC 16 ◎",
"◎ 2  CLK      INH 15 ◎",
"◎ 3  E          D 14 ◎",
"◎ 4  F          C 13 ◎",
"◎ 5  G          B 12 ◎",
"◎ 6  H          A 11 ◎",
"◎ 7  _QH      SER 10 ◎",
"◎ 8  GND        QH 9 ◎"
