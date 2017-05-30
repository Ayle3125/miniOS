
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                               syscall.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                                                     Forrest Yu, 2005
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

%include "sconst.inc"

_NR_get_ticks       equ 0 ; 要跟 global.c 中 sys_call_table 的定义相对应！
_NR_sendmsg       	equ 1 ;
_NR_recvmsg       	equ 2 ;
INT_VECTOR_SYS_CALL equ 0x90

; 导出符号
global	get_ticks
global	sendmsg
global	recvmsg

bits 32
[section .text]

; ====================================================================
;                              get_ticks
; ====================================================================
get_ticks:
	mov	eax, _NR_get_ticks
	int	INT_VECTOR_SYS_CALL
	ret

; ====================================================================================
;                  sendmsg(int pid, int src_dest, MESSAGE* msg);
; ====================================================================================
sendmsg:
	mov	eax, _NR_sendmsg
	mov	ebx, [esp + 4]
	mov	ecx, [esp + 8]
	mov	edx, [esp + 12]
	int	INT_VECTOR_SYS_CALL
	ret

; ====================================================================================
;                  recvmsg(int pid, int src_dest, MESSAGE* msg);
; ====================================================================================
recvmsg:
	mov	eax, _NR_recvmsg
	mov	ebx, [esp + 4]
	mov	ecx, [esp + 8]
	mov	edx, [esp + 12]
	int	INT_VECTOR_SYS_CALL
	ret
