
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            proto.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* klib.asm */
PUBLIC void	out_byte(u16 port, u8 value);
PUBLIC u8	in_byte(u16 port);
PUBLIC void	disp_str(char * info);
PUBLIC void	disp_color_str(char * info, int color);
PUBLIC void	disable_irq(int irq);
PUBLIC void	enable_irq(int irq);

/* string.asm */
PUBLIC char*	strcpy(char* dst, const char* src);

/* protect.c */
PUBLIC void	init_prot();
PUBLIC u32	seg2phys(u16 seg);

/* klib.c */
PUBLIC void	delay(int time);
PUBLIC void	disp_int(int input);

/* kernel.asm */
void restart();

/* main.c */
PUBLIC int  get_ticks();
PUBLIC int TestA();
PUBLIC int TestB();
PUBLIC int TestC();
PUBLIC int TestD();

/* i8259.c */
PUBLIC void init_8259A();
PUBLIC void put_irq_handler(int irq, irq_handler handler);
PUBLIC void spurious_irq(int irq);

/* clock.c */
PUBLIC void clock_handler(int irq);
PUBLIC void milli_delay(int milli_sec);

/* proc.c */
PUBLIC  int   sys_get_ticks();        /* sys_call */
PUBLIC	void	schedule();
PUBLIC	void*	va2la(int pid, void* va);
PUBLIC	int	  ldt_seg_linear(PROCESS* p, int idx);
PUBLIC	void	reset_msg(MESSAGE* p);
PUBLIC  int   sys_get_ticks();
PUBLIC  int   sys_msg_send(int pid, int dest, MESSAGE* m, PROCESS* current);
PUBLIC  int   sys_msg_receive(int pid, int dest, MESSAGE* m, PROCESS* current);

/* proc.c */
PUBLIC int task_dispatch_msg();

/* syscall.asm */
PUBLIC  void   sys_call();             /* int_handler */
PUBLIC  int    get_ticks();
/* 系统调用 - 用户级 */
//PUBLIC	int	sendrec(int function, int src_dest, MESSAGE* p_msg);
PUBLIC	int	sendmsg(int pid, int src_dest, MESSAGE* msg);
PUBLIC	int	recvmsg(int pid, int src_dest, MESSAGE* msg);
