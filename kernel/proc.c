
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "global.h"
#include "proto.h"

/*======================================================================*
                              schedule
 *======================================================================*/
PUBLIC void schedule()
{
	PROCESS* p;
	int	 greatest_ticks = 0;

	while (!greatest_ticks) {
		for (p = &FIRST_PROC; p < &LAST_PROC; p++) {
			if (p->p_flags == 0 && p->ticks > greatest_ticks) {
				greatest_ticks = p->ticks;
				p_proc_ready = p;
			}
		}

		if (!greatest_ticks) {
			for (p = &FIRST_PROC; p < &LAST_PROC; p++) {
				if (p->p_flags == 0)
					p->ticks = p->priority;
			}
		}
	}
}

/*****************************************************************************
 *				  ldt_seg_linear
 *****************************************************************************/
/**
 * <Ring 0~1> Calculate the linear address of a certain segment of a given
 * proc.
 *
 * @param p   Whose (the proc ptr).
 * @param idx Which (one proc has more than one segments).
 *
 * @return  The required linear address.
 *****************************************************************************/
PUBLIC int ldt_seg_linear(PROCESS* p, int idx)
{
	DESCRIPTOR* d = &p->ldts[idx];

	return d->base_high << 24 | d->base_mid << 16 | d->base_low;
}

/*****************************************************************************
 *				  va2la
 *****************************************************************************/
/**
 * <Ring 0~1> Virtual addr --> Linear addr.
 *
 * @param pid  PID of the proc whose address is to be calculated.
 * @param va   Virtual address.
 *
 * @return The linear address for the given virtual address.
 *****************************************************************************/
PUBLIC void* va2la(int pid, void* va)
{
	PROCESS* p = &proc_table[pid];

	u32 seg_base = ldt_seg_linear(p, INDEX_LDT_RW);
	u32 la = seg_base + (u32)va;

	return (void*)la;
}

/*****************************************************************************
 *                                reset_msg
 *****************************************************************************/
/**
 * <Ring 0~3> Clear up a MESSAGE by setting each byte to 0.
 *
 * @param p  The message to be cleared.
 *****************************************************************************/
PUBLIC void reset_msg(MESSAGE* p)
{
	memset(p, 0, sizeof(MESSAGE));
}

/*======================================================================*
                           sys_get_ticks
 *======================================================================*/
PUBLIC int sys_get_ticks()
{
	return ticks;
}

PRIVATE int deadlock(int src, int dest)
{/*
	struct proc* p = proc_table + dest;

	while (1) {
		if (p->p_flags & SENDING) {
			if (p->p_sendto == src) {
				p = proc_table + dest;
				do {
					p = proc_table + p->p_sendto;
				} while (p != proc_table + src);

				return 1;
			}
			//p = proc_table + p->p_sendto;
		}
		else {
			break;
		}
	}
	*/
	return 0;
}


/*****************************************************************************
 *                                sys_msg_send
 *****************************************************************************/
/**
 * <Ring 0> Send a message to the dest proc mail box.
 * @param current  The caller, the sender.
 * @param dest     To whom the message is sent.
 * @param m        The message.
 *
 * @return Zero if success.
 *****************************************************************************/
PUBLIC int sys_msg_send(int pid, int dest, MESSAGE* m, PROCESS* current)
{
	PROCESS* sender = current;
	PROCESS* p_dest = proc_table + dest; /* proc dest */

//	if ((p_dest->p_flags & RECEIVING) && p_dest->nr_recvmsg < MAX_MSG) {
	if ( p_dest->nr_recvmsg < MAX_MSG) {
		 p_dest->p_flags = RECEIVING;
		 int tail = (p_dest->msg_head + p_dest->nr_recvmsg)%MAX_MSG;
		 MESSAGE* msg_tail = &p_dest->msg_queue[tail];
		//MESSAGE* msg_tail = p_dest->msg_head;
		//while (msg_tail->next_msg)
		//	msg_tail = msg_tail->next_msg;
		//msg_tail = m;
 		//msg_tail->next_msg = 0;
		phys_copy(va2la(dest, msg_tail),
			  va2la(proc2pid(sender), m),
			  sizeof(MESSAGE));
		p_dest->nr_recvmsg++;
		p_dest->p_flags &= ~RECEIVING; /* dest has received the msg */
	}
	else { /* dest is not waiting for the msg */
		p_dest->p_flags =SENDING;
		return 1;
	}

	return 0;
}


/*****************************************************************************
 *                                sys_msg_receive
 *****************************************************************************/
/**
 * <Ring 0> Try to get a message from the mail box.
 *
 * @param current The caller, the proc who wanna receive.
 * @param src     From whom the message will be received.
 * @param m       The message ptr to accept the message.
 *
 * @return  Zero if success.
 *****************************************************************************/
PUBLIC int sys_msg_receive(int src, int dest, MESSAGE* m, PROCESS* current)
{
	PROCESS* p_recv = current;

	if ((p_recv->has_int_msg) &&
	    ((src == ANY) || (src == INTERRUPT))) {
		/* There is an interrupt needs p_who_wanna_recv's handling and
		 * p_who_wanna_recv is ready to handle it.
		 */
		MESSAGE msg;
		reset_msg(&msg);
		msg.source = INTERRUPT;
		msg.type = HARD_INT;
		phys_copy(va2la(proc2pid(p_recv), m), &msg,
			  sizeof(MESSAGE));
		p_recv->has_int_msg = 0;

		return 0;
	}

	if ( (p_recv->nr_recvmsg >0) &&
				(src == ANY || src >= 0 && src < NR_TASKS + NR_PROCS) ) {
			p_recv->p_flags = RECEIVING;
		 /* copy the message */
		 //phys_copy(va2la(proc2pid(p_recv), m),
		//		va2la(proc2pid(p_recv), p_recv->msg_head),
		//  	sizeof(MESSAGE));
		 //strcpy(m->content, "Good News!");
		MESSAGE* msg_head = &p_recv->msg_queue[p_recv->msg_head];
 		phys_copy(va2la(proc2pid(current), m),
 			  va2la(proc2pid(current), msg_head),
 			  sizeof(MESSAGE));
		 p_recv->nr_recvmsg--;
		 //p_recv->msg_head =  p_recv->msg_head->next_msg;
		 p_recv->msg_head = (p_recv->msg_head+1)%MAX_MSG;
	}
	else {  /* nobody's sending any msg */
		p_recv->p_flags |= RECEIVING;
		return -1;
	}

	return 0;
}
