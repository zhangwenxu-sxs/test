#ifndef _EDID_H
#define _EDID_H

#define HdmiSink 0x01
#define VgaSink 0x00

int GetTxEdid(unsigned char sink);
void RebuildSinkEdid(unsigned char rx, unsigned char tx);
void BuildSourceEdid(unsigned char rx);
void SaveEdidToShadow(unsigned char rx);

#endif

