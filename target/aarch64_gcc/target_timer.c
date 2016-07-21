/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2013-2015 by Embedded and Real-Time Systems Laboratory
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
 *		タイマドライバ（aarch64用）
 */

#include "kernel_impl.h"
#include "time_event.h"
#include "target_timer.h"

static uint64_t
clock_read_ctrl(void) {
	uint64_t cntv_ctrl;

	asm volatile ("isb; mrs %0, cntv_ctl_el0; isb;" 
	    : "=r"(cntv_ctrl) :: "memory");

	return cntv_ctrl;
}

static void
clock_write_ctrl(uint64_t cntv_ctrl) {

	asm volatile ("isb; msr cntv_ctl_el0, %0; isb;" 
	    :: "r"(cntv_ctrl) : "memory");
	return ;
}

static uint64_t
clock_read_counter(void){
	uint64_t cval;

	asm volatile("mrs %0, cntvct_el0" : "=r" (cval));

	return cval;
}

static  uint64_t
clock_read_tval(void){
	uint64_t cntv_tval;

	asm volatile ("isb; mrs %0, cntv_tval_el0; isb;" : "=r"(cntv_tval)
	    :: "memory");
	return cntv_tval;
}

static  void
clock_write_tval(uint64_t cntv_tval){

	asm volatile ("isb; msr cntv_tval_el0, %0; isb;" :: "r"(cntv_tval)
	    : "memory");
}

static  uint64_t 
get_timer_freq(void) {
	uint64_t freq_hz;

	asm volatile ("mrs %0, cntfrq_el0; isb; " : "=r"(freq_hz) :: "memory");
	return freq_hz;
}
/*
 *  タイマ割込み要求のクリア
 */
void
target_hrt_int_clear(void){

	gicd_clear_pending(TINTNO_HRT);
}

static  void
target_hrt_stop_timer(void) {
	uint64_t cntv_ctl;

	cntv_ctl = clock_read_ctrl();
	cntv_ctl &= ~( CNTV_CTL_ENABLE | CNTV_CTL_START ); //disable
	cntv_ctl |=  CNTV_CTL_MASK; //mask
	clock_write_ctrl(cntv_ctl);
}

static  void
target_hrt_start_timer(void) {
	uint64_t cntv_ctl;

	cntv_ctl = clock_read_ctrl();
	cntv_ctl |=  ( CNTV_CTL_ENABLE | CNTV_CTL_START ); //disable
	cntv_ctl &= ~CNTV_CTL_MASK; //mask
	clock_write_ctrl(cntv_ctl);
}

/*
 *  高分解能タイマの初期化処理
 */
void
target_hrt_initialize(intptr_t exinf) {
	uint64_t cntv_ctl;

	target_hrt_stop_timer();
	clock_write_tval(~(0UL));
	target_hrt_start_timer();  /* Start */
	target_hrt_int_clear();
}

/*
 *  高分解能タイマの終了処理
 */
void
target_hrt_terminate(intptr_t exinf){

	target_hrt_stop_timer();
	target_hrt_int_clear();
}

/*
 *  高分解能タイマの現在のカウント値の読出し
 */
HRTCNT
target_hrt_get_current(void){
	uint64_t div;

	div = get_timer_freq() / 1000000UL;  /* cnt/us */
	
	return (HRTCNT)(( clock_read_counter() / div ) % (1UL << 32));
}

/*
 *  高分解能タイマ割込みの要求
 */
void
target_hrt_raise_event(void){

	gicd_raise_int(TINTNO_HRT);
}

/*
 *  高分解能タイマへの割込みタイミングの設定
 */
void
target_hrt_set_event(HRTCNT hrtcnt){
	uint64_t	   next;
	uint64_t	   curr;
	uint64_t            div;

	target_hrt_stop_timer();
	target_hrt_int_clear();

	div = get_timer_freq() / 1000000UL;  /* 1us */
	/*
	 *  現在のカウント値を読み，hrtcnt後に割込みが発生するように設定する．
	 */
	curr = target_hrt_get_current();
	
	next =  hrtcnt * div;

	if ( hrtcnt == HRTCNT_BOUND )  
		next = (HRTCNT_BOUND - curr) * div;

	clock_write_tval(next);

	target_hrt_start_timer();

	/*
	 *  上でカウンタを設定後に，next以上カウントアップしてい
	 *  た場合には，割込みを発生させる．
	 */
	if ( target_hrt_get_current() > ( curr + next ) )
		target_hrt_raise_event();
}

/*
 *  高分解能タイマ割込みハンドラ
 */
void
target_hrt_handler(void){

	target_hrt_int_clear();
	signal_time();					/* 高分解能タイマ割込みの処理 */
}

#ifdef TOPPERS_SUPPORT_OVRHDR

/*
 *  オーバランタイマの初期化処理
 */
void
target_ovrtimer_initialize(intptr_t exinf)
{
}

/*
 *  オーバランタイマの終了処理
 */
void
target_ovrtimer_terminate(intptr_t exinf)
{
}

/*
 *  オーバランタイマの動作開始
 */
void
target_ovrtimer_start(PRCTIM ovrtim)
{
}

/*
 *  オーバランタイマの停止
 */
PRCTIM
target_ovrtimer_stop(void)
{
	return(0U);
}

/*
 *  オーバランタイマの現在値の読出し
 */
PRCTIM
target_ovrtimer_get_current(void)
{
	return(0U);
}

/*
 *  オーバランタイマ割込みハンドラ
 */
void
target_ovrtimer_handler(void)
{
	call_ovrhdr();					/* オーバランハンドラの起動処理 */
}

#endif /* TOPPERS_SUPPORT_OVRHDR */
