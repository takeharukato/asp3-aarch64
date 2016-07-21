/*
 *		cfg1_out.cのリンクに必要なスタブの定義
 *
 *  $Id$
 */

#include <t_stddef.h>

void sta_ker(void)
{
}

void hardware_init_hook(void)
{
}

void software_init_hook(void)
{
}

const size_t	_kernel_istksz = 0;
STK_T *const	_kernel_istk = NULL;
STK_T *const	_kernel_istkpt;
/*
 *  offset.h
 */
const uint8_t	MAGIC_1 = 0x12;
const uint16_t	MAGIC_2 = 0x1234;
const uint32_t	MAGIC_4 = 0x12345678;
