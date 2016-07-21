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
 *		カーネルのターゲット依存部（ダミーターゲット用）
 *
 *  カーネルのターゲット依存部のヘッダファイル．kernel_impl.hのターゲッ
 *  ト依存部の位置付けとなる．
 */

#ifndef TOPPERS_TARGET_KERNEL_IMPL_H
#define TOPPERS_TARGET_KERNEL_IMPL_H

#include <kernel.h>

/*
 *  ターゲットシステムのハードウェア資源の定義
 */
#include "aarch64.h"

/*
 *  エラーチェック方法の指定
 */
#define TOPPERS_AARCH64_ENABLE_CHECK
#if defined(TOPPERS_AARCH64_ENABLE_CHECK)
#define CHECK_STKSZ_ALIGN      16	/* スタックサイズのアライン単位 */
#define CHECK_INTPTR_ALIGN	8	/* intptr_t型の変数のアライン単位 */
#define CHECK_INTPTR_NONNULL	        /* intptr_t型の変数の非NULLチェック */
#define CHECK_FUNC_ALIGN	4	/* 関数のアライン単位 */
#define CHECK_FUNC_NONNULL		/* 関数の非NULLチェック */
#define CHECK_STACK_ALIGN       8	/* スタック領域のアライン単位 */
#define CHECK_STACK_NONNULL		/* スタック領域の非NULLチェック */
#define CHECK_MPF_ALIGN		8	/* 固定長メモリプール領域のアライン単位 */
#define CHECK_MPF_NONNULL		/* 固定長メモリプール領域の非NULLチェック */
#define CHECK_MB_ALIGN		8	/* 管理領域のアライン単位 */
#endif  /*  TOPPERS_AARCH64_ENABLE_CHECK  */

/*
 *  トレースログに関する設定
 */
#ifdef TOPPERS_ENABLE_TRACE
#include "arch/tracelog/trace_log.h"
#endif /* TOPPERS_ENABLE_TRACE */

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_INH_ENTER
#define LOG_INH_ENTER(inhno)
#endif /* LOG_INH_ENTER */

#ifndef LOG_INH_LEAVE
#define LOG_INH_LEAVE(inhno)
#endif /* LOG_INH_LEAVE */

#ifndef LOG_EXC_ENTER
#define LOG_EXC_ENTER(excno)
#endif /* LOG_EXC_ENTER */

#ifndef LOG_EXC_LEAVE
#define LOG_EXC_LEAVE(excno)
#endif /* LOG_EXC_LEAVE */
#define DEFAULT_ISTKSZ			0x2000

#ifndef TOPPERS_MACRO_ONLY

#if 0
extern uintptr_t _aarch64_kernel_istk;
#define DEFAULT_ISTK                    ((void *)(&_aarch64_kernel_istk))
#endif 

#define arm_prc_index()  (0)

#define TOPPERS_ISTKPT(istk, istksz)  ((STK_T *)(char *)( (istk) + (istksz) ) )

/*
 *  タスクコンテキストブロックの定義
 */
typedef struct task_context_block {
	void		*sp;			/* スタックポインタ */
	void		*pc;			/* 実行再開番地 */
} TSKCTXB;

extern uint32_t	excpt_nest_count;		/* 例外ネストカウント */
/*
 *  コンテキストの参照
 */
Inline bool_t
sense_context(void){

	return ( excpt_nest_count > 0 );
}

/*
 *  CPUロック状態への遷移
 */
Inline void
lock_cpu(void) {

	__asm__ __volatile__ ("msr daifset, %0\n\t" : 
	    : "i"( AARCH64_DAIF_IRQ | AARCH64_DAIF_FIQ ));
}

/*
 *  CPUロック状態への移行（ディスパッチできる状態）
 */
#define lock_cpu_dsp()		lock_cpu()

/*
 *  CPUロック状態の解除
 */
Inline void
unlock_cpu(void){

	__asm__ __volatile__ ("msr daifclr, %0\n\t" :
	    : "i"( AARCH64_DAIF_IRQ | AARCH64_DAIF_FIQ ) );
}

/*
 *  CPUロック状態の解除（ディスパッチできる状態）
 */
#define unlock_cpu_dsp()	unlock_cpu()

/*
 *  CPUロック状態の参照
 */
Inline bool_t
sense_lock(void){

	return( ( aarch64_get_daif() & (AARCH64_DAIF_IRQ | AARCH64_DAIF_FIQ) ) 
	    == (AARCH64_DAIF_IRQ | AARCH64_DAIF_FIQ) );
}

/*
 *  割込みを受け付けるための遅延処理
 */
Inline void
delay_for_interrupt(void)
{
}

/*  ターゲット固有の割込みハンドラ */
#include "target_exception_impl.h"

#if defined(OMIT_GIC)

/*
 *  割込み優先度マスクの設定
 */
Inline void
t_set_ipm(PRI intpri)
{
}

/*
 *  割込み優先度マスクの参照
 */
Inline PRI
t_get_ipm(void)
{
	return(TIPM_ENAALL);
}

/*
 *  割込み番号，割込みハンドラ番号，CPU例外ハンドラ番号の範囲の判定
 */
#define	VALID_INTNO(intno)	(0U <= (intno) && (intno) <= 31U)
#define VALID_INHNO(inhno)	VALID_INTNO((INTNO)(inhno))
#define VALID_EXCNO(excno)	(0U <= (excno) && (excno) <= 7U)

/*
 *  割込み要求禁止フラグのセット
 *
 *  割込み属性が設定されていない割込み要求ラインに対して割込み要求禁止
 *  フラグをセットしようとした場合には，falseを返す．
 */
Inline bool_t
disable_int(INTNO intno)
{
	return(true);
}

/*
 *  割込み要求禁止フラグのクリア
 *
 *  割込み属性が設定されていない割込み要求ラインに対して割込み要求禁止
 *  フラグをクリアしようとした場合には，falseを返す．
 */
Inline bool_t
enable_int(INTNO intno)
{
	return(true);
}

/*
 *  割込み要求のクリア
 */
Inline void
clear_int(INTNO intno)
{
}

/*
 *  割込み要求のチェック
 */
Inline bool_t
probe_int(INTNO intno)
{
	return(true);
}
#endif /*  OMIT_GIC  */


#include "gic_kernel_impl.h"

/*
 *  最高優先順位タスクへのディスパッチ
 */
extern void	dispatch(void);

/*
 *  非タスクコンテキストからのディスパッチ要求
 */
#define request_dispatch()

/*
 * 次タスクへの切り替え処理
 */
void dispatcher_0(void);

/*
 *  ディスパッチャの動作開始
 */
extern void start_dispatch(void);

/*
 *  現在のコンテキストを捨ててディスパッチ
 */
extern void	exit_and_dispatch(void);

/*
 *  割込みハンドラ出入口処理
 */
extern void	int_handler_entry(void);

/*
 *  CPU例外ハンドラ出入口処理
 */
extern void	exc_handler_entry(void);

/*
 *  カーネルの終了処理の呼出し
 */
extern void call_exit_kernel(void) NoReturn;

/*
 *  タスクコンテキストの初期化
 */
extern void	start_r(void);

	/* 指定されたタスク（p_tcb）のTCB中のスタックポインタを初期化する */
	/* start_rを，実行再開番地として自タスクのTCBに保存する */
#define activate_context(p_tcb)	do{					\
	(p_tcb)->tskctxb.sp = (void *)((char *)((p_tcb)->p_tinib->stk)	\
					+ (p_tcb)->p_tinib->stksz);	\
	(p_tcb)->tskctxb.pc = (FP) start_r;				\
	}while(0)

#if !defined(OMIT_DEFAULT_INT_HANDLER)
void default_int_handler(void);
#endif  /* OMIT_DEFAULT_INT_HANDLER */

#if !defined(OMIT_DEFAULT_EXC_HANDLER)
void default_exc_handler(void *p_excinf);
#endif  /* OMIT_DEFAULT_EXC_HANDLER */

/*
 *  ターゲットシステム依存の初期化
 */
extern void	target_initialize(void);

/*
 *  ターゲットシステムの終了
 *
 *  システムを終了する時に使う．
 */
extern void	target_exit(void) NoReturn;

/*  遅延ディスパッチ処理  */
extern void handle_post_exception(T_EXCINF *excinf);
#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_TARGET_KERNEL_IMPL_H */
