
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

*   虽然orange's书上后面章节不再使用typedef这样的用法（个人也认为不用比较好），但
* 鉴于此处实现ipc并不想更改之前代码也没打算加入输入输出所以没有再进行修改。
																										Ayle, May, 2017
*********************************************************************/
typedef struct stackframe {	/* proc_ptr points here				↑ Low			*/
	u32	gs;		/* ┓						│			*/
	u32	fs;		/* ┃						│			*/
	u32	es;		/* ┃						│			*/
	u32	ds;		/* ┃						│			*/
	u32	edi;		/* ┃						│			*/
	u32	esi;		/* ┣ pushed by save()				│			*/
	u32	ebp;		/* ┃						│			*/
	u32	kernel_esp;	/* <- 'popad' will ignore it			│			*/
	u32	ebx;		/* ┃						↑栈从高地址往低地址增长*/
	u32	edx;		/* ┃						│			*/
	u32	ecx;		/* ┃						│			*/
	u32	eax;		/* ┛						│			*/
	u32	retaddr;	/* return address for assembly code save()	│			*/
	u32	eip;		/*  ┓						│			*/
	u32	cs;		/*  ┃						│			*/
	u32	eflags;		/*  ┣ these are pushed by CPU during interrupt	│			*/
	u32	esp;		/*  ┃						│			*/
	u32	ss;		/*  ┛						┷High			*/
}STACK_FRAME;

typedef struct proc {
	STACK_FRAME regs;    /* process registers saved in stack frame */

	u16 ldt_sel;               /* gdt selector giving ldt base and limit */
	DESCRIPTOR ldts[LDT_SIZE]; /* local descs for code and data */

  int ticks;                 /* remained ticks */
  int priority;

	u32 pid;                   /* process id passed in from MM */
	char name[16];		   			/* name of the process */

	int  p_flags;              /**
				    * process flags.
				    * A proc is runnable if p_flags==0
				    */

	int has_int_msg;           /**
				    * nonzero if an INTERRUPT occurred when
				    * the task is not ready to deal with it.
				    */

	MESSAGE* msg_head;   /**
				    * the head of the MESSAGE queue
				    */
	int nr_recvmsg;/**
				    * the number of RECEIVE MESSAGE
				    */

} PROCESS;

typedef struct s_task {
	task_f	initial_eip;
	int			stacksize;
	char		name[32];
} TASK;

#define proc2pid(x) (x - proc_table)

/* Number of tasks */
#define NR_TASKS	2
#define NR_PROCS	4
#define FIRST_PROC	proc_table[0]
#define LAST_PROC	proc_table[NR_TASKS + NR_PROCS - 1]


/* stacks of tasks */
#define STACK_SIZE_sendmsg		0x8000
#define STACK_SIZE_recvmsg		0x8000
#define STACK_SIZE_TESTA	0x8000
#define STACK_SIZE_TESTB	0x8000
#define STACK_SIZE_TESTC	0x8000
#define STACK_SIZE_TESTD	0x8000

#define STACK_SIZE_TOTAL	(STACK_SIZE_sendmsg + \
				STACK_SIZE_recvmsg + \
				STACK_SIZE_TESTA + \
				STACK_SIZE_TESTB + \
				STACK_SIZE_TESTC+ \
				STACK_SIZE_TESTD)
