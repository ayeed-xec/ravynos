#ifndef _DEV_PCI_PCIREG_SHIM_H_
#define _DEV_PCI_PCIREG_SHIM_H_

#include <sys/types.h>

#define PCIR_VENDOR 0x00
#define PCIR_DEVICE 0x02
#define PCIR_COMMAND 0x04
#define PCIR_STATUS 0x06
#define PCIR_REVID 0x08
#define PCIR_PROGIF 0x09
#define PCIR_SUBCLASS 0x0a
#define PCIR_CLASS 0x0b
#define PCIR_CACHELNSZ 0x0c
#define PCIR_LATTIMER 0x0d
#define PCIR_HDRTYPE 0x0e
#define PCIR_BIST 0x0f
#define PCIR_CAPS_PTR 0x34
#define PCIR_INTRLINE 0x3c
#define PCIR_INTPIN 0x3d
#define PCIR_MINGNT 0x3e
#define PCIR_MAXLAT 0x3f

#define PCIR_BAR(x) (0x10 + ((x) * 4))

#define PCIM_CMD_PORTEN 0x1
#define PCIM_CMD_MEMEN 0x2
#define PCIM_CMD_BUSMASTEREN 0x4

#define PCIM_HDRTYPE 0x7
#define PCIM_HDRTYPE_NORMAL 0
#define PCIM_HDRTYPE_BRIDGE 1
#define PCIM_HDRTYPE_CARDBUS 2
#define PCIM_MFDEV 0x80

#define PCIR_BAR_MSIXTABLE 0xf000

#define PCIY_MSI 0x05
#define PCIY_MSIX 0x11
#define PCIY_EXPRESS 0x10
#define PCIY_PMG 0x01

#define PCIC_BRIDGE 0x06
#define PCIS_BRIDGE_PCI 0x04

#endif
