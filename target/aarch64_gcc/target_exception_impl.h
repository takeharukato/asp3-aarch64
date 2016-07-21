
#ifndef TOPPERS_TARGET_EXCEPTION_IMPL_H
#define TOPPERS_TARGET_EXCEPTION_IMPL_H

#include "aarch64.h"

/*
 *  標準の割込み管理機能の初期化処理を用いない
 */
#define OMIT_INITIALIZE_INTERRUPT
/*
 *  割込みハンドラテーブル（kernel_cfg.c）
 */
extern const FP inh_table[];

/*
 *  割込み要求ライン設定テーブル（kernel_cfg.c）
 * 
 *  割込み要求ラインに対して割込み属性が設定されていれば1，設定されてい
 *  なければ0を保持するテーブル．
 */
#ifdef USE_INTCFG_TABLE
extern const uint8_t intcfg_table[];
#endif /* USE_INTCFG_TABLE */

/*
 *  標準の例外管理機能の初期化処理を用いない
 */
#define OMIT_INITIALIZE_EXCEPTION


typedef struct _excinf {
	uint64_t	intpri;					/* 割込み優先度マスク */
	uint64_t	irq;					/* 割込み番号         */
	uint64_t	exnc;				        /* 例外ネストカウント */
	uint64_t	esr;                            /* エラーシンドロームレジスタ */
	uint64_t        sp_el0;                    /* EL0(ユーザ空間)でのSP(将来予約) */
	uint64_t	elr;                                     /* ELR(例外復帰先PC) */
	uint64_t	spsr;                 /* SPSR(NZCVフラグ, DAIF割込み状態など) */
	uint64_t	r0;
	uint64_t	r1;
	uint64_t	r2;
	uint64_t	r3;
	uint64_t	r4;
	uint64_t	r5;
	uint64_t	r6;
	uint64_t	r7;
	uint64_t	r8;
	uint64_t	r9;
	uint64_t	r10;
	uint64_t	r11;
	uint64_t	r12;
	uint64_t	r13;
	uint64_t	r14;
	uint64_t	r15;
	uint64_t	r16;
	uint64_t	r17;
	uint64_t	r18;
	uint64_t	r19;
	uint64_t	r20;
	uint64_t	r21;
	uint64_t	r22;
	uint64_t	r23;
	uint64_t	r24;
	uint64_t	r25;
	uint64_t	r26;
	uint64_t	r27;
	uint64_t	r28;
	uint64_t	r29;
	uint64_t	r30;
} T_EXCINF;

/*
 *  CPU例外ハンドラテーブル（kernel_cfg.c）
 */
extern const FP exc_table[TNUM_EXCNO];

/*
 *  CPU例外ハンドラの初期化
 */
Inline void
initialize_exception(void)
{
}

#include "gic_kernel_impl.h"

/*
 *  CPU例外の発生した時のコンテキストの参照
 *
 *  CPU例外の発生した時のコンテキストが，タスクコンテキストの時にfalse，
 *  そうでない時にtrueを返す．
 */
Inline bool_t
exc_sense_context(void *p_excinf)
{
	return(((T_EXCINF *)(p_excinf))->exnc != 0U);
}

/*
 *  CPU例外の発生した時の割込み優先度マスクの参照
 */
Inline PRI
exc_get_intpri(void *p_excinf) {

	return((PRI)EXT_IPM(((T_EXCINF *)(p_excinf))->intpri));
}

/*
 *  CPUロック状態または割込みロック状態かの参照
 */
Inline bool_t
exc_sense_lock(void *p_excinf) {

	return  ( ( ((T_EXCINF *)(p_excinf))->spsr & (AARCH64_PSTATE_I|AARCH64_PSTATE_F) ) 
	    == (AARCH64_PSTATE_I|AARCH64_PSTATE_F) );
}

/*
 *  CPU例外の発生した時のコンテキストと割込みのマスク状態の参照
 *
 *  CPU例外の発生した時のシステム状態が，カーネル実行中でなく，タスクコ
 *  ンテキストであり，全割込みロック状態でなく，CPUロック状態でなく，割
 *  込み優先度マスク全解除状態である時にtrue，そうでない時にfalseを返す
 *  （CPU例外がカーネル管理外の割込み処理中で発生した場合にもfalseを返
 *  す）．
 *
 */
Inline bool_t
exc_sense_intmask(void *p_excinf) {

	return(!exc_sense_context(p_excinf)
		&& exc_get_intpri(p_excinf) == TIPM_ENAALL
		&& !exc_sense_lock(p_excinf));
}
#endif  /*  TOPPERS_TARGET_EXCEPTION_IMPL_H  */
