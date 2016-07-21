#ifndef tecs_mruby_h__
#define tecs_mruby_h__

#ifndef TECSGEN

// tecsgen doesn't include actual mruby.h
#include "mruby.h"
#include "mruby/class.h"
#include "mruby/data.h"
#include "mruby/string.h"

#include "TECSPointer.h"
#include "TECSStruct.h"

#if  ! defined( MRUBY_RELEASE_MAJOR ) || MRUBY_RELEASE_MAJOR == 1 && MRUBY_RELEASE_MINOR < 2
#ifndef MRB_ARGS_REQ
#define MRB_ARGS_REQ(n)     ARGS_REQ(n)
#define MRB_ARGS_OPT(n)     ARGS_OPT(n)
#define MRB_ARGS_ARG(n1,n2) ARGS_ARG(n1,n2)
#define MRB_ARGS_REST()     ARGS_REST()
#define MRB_ARGS_POST(n)    ARGS_POST(n)
#define MRB_ARGS_KEY(n1,n2) ARGS_KEY(n1,n2)
#define MRB_ARGS_BLOCK()    ARGS_BLOCK()
#define MRB_ARGS_ANY()      ARGS_ANY()
#define MRB_ARGS_NONE()     ARGS_NONE()
#endif /* MRB_ARGS_REQ */
#endif

#if  ! defined( MRUBY_RELEASE_MAJOR )
#define mrb_float_value( mrb, val )  mrb_float_value( val )
#endif

#else

/*
 * fake tecsgen because tecsgen cannot accept actual mruby.h in case of below.
 *   types:   long long, long long int
 *   special keyword __attribute__(x), __extension__
 */
typedef int mrb_state;
struct  RClass {int dummy;};

typedef int CELLCB;

#endif /* TECSGEN */

#endif /* tecs_mruby_h__ */
