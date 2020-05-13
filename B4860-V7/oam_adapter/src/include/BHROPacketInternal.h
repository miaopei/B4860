/*************************************************************************
	> File Name: BHROPacketInternal.h
	> Author: miaopei
	> Mail: miaopei@baicells.com 
	> Created Time: 2020年04月27日 星期三 11时16分50秒
 ************************************************************************/
#ifndef BHROPACKETINTERNAL_H
#define BHROPACKETINTERNAL_H

#include <iostream>
#include "Packet.h"


#define PACK_STRUCT     __attribute__((packed))

typedef struct PACK_STRUCT BHRO_S_PACKET_HDR {
    uint8_t source;
    uint8_t destination;
    uint16_t len;   // Lenght of data (bytes)
} BHRO_T_PACKET_HDR;

typedef struct PACK_STRUCT BHRO_S_TLV {
    uint8_t tag;
    uint16_t len;
    uint8_t value[];
} BHRO_T_TLV;

typedef struct PACK_STRUCT BHRO_S_PACKET {
    BHRO_T_PACKET_HDR packet_head;
    uint8_t tlv_data[];
} BHRO_T_PACKET;

typedef struct PACK_STRUCT BHRO_S_CONNECT_REQ {
    uint8_t resultID;
} BHRO_T_CONNECT_REQ;

typedef struct PACK_STRUCT BHRO_S_CONNECT_RSP {
    uint8_t resultID;
} BHRO_T_CONNECT_RSP;

typedef struct PACK_STRUCT BHRO_S_TOPO {
    uint8_t *addr;
    uint8_t *mac;
    uint8_t hop;
    uint8_t port;
} BHRO_T_TOPO;





#endif
