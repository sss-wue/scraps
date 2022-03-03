#include <linux/init.h>
#include <linux/errno.h>

/* 
 * This file is automatically generated from `helloproc_main.c'.
 * by lnxmod_proxy_autogen.  You should not edit this file, it should
 * be autogenerated every time the kernel module is build. 
 * See vf_README.txt for information.
 */

/* 
 * We depend on these two procedures, the user of the vf_api must
 * provide it to us
 */
int helloproc_main_module_init();
void helloproc_main_module_exit();


static int __init vf_module_init()
{
	if (helloproc_main_module_init() == 0){
		return 0;
	}else{
		return -ENOMEM;;
	}
}

static void __exit vf_module_exit()
{
	helloproc_main_module_exit();
}

module_init(vf_module_init)
module_exit(vf_module_exit)