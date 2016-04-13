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

#include "globals.h"
#include "errno.h"

#include "util/string.h"
#include "util/debug.h"

#include "mm/mman.h"
#include "mm/page.h"
#include "mm/mm.h"
#include "mm/kmalloc.h"

#include "proc/proc.h"

#include "vm/vmmap.h"

#include "api/access.h"
#include "api/syscall.h"

/* copy_to_user and copy_from_user are used to copy to and from the
 * user space of the current process.  They first check that the range
 * of addresses has valid mappings, then call vmmap_read/write.
 */
int copy_from_user(void *kaddr, const void *uaddr, size_t nbytes)
{
        if (!range_perm(curproc, uaddr, nbytes, PROT_READ)) {
                return -EFAULT;
        }
        return vmmap_read(curproc->p_vmmap, uaddr, kaddr, nbytes);
}

int copy_to_user(void *uaddr, const void *kaddr, size_t nbytes)
{
        if (!range_perm(curproc, uaddr, nbytes, PROT_WRITE)) {
                return -EFAULT;
        }
        return vmmap_write(curproc->p_vmmap, uaddr, kaddr, nbytes);
}

/* Like strndup(), but gets the string from user space, ensuring
 * that the entire string (up to its length) has valid mappings.
 * The resulting string can be freed with kfree().
 * This function may block (as vmmap_read may block)
 */
char *user_strdup(argstr_t *ustr)
{
        char *kstr;
        int ret;

        if (NULL == (kstr = (char *) kmalloc(ustr->as_len + 1))) {
                curthr->kt_errno = ENOMEM;
                return NULL;
        }
        if (0 > (ret = copy_from_user(kstr, ustr->as_str, ustr->as_len + 1))) {
                curthr->kt_errno = -ret;
                kfree(kstr);
                return NULL;
        }
        return kstr;
}

/* Copies in an entire vector of strings from user space, similarly to
 * user_strdup. The vector of strings and each string can be
 * freed (separately) with kfree */
char **user_vecdup(argvec_t *uvec)
{
        char **kvec = NULL;
        argstr_t *temp_kvec = NULL;
        size_t i;
        int ret;

        if (NULL == (temp_kvec = (argstr_t *) kmalloc((uvec->av_len + 1) * sizeof(argstr_t)))) {
                ret = -ENOMEM;
                goto fail;
        }
        if (NULL == (kvec = (char **) kmalloc((uvec->av_len + 1) * sizeof(char *)))) {
                ret = -ENOMEM;
                goto fail;
        }
        /* Copy over the array of argstrs */
        if (0 > (ret = copy_from_user(temp_kvec, uvec->av_vec,
                                      (uvec->av_len + 1) * sizeof(argstr_t)))) {
                goto fail;
        }

        /* For each arstr in temp_kvec, user_strdup a copy and put in kvec */
        for (i = 0; i < uvec->av_len; i++) {
                if (NULL == (kvec[i] = user_strdup(&temp_kvec[i]))) {
                        /* Need to clean up all allocated stuff; errno set in strdup */
                        ret = -curthr->kt_errno;
                        goto fail;
                }
        }
        /* Add null entry */
        kvec[uvec->av_len] = NULL;
        kfree(temp_kvec);
        return kvec;

fail:
        if (kvec != NULL) {
                for (i = 0; kvec[i] != NULL; i++) {
                        if (kvec[i] != NULL) {
                                kfree(kvec[i]);
                        }
                }
                kfree(kvec);
        }
        kfree(temp_kvec);

        curthr->kt_errno = -ret;
        return NULL;
}

/*
 * addr_perm checks to see if the address vaddr in the process p is valid
 * for all the operations specifed in perm. (A combination of one or more
 * of PROT_READ, PROT_WRITE, and PROT_EXEC).  You need to find the process's
 * vm_area that contains that virtual address, and verify that the protections
 * allow access.  The page protections need not match the specified permissions
 * exactly, as long as at least the specifed permissions are satisfied.  This
 * function should return 1 on success, and 0 on failure (think of it as
 * anwering the question "does process p have permission perm on address vaddr?")
 */
int
addr_perm(struct proc *p, const void *vaddr, int perm)
{
        /*NOT_YET_IMPLEMENTED("VM: addr_perm");*/

        /* Convert the address to page number

        uint32_t addr = ADDR_TO_PN(vaddr);

         Obtain the process vm_areas that has the specified address

        vmarea_t *req_vmarea = vmmap_lookup(p->p_vmmap, addr);

        Check the permission passed on the address and return

        if((req_vmarea->vma_prot & perm) ==  perm)
        {

          dbg(DBG_PRINT,"(GRADING3B 1 ACC5)\n");
          return 1;
        }
        else
        {
          dbg(DBG_PRINT,"(GRADING3B 1 ACC6)\n");
         return 0;
        }
        */
        return 0;

}

/*
 * range_perm is essentially a version of addr_perm that checks an entire
 * range of addresses (from avaddr to avaddr+len).  Though you will
 * probably want to use your addr_perm() function in your implementation of
 * range_perm, you don't need to check every possible address.  Remember
 * that page protections have, as the name suggests, page granularity.
 * Like addr_perm, this function should return 1 if the range is valid for
 * the given permissions, and 0 otherwise.
 */
int
range_perm(struct proc *p, const void *avaddr, size_t len, int perm)
{
        /*NOT_YET_IMPLEMENTED("VM: range_perm");
        */
        uint32_t startaddr = ADDR_TO_PN(avaddr);
        uint32_t endaddr = ADDR_TO_PN(avaddr) + len;

        /* Obtain the process vm_areas that has the specified start address*/

        vmarea_t *req_vmarea = vmmap_lookup(p->p_vmmap, startaddr);
        vmarea_t *temp_vmarea;
        dbg(DBG_PRINT,"(GRADING3B 1)\n");
        /*
        Check the initial permission for the start address
        if incorrect return 0
        */
      /*  if((req_vmarea->vma_prot & perm) !=  perm)
        {
           dbg(DBG_PRINT,"(GRADING3B 1 ACC1)\n");
           return 0;
        }
        */

        /*
           Check the end address in a loop to see if its greater.
        */
        if(endaddr > req_vmarea->vma_end)
        {
          /*
             Browse through the sorted vmareas
             and check the permission of each
             vmarea
          */

          dbg(DBG_PRINT,"(GRADING3B 1)\n");
          while(endaddr > req_vmarea->vma_end)
          {
            temp_vmarea = list_item(req_vmarea->vma_plink.l_next,vmarea_t,vma_plink);

            /* If the permission is not satisfied return 0*/
            /*if((temp_vmarea->vma_prot & perm) !=  perm)
            {

              dbg(DBG_PRINT,"(GRADING3B 1 ACC3)\n");
              return 0;
            }
            */

            req_vmarea = temp_vmarea;
          }


        }
      /*  else
        {

          dbg(DBG_PRINT,"(GRADING3B 1 ACC4)\n");
          return 1;
        }
        */

        return 1;
}