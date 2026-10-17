// offsets.h - TU TEGO PLIKU WPISUJESZ ADRESY dla swojej wersji gry.
// 0 = opcja wyłączona (nic nie zapisze). Wszystko jest big-endian (PS3).
#ifndef OFFSETS_H
#define OFFSETS_H

#define ADDR_FLY        0x00000000  // bajt: 1 = latanie włączone
#define ADDR_GODMODE    0x00000000  // bajt: 1 = nieśmiertelność
#define ADDR_SPEED      0x00000000  // float: mnożnik prędkości
#define ADDR_NOCLIP     0x00000000  // bajt: 1 = przenikanie przez bloki

// Dozwolony zakres pamięci gry (zabezpieczenie przed zapisem w złe miejsce).
// Dostosuj po sprawdzeniu mapy pamięci swojej wersji.
#define MEM_MIN         0x00010000
#define MEM_MAX         0x10000000

#endif
