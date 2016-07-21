/* -*- mode: C; coding:utf-8 -*- */
/**********************************************************************/
/*  Yet Another Teachable Operating System                            */
/*  Copyright 2016 Takeharu KATO                                      */
/*                                                                    */
/*  kernel printf definitions                                         */
/*                                                                    */
/**********************************************************************/
#if !defined(__KERN_KPRINTF_H)
#define __KERN_KPRINTF_H

#include <stdint.h>
#include <stddef.h>

#include <stdarg.h>

#define KERN_CRI   (0)
#define KERN_ERR   (1)
#define KERN_WAR   (2)
#define KERN_INF   (3)
#define KERN_DBG   (4)


int kvsnprintf(char *, size_t , const char *, va_list );
int ksprintf(char *, const char *,...);
int ksnprintf(char *, size_t , const char *,...);
int kprintf(const char *fmt,...);
int doprintf(int (*___putc)(char __c, void *__argp), void *_argp, const char *_fmt, va_list _args);
#endif  /*  __KERN_KPRINTF_H  */
