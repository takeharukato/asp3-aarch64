/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2013-2016 by Embedded and Real-Time Systems Laboratory
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
 *		ターゲット依存モジュール（ダミーターゲット用）
 */

#include "kernel_impl.h"
#include "task.h"
#ifdef TOPPERS_SUPPORT_OVRHDR
#include "overrun.h"
#endif /* TOPPERS_SUPPORT_OVRHDR */
#include "t_stddef.h"
#include "gic_kernel_impl.h"

uint32_t excpt_nest_count=1U;  /* 例外ネストカウント  */

extern void init_vectors(void);  /*  ベクタ初期化処理  */
extern void initialize_interrupt(void);  /*  割込み初期化処理  */

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_DSP_ENTER
#define LOG_DSP_ENTER(p_tcb)
#endif /* LOG_DSP_ENTER */

#ifndef LOG_DSP_LEAVE
#define LOG_DSP_LEAVE(p_tcb)
#endif /* LOG_DSP_LEAVE */


/*
 *  ディスパッチャの動作開始
 */
void
start_dispatch(void){

	/* タスクコンテキストに切り換える */
	excpt_nest_count = 0;
	/* CPUロック状態・割込み優先度マスク全解除状態にする */
	unlock_cpu();
	/* dispatcher_0に分岐する */
	dispatcher_0();
}

/*
 *  割込みハンドラ出入口処理
 */
void
int_handler_entry(void){
}

/*
 *  CPU例外ハンドラ出入口処理
 */
void
exc_handler_entry(void){
}

/*
 *  タスク開始時処理
 */
void
start_r(void){

#ifdef TOPPERS_SUPPORT_OVRHDR
	ovrtimer_start();		/* オーバランタイマの動作開始 */
#endif /* TOPPERS_SUPPORT_OVRHDR */
	
	unlock_cpu();  /* CPUロック解除状態にする */

	/* 自タスク（p_runtsk）の起動番地を，拡張情報をパラメータとして呼び出す */
	p_runtsk->p_tinib->task(p_runtsk->p_tinib->exinf);

	ext_tsk(); /* ext_tskに分岐する */
}

#if !defined(OMIT_DEFAULT_INT_HANDLER)
void
default_int_handler(void) {
}
#endif  /* OMIT_DEFAULT_INT_HANDLER */

#if !defined(OMIT_DEFAULT_EXC_HANDLER)
void
default_exc_handler(void *p_excinf){
}
#endif  /* OMIT_DEFAULT_EXC_HANDLER */

/*
 *  ターゲット依存の初期化
 */
void
target_initialize(void){

	init_vectors();

	gicd_initialize();

	gicc_initialize();

	initialize_interrupt();
}

/*
 *  ターゲット依存の終了処理
 */
void
target_exit(void){

	lock_cpu();
	gicc_terminate();
	gicd_terminate();
	while(1);
}
