/****************************************************************************
 *
 * rg/pkg/util/rg_error.h
 * 
 * Copyright (C) Jungo LTD 2004
 * 
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General 
 * Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02111-1307, USA.
 *
 * Developed by Jungo LTD.
 * Residential Gateway Software Division
 * www.jungo.com
 * info@jungo.com
 */
/* SYNC: rg/pkg/util/rg_error.h <-> project/tools/util/rg_error.h */

#ifndef _RG_ERROR_H_
#define _RG_ERROR_H_

#define OS_INCLUDE_STDARG
#include <os_includes.h>
#include <cc_config.h>

#define RG_ERROR_FIREWALL_PREFIX "RGFW-"
#define RG_ERROR_NAT_PREFIX RG_ERROR_FIREWALL_PREFIX "NAT"
#define RG_ERROR_LOGIN_PREFIX "LOGIN"
#define RG_ERROR_CLI_LOGIN_PREFIX RG_ERROR_LOGIN_PREFIX "-CLI"
#define RG_ERROR_WBM_LOGIN_PREFIX RG_ERROR_LOGIN_PREFIX "-WBM"
#define RG_ERROR_FTP_LOGIN_PREFIX RG_ERROR_LOGIN_PREFIX "-FTP"
#define RG_ERROR_MAIL_LOGIN_PREFIX RG_ERROR_LOGIN_PREFIX "-MAIL"
#define RG_ERROR_SYS_UPDOWN_PREFIX "SYS_UPDOWN-"

/* see 'man 2 syslog' and 'man 3 syslog' for LOG_xxx and KERN_xxx loglevel
 * definitions */
#define LLEVEL_MASK 0xf
#define LEMERG 0
#define LALERT 1
#define LCRIT 2
#define LERR 3
#define LWARNING 4
#define LNOTICE 5
#define LINFO 6
#define LDEBUG 7

/* From here on it acts as a bit field. */
#define LFLAGS_MASK 0xf00
#define LCONSOLE 0x100
#define LDOEXIT 0x200
#define LDOREBOOT 0x400
#define LSYSLOGBASIC 0x800

/* 16 most significant bits are user defined */
#define LUSER_MASK 0xFFFF0000

typedef unsigned int rg_error_level_t;

#define LEXIT (LCRIT | LDOEXIT)
#define LPANIC (LEMERG | LDOEXIT)
#define LWARN LWARNING

extern char *rg_error_level_str[];

/* XXX - A temporary hack that defines a level for "good" messages that should
 *       be given high priority than LINFO (the same priority as LERR).
 */
#define LINFO_ERR LNOTICE

typedef void (*rg_error_func_t)(void *data, char *msg, rg_error_level_t level);

/* The space before the ',' is important. Dont del it ! */
#define rg_error_f(SEVERITY, FMT, ARGS...) \
    rg_error(SEVERITY, "%s:%d: " FMT, __FUNCTION__, __LINE__ , ## ARGS)

#ifdef OPENRG_DEBUG
#define RG_DEBUG(level, params...) rg_error_f(level, ## params)
#else
#define RG_DEBUG(level, params...) do {} while (0)
#endif
    
/* Every registered function gets a reference to every log message from the
 * level specified and up.
 */
void rg_error_exit_reboot_set(void);

/* Set openrg thread_id for rg_error */
void rg_error_set_mt_id(void);

/* Every registered function gets a reference to every log message from the
 * level specified and up.
 */
void default_error_cb(void *data, char *msg, rg_error_level_t level);
void rg_error_register(int sequence, rg_error_level_t level,
    rg_error_func_t func, void *data);
int rg_error_unregister(rg_error_func_t func, void *data);

void rg_openlog(char *ident, int option, int  facility);
int rg_error(rg_error_level_t severity, const char *format, ...)
    __attribute__((__format__(__printf__, 2, 3)));
int rg_verror(rg_error_level_t severity, const char *format, va_list ap)
    __attribute__((nonnull(3)));
int rg_perror(rg_error_level_t severity, const char *format, ...)
    __attribute__((__format__(__printf__, 2, 3)));
int rg_vperror(rg_error_level_t severity, const char *format, va_list ap)
    __attribute__((nonnull(3)));
void rg_closelog(void);

#ifdef RG_ERROR_INTERNALS
/* Deliver log message directly, must be used only within openrg */
void log_msg_deliver(rg_error_level_t severity, char *msg);
#endif

void rg_error_init(int _reboot_on_exit, int _reboot_on_panic,
    int strace, char *process, rg_error_level_t console_level,
    int (*_main_addr)(int, char *[]), const char *_main_name);
void rg_error_init_default(rg_error_level_t level);
void rg_error_init_nodefault(void);

#ifdef __KERNEL__
extern int (*rg_error_logdev_hook)(char *msg, int exact_len);
#endif

#endif

