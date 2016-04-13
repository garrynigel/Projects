/******************************************************************************/
/* Important Fall 2015 CSCI 402 usage information:                            */
/*                                                                            */
/* This fils is part of CSCI 402 kernel programming assignments at USC.       */
/* Please understand that you are NOT permitted to distribute or publically   */
/*         display a copy of this file (or ANY PART of it) for any reason.    */
/* If anyone (including your prospective employer) asks you to post the code, */
/*         you must inform them that you do NOT have permissions to do so.    */
/* You are also NOT permitted to remove or alter this comment block.          */
/* If this comment block is removed or altered in a submitted file, 20 points */
/*         will be deducted.                                                  */
/******************************************************************************/

#include "types.h"
#include "globals.h"
#include "kernel.h"

#include "util/gdb.h"
#include "util/init.h"
#include "util/debug.h"
#include "util/string.h"
#include "util/printf.h"

#include "mm/mm.h"
#include "mm/page.h"
#include "mm/pagetable.h"
#include "mm/pframe.h"

#include "vm/vmmap.h"
#include "vm/shadowd.h"
#include "vm/shadow.h"
#include "vm/anon.h"

#include "main/acpi.h"
#include "main/apic.h"
#include "main/interrupt.h"
#include "main/gdt.h"

#include "proc/sched.h"
#include "proc/proc.h"
#include "proc/kthread.h"

#include "drivers/dev.h"
#include "drivers/blockdev.h"
#include "drivers/disk/ata.h"
#include "drivers/tty/virtterm.h"
#include "drivers/pci.h"

#include "api/exec.h"
#include "api/syscall.h"

#include "fs/vfs.h"
#include "fs/vnode.h"
#include "fs/vfs_syscall.h"
#include "fs/fcntl.h"
#include "fs/stat.h"

#include "test/kshell/kshell.h"
#include "errno.h"

GDB_DEFINE_HOOK(boot)
GDB_DEFINE_HOOK(initialized)
GDB_DEFINE_HOOK(shutdown)

static void       hard_shutdown(void);
static void      *bootstrap(int arg1, void *arg2);
static void      *idleproc_run(int arg1, void *arg2);
static kthread_t *initproc_create(void);
static void      *initproc_run(int arg1, void *arg2);

static context_t bootstrap_context;
extern int gdb_wait;

extern void *sunghan_test(int, void*);
extern void *sunghan_deadlock_test(int, void*);
extern void *faber_thread_test(int, void*);
extern void *vfstest_main(int,void *);

extern int faber_fs_thread_test(kshell_t *ksh, int argc, char **argv);
extern int faber_directory_test(kshell_t *ksh, int argc, char **argv);

#ifdef __DRIVERS__


    int faber(kshell_t *kshell, int argc,char **argv)
      {
        int status;
        proc_t *faberproc = proc_create("faber");
        kthread_t *faberthread = kthread_create(faberproc,faber_thread_test,0,NULL);

        sched_make_runnable(faberthread);
        KASSERT(kshell != NULL);
        dbg(DBG_INIT,"(GRADING1C: Faber test is invoked)\n");
        while(do_waitpid(-1,0,&status)!=-ECHILD);
        return 0;
      }

      int sunghan_deadlock(kshell_t *kshell, int argc,char **argv)
        {
          int status;
          proc_t *sunghan_deadlock_testproc = proc_create("sunghan_deadlock");
          kthread_t *sunghan_deadlock_testthread = kthread_create(sunghan_deadlock_testproc,sunghan_deadlock_test,0,NULL);

          sched_make_runnable(sunghan_deadlock_testthread);
          KASSERT(kshell != NULL);
          dbg(DBG_INIT,"(GRADING1D)\n");
          while(do_waitpid(-1,0,&status)!=-ECHILD);
          return 0;
        }

        int sunghan(kshell_t *kshell, int argc,char **argv)
          {
            int status;
            proc_t *sunghan_testproc = proc_create("sunghan");
            kthread_t *sunghan_testthread = kthread_create(sunghan_testproc,sunghan_test,0,NULL);

            sched_make_runnable(sunghan_testthread);
            KASSERT(kshell != NULL);
            dbg(DBG_INIT,"(GRADING1D)\n");
            while(do_waitpid(-1,0,&status)!=-ECHILD);
            return 0;
          }

	 int vfstest(kshell_t *kshell, int argc,char **argv)
      	 {
           int status;
           proc_t *vfstestproc = proc_create("vfstest");
           kthread_t *vfstestthread = kthread_create(vfstestproc,vfstest_main,1,NULL);

           sched_make_runnable(vfstestthread);
           KASSERT(kshell != NULL);
           dbg(DBG_INIT,"(GRADING2B: Vfs test is invoked)\n");
           while(do_waitpid(-1,0,&status)!=-ECHILD);
           return 0;
         }

	void *hellotest(int argc1, char **argv1)
	{
		char *argv[] = { NULL };
		char *envp[] = { NULL };
		int status;

		kernel_execve("/usr/bin/hello", argv, envp);
		proc_t* p = NULL;
		/*while (do_waitpid(-1,0,&status) != -ECHILD);*/
		list_iterate_begin(&curproc->p_children,p,proc_t,p_child_link){
		do_waitpid(p->p_pid,0,&p->p_status);
	}list_iterate_end();

	return 0;
	}

	int hello_open(kshell_t *kshell, int argc,char **argv)
      	 {
           int status;
           proc_t *hellotestproc = proc_create("hellotest");
           kthread_t *hellotestthread = kthread_create(hellotestproc,(kthread_func_t)hellotest,0,NULL);

           sched_make_runnable(hellotestthread);
           KASSERT(kshell != NULL);
           dbg(DBG_INIT,"(GRADING3B: hello test is invoked)\n");
           while(do_waitpid(-1,0,&status)!=-ECHILD);
           return 0;
         }

	void *argstest()
	{
		char *argv[] = { "/usr/bin/args","ab","cde","fghi","j",NULL };

		char *envp[] = { NULL };
  do_chdir("usr");
  do_chdir("bin");
		kernel_execve("/usr/bin/args",argv, envp);
/*		proc_t* p = NULL;
		while (do_waitpid(-1,0,&status) != -ECHILD);
	list_iterate_begin(&curproc->p_children,p,proc_t,p_child_link){
		do_waitpid(p->p_pid,0,&p->p_status);
    */
	/*}list_iterate_end();
*/
	return 0;
	}

	int args_open(kshell_t *kshell, int argc,char **argv)
      	 {
           int status;
           proc_t *argstestproc = proc_create("argstest");
           kthread_t *argstestthread = kthread_create(argstestproc,(kthread_func_t)argstest,0,NULL);

           sched_make_runnable(argstestthread);
           KASSERT(kshell != NULL);
           /*dbg(DBG_INIT,"(GRADING2B: args test is invoked)\n");
           */
           while(do_waitpid(-1,0,&status)!=-ECHILD);
           return 0;
         }

	void *memtest(int argc1, char **argv1)
	{
		char *argv[] = { NULL };
		char *envp[] = { NULL };
		int status;

		kernel_execve("/usr/bin/memtest", argv, envp);
		proc_t* p = NULL;
		/*while (do_waitpid(-1,0,&status) != -ECHILD);*/
		list_iterate_begin(&curproc->p_children,p,proc_t,p_child_link){
		do_waitpid(p->p_pid,0,&p->p_status);
	}list_iterate_end();

	return 0;
	}

	int mem_open(kshell_t *kshell, int argc,char **argv)
      	 {
           int status;
           proc_t *memtestproc = proc_create("memtest");
           kthread_t *memtestthread = kthread_create(memtestproc,(kthread_func_t)memtest,0,NULL);

           sched_make_runnable(memtestthread);
           KASSERT(kshell != NULL);
           /*dbg(DBG_INIT,"(GRADING2B: mem test is invoked)\n");
           */
           while(do_waitpid(-1,0,&status)!=-ECHILD);
           return 0;
         }
	void *eatmemtest(int argc1, char **argv1)
	{
		char *argv[] = { NULL };
		char *envp[] = { NULL };
		int status;

		kernel_execve("/usr/bin/eatmem", argv, envp);
		proc_t* p = NULL;
		/*while (do_waitpid(-1,0,&status) != -ECHILD);*/
		list_iterate_begin(&curproc->p_children,p,proc_t,p_child_link){
		do_waitpid(p->p_pid,0,&p->p_status);
	}list_iterate_end();

	return 0;
	}

	int eatmem_open(kshell_t *kshell, int argc,char **argv)
      	 {
           int status;
           proc_t *eatmemtestproc = proc_create("eatmemtest");
           kthread_t *eatmemtestthread = kthread_create(eatmemtestproc,(kthread_func_t)eatmemtest,0,NULL);

           sched_make_runnable(eatmemtestthread);
           KASSERT(kshell != NULL);
           /*dbg(DBG_INIT,"(GRADING2B: eatmem test is invoked)\n");
           */
           while(do_waitpid(-1,0,&status)!=-ECHILD);
           return 0;
         }
	void *umaintest(int argc1, char **argv1)
	{
		char *argv[] = { "/bin/uname","-a" };
		char *envp[] = { NULL };
		int status;

		kernel_execve("/bin/uname", argv, envp);
		proc_t* p = NULL;
		/*while (do_waitpid(-1,0,&status) != -ECHILD);*/
		list_iterate_begin(&curproc->p_children,p,proc_t,p_child_link){
		do_waitpid(p->p_pid,0,&p->p_status);
	}list_iterate_end();

	return 0;
	}

	int uname_open(kshell_t *kshell, int argc,char **argv)
      	 {
           int status;
           proc_t *umaintestproc = proc_create("unametest");
           kthread_t *umaintestthread = kthread_create(umaintestproc,(kthread_func_t)umaintest,0,NULL);

           sched_make_runnable(umaintestthread);
           KASSERT(kshell != NULL);
           /*dbg(DBG_INIT,"(GRADING2B: uname test is invoked)\n");
           */
           while(do_waitpid(-1,0,&status)!=-ECHILD);
           return 0;
         }




        /* int faber_fs_thread(kshell_t *kshell, int argc, char **argv)
         {
           KASSERT(kshell != NULL);
           dbg(DBG_INIT,"(GRADING2C: Faber Fs thread test is invoked)\n");
           faber_fs_thread_test(kshell,0,NULL);
           return 0;
         }


         int faber_directory(kshell_t *kshell, int argc, char **argv)
         {
             KASSERT(kshell != NULL);
             dbg(DBG_INIT,"(GRADING2C: Faber fs directory test is invoked)\n");
             faber_directory_test(kshell,0,NULL);
             return 0;
         }*/


#endif
/**
 * This is the first real C function ever called. It performs a lot of
 * hardware-specific initialization, then creates a pseudo-context to
 * execute the bootstrap function in.
 */
void
kmain()
{
        GDB_CALL_HOOK(boot);

        dbg_init();
        dbgq(DBG_CORE, "Kernel binary:\n");
        dbgq(DBG_CORE, "  text: 0x%p-0x%p\n", &kernel_start_text, &kernel_end_text);
        dbgq(DBG_CORE, "  data: 0x%p-0x%p\n", &kernel_start_data, &kernel_end_data);
        dbgq(DBG_CORE, "  bss:  0x%p-0x%p\n", &kernel_start_bss, &kernel_end_bss);

        page_init();

        pt_init();
        slab_init();
        pframe_init();

        acpi_init();
        apic_init();
        pci_init();
        intr_init();

        gdt_init();

        /* initialize slab allocators */
#ifdef __VM__
        anon_init();
        shadow_init();
#endif
        vmmap_init();
        proc_init();
        kthread_init();

#ifdef __DRIVERS__
        bytedev_init();
        blockdev_init();
#endif

        void *bstack = page_alloc();
        pagedir_t *bpdir = pt_get();
        KASSERT(NULL != bstack && "Ran out of memory while booting.");
        /* This little loop gives gdb a place to synch up with weenix.  In the
         * past the weenix command started qemu was started with -S which
         * allowed gdb to connect and start before the boot loader ran, but
         * since then a bug has appeared where breakpoints fail if gdb connects
         * before the boot loader runs.  See
         *
         * https://bugs.launchpad.net/qemu/+bug/526653
         *
         * This loop (along with an additional command in init.gdb setting
         * gdb_wait to 0) sticks weenix at a known place so gdb can join a
         * running weenix, set gdb_wait to zero  and catch the breakpoint in
         * bootstrap below.  See Config.mk for how to set GDBWAIT correctly.
         *
         * DANGER: if GDBWAIT != 0, and gdb is not running, this loop will never
         * exit and weenix will not run.  Make SURE the GDBWAIT is set the way
         * you expect.
         */
        while (gdb_wait) ;
        context_setup(&bootstrap_context, bootstrap, 0, NULL, bstack, PAGE_SIZE, bpdir);
        context_make_active(&bootstrap_context);

        panic("\nReturned to kmain()!!!\n");
}

/**
 * Clears all interrupts and halts, meaning that we will never run
 * again.
 */
static void
hard_shutdown()
{
#ifdef __DRIVERS__
        vt_print_shutdown();
#endif
        __asm__ volatile("cli; hlt");

}

/**
 * This function is called from kmain, however it is not running in a
 * thread context yet. It should create the idle process which will
 * start executing idleproc_run() in a real thread context.  To start
 * executing in the new process's context call context_make_active(),
 * passing in the appropriate context. This function should _NOT_
 * return.
 *
 * Note: Don't forget to set curproc and curthr appropriately.
 *
 * @param arg1 the first argument (unused)
 * @param arg2 the second argument (unused)
 */
static void *
bootstrap(int arg1, void *arg2)
{
       /* If the next line is removed/altered in your submission, 20 points will be deducted. */
        dbgq(DBG_CORE, "SIGNATURE: 53616c7465645f5fd0f5f4e9adc70694ad12fcfaae6423bd5d01ca122cf44b611898d35ebf9b3fab4a0fbdefab9ecc12\n");
        /* necessary to finalize page table information */
        /*Cody Modifying*/
        pt_template_init();
        proc_t *p=proc_create("idle");
        kthread_t *t=kthread_create(p,idleproc_run,NULL,NULL);
        curproc=p;
        curthr=t;
        KASSERT(NULL!=curproc);
        dbg(DBG_PRINT,"(GRADING1A 1.a)\n");
        KASSERT(PID_IDLE == curproc->p_pid);
        dbg(DBG_PRINT,"(GRADING1A 1.a)\n");
        KASSERT(NULL != curthr);
        dbg(DBG_PRINT,"(GRADING1A 1.a)\n");

        context_make_active(&curthr->kt_ctx);

    /*    NOT_YET_IMPLEMENTED("PROCS: bootstrap");*/

        /*panic("weenix returned to bootstrap()!!! BAD!!!\n");*/
        return NULL;
}

/**
 * Once we're inside of idleproc_run(), we are executing in the context of the
 * first process-- a real context, so we can finally begin running
 * meaningful code.
 *
 * This is the body of process 0. It should initialize all that we didn't
 * already initialize in kmain(), launch the init process (initproc_run),
 * wait for the init process to exit, then halt the machine.
 *
 * @param arg1 the first argument (unused)
 * @param arg2 the second argument (unused)
 */
static void *
idleproc_run(int arg1, void *arg2)
{
        int status;
        pid_t child;

        /* create init proc */
        kthread_t *initthr = initproc_create();
        init_call_all();
        GDB_CALL_HOOK(initialized);

        /* Create other kernel threads (in order) */

#ifdef __VFS__
        /* Once you have VFS remember to set the current working directory
         * of the idle and init processes */
	/*NOT_YET_IMPLEMENTED("VFS: idleproc_run");*/

	curproc->p_cwd = vfs_root_vn;
	initthr->kt_proc->p_cwd = vfs_root_vn;

	vref(vfs_root_vn);
	vref(vfs_root_vn);

        /* Here you need to make the null, zero, and tty devices using mknod */
        /* You can't do this until you have VFS, check the include/drivers/dev.h
         * file for macros with the device ID's you will need to pass to mknod */
        /*NOT_YET_IMPLEMENTED("VFS: idleproc_run");*/

	do_mkdir("/dev");
	do_mknod("/dev/null", S_IFCHR, MKDEVID(1,0));
	do_mknod("/dev/zero", S_IFCHR, MKDEVID(1,1));
	do_mknod("/dev/tty0", S_IFCHR, MKDEVID(2,0));
        /*do_mknod("/dev/tty1", S_IFCHR, MKDEVID(2,1));*/

#endif

        /* Finally, enable interrupts (we want to make sure interrupts
         * are enabled AFTER all drivers are initialized) */
        intr_enable();

        /* Run initproc */
        sched_make_runnable(initthr);
        /* Now wait for it */
        child = do_waitpid(-1, 0, &status);
        KASSERT(PID_INIT == child);

#ifdef __MTP__
        kthread_reapd_shutdown();
#endif


#ifdef __SHADOWD__
        /* wait for shadowd to shutdown */
        shadowd_shutdown();
#endif

#ifdef __VFS__
        /* Shutdown the vfs: */
        dbg_print("weenix: vfs shutdown...\n");
        vput(curproc->p_cwd);
        if (vfs_shutdown())
                panic("vfs shutdown FAILED!!\n");

#endif

        /* Shutdown the pframe system */
#ifdef __S5FS__
        pframe_shutdown();
#endif

        dbg_print("\nweenix: halted cleanly!\n");
        GDB_CALL_HOOK(shutdown);
        hard_shutdown();
        dbg(DBG_PRINT,"*****************************************Leaving idleproc_run\n");
        return NULL;
}

/**
 * This function, called by the idle process (within 'idleproc_run'), creates the
 * process commonly refered to as the "init" process, which should have PID 1.
 *
 * The init process should contain a thread which begins execution in
 * initproc_run().
 *
 * @return a pointer to a newly created thread which will execute
 * initproc_run when it begins executing
 */
static kthread_t *
initproc_create(void)
{
        proc_t *initprocess;
        initprocess=proc_create("initproc");
        KASSERT(NULL != initprocess/* pointer to the "init" process */);
        dbg(DBG_PRINT,"(GRADING1A 1.b)\n");

        KASSERT(PID_INIT ==initprocess->p_pid /* pointer to the "init" process */);
        dbg(DBG_PRINT,"(GRADING1A 1.b)\n");

        kthread_t *initthread=kthread_create(initprocess,initproc_run,NULL,NULL);
        KASSERT(/* pointer to the thread for the "init" process */ initthread!= NULL);
        dbg(DBG_PRINT,"(GRADING1A 1.b)\n");

      /*  NOT_YET_IMPLEMENTED("PROCS: initproc_create"); */
        return initthread;
}

/**
 * The init thread's function changes depending on how far along your Weenix is
 * developed. Before VM/FI, you'll probably just want to have this run whatever
 * tests you've written (possibly in a new process). After VM/FI, you'll just
 * exec "/sbin/init".
 *
 * Both arguments are unused.
 *
 * @param arg1 the first argument (unused)
 * @param arg2 the second argument (unused)
 */
static void *
initproc_run(int arg1, void *arg2)
{

  /*     NOT_YET_IMPLEMENTED("PROCS: initproc_run");


        return NULL; */
    int status;
/*    pid_t proc_id;

    proc_t *vfs_test;
    vfs_test = proc_create("vfs_test");
    KASSERT(NULL != vfs_test pointer to the "init" process);
    dbg(DBG_PRINT,"(GRADING vfs test should not be NULL)\n");

    kthread_t *vfstest_thread=kthread_create(vfs_test,vfstest_main,0,NULL);
    KASSERT( pointer to the thread for the "init" processvfstest_thread!= NULL);
    dbg(DBG_PRINT,"(GRADING vfs thread create successfully)\n");

    sched_make_runnable(vfstest_thread);
    while (do_waitpid(vfs_test->p_pid,0,&status) != -ECHILD);
    */
  /*  int status;

    proc_t *vfstestproc = proc_create("vfstest");
    kthread_t *vfstestthread = kthread_create(vfstestproc,vfstest_main,1,NULL);

    sched_make_runnable(vfstestthread);
   KASSERT(kshell != NULL);
    dbg(DBG_INIT,"(GRADING2B: Vfs test is invoked)\n");
    while(do_waitpid(-1,0,&status)!=-ECHILD);
    return 0;
    */

#ifdef __DRIVERS__

  kshell_add_command("faber",faber,"faber tests");
  kshell_add_command("sunghan",sunghan,"Sunghan tests");
  kshell_add_command("sunghan_deadlock",sunghan_deadlock,"Sunghan Deadlock tests");
  kshell_add_command("vfstest",vfstest,"vfs test");
  kshell_add_command("thrtest", faber_fs_thread_test, "Run faber_fs_thread_test().");
  kshell_add_command("dirtest", faber_directory_test, "Run faber_directory_test().");
  kshell_add_command("hellotest", hello_open, "hello world program");
  kshell_add_command("argstest", args_open, "args program");
  kshell_add_command("memtest", mem_open, "memtest program");
  kshell_add_command("eatmem", mem_open, "eatmem program");
  kshell_add_command("uname", uname_open, "unamemem program");
  /*kshell_add_command("foo", do_foo, "invoke do_foo() to print a message...");*/



  /*char *argv[] = { NULL };
  char *envp[] = { NULL };
*/
  /*
  #if CS402INITCHOICE > 0
      #if CS402INITCHOICE > 1
        #if CS402INITCHOICE > 2
          #if CS402INITCHOICE > 3
            #if CS402INITCHOICE > 4
              kernel_execve("/usr/bin/args", argv, envp);
            #else
              kernel_execve("/usr/bin/memtest", argv, envp);
            #endif
          #else
            kernel_execve("/usr/bin/vfstest", argv, envp);
          #endif
        #else
          kernel_execve("/usr/bin/fork-and-wait", argv, envp);
        #endif
      #else
        kernel_execve("/bin/uname", argv, envp);
      #endif
    #else
      kernel_execve("/usr/bin/hello", argv, envp);
    #endif
*/
  /*kernel_execve("/sbin/init", argv, envp);*/

  /*kernel_execve("usr/bin/hello", argv, envp);*/


/*#ifdef __VM__
proc_t* vm = proc_create("VM_Test");
kthread_t* kt = kthread_create(vm,(kthread_func_t)vm_test,0,NULL);
sched_make_runnable(kt);
#endif
  while (do_waitpid(-1,0,&status) != -ECHILD);
*/
  /*do_open('/dev/tty0',O_RDONLY);
  do_open('/dev/tty0',O_WRONLY);
  */
   kshell_t *kshell = kshell_create(0);

  if (NULL == kshell) panic("init: Couldn't create kernel shell\n");
  dbg(DBG_PRINT,"(GRADING1B)\n");
  while (kshell_execute_next(kshell));
  kshell_destroy(kshell);


  /*kernel_execve("/sbin/init", argv, envp);*/

#endif
/*dbg(DBG_PRINT,"*****************************************Leaving initproc_run\n");
*/


return NULL;




}