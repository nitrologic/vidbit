[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT) 

![alt text](vidbitsnap.png) 

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


# integrated circuit blocks

```
# 74-165 Shift Register

⬜╔═1═LOAD═□═VCC 16═╗🟥
⬜║ 2 CLK    INH 15 ║⬜
⬛║ 3 E        D 14 ║⬛
⬛║ 4 F        C 13 ║⬛
⬛║ 5 G        B 12 ║⬛
⬛║ 6 H        A 11 ║⬛
⬜║ 7 _QH    SER 10 ║⬜
🟩╚═8═GND══════QH═9═╝⬜
```

# codeblocks

```
"rect":"▪▫■□▢▣",
"hatched":"▤▥▦▧▨▩",
"pointChars":"◯⊙⊚⦾⦿◉◎◍❂○●◦◌",
"starChars":"✩✪✫✬✭✮✯✰✱✲✳✴✵✶✷✸✹✺✻✼✽✾✿❀❁",
"circles":"◐◒◑◓◔◕",
"markdown":"↩↪↗↘↖↙◀▶✳✴",
"square":"🔳🔲✅❎",
```

# pics

<img src="joy9.jpg" width="480"> 

"upper": "𝙰𝙱𝙲𝙳𝙴𝙵𝙶𝙷𝙸𝙹𝙺𝙻𝙼𝙽𝙾𝙿𝚀𝚁𝚂𝚃𝚄𝚅𝚆𝚇𝚈𝚉"
"lower": "𝚊𝚋𝚌𝚍𝚎𝚏𝚐𝚑𝚒𝚓𝚔𝚕𝚖𝚗𝚘𝚙𝚚𝚛𝚜𝚝𝚞𝚟𝚠𝚡𝚢𝚣"
"digits": "𝟶𝟷𝟸𝟹𝟺𝟻𝟼𝟽𝟾𝟿"

# ①②③④⑤⑥⑦⑧⑨⑩⑪⑫⑬⑭⑮⑯⑰⑱⑲⑳

import unicodedata
clean = unicodedata.normalize("NFKC", "𝟶𝟷𝟸𝟹")  # → "0123"
