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
 *		トラップ/割込み処理モジュール（aarch64用）
 */
#include "kernel_impl.h"
#include "task.h"
#include "interrupt.h"
#ifdef TOPPERS_SUPPORT_OVRHDR
#include "overrun.h"
#endif /* TOPPERS_SUPPORT_OVRHDR */
#include "t_stddef.h"
#include "kprintf.h"
#include "gic_kernel_impl.h"

static void
call_exc_handler(T_EXCINF *excinf, uint_t trapno) {
	struct TOPPERS_dummy_t dummy;

	if ( exc_table[trapno] == NULL )
		return;

	if ( !( excinf->spsr & AARCH64_PSTATE_I ) )
		aarch64_enable_intr();

#ifdef LOG_EXC_ENTER
		LOG_EXC_ENTER(trapno);
#endif /* LOG_EXC_ENTER */

	dummy.TOPPERS_dummy_field = trapno;
	exc_table[trapno](dummy);

#ifdef LOG_EXC_LEAVE
		LOG_EXC_LEAVE(trapno);
#endif /* LOG_EXC_LEAVE */
	aarch64_disable_intr();
}

static void
call_interrupt_handler(T_EXCINF *excinf) {
	int                        i;
	struct TOPPERS_dummy_t dummy;
	INTINIB                *inib;

	//kprintf("irq:%d\n", irq);
	for(i = 0; i < tnum_cfg_intno; ++i) {

		inib = (INTINIB *)&intinib_table[i];

		if ( inib->intno != excinf->irq )
			continue;

		if ( inh_table[inib->intno] == NULL )
			continue;
			
		/* CPUロック解除状態にする（受け付けた割込みよりも優先度の高い割込みを
		 * 受け付けるようにする）
		 */
#ifdef LOG_INH_ENTER
		LOG_INH_ENTER(inib->intno);
#endif /* LOG_INH_ENTER */
		aarch64_enable_intr();

		dummy.TOPPERS_dummy_field = inib->intno;
		inh_table[inib->intno](dummy);

		aarch64_disable_intr();
#ifdef LOG_INH_LEAVE
		LOG_INH_LEAVE(inib->intno);
#endif /* LOG_INH_LEAVE */
		break;
	}
}

void
bad_exception_handler(T_EXCINF *excinf, uint64_t trapno){

	call_exc_handler(excinf, trapno);
}

void
el1_sync_handler(void *excinf, uint64_t trapno){

	call_exc_handler(excinf, trapno);
}

void
el1_irq_handler(T_EXCINF *excinf, uint64_t trapno){

	call_interrupt_handler(excinf);
}	

void
el1_fiq_handler(T_EXCINF *excinf, uint64_t trapno){

	call_interrupt_handler(excinf);
}

void
el1_error_handler(T_EXCINF *excinf, uint64_t trapno){

	call_exc_handler(excinf, trapno);
}

void
el0_sync_handler(T_EXCINF *excinf, uint64_t trapno){

	call_exc_handler(excinf, trapno);
}

void
el0_irq_handler(T_EXCINF *excinf, uint64_t trapno){

	call_interrupt_handler(excinf);
}

void
el0_fiq_handler(T_EXCINF *excinf, uint64_t trapno){

	call_interrupt_handler(excinf);
}

void
el0_error_handler(T_EXCINF *excinf, uint64_t trapno){

	call_exc_handler(excinf, trapno);
}

void
aarch32_sync_handler(T_EXCINF *excinf, uint64_t trapno){

	call_exc_handler(excinf, trapno);
}

void
aarch32_irq_handler(T_EXCINF *excinf, uint64_t trapno){

	call_interrupt_handler(excinf);
}

void
aarch32_fiq_handler(T_EXCINF *excinf, uint64_t trapno){

	call_interrupt_handler(excinf);
}

void
aarch32_error_handler(T_EXCINF *excinf, uint64_t trapno){

	call_exc_handler(excinf, trapno);
}


void
handle_post_exception(T_EXCINF *excinf) {

	if ( !exc_sense_intmask(excinf) )
		return;
	
	if ( p_runtsk != p_schedtsk ) {

		dispatch();  /* 遅延ディスパッチ  */
	}
}
