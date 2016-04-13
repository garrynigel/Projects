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
 *  FILE: vfs_syscall.c
 *  AUTH: mcc | jal
 *  DESC:
 *  DATE: Wed Apr  8 02:46:19 1998
 *  $Id: vfs_syscall.c,v 1.10 2014/12/22 16:15:17 william Exp $
 */

#include "kernel.h"
#include "errno.h"
#include "globals.h"
#include "fs/vfs.h"
#include "fs/file.h"
#include "fs/vnode.h"
#include "fs/vfs_syscall.h"
#include "fs/open.h"
#include "fs/fcntl.h"
#include "fs/lseek.h"
#include "mm/kmalloc.h"
#include "util/string.h"
#include "util/printf.h"
#include "fs/stat.h"
#include "util/debug.h"

/* To read a file:
 *      o fget(fd)
 *      o call its virtual read fs_op
 *      o update f_pos
 *      o fput() it
 *      o return the number of bytes read, or an error
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd is not a valid file descriptor or is not open for reading.
 *      o EISDIR
 *        fd refers to a directory.
 *
 * In all cases, be sure you do not leak file refcounts by returning before
 * you fput() a file that you fget()'ed.
 */
int
do_read(int fd, void *buf, size_t nbytes)
{
        /*NOT_YET_IMPLEMENTED("VFS: do_read");
        */
        int readbytes;
        /*dbg(DBG_PRINT,"Entering do_read ****************************************\n");
        */
        file_t *f = fget(fd);
        if (NULL == f || (f->f_mode & FMODE_READ) != FMODE_READ)
        {
          dbg(DBG_PRINT,"(GRADING2C)\n");
          return -EBADF;
        }

        if (S_ISDIR(f->f_vnode->vn_mode))
        {
          fput(f);
          /*dbg(DBG_PRINT,"(GRADING2C) ****************************************\n");
          */
          dbg(DBG_PRINT,"(GRADING2C)\n");
          return -EISDIR;
        }

        readbytes = f->f_vnode->vn_ops->read(f->f_vnode,f->f_pos,buf,nbytes);
        f->f_pos = f->f_pos + (off_t)readbytes;

        fput(f);
        /*dbg(DBG_PRINT,"Leaving do_read ****************************************\n");
        */
        return readbytes;
        /*return -1;
        */
}

/* Very similar to do_read.  Check f_mode to be sure the file is writable.  If
 * f_mode & FMODE_APPEND, do_lseek() to the end of the file, call the write
 * fs_op, and fput the file.  As always, be mindful of refcount leaks.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd is not a valid file descriptor or is not open for writing.
 */
int
do_write(int fd, const void *buf, size_t nbytes)
{
        /*NOT_YET_IMPLEMENTED("VFS: do_write");
        */

        int writebytes;
        /*dbg(DBG_PRINT,"Entering do_write ****************************************\n");
        */
        file_t *f = fget(fd);
        if (NULL == f)
        {
          /*dbg(DBG_PRINT,"EBADF Leaving do_write ****************************************\n");
          */
          dbg(DBG_PRINT,"(GRADING2C)\n");
          return -EBADF;
        }
       if ((f->f_mode & FMODE_WRITE) == FMODE_WRITE)
       {
         dbg(DBG_PRINT,"(GRADING2C)\n");

           if ((f->f_mode & FMODE_APPEND) == FMODE_APPEND)
           {
             dbg(DBG_PRINT,"(GRADING2C)\n");
             do_lseek(fd,0,SEEK_END);
           }
           writebytes = f->f_vnode->vn_ops->write(f->f_vnode,f->f_pos,buf,nbytes);
           if(writebytes >= 0){

           KASSERT((S_ISCHR(f->f_vnode->vn_mode)) || (S_ISBLK(f->f_vnode->vn_mode)) || ((S_ISREG(f->f_vnode->vn_mode)) && (f->f_pos <= f->f_vnode->vn_len)));
           dbg(DBG_PRINT,"\n(GRADING2A 3.a)\n");

           f->f_pos = f->f_pos + (off_t)writebytes;
           }



           fput(f);

           /*dbg(DBG_PRINT,"Leaving do_write ****************************************\n");
           */
           return writebytes;

       }
       else
       {

                fput(f);
                /*dbg(DBG_PRINT,"EBADF Leaving do_write ****************************************\n");
                */
                dbg(DBG_PRINT,"(GRADING2C)\n");
                return -EBADF;

       }

}

/*
 * Zero curproc->p_files[fd], and fput() the file. Return 0 on success
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd isn't a valid open file descriptor.
 */
int
do_close(int fd)
{
        /*NOT_YET_IMPLEMENTED("VFS: do_close");
        */
        /*dbg(DBG_PRINT,"Entering do_close ****************************************\n");
        */
        dbg(DBG_PRINT,"(GRADING2C)\n");
        if (fd < 0 || fd >= NFILES || curproc->p_files[fd] ==  NULL)
             {
              /*dbg(DBG_PRINT,"Leaving do_close ****************************************\n");*/
              dbg(DBG_PRINT,"(GRADING2C)\n");
              return -EBADF;
             }
        fput(curproc->p_files[fd]);
        curproc->p_files[fd] = NULL;
        /*dbg(DBG_PRINT,"Leaving do_close ****************************************\n");
        */
        return 0;
}

/* To dup a file:
 *      o fget(fd) to up fd's refcount
 *      o get_empty_fd()
 *      o point the new fd to the same file_t* as the given fd
 *      o return the new file descriptor
 *
 * Don't fput() the fd unless something goes wrong.  Since we are creating
 * another reference to the file_t*, we want to up the refcount.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd isn't an open file descriptor.
 *      o EMFILE
 *        The process already has the maximum number of file descriptors open
 *        and tried to open a new one.
 */
int
do_dup(int fd)
{
        /*NOT_YET_IMPLEMENTED("VFS: do_dup");
        return -1;
        */
        /*dbg(DBG_PRINT,"Entering do_dup ****************************************\n");
        */
        int newfd;
        dbg(DBG_PRINT,"(GRADING2C)\n");
        if (fd == -1)
        {
          /*dbg(DBG_PRINT," EBADF Leaving do_dup ****************************************\n");
          */
          dbg(DBG_PRINT,"(GRADING2C)\n");
          return -EBADF;
        }
        file_t *f = fget(fd);
        if (NULL == f){
          dbg(DBG_PRINT,"(GRADING2C)\n");
          /*dbg(DBG_PRINT," EBADF Leaving do_dup ****************************************\n");*/
          return -EBADF;
        }
        newfd = get_empty_fd(curproc);
        /*if( newfd == -EMFILE)
        {
          dbg(DBG_PRINT,"(GRADING2C V12)\n");
          fput(f);
          dbg(DBG_PRINT," EMFILE Leaving do_dup ****************************************\n");

          return -EMFILE;
        }
        */
        curproc->p_files[newfd] = f;
        /*dbg(DBG_PRINT," Leaving do_dup ****************************************\n");*/
        return newfd;


}

/* Same as do_dup, but insted of using get_empty_fd() to get the new fd,
 * they give it to us in 'nfd'.  If nfd is in use (and not the same as ofd)
 * do_close() it first.  Then return the new file descriptor.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        ofd isn't an open file descriptor, or nfd is out of the allowed
 *        range for file descriptors.
 */
int
do_dup2(int ofd, int nfd)
{
    /*dbg(DBG_PRINT," Entering do_dup2 ****************************************\n");
    */
    if (nfd < 0 || nfd >= NFILES || ofd == -1)
    {
      /*dbg(DBG_PRINT,"ofd %d nfd %d EBADF Leaving do_dup2 ****************************************\n",ofd,nfd);*/
      dbg(DBG_PRINT,"(GRADING2C)\n");
      return -EBADF;
    }
    file_t *f = fget(ofd);
    dbg(DBG_PRINT,"(GRADING2C)\n");
    if (NULL == f){
         dbg(DBG_PRINT,"(GRADING2C)\n");
         /*dbg(DBG_PRINT,"ofd %d nfd %d EBADF Leaving do_dup2 ****************************************\n",ofd,nfd);
         */
         return -EBADF;
       }
   if(nfd == ofd)
   {
     dbg(DBG_PRINT,"(GRADING2C)\n");
     fput(f);
     return nfd;
   }

    if (curproc->p_files[nfd] != NULL && nfd != ofd)
    {
      dbg(DBG_PRINT,"(GRADING2C)\n");
      do_close(nfd);
    }

    curproc->p_files[nfd] = f;
    /*dbg(DBG_PRINT," Leaving do_dup2 ****************************************\n");*/
    return nfd;

}

/*
 * This routine creates a special file of the type specified by 'mode' at
 * the location specified by 'path'. 'mode' should be one of S_IFCHR or
 * S_IFBLK (you might note that mknod(2) normally allows one to create
 * regular files as well-- for simplicity this is not the case in Weenix).
 * 'devid', as you might expect, is the device identifier of the device
 * that the new special file should represent.
 *
 * You might use a combination of dir_namev, lookup, and the fs-specific
 * mknod (that is, the containing directory's 'mknod' vnode operation).
 * Return the result of the fs-specific mknod, or an error.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EINVAL
 *        mode requested creation of something other than a device special
 *        file.
 *      o EEXIST
 *        path already exists.
 *      o ENOENT
 *        A directory component in path does not exist.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
 int
 do_mknod(const char *path, int mode, unsigned devid)
 {
         /*NOT_YET_IMPLEMENTED("VFS: do_mknod");*/

 	/*dbg(DBG_PRINT,"Entering do_mknode\n");
  */

 	int retvar,errcode;
 	const char *name;
 	size_t len = 0;

 	vnode_t *res_vnode;
  vnode_t *result_vnode;
  dbg(DBG_PRINT,"(GRADING2C)\n");

 	if(mode!=S_IFCHR && mode!=S_IFBLK)
 	{
 		dbg(DBG_PRINT,"Mode requested is Invalid\n");

    dbg(DBG_PRINT,"(GRADING2C V19)\n");
 		return -EINVAL;
 	}

 	retvar=dir_namev(path,&len,&name,NULL,&res_vnode);

 	/*if(errcode < 0)
 	{
    dbg(DBG_PRINT,"(GRADING2C V20)\n");
 		dbg(DBG_PRINT,"dir_namev error\n");

 		return errcode;
 	}
  */

 	retvar=lookup(res_vnode,name,len,&result_vnode);

 if(retvar == 0)
 	{
    dbg(DBG_PRINT,"(GRADING2C)\n");
 		dbg(DBG_PRINT,"Path already exists\n");

 		vput(result_vnode);

     	vput(res_vnode);
 		return -EEXIST;
 	}


 	/* A dir component in the path does not exist*/
 	if(retvar == -ENOENT)
 	{
 		KASSERT(NULL != res_vnode->vn_ops->mknod);
 		dbg(DBG_PRINT,"(GRADING2A 3.b)\n");
 		errcode = res_vnode->vn_ops->mknod(res_vnode, name, len, mode, devid);
    vput(res_vnode);
    return errcode;
 	}

  /*if(retvar < 0)
  {

    vput(result_vnode);
    return retvar;

  }
  */
    vput(result_vnode);
  	return	retvar;
 	/*else
  {
    dbg(DBG_PRINT,"(GRADING2C V22)\n");
 		return errcode;
  }
  */

/*
 	dbg(DBG_PRINT,"Exiting do_mknod\n");
         return -1;
  */
 }

/* Use dir_namev() to find the vnode of the dir we want to make the new
 * directory in.  Then use lookup() to make sure it doesn't already exist.
 * Finally call the dir's mkdir vn_ops. Return what it returns.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EEXIST
 *        path already exists.
 *      o ENOENT
 *        A directory component in path does not exist.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
 int
 do_mkdir(const char *path)
 {
         /*NOT_YET_IMPLEMENTED("VFS: do_mkdir");*/

 	/*dbg(DBG_PRINT,"Entering do_mkdir\n");
  */

 	int errcode;
 	const char *name;
 	size_t len = 0;

 	vnode_t *res_vnode;
  vnode_t *result_vnode;
  dbg(DBG_PRINT,"(GRADING2C)\n");
 	errcode=dir_namev(path,&len,&name,NULL,&res_vnode);

 	if(errcode < 0)
 	{
 		dbg(DBG_PRINT,"dir_namev error\n");

    dbg(DBG_PRINT,"(GRADING2C)\n");

 		return errcode;
 	}

  if(!S_ISDIR(res_vnode->vn_mode))
  {
    dbg(DBG_PRINT,"(GRADING2C) Not a directory as lookup() null or S_ISDIR null\n");
    dbg(DBG_PRINT,"(GRADING2C 2)\n");
    vput(res_vnode);
    return -ENOTDIR;
  }



 	errcode=lookup(res_vnode,name,len,&result_vnode);

 	if(errcode == 0)
 	{
    dbg(DBG_PRINT,"(GRADING2C)\n");
    /*
 		dbg(DBG_PRINT,"Path already exists\n");
    */
 		vput(result_vnode);
    vput(res_vnode);

 		return -EEXIST;
 	}

 	/* A dir does not exist so create it*/
 	if(errcode == -ENOENT)
 	{
 		KASSERT(NULL != res_vnode->vn_ops->mkdir);
 		dbg(DBG_PRINT,"(GRADING2A 3.c)\n");
 		int retvar;
 		retvar = res_vnode->vn_ops->mkdir(res_vnode, name, len);
    vput(res_vnode);
 		return	retvar;
 	}
 	else
  {
    dbg(DBG_PRINT,"(GRADING2C)\n");
    vput(res_vnode);
 		return errcode;
  }


 	/*dbg(DBG_PRINT,"Exiting do_mkdir\n");*/
         return -1;
 }

/* Use dir_namev() to find the vnode of the directory containing the dir to be
 * removed. Then call the containing dir's rmdir v_op.  The rmdir v_op will
 * return an error if the dir to be removed does not exist or is not empty, so
 * you don't need to worry about that here. Return the value of the v_op,
 * or an error.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EINVAL
 *        path has "." as its final component.
 *      o ENOTEMPTY
 *        path has ".." as its final component.
 *      o ENOENT
 *        A directory component in path does not exist.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
 int
 do_rmdir(const char *path)
 {
         /*NOT_YET_IMPLEMENTED("VFS: do_rmdir");*/

 	/*dbg(DBG_PRINT,"Entering do_rmdir\n");
*/

 	int errcode;
 	const char *name = NULL;
 	size_t len = 0;

 	vnode_t *res_vnode;
	vnode_t *result_vnode;

  dbg(DBG_PRINT,"(GRADING2C)\n");

  errcode=dir_namev(path,&len,&name,NULL,&res_vnode);

  if(errcode < 0)
 	{
 		dbg(DBG_PRINT,"dir_namev error\n");

    dbg(DBG_PRINT,"(GRADING2C)\n");
 		return errcode;
 	}

    if(name_match(".",name,len))
    {
      vput(res_vnode);
      /*dbg(DBG_PRINT,". as the final component\n");*/
      dbg(DBG_PRINT,"(GRADING2C)\n");
      return -EINVAL;
    }

    if(name_match("..",name,len))
    {
      vput(res_vnode);
      dbg(DBG_PRINT,"(GRADING2C)\n");
      /*dbg(DBG_PRINT,".. as the final component\n");
      */
      return -ENOTEMPTY;
    }

  if(/*(dir->vn_ops->lookup == NULL) ||*/ !S_ISDIR(res_vnode->vn_mode))
	{
		dbg(DBG_PRINT,"(GRADING2C) Not a directory as lookup() null or S_ISDIR null\n");
		dbg(DBG_PRINT,"(GRADING2C 2)\n");
    vput(res_vnode);
		return -ENOTDIR;
	}

        /*vput(res_vnode);

        errcode=lookup(res_vnode,name,len,&result_vnode);

	dbg(DBG_PRINT,"\n dir = %d\n",res_vnode->vn_vno);

	dbg(DBG_PRINT,"\nname = %s\n",name);

	dbg(DBG_PRINT,"\nERRCODE = %d\n",errcode);
	*/
/*	if(errcode == -ENOENT)
        {
          dbg(DBG_PRINT," ENOENT Leaving rmdir ****************************************\n");

          dbg(DBG_PRINT,"(GRADING2C)\n");
          return -ENOENT;
        }

	if(errcode == -ENOTDIR)
        {
          dbg(DBG_PRINT,"(GRADING2C)\n");
          dbg(DBG_PRINT,"ENOTDIR Leaving rmdir  ****************************************\n");
          return -ENOTDIR;
        }
*/
	/*if(errcode < 0)
 	{
 		dbg(DBG_PRINT,"dir_namev error\n");

    dbg(DBG_PRINT,"(GRADING2C V32)\n");
 		return errcode;
 	}
  */

 	KASSERT(NULL != res_vnode->vn_ops->rmdir);
 	dbg(DBG_PRINT,"(GRADING2A 3.d)\n");
  dbg(DBG_PRINT,"(GRADING2C)\n");

	int retvar;
 	retvar = res_vnode->vn_ops->rmdir(res_vnode, name, len);
	vput(res_vnode);

  /*
	dbg(DBG_PRINT,"Exiting do_rmdir\n");
  */

	return	retvar;


 }

/*
 * Same as do_rmdir, but for files.         n
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EISDIR
 *        path refers to a directory.
 *      o ENOENT
 *        A component in path does not exist.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int
do_unlink(const char *path)
{
        /*NOT_YET_IMPLEMENTED("VFS: do_unlink");
        return -1;
        */
        int retvar;
        size_t namelen;
        vnode_t *res_vnode = NULL;
        vnode_t *result_vnode = NULL;
        const char *name;
        dbg(DBG_PRINT,"(GRADING2C)\n");

        retvar = dir_namev(path, &namelen, &name,NULL, &res_vnode);

	/*if(retvar == -ENOENT)
        {
          dbg(DBG_PRINT," ENOENT Leaving do_unlink ****************************************\n");

          dbg(DBG_PRINT,"(GRADING2C V35)\n");
          return -ENOENT;
        }

        if(retvar == -ENOTDIR)
        {
          dbg(DBG_PRINT,"(GRADING2C V36)\n");
          dbg(DBG_PRINT," ENOTDIR Leaving do_unlink ****************************************\n");

          return -ENOTDIR;
        }

	if(retvar < 0)
 	{

 		dbg(DBG_PRINT,"dir_namev error\n");

    dbg(DBG_PRINT,"(GRADING2C V37)\n");
 		return retvar;
 	}
  */

 	retvar=lookup(res_vnode,name,namelen,&result_vnode);

	if(retvar < 0)
 	{
		vput(res_vnode);
 		/*dbg(DBG_PRINT,"lookup error\n");
    */

    dbg(DBG_PRINT,"(GRADING2C)\n");
 		return retvar;
 	}

	if(S_ISDIR(result_vnode->vn_mode))
        {
          dbg(DBG_PRINT,"(GRADING2C)\n");
	        vput(result_vnode);
	        vput(res_vnode);
          /*dbg(DBG_PRINT," EISDIR Leaving do_unlink ****************************************\n");
          */
          return -EISDIR;
        }



	KASSERT(NULL != res_vnode->vn_ops->unlink);
 	dbg(DBG_PRINT,"(GRADING2A 3.e)\n");
  dbg(DBG_PRINT,"(GRADING2C V40)\n");
  retvar = res_vnode->vn_ops->unlink(res_vnode,name,namelen);
  vput(result_vnode);
	vput(res_vnode);

        return retvar;
}

/* To link:
 *      o open_namev(from)
 *      o dir_namev(to)
 *      o call the destination dir's (to) link vn_ops.
 *      o return the result of link, or an error
 *
 * Remember to vput the vnodes returned from open_namev and dir_namev.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EEXIST
 *        to already exists.
 *      o ENOENT
 *        A directory component in from or to does not exist.
 *      o ENOTDIR
 *        A component used as a directory in from or to is not, in fact, a
 *        directory.
 *      o ENAMETOOLONG
 *        A component of from or to was too long.
 *      o EISDIR
 *        from is a directory.
 */
int
do_link(const char *from, const char *to)
{
        NOT_YET_IMPLEMENTED("VFS: do_link");

        /*dbg(DBG_PRINT,"Entering do_unlink ****************************************\n");

        int retvar_from,retvar_to;
        vnode_t *from_vnode,*to_vnode;
        size_t *name_len;
        const char *name

        retvar_from = open_namev(from, NULL, &from_vnode, NULL);
        retvar_to = dir_namev(to,name_len,&name,NULL,&to_vnode);
        if (retvar_to == 0)
        {
          vput(to_vnode);
          dbg(DBG_PRINT,"EEXIST Leaving do_link ****************************************\n");
          return -EEXIST;
        }

        if (retvar_to == -ENOENT || retvar_from == -ENOENT){
          vput(to_vnode);
          dbg(DBG_PRINT,"ENOENT Leaving do_link ****************************************\n");
          return -ENOENT;
        }
        if (retvar_to == -ENOTDIR || retvar_from == -ENOTDIR){
          vput(to_vnode);
          dbg(DBG_PRINT,"ENOTDIR Leaving do_link ****************************************\n");
          return -ENOTDIR;
        }
        if(retvar_to ==  -ENAMETOOLONG || retvar_from == -ENAMETOOLONG)
        {
          vput(to_vnode)
          dbg(DBG_PRINT,"ENAMETOOLONG Leaving do_link ****************************************\n");
          return -ENAMETOOLONG;
        }

        if( retvar_from == -EISDIR)
        {
          vput(to_vnode)
          dbg(DBG_PRINT,"EISDIR Leaving do_link ****************************************\n");
          return -EISDIR;
        }

        return (to_vnode->vn_ops->link(from_vnode, to_vnode,name,name_len));
        */
        return -1;
}

/*      o link newname to oldname
 *      o unlink oldname
 *      o return the value of unlink, or an error
 *
 * Note that this does not provide the same behavior as the
 * Linux system call (if unlink fails then two links to the
 * file could exist).
 */
int
do_rename(const char *oldname, const char *newname)
{
        NOT_YET_IMPLEMENTED("VFS: do_rename");
        return -1;
      /*
        dbg(DBG_PRINT,"Entering do_rename ****************************************\n");
        int retvar;
        if((retvar = do_link(oldname,newname))< 0)
          {
            dbg(DBG_PRINT,"Error from do_link : Leaving do_rename ****************************************\n");
            return retvar;
          }

        retvar = do_unlink(oldname))
        dbg(DBG_PRINT,"Leaving do_rename ****************************************\n");
        return retvar;
        */


}

/* Make the named directory the current process's cwd (current working
 * directory).  Don't forget to down the refcount to the old cwd (vput()) and
 * up the refcount to the new cwd (open_namev() or vget()). Return 0 on
 * success.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o ENOENT
 *        path does not exist.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 *      o ENOTDIR
 *        A component of path is not a directory.
 */
int
do_chdir(const char *path)
{
        /*NOT_YET_IMPLEMENTED("VFS: do_chdir");
        return -1;
        */
       /*dbg(DBG_PRINT,"Entering do_chdir ****************************************\n");
       */
       int retvar;
      /* size_t namelen;
      const char *name;
       vnode_t *res_vnode;
        retvar = dir_namev(path,&namelen,&name,NULL,&res_vnode);

       */
       /*dbg(DBG_PRINT,"(GRADING2C V41)\n");
       */
       vnode_t *res_vnode;
       retvar = open_namev(path,000,&res_vnode,NULL);
        if (retvar < 0)
        {
          /*vput(res_vnode);
          */
          dbg(DBG_PRINT,"(GRADING2C)\n");
          /*dbg(DBG_PRINT,"retvar %d Leaving do_chdir ****************************************\n",retvar);
          */
          return retvar;
        }
        if(!S_ISDIR(res_vnode->vn_mode))
        {
          vput(res_vnode);
          dbg(DBG_PRINT,"(GRADING2C)\n");
          return -ENOTDIR;
        }
        vput(curproc->p_cwd);
        curproc->p_cwd = res_vnode;
        dbg(DBG_PRINT,"(GRADING2C)\n");
        /*dbg(DBG_PRINT,"Leaving do_chdir ****************************************\n");
      */
        return 0;
}

/* Call the readdir fs_op on the given fd, filling in the given dirent_t*.
 * If the readdir fs_op is successful, it will return a positive value which
 * is the number of bytes copied to the dirent_t.  You need to increment the
 * file_t's f_pos by this amount.  As always, be aware of refcounts, check
 * the return value of the fget and the virtual function, and be sure the
 * virtual function exists (is not null) before calling it.
 *
 * Return either 0 or sizeof(dirent_t), or -errno.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        Invalid file descriptor fd.
 *      o ENOTDIR
 *        File descriptor does not refer to a directory.
 */
int
do_getdent(int fd, struct dirent *dirp)
{
        /*NOT_YET_IMPLEMENTED("VFS: do_getdent");
        return -1;
        */
        int retvar = 0;
        dbg(DBG_PRINT,"(GRADING2C)\n");
        if(fd == -1)
        {
          dbg(DBG_PRINT,"(GRADING2C)\n");
          /*dbg(DBG_PRINT," EBADF : Leaving do_getdent ****************************************\n");
          */
          return -EBADF;
        }
        file_t *f = fget(fd);
        if (NULL == f )
        {
          dbg(DBG_PRINT,"(GRADING2C)\n");
          /*dbg(DBG_PRINT," EBADF : Leaving do_getdent ****************************************\n");
          */
          return -EBADF;
        }
        if (!S_ISDIR(f->f_vnode->vn_mode))
        {
          fput(f);
          dbg(DBG_PRINT,"(GRADING2C)\n");
          /*dbg(DBG_PRINT," ENOTDIR : Leaving do_getdent ****************************************\n");
          */
          return -ENOTDIR;
        }
              retvar = f->f_vnode->vn_ops->readdir(f->f_vnode,f->f_pos,dirp);
              f->f_pos = f->f_pos + retvar;
              fput(f);
              if (retvar == 0)
              {
                dbg(DBG_PRINT,"(GRADING2C)\n");
                return 0;
              }
              return sizeof(dirent_t);

}

/*
 * Modify f_pos according to offset and whence.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd is not an open file descriptor.
 *      o EINVAL
 *        whence is not one of SEEK_SET, SEEK_CUR, SEEK_END; or the resulting
 *        file offset would be negative.
 */
int
do_lseek(int fd, int offset, int whence)
{
      /*  NOT_YET_IMPLEMENTED("VFS: do_lseek");
        return -1;
        */
        int res_offset;
        /*dbg(DBG_PRINT," fd %d offset %d whence %d Entering do_lseek ****************************************\n",fd,offset,whence);
        */dbg(DBG_PRINT,"(GRADING2C)\n");
        if(fd == -1){
          /*dbg(DBG_PRINT," -EBADF : fd %d offset %d whence %d Leaving do_lseek ****************************************\n",fd,offset,whence);
          */dbg(DBG_PRINT,"(GRADING2C)\n");
          return -EBADF;
        }
        if(whence == SEEK_SET || whence == SEEK_CUR || whence == SEEK_END)
        {
            file_t *f =fget(fd);
            dbg(DBG_PRINT,"(GRADING2C)\n");
            if(f == NULL){
              /*dbg(DBG_PRINT," -EBADF : fd %d offset %d whence %d Leaving do_lseek ****************************************\n",fd,offset,whence);
              */dbg(DBG_PRINT,"(GRADING2C)\n");
              return -EBADF;
            }
            if (whence == SEEK_SET)
            {
              dbg(DBG_PRINT,"(GRADING2C)\n");
              res_offset = offset;
            }
            else if(whence == SEEK_END)
            {
              dbg(DBG_PRINT,"(GRADING2C)\n");
              res_offset = f->f_vnode->vn_len + offset;
            }
            else
            {
              dbg(DBG_PRINT,"(GRADING2C)\n");
              res_offset = f->f_pos + offset;
            }
            fput(f);
            if(res_offset < 0){
              dbg(DBG_PRINT,"(GRADING2C)\n");
              f->f_pos = 0;
              return -EINVAL;
            }

            f->f_pos = res_offset;

            /*dbg(DBG_PRINT,"fd %d offset %d whence %d Leaving do_lseek ****************************************\n",fd,offset,whence);
             */
            return f->f_pos;

        }
        else
        {
          dbg(DBG_PRINT,"(GRADING2C)\n");
          return -EINVAL;
        }

}

/*
 * Find the vnode associated with the path, and call the stat() vnode operation.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o ENOENT
 *        A component of path does not exist.
 *      o ENOTDIR
 *        A component of the path prefix of path is not a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int
do_stat(const char *path, struct stat *buf)
{
      /*  NOT_YET_IMPLEMENTED("VFS: do_stat");
        return -1;
        */
    /*dbg(DBG_PRINT,"Entering do_stat ****************************************\n");
     */

    if (strlen(path) == 0)
    {
      /*dbg(DBG_PRINT,"EINVAL Leaving do_stat ****************************************\n");
      */
      dbg(DBG_PRINT,"(GRADING2C)\n");
      return -EINVAL;
    }
    int retvar;
    size_t namelen;
    const char *name;
    struct vnode *res_vnode,*result_vnode;
    dbg(DBG_PRINT,"(GRADING2C)\n");



    retvar = dir_namev(path,&namelen,&name,NULL,&res_vnode);

    /*if(retvar < 0)
 	{
    dbg(DBG_PRINT,"(GRADING2C V61)\n");
 		return retvar;
 	}
  */

    retvar=lookup(res_vnode,name,namelen,&result_vnode);

    if (retvar == -ENOENT)
    {
      dbg(DBG_PRINT,"(GRADING2C)\n");
      /*dbg(DBG_PRINT,"ENOENT: Leaving do_stat ****************************************\n");
      */

      vput(res_vnode);
      return -ENOENT;
    }

    /*if(retvar == -ENOTDIR)
    {
      dbg(DBG_PRINT,"ENOTDIR: Leaving do_stat ****************************************\n");

      dbg(DBG_PRINT,"(GRADING2C V63)\n");
      return -ENOTDIR;
    }
    */

    if (retvar < 0)
    {
      dbg(DBG_PRINT,"(GRADING2C V64)\n");
      vput(result_vnode);

      return retvar;
    }
     KASSERT(NULL != res_vnode->vn_ops->stat);
     dbg(DBG_PRINT,"(GRADING2A 3.f)\n");

     retvar = res_vnode->vn_ops->stat(result_vnode,buf);

      vput(res_vnode);
      vput(result_vnode);
      /*dbg(DBG_PRINT,"Leaving do_stat ****************************************\n");
    */

     return retvar;
}

#ifdef __MOUNTING__
/*
 * Implementing this function is not required and strongly discouraged unless
 * you are absolutely sure your Weenix is perfect.
 *
 * This is the syscall entry point into vfs for mounting. You will need to
 * create the fs_t struct and populate its fs_dev and fs_type fields before
 * calling vfs's mountfunc(). mountfunc() will use the fields you populated
 * in order to determine which underlying filesystem's mount function should
 * be run, then it will finish setting up the fs_t struct. At this point you
 * have a fully functioning file system, however it is not mounted on the
 * virtual file system, you will need to call vfs_mount to do this.
 *
 * There are lots of things which can go wrong here. Make sure you have good
 * error handling. Remember the fs_dev and fs_type buffers have limited size
 * so you should not write arbitrary length strings to them.
 */
int
do_mount(const char *source, const char *target, const char *type)
{
        NOT_YET_IMPLEMENTED("MOUNTING: do_mount");
        return -EINVAL;
}

/*
 * Implementing this function is not required and strongly discouraged unless
 * you are absolutley sure your Weenix is perfect.
 *
 * This function delegates all of the real work to vfs_umount. You should not worry
 * about freeing the fs_t struct here, that is done in vfs_umount. All this function
 * does is figure out which file system to pass to vfs_umount and do good error
 * checking.
 */
int
do_umount(const char *target)
{
        NOT_YET_IMPLEMENTED("MOUNTING: do_umount");
        return -EINVAL;
}
#endif
