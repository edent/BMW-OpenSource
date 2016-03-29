/* Copyright (c) 2010-2012, The Linux Foundation. All rights reserved.
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

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/reboot.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/cpu.h>
#include <linux/interrupt.h>
#include <linux/mfd/pmic8058.h>
#include <linux/mfd/pmic8901.h>
#include <linux/mfd/pm8xxx/misc.h>

#include <asm/mach-types.h>

#include <mach/msm_iomap.h>
#include <mach/restart.h>
#include <mach/socinfo.h>
#include <mach/irqs.h>
#include <mach/scm.h>
#include "msm_watchdog.h"
#include "timer.h"

#define WDT0_RST	0x38
#define WDT0_EN		0x40
#define WDT0_BARK_TIME	0x4C
#define WDT0_BITE_TIME	0x5C

#define PSHOLD_CTL_SU (MSM_TLMM_BASE + 0x820)

#define RESTART_REASON_ADDR 0x65C
#define DLOAD_MODE_ADDR     0x0

#define SCM_IO_DISABLE_PMIC_ARBITER	1

static int restart_mode;
void *restart_reason;

int pmic_reset_irq;
static void __iomem *msm_tmr0_base;

#ifdef CONFIG_MSM_DLOAD_MODE
static int in_panic;
static void *dload_mode_addr;

/* Download mode master kill-switch */
static int dload_set(const char *val, struct kernel_param *kp);
static int download_mode = 1;
module_param_call(download_mode, dload_set, param_get_int,
			&download_mode, 0644);

static int panic_prep_restart(struct notifier_block *this,
			      unsigned long event, void *ptr)
{
	in_panic = 1;
	return NOTIFY_DONE;
}

static struct notifier_block panic_blk = {
	.notifier_call	= panic_prep_restart,
};

static void set_dload_mode(int on)
{
	if (dload_mode_addr) {
		__raw_writel(on ? 0xE47B337D : 0, dload_mode_addr);
		__raw_writel(on ? 0xCE14091A : 0,
		       dload_mode_addr + sizeof(unsigned int));
		mb();
	}
}

static int dload_set(const char *val, struct kernel_param *kp)
{
	int ret;
	int old_val = download_mode;

	ret = param_set_int(val, kp);

	if (ret)
		return ret;

	/* If download_mode is not zero or one, ignore. */
	if (download_mode >> 1) {
		download_mode = old_val;
		return -EINVAL;
	}

	set_dload_mode(download_mode);

	return 0;
}
#else
#define set_dload_mode(x) do {} while (0)
#endif

static char lastRestartCauseStr_alloc[RESTART_DETECT_MAX_STRING_SIZE]="Unknown"; /* space to expose the reason string in /sys/modules */ 
static char *lastRestartCauseStr=&lastRestartCauseStr_alloc[0]; /* let the Parameter variable point to allocated space */
static int lastRestartCauseInt; /* Will contain the lower part of the reason code */

module_param(lastRestartCauseStr, charp, 0444); /* expose 2 parameters als read only to the world in /sys */
module_param(lastRestartCauseInt, int , 0444);

static void set_restart_reason(int code)
{
  void *magic_addr, *code_addr;
  magic_addr = RESTART_DETECT_MAGIC_ADDR;
  code_addr  = RESTART_DETECT_REASON_ADDR;
  __raw_writel(RESTART_DETECT_MAGIC, magic_addr);
  __raw_writel(code, code_addr);
}
static void print_restart_reason(void)
{
  void *magic_addr, *code_addr;
  char *reason_str, tmp_str[RESTART_DETECT_MAX_STRING_SIZE];
  int reason;
  magic_addr = RESTART_DETECT_MAGIC_ADDR;
  code_addr  = RESTART_DETECT_REASON_ADDR;
  reason = __raw_readl(magic_addr);
  if (RESTART_DETECT_MAGIC != reason){
    reason_str="Cold Start / Power on";
    lastRestartCauseInt=RESTART_DETECT_REASON_COLD_START;
  }else{
    reason = __raw_readl(code_addr);
    lastRestartCauseInt=reason;
    if (RESTART_DETECT_REASON_HW_RESET == reason){reason_str="Hardware Reset";}
    else if (RESTART_DETECT_REASON_PANIC == reason){reason_str="Linux Panic";}
    else if (RESTART_DETECT_REASON_DLOAD == reason){reason_str="Enter Diag DLOAD Mode";}
    else if (RESTART_DETECT_REASON_PMIC_SHUTDOWN == reason){reason_str="PMIC initiated shutdown";}
    else if (RESTART_DETECT_REASON_PMIC_CPU_SHUTDOWN == reason){reason_str="PMIC initiated CPU shutdown";}
    else if (RESTART_DETECT_REASON_RECOVERY == reason){reason_str="Reboot, recovery mode";}
    else if (RESTART_DETECT_REASON_FASTBOOT == reason){reason_str="Reboot, fastboot mode";}
    else if (RESTART_DETECT_REASON_REBOOT == reason){reason_str="Reboot, user initiated";}
    else if (RESTART_DETECT_REASON_OTHER == reason){reason_str="Other SW scheduled Reset";}
    else{snprintf(tmp_str,sizeof(tmp_str),"unknown, reasoncode = 0x%08X\n", reason);reason_str=tmp_str;}
  }
  lastRestartCauseInt &= 0xffffu; /* for exposure, just use the lower part to display a cause value number */
  pr_err("\nRESTART, reason = %s [%d]\n",reason_str,lastRestartCauseInt);
  strncpy(lastRestartCauseStr,reason_str,sizeof(lastRestartCauseStr_alloc));
}

void msm_set_restart_mode(int mode)
{
	download_mode = 1;
	restart_mode  = mode;
}
EXPORT_SYMBOL(msm_set_restart_mode);

static void __msm_power_off(int lower_pshold)
{
	printk(KERN_CRIT "Powering off the SoC\n");
#ifdef CONFIG_MSM_DLOAD_MODE
	set_dload_mode(0);
#endif
	pm8xxx_reset_pwr_off(0);

	if (lower_pshold) {
		__raw_writel(0, PSHOLD_CTL_SU);
		mdelay(10000);
		printk(KERN_ERR "Powering off has failed\n");
	}
	return;
}

static void msm_power_off(void)
{
	/* MSM initiated power off, lower ps_hold */
	__msm_power_off(1);
}

static void cpu_power_off(void *data)
{
	int rc;

	set_restart_reason(RESTART_DETECT_REASON_PMIC_CPU_SHUTDOWN);
	pr_err("PMIC Initiated shutdown %s cpu=%d\n", __func__,
						smp_processor_id());
	if (smp_processor_id() == 0) {
		/*
		 * PMIC initiated power off, do not lower ps_hold, pmic will
		 * shut msm down
		 */
		__msm_power_off(0);

		pet_watchdog();
		pr_err("Calling scm to disable arbiter\n");
		/* call secure manager to disable arbiter and never return */
		rc = scm_call_atomic1(SCM_SVC_PWR,
						SCM_IO_DISABLE_PMIC_ARBITER, 1);

		pr_err("SCM returned even when asked to busy loop rc=%d\n", rc);
		pr_err("waiting on pmic to shut msm down\n");
	}

	preempt_disable();
	while (1)
		;
}

static irqreturn_t resout_irq_handler(int irq, void *dev_id)
{
  set_restart_reason(RESTART_DETECT_REASON_PMIC_SHUTDOWN);
	pr_warn("%s PMIC Initiated shutdown\n", __func__);
	oops_in_progress = 1;
	smp_call_function_many(cpu_online_mask, cpu_power_off, NULL, 0);
	if (smp_processor_id() == 0)
		cpu_power_off(NULL);
	preempt_disable();
	while (1)
		;
	return IRQ_HANDLED;
}

void arch_reset(char mode, const char *cmd)
{
#ifdef CONFIG_MSM_DLOAD_MODE

	/* This looks like a normal reboot at this point. */
	set_dload_mode(0);

	/* Write download mode flags if we're panic'ing */
	set_dload_mode(in_panic);

	/* Write download mode flags if restart_mode says so */
	if (restart_mode == RESTART_DLOAD)
		set_dload_mode(1);

	/* Kill download mode if master-kill switch is set */
	if (!download_mode)
		set_dload_mode(0);
#endif


	if (cmd != NULL) {
		if (!strncmp(cmd, "bootloader", 10)) {
		  set_restart_reason(RESTART_DETECT_REASON_FASTBOOT);
			__raw_writel(0x77665500, restart_reason);
		} else if (!strncmp(cmd, "recovery", 8)) {
		  set_restart_reason(RESTART_DETECT_REASON_RECOVERY);
			__raw_writel(0x77665502, restart_reason);
		} else if (!strncmp(cmd, "oem-", 4)) {
			unsigned long code;
			code = simple_strtoul(cmd + 4, NULL, 16) & 0xff;
      set_restart_reason(RESTART_DETECT_REASON_OTHER);
			__raw_writel(0x6f656d00 | code, restart_reason);
		} else {
		  set_restart_reason(RESTART_DETECT_REASON_REBOOT);
			__raw_writel(0x77665501, restart_reason);
		}
	} else {
#ifdef CONFIG_MSM_DLOAD_MODE
	  if (in_panic) {set_restart_reason(RESTART_DETECT_REASON_PANIC);}
	  else if (RESTART_DLOAD == restart_mode) {set_restart_reason(RESTART_DETECT_REASON_DLOAD);}
	  else
#endif
    {
	    set_restart_reason(RESTART_DETECT_REASON_OTHER);
    __raw_writel(0x77665503, restart_reason);
    }
	}
  printk(KERN_ERR "Going down for restart now\n");

  pm8xxx_reset_pwr_off(1);

	__raw_writel(0, msm_tmr0_base + WDT0_EN);
	if (!(machine_is_msm8x60_fusion() || machine_is_msm8x60_fusn_ffa())) {
		mb();
		__raw_writel(0, PSHOLD_CTL_SU); /* Actually reset the chip */
		mdelay(5000);
		pr_notice("PS_HOLD didn't work, falling back to watchdog\n");
	}

	__raw_writel(1, msm_tmr0_base + WDT0_RST);
	__raw_writel(5*0x31F3, msm_tmr0_base + WDT0_BARK_TIME);
	__raw_writel(0x31F3, msm_tmr0_base + WDT0_BITE_TIME);
	__raw_writel(1, msm_tmr0_base + WDT0_EN);

	mdelay(10000);
	printk(KERN_ERR "Restarting has failed\n");
}

static int __init msm_pmic_restart_init(void)
{
	int rc;

	if (pmic_reset_irq != 0) {
		rc = request_any_context_irq(pmic_reset_irq,
					resout_irq_handler, IRQF_TRIGGER_HIGH,
					"restart_from_pmic", NULL);
		if (rc < 0)
			pr_err("pmic restart irq fail rc = %d\n", rc);
	} else {
		pr_warn("no pmic restart interrupt specified\n");
	}

	return 0;
}

late_initcall(msm_pmic_restart_init);

static int __init msm_restart_init(void)
{
#ifdef CONFIG_MSM_DLOAD_MODE
	atomic_notifier_chain_register(&panic_notifier_list, &panic_blk);
	dload_mode_addr = MSM_IMEM_BASE + DLOAD_MODE_ADDR;
	set_dload_mode(download_mode);
#endif
	msm_tmr0_base = msm_timer_get_timer0_base();
	restart_reason = MSM_IMEM_BASE + RESTART_REASON_ADDR;
	pm_power_off = msm_power_off;
  print_restart_reason();
  set_restart_reason(RESTART_DETECT_REASON_HW_RESET);
	return 0;
}
early_initcall(msm_restart_init);
