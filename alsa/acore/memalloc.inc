#include <linux/config.h>
#include <linux/version.h>

#if defined(CONFIG_MODVERSIONS) && !defined(__GENKSYMS__) && !defined(__DEPEND__)
#define MODVERSIONS
#include <linux/modversions.h>
#include "sndversions.h"
#endif

#include "config.h"
#include "adriver.h"

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,3,0) && defined(CONFIG_PROC_FS)
#include <linux/proc_fs.h>
static inline struct proc_dir_entry *create_proc_read_entry(const char *name,
	mode_t mode, struct proc_dir_entry *base, 
	read_proc_t *read_proc, void * data)
{
	struct proc_dir_entry *res=create_proc_entry(name,mode,base);
	if (res) {
		res->read_proc=read_proc;
		res->data=data;
	}
	return res;
}
#endif