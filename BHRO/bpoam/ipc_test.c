/*
 * Copyright 2011-2013 Freescale Semiconductor, Inc.
 *
 * Author: Ashish Kumar <ashish.kumar@freescale.com>
 * Author: Manish Jaggi <manish.jaggi@freescale.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>

#include "bpoam_message.h"

#include "fsl_usmmgr.h"
#include "fsl_bsc913x_ipc.h"
#include "fsl_ipc_errorcodes.h"
#define UIO_INTERFACE   "/dev/uio0"
#define UIO_INTERFACE2   "/dev/uio1"
#define CMD_LINE_OPT "r:i:"

#define ENTER()	printf(">> %s %d %s\n", __FILE__, __LINE__, __func__)
#define EXIT(A)	printf("<< (%d) %s %d %s\n", A, __FILE__, __LINE__, __func__)
#define mute_print(...)

#define NUM_MAX_CARRIER 2

fsl_usmmgr_t usmmgr;
fsl_ipc_t ipc[NUM_MAX_CARRIER];

int chinit;
int rat_id, loop, mute_flag, loop2;

/* Logging Function */
void dump_msg(char *msg, int len)
{
	int i = 0;
	for (i = 0; i < len; i++)
		printf("%x", msg[i]);

	printf("\n");
}

///////////////// Create Channels ////////////////////
int isbitset(uint64_t v, int bit)
{
	if ((v >> (63 - bit)) & 0x1)
		return 1;

	return 0;
}

void *test_p2v(unsigned long phys_addr)
{
	return fsl_usmmgr_p2v(phys_addr, usmmgr);
}

void bpoamDelayT14Req(void *arg)
{
	int ret;
	int ts = 0;
	
	BPOAM_T_DELAY_T14_REQ *T14Req = 
		(BPOAM_T_DELAY_T14_REQ*)malloc(sizeof(BPOAM_T_DELAY_T14_REQ));

	T14Req->msg_head.MessageID = (uint16_t)MSG_OAM_CABLE_DELAY_MEA_REQ;
	T14Req->msg_head.SequenceNumber = 0;
	T14Req->msg_head.MessageBodyLength = 0;
	
	FUN_T_PARA *para;
	para = (FUN_T_PARA*)arg;

	ENTER();
	do {
		ret = fsl_ipc_send_msg(PA_TO_SC_OAM_REQ_CH, T14Req, sizeof(BPOAM_T_DELAY_T14_REQ), ipc[para->carrierIdx]);
		usleep(1000);
		ts++;
		if (ret == ERR_SUCCESS)
			ts = 0;
		else if (ts == 500) {
			/* wait for 0.5 sec*/
			printf("Timed out. Not able to send"
					" message on ch#5\n");
			pthread_exit(0);
		}
	} while (ret == -ERR_CHANNEL_FULL);
	if (ret) {
		printf("Error code = %x\n", ret);
		printf("Exiting Thread for ch5\n");
		goto end;
	}
	
end:
	printf("Exiting Thread for PA_TO_SC_OAM_REQ_CH\n");
	EXIT(ret);
	pthread_exit(0);
}

void bpoamDelayT14Rsp(void *arg)
{
	unsigned long p;
	uint32_t len;
	int ret;
	int tr = 0;
	int depth = 16;
	void *vaddr;
	uint16_t msgid;
	
	FUN_T_PARA *para;
	para = (FUN_T_PARA*)arg;
	
	ENTER();
	ret = fsl_ipc_configure_channel(SC_TO_PA_OAM_RSP_CH, depth, IPC_PTR_CH, 0, 0, NULL, ipc[para->carrierIdx]);
	if (ret) {
		printf("\n ret %x \n", ret);
		EXIT(ret);
		pthread_exit(0);
	}
	chinit = 1;

	do {
		ret = fsl_ipc_recv_ptr(SC_TO_PA_OAM_RSP_CH, &p, &len, ipc);
		usleep(1000);
		tr++;
		if (ret == ERR_SUCCESS)
			tr = 0;
		else if (tr == 500) {
			/* wait for 0.5 sec*/
			printf("Timed out. Message not received"
					" on ch#4\n");
			pthread_exit(0);
		}
	} while (ret == -ERR_CHANNEL_EMPTY);
	if (ret) {
		printf("\n ERROR ret %x \n", ret);
		printf("Exiting Thread for ch4\n");
		goto end;
	}

	vaddr = fsl_usmmgr_p2v(p, usmmgr);
	if (!vaddr) {
		ret = -ERR_NULL_VALUE;
		printf("\n Error in translating physical address %lx"
			" to virtual address\n", p);
		goto end;
	}
	
	msgid = (uint16_t)(((BPOAM_T_DELAY_T14_RSP*)vaddr)->msg_head.MessageID);
	printf("Delay msgid=%d\n", msgid);
	
	para->t14 = (uint32_t)(((BPOAM_T_DELAY_T14_RSP*)vaddr)->T14);
	printf("Delay T14=%d\n", para->t14);
	
end:
	printf("Exiting Thread for SC_TO_PA_OAM_RSP_CH\n");
	EXIT(ret);
	pthread_exit(0);
}

int ipc_init_nxp(int rat_id, FUN_T_PARA *para)
{	
	uint64_t bmask;
	int ret = 0;
	int ret1, ret2;
	pthread_t thread1, thread2;
	mem_range_t sh_ctrl;
	mem_range_t dsp_ccsr;
	mem_range_t pa_ccsr;
	
	uint8_t cIdx = 0;

	printf("\n=========$IPC TEST$====%s %s====\n", __DATE__, __TIME__);

	usmmgr = fsl_usmmgr_init();
	if (!usmmgr) {
		printf("Error in Initializing User Space Memory Manager\n");
		return 1;
	}

	/* get values from mmgr */
	ret = get_pa_ccsr_area(&pa_ccsr, usmmgr);
	if (ret) {
		printf("Error in obtaining PA CCSR Area information\n");
		return 1;
	}

	ret = get_dsp_ccsr_area(&dsp_ccsr, usmmgr);
	if (ret) {
		printf("Error in obtaining DSP CCSR Area information\n");
		return 1;
	}

	ret = get_shared_ctrl_area(&sh_ctrl, usmmgr);
	if (ret) {
		printf("Error in obtaining Shared Control Area information\n");
		return 1;
	}

	ipc[0] = fsl_ipc_init(test_p2v, sh_ctrl, dsp_ccsr, pa_ccsr, UIO_INTERFACE);
	if (!ipc[0]) {
		printf("Issue with fsl_ipc_init ipc[0] %d\n", ret);
		return 1;
	}
	
	ipc[1] = fsl_ipc_init(test_p2v, sh_ctrl, dsp_ccsr, pa_ccsr, UIO_INTERFACE2);
	if (!ipc[1]) {
		printf("Issue with fsl_ipc_init ipc[1] %d\n", ret);
		return 1;
	}
	
	for(cIdx = 0; cIdx < NUM_MAX_CARRIER; cIdx++)
    {
		do {
			fsl_ipc_chk_recv_status(&bmask, ipc[cIdx]);
			usleep(10000);
			if (!bmask)
				printf("\n main loop #ret %llx \n",
					(long long unsigned int)bmask);
		} while (!(isbitset(bmask, 0)));
		
		fsl_ipc_open_prod_ch(PA_TO_SC_OAM_REQ_CH, ipc[cIdx]);
	}
	
	ret1 = pthread_create(&thread1, NULL,
			(void *)&bpoamDelayT14Req, para);
	if (ret1) {
		printf("pthread_create returns with error: %d", ret1);
		printf("from bpoamDelayT14Req\n");
		return 1;
	}

	ret2 = pthread_create(&thread2, NULL,
			(void *)&bpoamDelayT14Rsp, para);
	if (ret2) {
		printf("pthread_create returns with error: %d", ret2);
		printf("from bpoamDelayT14Rsp\n");
		return 1;
	}

	mute_print("ptherad_create %d %d\n", ret1, ret2);

	while (!chinit) {
		mute_print(".");
		usleep(1000);
	}

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	
	printf("arraycomm NXP IPC init ok\n");
	return 0;
}
