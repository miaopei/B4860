#ifndef _RHUP_H_
#define _RHUP_H_
#include "gpmc_mpi.h"

#define UP	0
#define DOWN 1

struct rhup_cpri_stat{
	uint8_t negotiation_fsm_status;
	uint8_t sync_fsm_status;
	uint8_t	negotiation_fsm_cnt;
	uint8_t sync_fsm_cnt;
	uint8_t local_los_cnt;
	uint8_t local_lof_cnt;
	uint8_t remote_los_cnt;
	uint8_t remote_lof_cnt;
};

struct rhup_data_delay{
	uint16_t delay1;
	uint16_t delay2;
	uint16_t delay3;
	uint16_t delay4;
	uint16_t delay5;
	uint16_t delay6;
	uint16_t delay7;
	uint16_t delay8;
	uint16_t delay9;
};

struct rhup_t14_delay{
	uint32_t delay1;
	uint32_t delay2;
	uint32_t delay3;
	uint32_t delay4;
	uint32_t delay5;
	uint32_t delay6;
	uint32_t delay7;
	uint32_t delay8;
	uint32_t delay9;
};

extern uint16_t get_rhup_port_id(int mpi_fd, uint8_t port);
extern void get_rhup_cpri_stat(int mpi_fd, uint8_t dir, uint8_t port, struct rhup_cpri_stat* cpri_stat);
extern void get_rhup_delay(int mpi_fd, uint8_t dir,struct rhup_data_delay* rhup_delay);
extern uint16_t get_rhup_port_stat(int mpi_fd);
extern void get_rhup_t14_delay(int mpi_fd, struct rhup_t14_delay* t14_delay);

#endif
