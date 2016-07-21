/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2013-2014 by Embedded and Real-Time Systems Laboratory
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
 *		aarch64ターゲットのハードウェア資源の定義
 */

#ifndef TOPPERS_AARCH64_H
#define TOPPERS_AARCH64_H

#ifndef  TOPPERS_MACRO_ONLY
#include <stdint.h>
#endif  /*  TOPPERS_MACRO_ONLY  */

/*
 *  割込み番号（intno）の定義
 */
#define TINTNO_HRT		27		/* 高分解能タイマ */
#define TINTNO_OVRTIMER	        26		/* オーバランタイマ */
#define VIRT_IRQNO_UART0        33		/* SIOポート */
/*
 * Mach virtのGIC
 */
#define GIC_BASE                (0x08000000)
#define GICD_BASE               (GIC_BASE)
#define GICC_BASE               (GIC_BASE + 0x10000)
#define GIC_TNUM_INTNO          (64)
/*
 *  割込み優先度の範囲
 */
#define TMIN_INTPRI		(-15)		/* 割込み優先度の最小値（最高値）*/
#define TMAX_INTPRI		(-1)		/* 割込み優先度の最大値（最低値） */

#define TNUM_EXCNO              (16)
#define USE_INTCFG_TABLE
#define USE_INTINIB_TABLE
/*
 * 割込み関連マクロ/インライン関数
 */
#define AARCH64_DAIF_FIQ    (1)
#define AARCH64_DAIF_IRQ    (2)

/* 
 *  UARTレジスタのベースアドレス
 */                              
#define VIRT_UART0_BASE		(0x09000000)

/*
 *  ボーレート設定（38400bps）
 */ 
#define VIRT_UART_IBRD_38400	0x27U
#define VIRT_UART_FBRD_38400	0x04U

#ifndef  TOPPERS_MACRO_ONLY

#define AARCH64_PSTATE_N         (1 << 31)
#define AARCH64_PSTATE_Z         (1 << 30)
#define AARCH64_PSTATE_C         (1 << 29)
#define AARCH64_PSTATE_V         (1 << 28)
#define AARCH64_PSTATE_SS        (1 << 21)
#define AARCH64_PSTATE_IL        (1 << 20)
#define AARCH64_PSTATE_D         (1 << 9)
#define AARCH64_PSTATE_A         (1 << 8)
#define AARCH64_PSTATE_I         (1 << 7)
#define AARCH64_PSTATE_F         (1 << 6)
#define AARCH64_PSTATE_MODE4     (1 << 4)
#define AARCH64_PSTATE_MODE_MASK ( AARCH64_PSTATE_MODE4 - 1)

#if !defined(TECSGEN)
Inline uint64_t
aarch64_get_daif(void){
	uint64_t psw; 

	 __asm__ __volatile__ ("mrs %0, daif\n\t":"=r"(psw));

	 return psw;
}

Inline void
aarch64_enable_intr(void) {

	__asm__ __volatile__ ("msr daifset, %0\n\t" : : "i"(AARCH64_DAIF_IRQ));
}

Inline void
aarch64_disable_intr(void) {

	__asm__ __volatile__ ("msr daifclr, %0\n\t" : : "i"(AARCH64_DAIF_IRQ));
}
#endif  /*  TECSGEN  */
#endif  /*  TOPPERS_MACRO_ONLY  */

#endif /* TOPPERS_AARCH64_H */
