/*************************************************************************//**
 *****************************************************************************
 * @file   systask.c
 * @brief
 * @author Forrest Y. Yu
 * @date   2007
 *****************************************************************************
 *****************************************************************************/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "global.h"
#include "proto.h"


/*****************************************************************************
 *                                task_sys
 *****************************************************************************/
/**
 * <Ring 1> The main loop of TASK dispatch_msg.
 *
 *****************************************************************************/
PUBLIC int task_dispatch_msg()
{
	MESSAGE msg;
	msg.RETVAL = 0;
	while (1){

	PROCESS* p;
	for (p = &FIRST_PROC; p < &LAST_PROC; p++) {
		if (p->p_flags == RECEIVING) {
				//recvmsg(p->pid, ANY, &msg);
		}
		else if (p->p_flags == SENDING) {
				//sendmsg(p->pid, ANY, &msg);
				}
		}
	}
}
