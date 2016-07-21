/* #[<PREAMBLE>]#
 * #[<...>]# から #[</...>]# で囲まれたコメントは編集しないでください
 * tecsmerge によるマージに使用されます
 *
 * #[</PREAMBLE>]# */

/* プロトタイプ宣言や変数の定義をここに書きます #_PAC_# */
#include "nMruby_tCharPointer_tecsgen.h"

#ifndef E_OK
#define	E_OK	0		/* success */
#define	E_ID	(-18)	/* illegal ID */
#endif

GET_SET_INT( Char, char_t )
POINTER_CLASS( Char, char_t )
#ifndef  TECS_Use_MrbString_for_CharPointer
CHECK_AND_GET_POINTER( Char, char_t )
#else
CHECK_AND_GET_CHAR_POINTER( Char, char_t )
CHECK_AND_GET_CHAR_POINTER_MOD( Char, char_t )
#endif

/* 受け口関数 #_TEPF_# */
/* #[<ENTRY_PORT>]# eInitialize
 * entry port: eInitialize
 * signature:  nMruby_sInitializeTECSBridge
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eInitialize_initializeBridge
 * name:         eInitialize_initializeBridge
 * global_name:  nMruby_tCharPointer_eInitialize_initializeBridge
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
void
eInitialize_initializeBridge(mrb_state* mrb, struct RClass * TECS)
{
	struct  RClass *c;

	/* ここに処理本体を記述します #_TEFB_# */
	c = tecs_init_CharPointer( mrb, TECS );
	mrb_define_method(mrb, c, "to_s", CharPointer_to_s, MRB_ARGS_NONE());
	mrb_define_method(mrb, c, "from_s", CharPointer_from_s, MRB_ARGS_REQ(1));

}

/* #[<POSTAMBLE>]#
 *   これより下に非受け口関数を書きます
 * #[</POSTAMBLE>]#*/
