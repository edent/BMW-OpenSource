/* Copyright (c) 2012, Qualcomm QTI - Office of the Chief Scientist. All rights reserved.
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

#define pr_fmt(fmt) "crashdebug: %s(): " fmt, __func__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/list.h>
#include <linux/io.h>
#include <linux/kthread.h>
#include <linux/time.h>
#include <linux/suspend.h>

#include <asm/current.h>
#include "ramdump.h"

struct crash_region {
    const char *name;
    struct ramdump_segment memory;
};
struct crashdebug_dev {
    struct work_struct work;
    struct mutex in_progress;
};

static struct crash_region regions[] = {
    {
        "smem",
        {0x40000000, 0x100000} // 1MB
    },
    {
        "q6fw",
        {0x40200000, 6 * 0x100000} // 6MB
    },
    {
        "q6sw",
        {0x41800000, 64 * 0x100000} // 64MB
    },
    {
        "imem",
        {0x2b000000, 16 * 0x400} // 16kB
    },
    #if 0
    {
        "q6tcm",
        {0x2a000000, 256 * 0x400} // 256kB
    },
    #endif
    {
        "rpm_ram",
        {0x2000, 144 * 0x400} // 144kB
    },
    {
        "rpm_msg",
        {0x108000, 24 * 0x400} // 24kB
    },
    {
        "lpass",
        {0x28400000, 16 * 0x400} // 16kB
    },
};

static void *crash_devices[ARRAY_SIZE(regions)];
static struct crashdebug_dev *cd_dev;
static struct workqueue_struct *cd_wq;

/* Work function to dump memory regions */
static void crashdebug_wq_func(struct work_struct *work)
{
    int rv, i;
    struct crashdebug_dev *cd_dev = container_of(work,
                                      struct crashdebug_dev, work);

    // Send notifications that system is going down?

    // Dump memory regions
    for (i = 0; i < ARRAY_SIZE(regions); i++) {
        rv = do_ramdump(crash_devices[i], &regions[i].memory, 1);
        if (rv)
            panic("crashdump failed: %s\n.", regions[i].name);
    }

    // Set a flag somewhere?
    // Unlock the in_progress mutex
    mutex_unlock(&cd_dev->in_progress);

    // Let the user app reset us after dump is finished...
}

void crashdebug_crash (char *reason)
{
    int rv;
    pr_err("Reason=[%s]\n", reason);

    // Check if we have already been called
    if (!mutex_trylock(&cd_dev->in_progress)) {
        pr_err("Already in progress.\n");
		return;
    }

    // Init work function
    INIT_WORK(&cd_dev->work, crashdebug_wq_func);

    // Schedule work
    rv = queue_work(cd_wq, &cd_dev->work);
	if (rv < 0)
		panic("Unable to schedule work to crashdebug (%d).", rv);
}

EXPORT_SYMBOL(crashdebug_crash);

static int __init crashdebug_init(void)
{
	int i;

	cd_wq = alloc_workqueue("cd_wq", 0, 0);
	if (!cd_wq)
		panic("Couldn't allocate workqueue for subsystem restart.\n");

    cd_dev = kzalloc(sizeof(struct crashdebug_dev), GFP_ATOMIC);
    if (!cd_dev) {
        panic("Couldn't alloc crashdebug device.\n");
    }
    // Initialize local data
    mutex_init(&cd_dev->in_progress);

    // Create a ramdump device for each region
    for (i = 0; i < ARRAY_SIZE(regions); i++) {
        crash_devices[i] = create_ramdump_device(regions[i].name);
        if (crash_devices[i] == NULL) {
            panic ("Couldn't create crash device: %s", regions[i].name);
        }
    }
	return 0;
}

module_init(crashdebug_init);

MODULE_DESCRIPTION("Crash Debug Driver");
MODULE_LICENSE("GPL v2");
