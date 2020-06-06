#ifndef __BPOAM_MESSAGE_H
#define __BPOAM_MESSAGE_H

#define PACK_STRUCT __attribute__((packed))

typedef enum
{
    RESERVED0= 0,

	PA_TO_SC_OAM_REQ_CH = 41,
	SC_TO_PA_OAM_RSP_CH,
	SC_TO_PA_OAM_RPT_CH,
	PA_TO_SC_CPRI_CM_REQ_CH,
	SC_TO_PA_CPRI_CM_RSP_CH,

	MAX_IPC_SC_PA_CH_NUM
}ipcChannelNum;

typedef enum
{
	MSG_OAM_CABLE_DELAY_MEA_REQ		= 0x010F,
	MSG_OAM_CABLE_DELAY_MEA_RSP		= 0x020F,
} BPOAM_MSG_TYPE;

typedef struct MSG_S_HEAD {
	uint16_t	MessageID;
	uint16_t	SequenceNumber;
	uint32_t	MessageBodyLength;
} MSG_T_HEAD;

typedef struct PACK_STRUCT BPOAM_S_DELAY_T14_REQ {
	MSG_T_HEAD msg_head;
} BPOAM_T_DELAY_T14_REQ;

typedef struct PACK_STRUCT BPOAM_S_DELAY_T14_RSP {
	MSG_T_HEAD msg_head;
	uint32_t 	T14;
} BPOAM_T_DELAY_T14_RSP;

typedef struct FUN_S_PARA {
	uint32_t carrierIdx;
	uint32_t t14;
} FUN_T_PARA;

#endif // __BPOAM_MESSAGE_H

