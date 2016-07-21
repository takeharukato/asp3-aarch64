/* -*- mode: asm; coding:utf-8 -*- */
/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2006-2016 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 * 
 *  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 * 
 *  $Id$
 */

/*
 *		カーネルの割り込みベクタ関連処理
 */
#define TOPPERS_MACRO_ONLY
#define TOPPERS_ASM_MACRO
#define UINT_C(val)		(val)
#define ULONG_C(val)		(val)
#define CAST(type, val)		(val)
#include "kernel_impl.h"
#include "offset.h"
#include "core_asm.inc"

	ATEXT
	
/*
 * 割込み/例外コンテキスト保存処理
 */
.macro __build_trapframe
	msr	daifset, #3  /* 割込禁止  */
	/*
	 * x1-x30をペアで保存(x29,x30ペアからx1, x2まで)
	 */
	stp	x29, x30, [sp, #-16]!
	stp	x27, x28, [sp, #-16]!
	stp	x25, x26, [sp, #-16]!
	stp	x23, x24, [sp, #-16]!
	stp	x21, x22, [sp, #-16]!
	stp	x19, x20, [sp, #-16]!
	stp	x17, x18, [sp, #-16]!
	stp	x15, x16, [sp, #-16]!
	stp	x13, x14, [sp, #-16]!
	stp	x11, x12, [sp, #-16]!
	stp	x9, x10, [sp, #-16]!
	stp	x7, x8, [sp, #-16]!
	stp	x5, x6, [sp, #-16]!
	stp	x3, x4, [sp, #-16]!
	stp	x1, x2, [sp, #-16]!

	/*
	 * spsr_el1, x0をペアで保存
	 */
	mrs	x21, spsr_el1	
	stp	x21, x0, [sp, #-16]!	

	/*
	 * sp_el0領域(データは後で保存), elr_el1をペアで保存
	 */
	mov	x21, #0 /* SP_EL0用ダミー  */
	mrs	x22, elr_el1
	stp	x21, x22, [sp, #-16]!

	/*
	 * nest_count, esr_el1をペアで保存
	 */
	ldr	x21, =excpt_nest_count	/* 例外ネストカウント     */
	ldr	x21, [x21]
	mrs	x22, esr_el1
	stp	x21, x22, [sp, #-16]!
	/*
	 * 割込み関連領域を確保
	 */
	sub	sp, sp, #T_EXCINF_exnc
.endm

/*
 * 復帰先スタックポインタ保存(EL0での例外/割込み)
 */
.macro __save_return_sp_el0
	mrs	x21, sp_el0	/*  ユーザランドのSP  */
	str	x21, [sp, #T_EXCINF_sp_el0]  /* コンテキストに保存  */
.endm	

/*
 * 復帰先スタックポインタ保存(EL1での例外/割込み)
 */
.macro __save_return_sp_el1
	mov 	x21, sp
	add	x21, x21, #(T_EXCINF_r30 + 8 ) /*  割込み時SP算出  */
	str	x21, [sp, #T_EXCINF_sp_el0]  /* コンテキストに保存  */
.endm	
	
/*
 * 復帰先スタックポインタ復元(EL0での例外/割込み)
 */
.macro __restore_return_sp_el0
	ldr	x21, [sp, #T_EXCINF_sp_el0]  /* コンテキストから復元  */
	msr	sp_el0, x21		     /*  ユーザランドのSP復元  */	
.endm	

/*
 * 復帰先スタックポインタ復元(EL1での例外/割込み)
 */
.macro __restore_return_sp_el1
	ldr	x21, [sp, #T_EXCINF_sp_el0]  /* コンテキストから復元  */	
.endm	

/*
 * 非タスクコンテキスト突入処理
 * コンテキストのアドレスをx0に返却
 */
.macro __enter_non_task_context
	ldr	x22, =excpt_nest_count	   /* 例外ネストカウント読み込み     */
	ldr	x21, [x22]
	add	x21, x21, #1               /*  割込み時の例外ネストカウントインクリメント  */
	str	x21, [x22]
	cmp	x21, #1			   
	mov	x22, sp               /*  タスクスタックの現在値をx22に保存       */
	b.ne	1f		      /*  非タスクコンテキストでの割込みの場合は抜ける */
#ifdef TOPPERS_SUPPORT_OVRHDR
	/*
	 *  オーバランタイマを停止する．
	 */
	bl		ovrtimer_stop
#endif /* TOPPERS_SUPPORT_OVRHDR */
	/*
	 *  非タスクコンテキストスタックに切り替える
	 * (x22の割込みコンテキストアドレスを破壊しないこと)
	 */
	ldr	x20, =_kernel_istkpt  /*  非タスクコンテキストに切り替え          */
	ldr	x20, [x20]
	mov	sp, x20
	movz	x21, #0xbeef           /*  デバッグ用マジック番号(下位)           */
	movk	x21, #0xdead,lsl#16    /*  デバッグ用マジック番号(上位)           */
	stp	x21, x22, [sp, #-16]!  /* マジック番号とタスクスタック現在値保存  */
1:
	mov	x0, x22                /* 第1引数にコンテキストを保存             */
.endm

/*
 * Cの例外ハンドラ部に移行
 * x0にコンテキストのアドレスを保存されていること
 */
.macro __callL_hdlr NO, HDLR
	mov	x1, #\NO
	bl	\HDLR
.endm
	

/*
 * 割込み/例外共通出口処理部(タスクコンテキスト復元処理)
 */
.macro __leave_non_task_context
	msr	daifset, #3  /* 割込禁止  */
	/*
	 *  excpt_nest_countをロードし, デクリメントしたものをexcpt_nest_countに格納
	 */
	ldr	x22, =excpt_nest_count	   /* 例外ネストカウント読み込み     */
	ldr	x21, [x22]
	sub	x21, x21, #1               /*  割込み時の例外ネストカウントデクリメント  */
	str	x21, [x22]
	cmp	x21, #0			   
	b.ne	1f			   /*  非タスクコンテキストでの割込みの場合は抜ける */
	ldp	x21, x22, [sp], #16        /* マジック番号とタスクスタック値復元  */
	mov	sp, x22			   /* タスクスタック復元  */
#ifdef TOPPERS_SUPPORT_OVRHDR
	/*
	 *  オーバランタイマを動作開始する．
	 */
	bl		ovrtimer_start
#endif /* TOPPERS_SUPPORT_OVRHDR */
1:
	/*
	 * 割込みマスクを戻す
	 */
	mov	x0, sp
	bl	irc_end
	/*
	 *  タスクスタック下での例外出口処理
	 */
	mov	x0, sp
	bl	handle_post_exception
.endm
/*
 * 例外/割込み復帰処理
 */
.macro __return_from_exception
	/*
	 * 割込みコンテキスト中のデータを破棄
	 */
	add	sp, sp, #T_EXCINF_elr
	/*
	 * ELR/SPSRを復元
	 */
	ldp	x21, x22, [sp], #16	
	msr	elr_el1, x21		
	msr	spsr_el1, x22		
	/*
	 * x0からx30を復元
	 */
	ldr	x0, [sp], #8
	ldp	x1, x2, [sp], #16
	ldp	x3, x4, [sp], #16
	ldp	x5, x6, [sp], #16
	ldp	x7, x8, [sp], #16
	ldp	x9, x10, [sp], #16
	ldp	x11, x12, [sp], #16
	ldp	x13, x14, [sp], #16
	ldp	x15, x16, [sp], #16
	ldp	x17, x18, [sp], #16
	ldp	x19, x20, [sp], #16
	ldp	x21, x22, [sp], #16
	ldp	x23, x24, [sp], #16
	ldp	x25, x26, [sp], #16
	ldp	x27, x28, [sp], #16
	ldp	x29, x30, [sp], #16
	eret
.endm

/*
 * EL0からの例外入口処理
 */
.macro __exc_entry_el0
	__build_trapframe
	mov	x0, sp
	bl	irc_begin_exc
	__save_return_sp_el0
	__enter_non_task_context
.endm

/*
 * EL1からの例外入口処理
 */
.macro __exc_entry_el1
	__build_trapframe
	mov	x0, sp	
	bl	irc_begin_exc
	__save_return_sp_el1
	__enter_non_task_context
.endm

/*
 * EL0からの割込み入口処理
 */
.macro __int_entry_el0
	__build_trapframe
	mov	x0, sp
	bl irc_begin_int
	__save_return_sp_el0
	__enter_non_task_context
.endm

/*
 * EL1からの割込み入口処理
 */
.macro __int_entry_el1
	__build_trapframe
	mov	x0, sp	
	bl 	irc_begin_int
	__save_return_sp_el1
	__enter_non_task_context
.endm

	

/*
 * EL0からの割込み/例外出口処理
 */
.macro __exc_return_el0
	__leave_non_task_context
	__restore_return_sp_el0
	__return_from_exception
.endm

/*
 * EL1からの例外出口処理
 */
.macro __exc_return_el1
	__leave_non_task_context
	__restore_return_sp_el1
	__return_from_exception
.endm
	
	
	AGLOBAL(vectors)
/** 
 * ARM Cortex-A Series Programmer Guide for ARMv8-A
 * 10.4. AArch64 exception table
 */
	AALIGN(11)  /* align to 4096 byte (1<<11 = 0x1000)  */
ALABEL(vectors)
	/*
	 * From EL1 with SP0
	 */
	b	_sp0_el1_sync   /* Sync(Error) */
	AALIGN(7)  /* align to 128 byte( 1<<7 = 0x80)  */
	b	_sp0_el1_irq    /* IRQ(Error)  */
	AALIGN(7)  /* align to 128 byte( 1<<7 = 0x80)  */
	b	_sp0_el1_fiq    /* FIQ(Error)  */
	AALIGN(7)  /* align to 128 byte( 1<<7 = 0x80)  */
	b	_sp0_el1_error  /* Error(Error)  */

	/*
	 * From EL1 with SP1 ( kernel Trap )
	 */
	AALIGN(7)  /* align to 128 byte( 1<<7 = 0x80)  */
	b	_sp1_el1_sync	/* Sync EL1(kernel) */
	AALIGN(7)  /* align to 128 byte( 1<<7 = 0x80)  */
	b	_sp1_el1_irq	/* IRQ EL1(kernel) */
	AALIGN(7)  /* align to 128 byte( 1<<7 = 0x80)  */
	b	_sp1_el1_fiq	/* FIQ EL1(kernel) */
	AALIGN(7)  /* align to 128 byte( 1<<7 = 0x80)  */
	b	_sp1_el1_error	/* Error EL1(kernel) */

	/*
	 * From EL0 using AArch64 SP0
	 */
	AALIGN(7)  /* align to 128 byte( 1<<7 = 0x80)  */
	b	_sp0_el0_sync	/* Sync EL0 */
	AALIGN(7)  /* align to 128 byte( 1<<7 = 0x80)  */
	b	_sp0_el0_irq	/* IRQ EL0 */
	AALIGN(7)  /* align to 128 byte( 1<<7 = 0x80)  */
	b	_sp0_el0_fiq	/* FIQ EL0 */
	AALIGN(7)  /* align to 128 byte( 1<<7 = 0x80)  */
	b	_sp0_el0_error	/* Error EL0 */

	/*
	 * From  EL0 using AArch32 
	 */
	AALIGN(7)  /* align to 128 byte( 1<<7 = 0x80)  */
	b	_aarch32_sync  /* AArch32 sync */
	AALIGN(7)  /* align to 128 byte( 1<<7 = 0x80)  */
	b	_aarch32_irq  /* AArch32 IRQ  */
	AALIGN(7)  /* align to 128 byte( 1<<7 = 0x80)  */
	b	_aarch32_fiq  /* AArch32 FIQ  */
	AALIGN(7)  /* align to 128 byte( 1<<7 = 0x80)  */
	b	_aarch32_error /* AArch32 Error */

	AALIGN(4)  /* align to 16 byte( 1<<4 = 0x10)  */
_sp0_el1_sync:
	__exc_entry_el1
	__callL_hdlr	0, bad_exception_handler
	__exc_return_el1

	AALIGN(4)  /* align to 16 byte( 1<<4 = 0x10)  */	
_sp0_el1_irq:
	__exc_entry_el1
	__callL_hdlr	1, bad_exception_handler
	__exc_return_el1

	AALIGN(4)  /* align to 16 byte( 1<<4 = 0x10)  */	
_sp0_el1_fiq:
	__exc_entry_el1
	__callL_hdlr	2, bad_exception_handler
	__exc_return_el1

	AALIGN(4)  /* align to 16 byte( 1<<4 = 0x10)  */	
_sp0_el1_error:
	__exc_entry_el1
	__callL_hdlr	3, bad_exception_handler
	__exc_return_el1

	AALIGN(4)  /* align to 16 byte( 1<<4 = 0x10)  */	
_sp1_el1_sync:
	__exc_entry_el1
	__callL_hdlr	4, el1_sync_handler
	__exc_return_el1

	AALIGN(4)  /* align to 16 byte( 1<<4 = 0x10)  */
_sp1_el1_irq:
	__int_entry_el1
	__callL_hdlr	5, el1_irq_handler
	__exc_return_el1

	AALIGN(4)  /* align to 16 byte( 1<<4 = 0x10)  */
_sp1_el1_fiq:
	__int_entry_el1
	__callL_hdlr	6, el1_fiq_handler
	__exc_return_el1

	AALIGN(4)  /* align to 16 byte( 1<<4 = 0x10)  */
_sp1_el1_error:
	__exc_entry_el1
	__callL_hdlr	7, el1_error_handler
	__exc_return_el1

	AALIGN(4)  /* align to 16 byte( 1<<4 = 0x10)  */
_sp0_el0_sync:
	__exc_entry_el0
	__callL_hdlr	8, el0_sync_handler
	__exc_return_el0

	AALIGN(4)  /* align to 16 byte( 1<<4 = 0x10)  */
_sp0_el0_irq:
	__int_entry_el0
	__callL_hdlr	9, el0_irq_handler
	__exc_return_el0


	AALIGN(4)  /* align to 16 byte( 1<<4 = 0x10)  */
_sp0_el0_fiq:
	__int_entry_el0
	__callL_hdlr	10, el0_fiq_handler
	__exc_return_el0

	AALIGN(4)  /* align to 16 byte( 1<<4 = 0x10)  */
_sp0_el0_error:
	__int_entry_el0
	__callL_hdlr	11, el0_error_handler
	__exc_return_el0


	AALIGN(4)  /* align to 16 byte( 1<<4 = 0x10)  */
_aarch32_sync:
	__exc_entry_el0
	__callL_hdlr	12, aarch32_sync_handler
	__exc_return_el0

	AALIGN(4)  /* align to 16 byte( 1<<4 = 0x10)  */
_aarch32_irq:
	__int_entry_el0
	__callL_hdlr	13, aarch32_irq_handler
	__exc_return_el0

	AALIGN(4)  /* align to 16 byte( 1<<4 = 0x10)  */	
_aarch32_fiq:
	__int_entry_el0
	__callL_hdlr	14, aarch32_fiq_handler
	__exc_return_el0

	AALIGN(4)  /* align to 16 byte( 1<<4 = 0x10)  */
_aarch32_error:
	__exc_entry_el0
	__callL_hdlr	15, aarch32_error_handler
	__exc_return_el0


	AALIGN(4)  /* align to 16 byte( 1<<4 = 0x10)  */
	AGLOBAL(init_vectors)	
ALABEL(init_vectors)
	ldr x0, =vectors
	msr vbar_el1, x0
	ret
