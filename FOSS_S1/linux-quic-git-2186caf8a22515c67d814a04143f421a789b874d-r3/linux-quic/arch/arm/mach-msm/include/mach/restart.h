/* Copyright (c) 2011, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _ASM_ARCH_MSM_RESTART_H_
#define _ASM_ARCH_MSM_RESTART_H_

#define RESTART_NORMAL 0x0
#define RESTART_DLOAD  0x1

#ifdef CONFIG_MSM_NATIVE_RESTART
void msm_set_restart_mode(int mode);
#else
#define msm_set_restart_mode(mode)
#endif

extern int pmic_reset_irq;
/* !!! W A R N I N G  Start!!!
 *
 * the following 2 absolute addresses / offsets are taken from /located in shared IMEM space.
 * Their value is taken from a respective QC case response [Case: 01470712]
 * and are said to be safe for OEM usage
 *
 * In case Qualcomm changes its boot process design so this address space is reused, 
 * these addresses must be reviewed
 */
/* Start dependency */
#define RESTART_DETECT_MAGIC_OFFSET 0xa94 /* this contains a 32 bit "magic number" to verify that the code is intentional and valid */
#define RESTART_DETECT_REASON_OFFSET 0xa98 /* the restart reason code. only valid if the RESTART_DETECT_MAGIC value contains the "magic" number */
#define RESTART_DETECT_MAGIC_ADDR (MSM_IMEM_BASE + RESTART_DETECT_MAGIC_OFFSET)
#define RESTART_DETECT_REASON_ADDR (MSM_IMEM_BASE + RESTART_DETECT_REASON_OFFSET)
/* End  dependency
 *
 * !!! W A R N I N G End !!!
 */
/* everything from here on is independent from the used addresses */
#define RESTART_DETECT_MAX_STRING_SIZE 64 /* the maximum size that a restart reason descriptor string may have */
#define RESTART_DETECT_MAGIC 0xd6f3e7c1 /* totally random number used as "magic number" to identify validity */
#define RESTART_DETECT_REASON_BASE 0xb8d10000 /* Basevalue for all Reason codes. Just a random value for 16 MSBits*/
#define RESTART_DETECT_REASON_HW_RESET (RESTART_DETECT_REASON_BASE + 0x1) /* HW Reset */
#define RESTART_DETECT_REASON_COLD_START (RESTART_DETECT_REASON_BASE + 0x2) /* Cold Start / Power on */
#define RESTART_DETECT_REASON_PANIC (RESTART_DETECT_REASON_BASE + 0x3) /* PANIC */
#define RESTART_DETECT_REASON_DLOAD (RESTART_DETECT_REASON_BASE + 0x4) /* Enter Dload */
#define RESTART_DETECT_REASON_PMIC_SHUTDOWN (RESTART_DETECT_REASON_BASE + 0x5) /* PMIC initiated shutdown */
#define RESTART_DETECT_REASON_PMIC_CPU_SHUTDOWN (RESTART_DETECT_REASON_BASE + 0x6) /* PMIC initiated CPU shutdown */
#define RESTART_DETECT_REASON_RECOVERY (RESTART_DETECT_REASON_BASE + 0x7) /* Reboot into recovery */
#define RESTART_DETECT_REASON_FASTBOOT (RESTART_DETECT_REASON_BASE + 0x8) /* Reboot into fastboot */
#define RESTART_DETECT_REASON_REBOOT (RESTART_DETECT_REASON_BASE + 0x9) /* Reboot [initiated/scheduled]*/
#define RESTART_DETECT_REASON_OTHER (RESTART_DETECT_REASON_BASE + 0x10) /* Other scheduled reason */
#endif

