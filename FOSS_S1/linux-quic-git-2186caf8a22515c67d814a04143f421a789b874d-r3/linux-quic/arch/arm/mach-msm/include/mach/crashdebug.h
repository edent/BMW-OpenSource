/* Copyright (c) 2012, Qualcomm QTI - Qualcomm Research. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* Call to dump memory regions over device nodes in case of a crash of
 * any subsystem. This is meant as a replacement for subsystem restart
 * on 9x15 targets as there is no PIL and hence no facility to individually
 * restart processors. In this case we dump memory regions via device nodes
 * and force the entire target to reset. */
#if defined(CONFIG_MSM_CRASHDEBUG)

void crashdebug_crash (char *reason);

#else

static inline void crashdebug_crash (char *reason) { }

#endif
