#include "rhub.h"

/*
function name	: get_rhup_port_id
parameters	: fd,port
return		: reserve 4bit | 4bit port | 4bit rhup hop | 4bit rhup port to port
function	: read port
*/
uint16_t get_rhup_port_id(int mpi_fd, uint8_t port){
	uint16_t tmp,tmp1,tmp2;
	uint16_t addr;

	tmp1 = gpmc_mpi_read_device(mpi_fd,0x02,0x0046);
	addr = (port << 12) | 0x0048;
	tmp2 = gpmc_mpi_read_device(mpi_fd,0x02,addr);
	tmp = ((port &0x0f) << 8) | ((tmp1 &0x000f) << 4) | ((tmp2 &0x000f));

	return tmp;
}

/*
function name	: get_rhup_cpri_stat
parameters	: fd,dir,port,cpri_stat
return		: struct rhup_cpri_stat
function	: read rhup cpri status
*/
void get_rhup_cpri_stat(int mpi_fd, uint8_t dir, uint8_t port, struct rhup_cpri_stat* cpri_stat){
	uint16_t tmp;
	uint16_t addr;
	if(dir == UP){
		addr = port << 12;
		tmp = gpmc_mpi_read_device(mpi_fd,0x02,addr | 0x0002);
		cpri_stat->negotiation_fsm_status = (tmp & 0x00ff) >> 4;
		cpri_stat->sync_fsm_status = (tmp & 0xff00) >> 12;
		tmp = gpmc_mpi_read_device(mpi_fd,0x02,addr | 0x0080);
		cpri_stat->negotiation_fsm_cnt = tmp & 0x00ff;
		cpri_stat->sync_fsm_cnt =  (tmp & 0xff00) >> 8;
		cpri_stat->local_los_cnt = gpmc_mpi_read_device(mpi_fd,0x02,addr | 0x007a);
		cpri_stat->local_lof_cnt = gpmc_mpi_read_device(mpi_fd,0x02,addr | 0x0078);
		cpri_stat->remote_los_cnt = gpmc_mpi_read_device(mpi_fd,0x02,addr | 0x007e);
		cpri_stat->remote_lof_cnt = gpmc_mpi_read_device(mpi_fd,0x02,addr | 0x007c);
		
	}else if(dir == DOWN){
		addr = port << 12;
		tmp = gpmc_mpi_read_device(mpi_fd,0x02,addr | 0x0002);
		cpri_stat->negotiation_fsm_status = (tmp & 0x00ff) >> 4;
		cpri_stat->sync_fsm_status = (tmp & 0xff00) >> 12;
		tmp = gpmc_mpi_read_device(mpi_fd,0x02,addr | 0x0102);
		cpri_stat->negotiation_fsm_cnt = tmp & 0x00ff;
		cpri_stat->sync_fsm_cnt =  (tmp & 0xff00) >> 8;
		
		tmp = gpmc_mpi_read_device(mpi_fd,0x02,addr | 0x0100);
		cpri_stat->local_los_cnt = (tmp & 0xf000) >> 12;
		cpri_stat->local_lof_cnt = (tmp & 0x0f00) >> 8;
		cpri_stat->remote_los_cnt = (tmp & 0x00f0) >> 4;
		cpri_stat->remote_lof_cnt = tmp & 0x00f0;
	}
}


/*
function name	: get_rhup_delay
parameters	: fd,dir
return		: struct rhup_data_delay
function	: read delay,need delay*0.813ns
*/
void get_rhup_delay(int mpi_fd, uint8_t dir,struct rhup_data_delay* rhup_delay){
	
	if(dir == UP){
		rhup_delay->delay1 = gpmc_mpi_read_device(mpi_fd,0x01,0x0820);
		rhup_delay->delay2 = gpmc_mpi_read_device(mpi_fd,0x01,0x0821);
		rhup_delay->delay3 = gpmc_mpi_read_device(mpi_fd,0x01,0x0822);
		rhup_delay->delay4 = gpmc_mpi_read_device(mpi_fd,0x01,0x0823);
		rhup_delay->delay5 = gpmc_mpi_read_device(mpi_fd,0x01,0x0824);
		rhup_delay->delay6 = gpmc_mpi_read_device(mpi_fd,0x01,0x0825);
		rhup_delay->delay7 = gpmc_mpi_read_device(mpi_fd,0x01,0x0826);
		rhup_delay->delay8 = gpmc_mpi_read_device(mpi_fd,0x01,0x0827);
		rhup_delay->delay9 = gpmc_mpi_read_device(mpi_fd,0x01,0x0828);
	}else if(dir == DOWN){
		rhup_delay->delay1 = gpmc_mpi_read_device(mpi_fd,0x01,0x0810);
		rhup_delay->delay2 = gpmc_mpi_read_device(mpi_fd,0x01,0x0811);
		rhup_delay->delay3 = gpmc_mpi_read_device(mpi_fd,0x01,0x0812);
		rhup_delay->delay4 = gpmc_mpi_read_device(mpi_fd,0x01,0x0813);
		rhup_delay->delay5 = gpmc_mpi_read_device(mpi_fd,0x01,0x0814);
		rhup_delay->delay6 = gpmc_mpi_read_device(mpi_fd,0x01,0x0815);
		rhup_delay->delay7 = gpmc_mpi_read_device(mpi_fd,0x01,0x0816);
		rhup_delay->delay8 = gpmc_mpi_read_device(mpi_fd,0x01,0x0817);
		rhup_delay->delay9 = gpmc_mpi_read_device(mpi_fd,0x01,0x0818);

	}

}


/*
function name	: get_rhup_port_stat
parameters	: mpi_fd
return		: port status;reserve 4bit | 8bit down port status | 1bit up pot  | 1bit up port
function	: read port status
*/
uint16_t get_rhup_port_stat(int mpi_fd){
	uint16_t stat;

	stat = gpmc_mpi_read_device(mpi_fd,0x01,0x0006);
	return stat;
}
/*
function name	: get_rhup_t14_delay
parameters	: mpi_fd
return		: struct rhup_t14_delay
function	: read rhup t14 delay
*/
void get_rhup_t14_delay(int mpi_fd, struct rhup_t14_delay* t14_delay){
	uint16_t tmp1,tmp2;
	
	tmp1 = gpmc_mpi_read_device(mpi_fd,0x02,0x1040);
	tmp2 = gpmc_mpi_read_device(mpi_fd,0x02,0x1042);
	t14_delay->delay1 = (tmp1 << 16) | tmp2;
	tmp1 = gpmc_mpi_read_device(mpi_fd,0x02,0x8040);
	tmp2 = gpmc_mpi_read_device(mpi_fd,0x02,0x8042);
	t14_delay->delay2 = (tmp1 << 16) | tmp2;
	tmp1 = gpmc_mpi_read_device(mpi_fd,0x02,0x9040);
	tmp2 = gpmc_mpi_read_device(mpi_fd,0x02,0x9042);
	t14_delay->delay3 = (tmp1 << 16) | tmp2;
	tmp1 = gpmc_mpi_read_device(mpi_fd,0x02,0xa040);
	tmp2 = gpmc_mpi_read_device(mpi_fd,0x02,0xa042);
	t14_delay->delay4 = (tmp1 << 16) | tmp2;
	tmp1 = gpmc_mpi_read_device(mpi_fd,0x02,0xb040);
	tmp2 = gpmc_mpi_read_device(mpi_fd,0x02,0xb042);
	t14_delay->delay5 = (tmp1 << 16) | tmp2;
	tmp1 = gpmc_mpi_read_device(mpi_fd,0x02,0xc040);
	tmp2 = gpmc_mpi_read_device(mpi_fd,0x02,0xc042);
	t14_delay->delay6 = (tmp1 << 16) | tmp2;
	tmp1 = gpmc_mpi_read_device(mpi_fd,0x02,0xd040);
	tmp2 = gpmc_mpi_read_device(mpi_fd,0x02,0xd042);
	t14_delay->delay7 = (tmp1 << 16) | tmp2;
	tmp1 = gpmc_mpi_read_device(mpi_fd,0x02,0xe040);
	tmp2 = gpmc_mpi_read_device(mpi_fd,0x02,0xe042);
	t14_delay->delay8 = (tmp1 << 16) | tmp2;
	tmp1 = gpmc_mpi_read_device(mpi_fd,0x02,0xf040);
	tmp2 = gpmc_mpi_read_device(mpi_fd,0x02,0xf042);
	t14_delay->delay9 = (tmp1 << 16) | tmp2;
}












