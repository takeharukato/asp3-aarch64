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
 *		カーネルの割込みコントローラ依存部のアセンブリ言語部（GIC用）
 */

#define TOPPERS_MACRO_ONLY
#define TOPPERS_ASM_MACRO
#define UINT_C(val)			(val)		/* uint_t型の定数を作るマクロ */
#define ULONG_C(val)		(val)		/* ulong_t型の定数を作るマクロ */
#define CAST(type, val)		(val)		/* 型キャストを行うマクロ */
#define GIC_REG(base, offset)	((base) + (offset))
#include "kernel_impl.h"
#include "core_asm.inc"
#include "gic_kernel_impl.h"
#include "offset.h"

/*
 *  CPU例外発生前の割込み優先度の取得
 *
 *  CPU例外の発生で割込み優先度が変わることはないため，現在の割込み優先
 *  度を返す．
 */
	ATEXT
	AALIGN(4)
	AGLOBAL(irc_get_intpri)
ALABEL(irc_get_intpri)
	/*
	 *  割込み優先度マスクを外部表現に変換して返す．
	 */
	ldr		x1, =GICC_PMR
	ldr		x0, [x1]
	asr		x0, x0, #GIC_PRI_SHIFT
	sub		x0, x0, #GIC_PRI_MASK
	ret

/*
 *  割込みハンドラ呼出し前の割込みコントローラ操作
 *  引数
 *  x0: 割込みコンテキストのアドレス
 */
	ATEXT
	AALIGN(4)
	AGLOBAL(irc_begin_int)
ALABEL(irc_begin_int)
	/*
	 *  割込み要因を取得する(x9)．
	 */
	ldr		x9, =GICC_IAR
	ldr		x9, [x9]
	str		x9, [x0, #T_EXCINF_irq]  /* コンテキストに保存  */
	/*
	 *  割込み要因の割込み優先度を求め，割込み優先度マスクに設定する．
	 */
	ldr		x10, =GICC_RPR	/* 受け付けた割込みの割込み優先度を取得(x10) */
	ldr		x10, [x10]
	ldr		x11, =GICC_PMR	/* 割込み発生前の割込み優先度を取得(x12) */
	ldr		x12, [x11]
	str		x10, [x11]	/* 新しい割込み優先度マスクをセットする */
	asm_data_sync_barrier x13	/* 割込み優先度マスクがセットされるのを待つ */
	str		x12, [x0, #T_EXCINF_intpri]  /* コンテキストに保存  */

	/*
	 *  EOIを発行する．
	 */
	ldr		x11, =GICC_EOIR		/* EOIレジスタへの書込み */
	str		x9, [x11]
	ret
	
/*
 *  CPU例外ハンドラ呼出し前の割込みコントローラ操作
 *
 *  irc_endで用いる情報（CPU例外発生前の割込み優先度マスク）を，スタッ
 *  クの先頭に保存する．
 *  x0: 割込みコンテキストのアドレス	
 */
	AALIGN(4)
	AGLOBAL(irc_begin_exc)
ALABEL(irc_begin_exc)
	/*
	 *  割込み優先度マスクを保存する．
	 */
	ldr		x9, =GICC_PMR		/* 現在の割込み優先度を取得 */
	ldr		x9, [x9]
	str		x9, [x0, #T_EXCINF_intpri]   /* コンテキストに保存  */
	ret

/*
 *  CPU例外ハンドラ呼出し後の割込みコントローラ操作
 *  x0: 割込みコンテキストのアドレス		
 */
	AALIGN(4)
	AGLOBAL(irc_end)
ALABEL(irc_end)
	/*
	 *  割込み優先度マスクを元に戻す．
	 */
	ldr		x9, [x0, #T_EXCINF_intpri] /* 割込みマスク取得 */
	ldr		x10, =GICC_PMR		/* 割込み優先度マスクを元に戻す */
	str		x9, [x10]
	ret
