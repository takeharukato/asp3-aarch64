/* #[<PREAMBLE>]#
 * #[<...>]# から #[</...>]# で囲まれたコメントは編集しないでください
 * tecsmerge によるマージに使用されます
 *
 * #[</PREAMBLE>]# */

/* プロトタイプ宣言や変数の定義をここに書きます #_PAC_# */
#include "nMruby_tLongPointer_tecsgen.h"

#ifndef E_OK
#define	E_OK	0		/* success */
#define	E_ID	(-18)	/* illegal ID */
#endif

GET_SET_INT( Long, long )
POINTER_CLASS( Long, long )
CHECK_AND_GET_POINTER( Long, long )

/* 受け口関数 #_TEPF_# */
/* #[<ENTRY_PORT>]# eInitialize
 * entry port: eInitialize
 * signature:  nMruby_sInitializeTECSBridge
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eInitialize_initializeBridge
 * name:         eInitialize_initializeBridge
 * global_name:  nMruby_tLongPointer_eInitialize_initializeBridge
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
void
eInitialize_initializeBridge(mrb_state* mrb, struct RClass * TECS)
{
	/* ここに処理本体を記述します #_TEFB_# */
	(void)tecs_init_LongPointer( mrb, TECS );
}

/* #[<POSTAMBLE>]#
 *   これより下に非受け口関数を書きます
 * #[</POSTAMBLE>]#*/
