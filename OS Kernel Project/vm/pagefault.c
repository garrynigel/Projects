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
#include "errno.h"

#include "util/debug.h"

#include "proc/proc.h"

#include "mm/mm.h"
#include "mm/mman.h"
#include "mm/page.h"
#include "mm/mmobj.h"
#include "mm/pframe.h"
#include "mm/pagetable.h"

#include "vm/pagefault.h"
#include "vm/vmmap.h"

/*
 * This gets called by _pt_fault_handler in mm/pagetable.c The
 * calling function has already done a lot of error checking for
 * us. In particular it has checked that we are not page faulting
 * while in kernel mode. Make sure you understand why an
 * unexpected page fault in kernel mode is bad in Weenix. You
 * should probably read the _pt_fault_handler function to get a
 * sense of what it is doing.
 *
 * Before you can do anything you need to find the vmarea that
 * contains the address that was faulted on. Make sure to check
 * the permissions on the area to see if the process has
 * permission to do [cause]. If either of these checks does not
 * pass kill the offending process, setting its exit status to
 * EFAULT (normally we would send the SIGSEGV signal, however
 * Weenix does not support signals).
 *
 * Now it is time to find the correct page (don't forget
 * about shadow objects, especially copy-on-write magic!). Make
 * sure that if the user writes to the page it will be handled
 * correctly.
 *
 * Finally call pt_map to have the new mapping placed into the
 * appropriate page table.
 *
 * @param vaddr the address that was accessed to cause the fault
 *
 * @param cause this is the type of operation on the memory
 *              address which caused the fault, possible values
 *              can be found in pagefault.h
 */
void
handle_pagefault(uintptr_t vaddr, uint32_t cause)
{
      /*  NOT_YET_IMPLEMENTED("VM: handle_pagefault");*/
      /*1. get vfn and vmarea with fault */
      uint32_t vfn=ADDR_TO_PN(vaddr);
      vmarea_t *vma=vmmap_lookup(curproc->p_vmmap,vfn);
      /*2. check permission */
      if((vma->vma_prot & PROT_READ)!=PROT_READ)
      {
        if((vma->vma_prot & PROT_WRITE)!=PROT_WRITE)
        {
          do_exit(EFAULT);
        }
      }
      /*3. find correct page*/
      pframe_t *pf;
      int pagenum=vfn-vma->vma_start+vma->vma_off;
      int writeflag;
      if(cause&FAULT_WRITE){
        dbg(DBG_PRINT,"(GRADING3B 1)\n");
        writeflag=1;
      }
      else{
        dbg(DBG_PRINT,"(GRADING3B 1)\n");
        writeflag=0;
      }
      /*
      int retval=vma->vma_obj->mmo_ops->lookuppage(vma->vma_obj,pagenum,writeflag,&page);
      */
      int retval = pframe_lookup(vma->vma_obj,pagenum,writeflag,&pf);
      KASSERT(pf);
      dbg(DBG_PRINT,"(GRADING3A 5.a) pframe is valid\n");
      KASSERT(pf->pf_addr);
      dbg(DBG_PRINT,"(GRADING3A 5.a) pframe address is valid\n");
      /*if condition check      if() */
      if(retval!=0){
         do_exit(EFAULT);
       }
      /*4. new mapping*/
      /*pagedir_t *pd=curproc->p_pagedir;
      */
      uintptr_t paddr=pt_virt_to_phys((uintptr_t)pf->pf_addr);

      uint32_t ptflags=0;
      uint32_t pdflags=0;

      /*if((cause & FAULT_PRESENT)==FAULT_PRESENT)
      {
      ptflags=PT_PRESENT;
      pdflags=PD_PRESENT;
      }

      if((cause & FAULT_USER)==FAULT_USER)
      {
      ptflags=ptflags|PT_USER;
      pdflags=pdflags|PT_USER;
      }*/


      ptflags=(PT_PRESENT|PT_USER);
      pdflags=(PT_PRESENT|PT_USER);


      if ((cause & FAULT_WRITE)==FAULT_WRITE) {
        dbg(DBG_PRINT,"(GRADING3B 1)\n");
        ptflags=ptflags|PT_WRITE;
        pdflags=pdflags|PT_WRITE;
      }
      int retval1=pt_map(curproc->p_pagedir,(uintptr_t)PAGE_ALIGN_DOWN(vaddr),(uintptr_t)PAGE_ALIGN_DOWN(paddr),pdflags,ptflags);
      /*another if condition check      if()        */
      if(retval1!=0){
         do_exit(EFAULT);
       }



}
