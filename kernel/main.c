
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
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
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main()
{
	disp_str("-----\"kernel_main\" begins-----\n");

	TASK*		p_task		= task_table;
	PROCESS*	p_proc		= proc_table;
	char*		p_task_stack	= task_stack + STACK_SIZE_TOTAL;
	u16		selector_ldt	= SELECTOR_LDT_FIRST;
	u8    privilege;
	u8    rpl;
	int   eflags;
	int   i;
	int   prio;

	for (i = 0; i < NR_TASKS+NR_PROCS; i++) {
	        if (i < NR_TASKS) {     /* 任务 */
                        p_task    = task_table + i;
                        privilege = PRIVILEGE_TASK;
                        rpl       = RPL_TASK;
                        eflags    = 0x1202; /* IF=1, IOPL=1, bit 2 is always 1 */
			prio      = 15;
                }
                else {                  /* 用户进程 */
                        p_task    = user_proc_table + (i - NR_TASKS);
                        privilege = PRIVILEGE_USER;
                        rpl       = RPL_USER;
                        eflags    = 0x202; /* IF=1, bit 2 is always 1 */
			prio      = 5;
                }

		strcpy(p_proc->name, p_task->name);	/* name of the process */
		p_proc->pid = i;			/* pid */

		p_proc->ldt_sel = selector_ldt;

		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | privilege << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;
		p_proc->regs.cs	= (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ds	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.es	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.fs	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ss	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;
		p_proc->regs.eflags = eflags;

		//strcpy(msg.content, "Good News!");
		//msg.RETVAL = 0x0;
		//p_proc->msg_head = &msg;
		//disp_str(proc_table[i].msg_head->content);
		p_proc->p_flags = 0;
		p_proc->msg_head = 0;
		p_proc->has_int_msg = 0;
		p_proc->nr_recvmsg = 0;

		p_proc->ticks = p_proc->priority = prio;

		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}
	//proc_table[0].ticks = proc_table[0].priority = 15;

	k_reenter = 0;
	ticks = 0;

	p_proc_ready	= proc_table;

        /* 初始化 8253 PIT */
        out_byte(TIMER_MODE, RATE_GENERATOR);
        out_byte(TIMER0, (u8) (TIMER_FREQ/HZ) );
        out_byte(TIMER0, (u8) ((TIMER_FREQ/HZ) >> 8));

        put_irq_handler(CLOCK_IRQ, clock_handler); /* 设定时钟中断处理程序 */
        enable_irq(CLOCK_IRQ);                     /* 让8259A可以接收时钟中断 */
	p_proc		= proc_table;
	//disp_str(p_proc->msg_head->content);
	restart();
	while(1){}
}

/*======================================================================*
                               TestA
 *======================================================================*/
int TestA()
{
	int i = 0;
	int ret = 0;
	for ( i = 0; i < 100; i++){
		//disp_str("A.");
		MESSAGE msg;
		reset_msg(&msg);
		msg.type = GET_TICKS;
		msg.RETVAL = i;
		ret = sendmsg(1, 2, &msg);
		if (ret==0){
			disp_str("send succ. ");
		}
		else {
			disp_str("send error:");disp_int(ret);
		}
		milli_delay(20);
	}
	return 0;
}

/*======================================================================*
                               TestB
 *======================================================================*/
int TestB()
{
	int i = 0x1000;
	int ret = 0;
	MESSAGE msgstack;

	while(1){
		MESSAGE msg;
		//disp_str("B.");
		msg.RETVAL = 0;
		//disp_str(proc_table[0].msg_head->content);
		ret = recvmsg(0, 1, &msg);
		//disp_str("Ticks:");
		//disp_int(ret);
		if (ret==0){
			disp_str("recv succ:");//disp_str(msg.content);
			disp_int(msg.RETVAL);
		}
		else {
			disp_str("recv error:"); //disp_int(ret);
			if ( ret == -1 )
				disp_str("No message!");
		}
		milli_delay(10);
	}
	return 0;
}

/*======================================================================*
                               TestC
 *======================================================================*/
int TestC()
{
	int i = 0x2000;
	while(1){
		//disp_str("C.");
		milli_delay(10);
	}
	return 0;
}

/*======================================================================*
                               TestD
 *======================================================================*/
int TestD()
{
	int i = 0x2000;
	while(1){
		//disp_str("DCY amazing! ");
		milli_delay(10);
	}
	return 0;
}
