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
 *		カーネルのディスパッチ関連処理
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
	AALIGN(4)
/*
 *  現在のコンテキストを捨ててディスパッチ
 */
	AGLOBAL(exit_and_dispatch)
ALABEL(exit_and_dispatch)
	/*
	 *  このルーチンは，タスクコンテキスト・CPUロック状態・割込み優先度
	 *  マスク全解除状態・ディスパッチ許可状態で呼び出される．
	 */
#ifdef LOG_DSP_ENTER
	ldr		x0, =p_runtsk			/* p_runtsk → x0 */
	ldr		x0, [x0]
#endif /* LOG_DSP_ENTER */
	
/*
 *  ディスパッチャ本体
 */
	AGLOBAL(dispatcher)
ALABEL(dispatcher)
#ifdef LOG_DSP_ENTER
	/*
	 * 【この時点のレジスタ状態】
	 *  x0：p_runtsk（タスク切換え前）
	 */
	bl		log_dsp_enter
#endif /* LOG_DSP_ENTER */

	AGLOBAL(dispatcher_0)
ALABEL(dispatcher_0)
	/*
	 *  このルーチンは，タスクコンテキスト・CPUロック状態・割込み優先度
	 *  マスク全解除状態・ディスパッチ許可状態で呼び出される．実行再開番
	 *  地へもこの状態のまま分岐する．
	 */
	ldr		x0, =p_schedtsk
	ldr		x4, [x0]
	ldr		x1, =p_runtsk
	str		x4, [x1]
	tst		x4, x4			/* p_runtskがNULLならdispatcher_1へ */
	beq		dispatcher_1
	ldr		x2, [x4,#TCB_sp]        /* タスクスタックを復帰 */
	mov		sp, x2
#ifdef LOG_DSP_LEAVE
	mov		x0, x4			/* p_runtskをパラメータに渡す */
	bl		log_dsp_leave
#endif /* LOG_DSP_LEAVE */
	ldr		x0, [x4,#TCB_pc]	/* 実行再開番地を復帰 */
	br		x0			/* p_runtskをx4に入れた状態で実行再開 */

	/*
	 *  アイドル処理
	 *
	 *  割込みをすべて許可し，CPUロック解除状態にして割込みを待つ．
	 *
	 *  ターゲットによっては，省電力モード等に移行するため，標準の方法
	 *  と異なる手順が必要な場合がある．そのようなターゲットでは，ター
	 *  ゲット依存部でTOPPERS_CUSTOM_IDLEを定義し，アセンブラマクロとし
	 *  て，toppers_asm_custom_idleを用意すればよい．
	 */
ALABEL(dispatcher_1)
#ifdef TOPPERS_CUSTOM_IDLE
	toppers_asm_custom_idle
#else /* TOPPERS_CUSTOM_IDLE */
	msr 		daifclr, #3  	/* 割込みを許可*/
ALABEL(dispatcher_2)
	b		dispatcher_2	/* 割込み待ち */
#endif /* TOPPERS_CUSTOM_IDLE */
	

/*
 *  ディスパッチャ
 */
	ATEXT
	AALIGN(4)
	AGLOBAL(dispatch)
ALABEL(dispatch)
	/*
	 *  このルーチンは，タスクコンテキスト・CPUロック状態・割込み優先度
	 *  マスク全解除状態・ディスパッチ許可状態で呼び出される．
	 */
	stp     x29, x30, [sp, #-16]! 		/* FP(x29), 戻り番地(x30)を保存 */
#ifdef TOPPERS_SUPPORT_OVRHDR
	bl		ovrtimer_stop
#endif /* TOPPERS_SUPPORT_OVRHDR */
        stp     x27, x28, [sp, #-16]!		/* 非スクラッチレジスタの保存 */
        stp     x25, x26, [sp, #-16]!
        stp     x23, x24, [sp, #-16]!
        stp     x21, x22, [sp, #-16]!
        stp     x19, x20, [sp, #-16]!
        stp     x17, x18, [sp, #-16]!
        stp     x15, x16, [sp, #-16]!
        stp     x7, x8, [sp, #-16]!
        stp     x5, x6, [sp, #-16]!
        stp     x3, x4, [sp, #-16]!
        stp     x1, x2, [sp, #-16]!
        str     x0, [sp, #-8]!

	ldr	x0, =p_runtsk			/* p_runtsk → x0 */
	ldr	x0, [x0]
	mov     x19, sp
	str	x19, [x0,#TCB_sp]		/* スタックポインタを保存 */
	adr	x1, dispatch_r
	str	x1, [x0,#TCB_pc]		/* 実行再開番地を保存 */
	b	dispatcher			/* x0にはp_runtskが格納されている */

ALABEL(dispatch_r)
	ldr     x0, [sp], #8 /* 非スクラッチレジスタの復帰 */
	ldp     x1, x2, [sp], #16
	ldp     x3, x4, [sp], #16
	ldp     x5, x6, [sp], #16
	ldp     x7, x8, [sp], #16
	ldp     x15, x16, [sp], #16
	ldp     x17, x18, [sp], #16
	ldp     x19, x20, [sp], #16
	ldp     x21, x22, [sp], #16
	ldp     x23, x24, [sp], #16
	ldp     x25, x26, [sp], #16
	ldp     x27, x28, [sp], #16
#ifdef TOPPERS_SUPPORT_OVRHDR
	bl		ovrtimer_start
#endif /* TOPPERS_SUPPORT_OVRHDR */
	ldp     x29, x30, [sp], #16
	ret
/*
 *  カーネルの終了処理の呼出し
 *
 *  割込みロック状態にし，使用するスタックを非タスクコンテキスト用のスタッ
 *  ク領域に切り替え，exit_kernelを呼び出す．
 */
	AGLOBAL(call_exit_kernel)
ALABEL(call_exit_kernel)
	msr 		daifset, #2  	/* 割込みを許可）*/
	ldr		x2, =istkpt	/* 非タスクコンテキストのスタックへ */
	mov		sp, x2
	b		exit_kernel
	
