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

/*
 *  FILE: open.c
 *  AUTH: mcc | jal
 *  DESC:
 *  DATE: Mon Apr  6 19:27:49 1998
 */

#include "globals.h"
#include "errno.h"
#include "fs/fcntl.h"
#include "util/string.h"
#include "util/printf.h"
#include "fs/vfs.h"
#include "fs/vnode.h"
#include "fs/file.h"
#include "fs/vfs_syscall.h"
#include "fs/open.h"
#include "fs/stat.h"
#include "util/debug.h"

/* find empty index in p->p_files[] */
int
get_empty_fd(proc_t *p)
{
        int fd;

        for (fd = 0; fd < NFILES; fd++) {
                if (!p->p_files[fd])
                        return fd;
        }

        dbg(DBG_ERROR | DBG_VFS, "ERROR: get_empty_fd: out of file descriptors "
            "for pid %d\n", curproc->p_pid);
        return -EMFILE;
}

/*
 * There a number of steps to opening a file:
 *      1. Get the next empty file descriptor.
 *      2. Call fget to get a fresh file_t.
 *      3. Save the file_t in curproc's file descriptor table.
 *      4. Set file_t->f_mode to OR of FMODE_(READ|WRITE|APPEND) based on
 *         oflags, which can be O_RDONLY, O_WRONLY or O_RDWR, possibly OR'd with
 *         O_APPEND or O_CREAT.
 *      5. Use open_namev() to get the vnode for the file_t.
 *      6. Fill in the fields of the file_t.
 *      7. Return new fd.
 *
 * If anything goes wrong at any point (specifically if the call to open_namev
 * fails), be sure to remove the fd from curproc, fput the file_t and return an
 * error.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EINVAL
 *        oflags is not valid.
 *      o EMFILE
 *        The process already has the maximum number of files open.
 *      o ENOMEM
 *        Insufficient kernel memory was available.
 *      o ENAMETOOLONG
 *        A component of filename was too long.
 *      o ENOENT
 *        O_CREAT is not set and the named file does not exist.  Or, a
 *        directory component in pathname does not exist.
 *      o EISDIR
 *        pathname refers to a directory and the access requested involved
 *        writing (that is, O_WRONLY or O_RDWR is set).
 *      o ENXIO
 *        pathname refers to a device special file and no corresponding device
 *        exists.
 */

int
do_open(const char *filename, int oflags)
{
        /*NOT_YET_IMPLEMENTED("VFS: do_open");*/
        /*       1. Get the next empty file descriptor.*/

        /*dbg(DBG_PRINT,"Entering do_open ******************************************\n");
        dbg(DBG_PRINT,"filename : %s, oflags : %d\n",filename,oflags);
      */
        dbg(DBG_PRINT,"(GRADING2C)\n");
        int freefd=get_empty_fd(curproc);
        if(freefd!= -EMFILE){
          dbg(DBG_PRINT,"(GRADING2C)\n");
        }
        else{

          dbg(DBG_PRINT,"(GRADING2C O3)\n");
          return -EMFILE;
        }

      /*      2. Call fget to get a fresh file_t. */
      int lowflag=(oflags & 003);

      if (lowflag == 3)
      {
        dbg(DBG_PRINT,"(GRADING2C)\n");
        return -EINVAL;
      }

        file_t *f=fget(-1);
        if(f!=NULL){
          dbg(DBG_PRINT,"(GRADING2C)\n");

        }
      /*  else{
          dbg(DBG_PRINT,"(GRADING2C O6)\n");
          return -ENOMEM;
        }
        */
      /*      3. Save the file_t in curproc's file descriptor table.*/

      curproc->p_files[freefd]=f;
        f->f_mode=0;


    /*      4. Set file_t->f_mode to OR of FMODE_(READ|WRITE|APPEND) based on
                 oflags, which can be O_RDONLY, O_WRONLY or O_RDWR, possibly OR'd with
                O_APPEND or O_CREAT.*/
          /*lowflag to determine READ/WRITE/READ&WRITE,
          high flag to determine APPEND*/
        int highflag=(oflags & 0x700);
        if (lowflag == O_RDONLY)
        {
          dbg(DBG_PRINT,"(GRADING2C)\n");
          f->f_mode=FMODE_READ;
        }

        if (lowflag == O_WRONLY)
        {
          dbg(DBG_PRINT,"(GRADING2C)\n");
          f->f_mode=FMODE_WRITE;
        }

        if (lowflag == O_RDWR)
        {
          dbg(DBG_PRINT,"(GRADING2C)\n");
          f->f_mode = (FMODE_READ|FMODE_WRITE);
        }

        if(highflag==O_APPEND || highflag == (O_CREAT|O_APPEND))
        {
          dbg(DBG_PRINT,"(GRADING2C)\n");
          f->f_mode=(f->f_mode|FMODE_APPEND);
        }

      /*  if (f->f_mode==((FMODE_READ)||(FMODE_WRITE)||(FMODE_APPEND))){
            dbg(DBG_PRINT,"mode set\n");
          }
        else{
            dbg(DBG_PRINT,"mode set %d lowflag %d highflag %d******************************************",f->f_mode,lowflag,highflag);
            dbg(DBG_PRINT,"EINVAL :Leaving do_open ******************************************");
            return -EINVAL;
          }
        */

      /* 5. Use open_namev() to get the vnode for the file_t. */
      vnode_t *v;

      int res = open_namev(filename, oflags,&v,NULL);
      if(res<0){
                dbg(DBG_PRINT,"(GRADING2C)\n");
                fput(f);
                /*curproc->p_files[freefd] = NULL;
                */

                return res;
              }
      if (S_ISDIR(v->vn_mode) && ((lowflag == 1)|| (lowflag == 2)))
      {
        dbg(DBG_PRINT,"(GRADING2C)\n");
        fput(f);
        vput(v);
        /*curproc->p_files[freefd] = NULL;

*/
        /*dbg(DBG_ERROR,"Writing a directory error Leaving do_open *************************************\n");
        */
        return -EISDIR;

      }
      f->f_vnode = v;
      /* 6. Fill in the fields of the file_t. */
      f->f_pos=0;
      /*dbg(DBG_PRINT,"Leaving do_open ******************************************\n");
      */
      /* 7. Return new fd. */
      return freefd;
}
