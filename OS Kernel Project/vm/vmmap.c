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

#include "kernel.h"
#include "errno.h"
#include "globals.h"

#include "vm/vmmap.h"
#include "vm/shadow.h"
#include "vm/anon.h"

#include "proc/proc.h"

#include "util/debug.h"
#include "util/list.h"
#include "util/string.h"
#include "util/printf.h"

#include "fs/vnode.h"
#include "fs/file.h"
#include "fs/fcntl.h"
#include "fs/vfs_syscall.h"

#include "mm/slab.h"
#include "mm/page.h"
#include "mm/mm.h"
#include "mm/mman.h"
#include "mm/mmobj.h"

static slab_allocator_t *vmmap_allocator;
static slab_allocator_t *vmarea_allocator;

void
vmmap_init(void)
{
        vmmap_allocator = slab_allocator_create("vmmap", sizeof(vmmap_t));
        KASSERT(NULL != vmmap_allocator && "failed to create vmmap allocator!");
        vmarea_allocator = slab_allocator_create("vmarea", sizeof(vmarea_t));
        KASSERT(NULL != vmarea_allocator && "failed to create vmarea allocator!");
}

vmarea_t *
vmarea_alloc(void)
{
        vmarea_t *newvma = (vmarea_t *) slab_obj_alloc(vmarea_allocator);
        if (newvma) {
                newvma->vma_vmmap = NULL;
        }
        return newvma;
}

void
vmarea_free(vmarea_t *vma)
{
        KASSERT(NULL != vma);
        slab_obj_free(vmarea_allocator, vma);
}

/* a debugging routine: dumps the mappings of the given address space. */
size_t
vmmap_mapping_info(const void *vmmap, char *buf, size_t osize)
{
        KASSERT(0 < osize);
        KASSERT(NULL != buf);
        KASSERT(NULL != vmmap);

        vmmap_t *map = (vmmap_t *)vmmap;
        vmarea_t *vma;
        ssize_t size = (ssize_t)osize;

        int len = snprintf(buf, size, "%21s %5s %7s %8s %10s %12s\n",
                           "VADDR RANGE", "PROT", "FLAGS", "MMOBJ", "OFFSET",
                           "VFN RANGE");

        list_iterate_begin(&map->vmm_list, vma, vmarea_t, vma_plink) {
                size -= len;
                buf += len;
                if (0 >= size) {
                        goto end;
                }

                len = snprintf(buf, size,
                               "%#.8x-%#.8x  %c%c%c  %7s 0x%p %#.5x %#.5x-%#.5x\n",
                               vma->vma_start << PAGE_SHIFT,
                               vma->vma_end << PAGE_SHIFT,
                               (vma->vma_prot & PROT_READ ? 'r' : '-'),
                               (vma->vma_prot & PROT_WRITE ? 'w' : '-'),
                               (vma->vma_prot & PROT_EXEC ? 'x' : '-'),
                               (vma->vma_flags & MAP_SHARED ? " SHARED" : "PRIVATE"),
                               vma->vma_obj, vma->vma_off, vma->vma_start, vma->vma_end);
        } list_iterate_end();

end:
        if (size <= 0) {
                size = osize;
                buf[osize - 1] = '\0';
        }
        /*
        KASSERT(0 <= size);
        if (0 == size) {
                size++;
                buf--;
                buf[0] = '\0';
        }
        */
        return osize - size;
}

/* Create a new vmmap, which has no vmareas and does
 * not refer to a process. */
vmmap_t *
vmmap_create(void)
{
        /*NOT_YET_IMPLEMENTED("VM: vmmap_create");*/

	vmmap_t *new_vmmap=(vmmap_t *)slab_obj_alloc(vmmap_allocator);

	KASSERT(new_vmmap != NULL);

	list_init(&(new_vmmap)->vmm_list);
	new_vmmap->vmm_proc=NULL;


	return new_vmmap;

        /*return NULL;*/
}

/* Removes all vmareas from the address space and frees the
 * vmmap struct. */
void
vmmap_destroy(vmmap_t *map)
{
        /*NOT_YET_IMPLEMENTED("VM: vmmap_destroy");*/

	KASSERT(NULL!=map);
	dbg(DBG_PRINT,"(GRADING3A 3.a) Map is not null\n");

  if(!list_empty(&(map->vmm_list))){
	dbg(DBG_PRINT,"(GRADING3B 1)\n");  
  	vmarea_t *itr=NULL;

    	list_iterate_begin(&map->vmm_list,itr,vmarea_t,vma_plink){

    	if((itr->vma_obj!= NULL)&&(itr->vma_obj->mmo_refcount>itr->vma_obj->mmo_nrespages))
	{
				dbg(DBG_PRINT,"(GRADING3B 1)\n");  
    				itr->vma_obj->mmo_ops->put(itr->vma_obj);
	}
    		/*if(itr->vma_obj != NULL)*/
    		/*vmmap_remove(map,itr->vma_start,itr->vma_end-itr->vma_start);	*/
        list_remove(&itr->vma_plink);
        list_remove(&itr->vma_olink);
        vmarea_free(itr);
        }list_iterate_end();
      map->vmm_proc=NULL;
	   slab_obj_free(vmmap_allocator,map);
   }
}

/* Add a vmarea to an address space. Assumes (i.e. asserts to some extent)
 * the vmarea is valid.  This involves finding where to put it in the list
 * of VM areas, and adding it. Don't forget to set the vma_vmmap for the
 * area. */
void
vmmap_insert(vmmap_t *map, vmarea_t *newvma)
{
        /*NOT_YET_IMPLEMENTED("VM: vmmap_insert");*/

  	KASSERT(NULL != map && NULL != newvma);
  	dbg(DBG_PRINT,"(GRADING3A 3.b) Map or vmarea is not null\n");

          KASSERT(NULL == newvma->vma_vmmap);
  	dbg(DBG_PRINT,"(GRADING3A 3.b) newvma->vma_vmmap is not null\n");


  	KASSERT(newvma->vma_start < newvma->vma_end);
  	dbg(DBG_PRINT,"(GRADING3A 3.b) vma_start >= vma_end\n");

    KASSERT(ADDR_TO_PN(USER_MEM_LOW) <= newvma->vma_start && ADDR_TO_PN(USER_MEM_HIGH) >= newvma->vma_end);
  	dbg(DBG_PRINT,"(GRADING3A 3.b)\n");
    	newvma->vma_vmmap=map;

  	vmarea_t *itr=NULL;
  	int flag=0;

   /* if(!vmmap_is_range_empty(map,newvma->vma_start,(newvma->vma_end-newvma->vma_start)))
    {
	dbg(DBG_PRINT,"(GRADING3B 1vm3)\n");  
    	return;
    }*/
  	list_iterate_begin(&map->vmm_list,itr,vmarea_t,vma_plink)
  	{
  		if(newvma->vma_end <= itr->vma_start)
  		{
			dbg(DBG_PRINT,"(GRADING3B 1)\n");  
  			list_insert_before(&itr->vma_plink,&newvma->vma_plink);
  			flag=1;
  			goto inserttail;
  		}
  	}list_iterate_end();

  	inserttail: if(flag==0)
  	{
		dbg(DBG_PRINT,"(GRADING3B 1)\n");  
  		list_insert_tail(&map->vmm_list,&newvma->vma_plink);
  	}

    return;

}

/* Find a contiguous range of free virtual pages of length npages in
 * the given address space. Returns starting vfn for the range,
 * without altering the map. Returns -1 if no such range exists.
 *
 * Your algorithm should be first fit. If dir is VMMAP_DIR_HILO, you
 * should find a gap as high in the address space as possible; if dir
 * is VMMAP_DIR_LOHI, the gap should be as low as possible. */
int
vmmap_find_range(vmmap_t *map, uint32_t npages, int dir)
{
        /*NOT_YET_IMPLEMENTED("VM: vmmap_find_range");*/

	vmarea_t *itr=NULL;
  	uint32_t memHigh;
  	uint32_t memLow;

	/*dbg(DBG_PRINT,"(GRADING3B vm4)\n");*/  

	/*if(dir==VMMAP_DIR_LOHI)
	{
		dbg(DBG_PRINT,"(GRADING3B 1vm6)\n");  
    		memLow =  ADDR_TO_PN(USER_MEM_LOW);
		list_iterate_begin(&map->vmm_list,itr,vmarea_t,vma_plink)
		{
			 next ptr defined here 
			
			if((itr->vma_start - memLow) >= npages)
			{
				dbg(DBG_PRINT,"(GRADING3B 1vm7)\n");  
				return memLow;
			}
      else
      {
	dbg(DBG_PRINT,"(GRADING3B 1vm8)\n");  
        memLow = itr->vma_end;
      }

		}list_iterate_end();

    if(ADDR_TO_PN(USER_MEM_HIGH) < (memLow+npages))
    {
      dbg(DBG_PRINT,"(GRADING3B 1vm9)\n");  
      return -1;
    }
    else
    {
      dbg(DBG_PRINT,"(GRADING3B 1vm10)\n");  
      return memLow;
    }

	}

	else if(dir==VMMAP_DIR_HILO)
	{
		dbg(DBG_PRINT,"(GRADING3B 1vm11)\n");  
	        memHigh =  ADDR_TO_PN(USER_MEM_HIGH);
		list_iterate_reverse(&map->vmm_list,itr,vmarea_t,vma_plink)
		{
			prev ptr defined here 
			if((memHigh - itr->vma_end) >= npages-1)
			{
				dbg(DBG_PRINT,"(GRADING3B 1vm12)\n");  
				return itr->vma_end;
			}
      else
      {
	dbg(DBG_PRINT,"(GRADING3B 1vm13)\n");  
        memHigh = itr->vma_start;
      }
		}list_iterate_end();

    if(ADDR_TO_PN(USER_MEM_LOW) >(memHigh-npages))
    {
      dbg(DBG_PRINT,"(GRADING3B 1vm14)\n");  
      return -1;
    }
    else
    {
	dbg(DBG_PRINT,"(GRADING3B 1vm15)\n");   
      return memHigh-npages;
    }

	}
*/
		return -1;


}

/* Find the vm_area that vfn lies in. Simply scan the address space
 * looking for a vma whose range covers vfn. If the page is unmapped,
 * return NULL. */
vmarea_t *
vmmap_lookup(vmmap_t *map, uint32_t vfn)
{
        /*NOT_YET_IMPLEMENTED("VM: vmmap_lookup");*/

	KASSERT(NULL!=map);
	dbg(DBG_PRINT,"(GRADING3A 3.c) Map is not null\n");

	vmarea_t *itr;

		list_iterate_begin(&(map->vmm_list),itr,vmarea_t,vma_plink)
		{
			/* next ptr defined here */
			if((vfn >= itr->vma_start)  && (vfn < itr-> vma_end))
			{
				dbg(DBG_PRINT,"(GRADING3B 1)\n");  
				return itr;
			}
		}list_iterate_end();


    return NULL;
}

/* Allocates a new vmmap containing a new vmarea for each area in the
 * given map. The areas should have no mmobjs set yet. Returns pointer
 * to the new vmmap on success, NULL on failure. This function is
 * called when implementing fork(2). */
vmmap_t *
vmmap_clone(vmmap_t *map)
{
        /*NOT_YET_IMPLEMENTED("VM: vmmap_clone");*/
        return NULL;
}

/* Insert a mapping into the map starting at lopage for npages pages.
 * If lopage is zero, we will find a range of virtual addresses in the
 * process that is big enough, by using vmmap_find_range with the same
 * dir argument.  If lopage is non-zero and the specified region
 * contains another mapping that mapping should be unmapped.
 *
 * If file is NULL an anon mmobj will be used to create a mapping
 * of 0's.  If file is non-null that vnode's file will be mapped in
 * for the given range.  Use the vnode's mmap operation to get the
 * mmobj for the file; do not assume it is file->vn_obj. Make sure all
 * of the area's fields except for vma_obj have been set before
 * calling mmap.
 *
 * If MAP_PRIVATE is specified set up a shadow object for the mmobj.
 *
 * All of the input to this function should be valid (KASSERT!).
 * See mmap(2) for for description of legal input.
 * Note that off should be page aligned.
 *
 * Be very careful about the order operations are performed in here. Some
 * operation are impossible to undo and should be saved until there
 * is no chance of failure.
 *
 * If 'new' is non-NULL a pointer to the new vmarea_t should be stored in it.
 */
int
vmmap_map(vmmap_t *map, vnode_t *file, uint32_t lopage, uint32_t npages,
          int prot, int flags, off_t off, int dir, vmarea_t **new)
{
        /*NOT_YET_IMPLEMENTED("VM: vmmap_map");*/

  	KASSERT(NULL != map);
  	dbg(DBG_PRINT,"(GRADING3A 3.d)\n");

  	KASSERT(0 < npages);
  	dbg(DBG_PRINT,"(GRADING3A 3.d)\n");

          KASSERT((MAP_SHARED & flags) || (MAP_PRIVATE & flags));
  	dbg(DBG_PRINT,"(GRADING3A 3.d)\n");

  	KASSERT((0 == lopage) || (ADDR_TO_PN(USER_MEM_LOW) <= lopage));
  	dbg(DBG_PRINT,"(GRADING3A 3.d)\n");

  	KASSERT((0 == lopage) || (ADDR_TO_PN(USER_MEM_HIGH) >= (lopage + npages)));
        dbg(DBG_PRINT,"(GRADING3A 3.d)\n");

  	KASSERT(PAGE_ALIGNED(off));
  	dbg(DBG_PRINT,"(GRADING3A 3.d)\n");

  	uint32_t start_pg = 0;
  	int retvar;

  	/*KASSERT(NULL != new);*/

  	/*if(lopage == 0)
  	{
		dbg(DBG_PRINT,"(GRADING3B 1vm17)\n");  
  		retvar = vmmap_find_range(map,npages,dir);
      if (retvar < 0)
      {
	dbg(DBG_PRINT,"(GRADING3B 1vm18)\n");  
        return -ENOMEM;
      }
      start_pg = (uint32_t)retvar;

  	}
*/
  	if(lopage != 0)
  	{
		dbg(DBG_PRINT,"(GRADING3B 1)\n");  
  		if(vmmap_is_range_empty(map,lopage,npages) == 1)
  		{
		 dbg(DBG_PRINT,"(GRADING3B 1)\n");  
  		 start_pg = lopage;
  		}
  		else
  		{
			dbg(DBG_PRINT,"(GRADING3B 1)\n");  
  			/*unmap*/
  			retvar = vmmap_remove(map,lopage,npages);

  			KASSERT(retvar == 0);
  			start_pg=lopage;
  		}

  	}

  	vmarea_t *new_vmarea = vmarea_alloc();

  	new_vmarea->vma_start = start_pg;
  	new_vmarea->vma_end = start_pg + npages;
  	new_vmarea->vma_off = off;

  	new_vmarea->vma_prot = prot;
  	new_vmarea->vma_flags = flags;

  	/* plink and o link */
  	list_link_init(&new_vmarea->vma_plink);
  	list_link_init(&new_vmarea->vma_olink);


  	/*shadow obj if MAP_PRIVATE */

  	/*new_vmarea->vma_vmmap = map;*/

  	vmmap_insert(map,new_vmarea);
    /* Mmobj*/
    mmobj_t *new_mmobj;

  	if(file !=NULL)
  	{
		dbg(DBG_PRINT,"(GRADING3B 1)\n");  
  		retvar = file->vn_ops->mmap(file,new_vmarea,&new_mmobj);

  		/*new_vmarea->vma_olink = NULL;*/
  		if(retvar < 0)
  			return retvar;
  	}

  	/* if file is NULL ...!!!*/
  	else if(file == NULL)
  	{
		dbg(DBG_PRINT,"(GRADING3B 1)\n");  
  		new_mmobj=anon_create();
  	}

  	/*olink*/
  	/* ?????????????????????*/
    list_insert_tail(&new_mmobj->mmo_un.mmo_vmas,&new_vmarea->vma_olink);
    new_vmarea->vma_obj = new_mmobj;
    /*if (new != NULL)
    {
	dbg(DBG_PRINT,"(GRADING3B 1vm24)\n");  
       *new =new_vmarea;
    }*/

  	return 0;
}

/*
 * We have no guarantee that the region of the address space being
 * unmapped will play nicely with our list of vmareas.
 *
 * You must iterate over each vmarea that is partially or wholly covered
 * by the address range [addr ... addr+len). The vm-area will fall into one
 * of four cases, as illustrated below:
 *
 * key:
 *          [             ]   Existing VM Area
 *        *******             Region to be unmapped
 *
 * Case 1:  [   ******    ]
 * The region to be unmapped lies completely inside the vmarea. We need to
 * split the old vmarea into two vmareas. be sure to increment the
 * reference count to the file associated with the vmarea.
 *
 * Case 2:  [      *******]**
 * The region overlaps the end of the vmarea. Just shorten the length of
 * the mapping.
 *
 * Case 3: *[*****        ]
 * The region overlaps the beginning of the vmarea. Move the beginning of
 * the mapping (remember to update vma_off), and shorten its length.
 *
 * Case 4: *[*************]**
 * The region completely contains the vmarea. Remove the vmarea from the
 * list.
 */
int
vmmap_remove(vmmap_t *map, uint32_t lopage, uint32_t npages)
{
        /*NOT_YET_IMPLEMENTED("VM: vmmap_remove");*/

	vmarea_t *itr=NULL;

	uint32_t num = lopage+npages;

	dbg(DBG_PRINT,"(GRADING3B 1)\n");  
	list_iterate_begin(&(map->vmm_list),itr,vmarea_t,vma_plink)
	{
		/*
		if(itr->vma_start < lopage && itr->vma_end > num)
		{
			dbg(DBG_PRINT,"(GRADING3B 1vm26)\n");  
			creating a new vmarea
			vmarea_t *new_vmarea = vmarea_alloc();

			new_vmarea->vma_start = num;
			new_vmarea->vma_end = itr->vma_end;
			new_vmarea->vma_off = itr->vma_start + npages + (lopage - itr->vma_start) ;
			new_vmarea->vma_prot = itr->vma_prot;
			new_vmarea->vma_flags = itr->vma_flags;

			list_link_init(&new_vmarea->vma_plink);
			list_link_init(&new_vmarea->vma_olink);

			shadow obj if MAP_PRIVATE 

			new_vmarea->vma_vmmap = map;

			vmmap_insert(map,new_vmarea);

			changing the old vmarea values 

			itr->vma_end = lopage;

			if(itr->vma_obj->mmo_ops->ref != NULL)
			{
				dbg(DBG_PRINT,"(GRADING3B 1vm27)\n");  
				itr->vma_obj->mmo_ops->ref(itr->vma_obj);
			}
			pt_unmap_range(curproc->p_pagedir, (uintptr_t)PN_TO_ADDR(lopage), (uintptr_t)PN_TO_ADDR(num));
			return 0;
		}

		else if(itr->vma_start < lopage && lopage > itr->vma_end)
		{
			dbg(DBG_PRINT,"(GRADING3B 1vm28)\n");  
			pt_unmap_range(curproc->p_pagedir, (uintptr_t)PN_TO_ADDR(lopage), (uintptr_t)PN_TO_ADDR(itr->vma_end));
			itr->vma_end = lopage;
		}

		else if(num > itr->vma_start && itr->vma_end > num)
		{
			dbg(DBG_PRINT,"(GRADING3B 1vm29)\n");  
			pt_unmap_range(curproc->p_pagedir, (uintptr_t)PN_TO_ADDR(itr->vma_start), (uintptr_t)PN_TO_ADDR(num - (itr->vma_start - lopage)));
			itr->vma_start = num;
			itr->vma_off = itr->vma_off + (num - itr->vma_start);
		}
		*/
		 if(lopage <= itr->vma_start && num >= itr->vma_end)
		{
			dbg(DBG_PRINT,"(GRADING3B 1)\n");  
			if(itr->vma_obj->mmo_ops->put != NULL)
			{
				itr->vma_obj->mmo_ops->put(itr->vma_obj);
			}
			pt_unmap_range(curproc->p_pagedir, (uintptr_t)PN_TO_ADDR(itr->vma_start), (uintptr_t)PN_TO_ADDR(itr->vma_end));
			list_remove(&itr->vma_plink);
			list_remove(&itr->vma_olink);
			vmarea_free(itr);
		}

	}list_iterate_end();

        return 0;
}

/*
 * Returns 1 if the given address space has no mappings for the
 * given range, 0 otherwise.
 */
int
vmmap_is_range_empty(vmmap_t *map, uint32_t startvfn, uint32_t npages)
{
        /*NOT_YET_IMPLEMENTED("VM: vmmap_is_range_empty");*/

	uint32_t endvfn = startvfn + npages;
  	uint32_t memLow = ADDR_TO_PN(USER_MEM_LOW);

	KASSERT((startvfn < endvfn) && (ADDR_TO_PN(USER_MEM_LOW) <= startvfn) && (ADDR_TO_PN(USER_MEM_HIGH) >= endvfn));
	dbg(DBG_PRINT,"(GRADING3A 3.e)\n");

	vmarea_t *itr;

	list_iterate_begin(&(map->vmm_list),itr,vmarea_t,vma_plink)
	{
			/* next ptr defined here */
      if(memLow <= startvfn && itr->vma_start >= endvfn)
      {
	dbg(DBG_PRINT,"(GRADING3B 1)\n");  
        return 1;
      }
			/*else if((itr->vma_start > startvfn) && (itr->vma_start < endvfn))
			{
				dbg(DBG_PRINT,"(GRADING3B 1vm32)\n");  
				return 0;
			}*/
      /*else if(memLow > startvfn)
      {
	dbg(DBG_PRINT,"(GRADING3B 1vm33)\n");  
        return 0;
      }*/
      memLow = itr->vma_end;
	}list_iterate_end();

if((ADDR_TO_PN(USER_MEM_HIGH)>= endvfn) && (memLow <= startvfn))
{
  dbg(DBG_PRINT,"(GRADING3B 1)\n");  
  return 1;
}

        return 0;
}

/* Read into 'buf' from the virtual address space of 'map' starting at
 * 'vaddr' for size 'count'. To do so, you will want to find the vmareas
 * to read from, then find the pframes within those vmareas corresponding
 * to the virtual addresses you want to read, and then read from the
 * physical memory that pframe points to. You should not check permissions
 * of the areas. Assume (KASSERT) that all the areas you are accessing exist.
 * Returns 0 on success, -errno on error.
 */
int
vmmap_read(vmmap_t *map, const void *vaddr, void *buf, size_t count)
{
        /*NOT_YET_IMPLEMENTED("VM: vmmap_read");*/

	/* iteration may be required for multiple pframes/vmareas */

	/*size_t destaddr = (size_t)buf;*/

	/*once read is correctly implemented then this is the same as above*/
	size_t startvaddr = (size_t)vaddr;

	uint32_t start_vfn=ADDR_TO_PN(startvaddr);

 	size_t endvaddr = startvaddr + count;

	uint32_t end_vfn=ADDR_TO_PN(endvaddr);

	uint32_t new_offset=PAGE_OFFSET(startvaddr);

	vmarea_t *itr_vmarea=vmmap_lookup(map,start_vfn);

	KASSERT(NULL != itr_vmarea);

	uint32_t off = itr_vmarea->vma_off;

	uint32_t diff = start_vfn - itr_vmarea->vma_start;

	uint32_t phypage_no = off + diff;

	pframe_t *result=NULL;
	int retvar=0;

	dbg(DBG_PRINT,"(GRADING3B 1)\n");  

	if(end_vfn < itr_vmarea->vma_end)
	{
		dbg(DBG_PRINT,"(GRADING3B 1)\n");  
		while(1)
		{
		retvar = pframe_lookup(itr_vmarea->vma_obj,phypage_no,0,&result);
		if(retvar==0)
		{
			dbg(DBG_PRINT,"(GRADING3B 1)\n");  
			/*pframe_dirty(result);*/
			if(count < PAGE_SIZE)
			{
				dbg(DBG_PRINT,"(GRADING3B 1)\n");  
				memcpy(buf,(void *)((uint32_t)(result->pf_addr)+new_offset),count);
				return 0;
			}
			/*else
			{
				dbg(DBG_PRINT,"(GRADING3B 1vm39)\n");  
				memcpy(buf,(void *)((uint32_t)(result->pf_addr)+new_offset),(PAGE_SIZE-new_offset));
				count -= (PAGE_SIZE-new_offset);
				destaddr = destaddr + (PAGE_SIZE-new_offset);
				buf = (void*) ((PAGE_SIZE-new_offset) + (uint32_t)buf);
				new_offset=0;
				phypage_no++;
			}*/
		}
		/*else
		{
			dbg(DBG_PRINT,"(GRADING3B 1vm40)\n");  
			return retvar;
		}*/
		}
	}

	/*else
	{
		dbg(DBG_PRINT,"(GRADING3B 1vm41)\n");  
		while(end_vfn > itr_vmarea->vma_end)
		{

			off = itr_vmarea->vma_off;
			diff = start_vfn - itr_vmarea->vma_start;
			phypage_no = off + diff;

			uint32_t vf=start_vfn;

			while(vf <= itr_vmarea->vma_end)
			{

				retvar = pframe_lookup(itr_vmarea->vma_obj,phypage_no,0,&result);
				if(retvar==0)
				{
					dbg(DBG_PRINT,"(GRADING3B 1vm42)\n");  
					pframe_dirty(result);
					if(count < PAGE_SIZE)
					{
						dbg(DBG_PRINT,"(GRADING3B 1vm43)\n");  
						memcpy(buf,(void *)((uint32_t)(result->pf_addr)+new_offset),count);
						return 0;
					}
					else
					{
						dbg(DBG_PRINT,"(GRADING3B 1vm44)\n");  
						memcpy(buf,(void *)((uint32_t)(result->pf_addr)+new_offset),(PAGE_SIZE-new_offset));
						count -= (PAGE_SIZE-new_offset);
						destaddr = destaddr + (PAGE_SIZE-new_offset);
						buf = (void*) ((PAGE_SIZE-new_offset) + (uint32_t)buf);
						new_offset=0;
						phypage_no++;
					}
				}
				else
				{
					dbg(DBG_PRINT,"(GRADING3B 1vm45)\n");  
					return retvar;
				}
				vf++;
			}


			itr_vmarea = list_item(itr_vmarea->vma_plink.l_next,vmarea_t,vma_plink);
			start_vfn = itr_vmarea->vma_start;
		}

                last vmarea

		off = itr_vmarea->vma_off;
		phypage_no = off;


		while(1)
		{
		retvar = pframe_lookup(itr_vmarea->vma_obj,phypage_no,0,&result);
		if(retvar==0)
		{
			dbg(DBG_PRINT,"(GRADING3B 1vm46)\n");  
			pframe_dirty(result);
			if(count < PAGE_SIZE)
			{
				dbg(DBG_PRINT,"(GRADING3B 1vm47)\n");  
				memcpy(buf,(void *)((uint32_t)(result->pf_addr)+new_offset),count);
				return 0;
			}
			else
			{
				dbg(DBG_PRINT,"(GRADING3B 1vm48)\n");  
				memcpy(buf,result->pf_addr,PAGE_SIZE);
				count -= PAGE_SIZE;
				destaddr = destaddr + PAGE_SIZE;
				buf = (void*) (PAGE_SIZE + (uint32_t)buf);
				phypage_no++;
			}
		}
		else
		{
			dbg(DBG_PRINT,"(GRADING3B 1vm49)\n");  
			return retvar;
		}
		}



	}*/
	return retvar;


}

/* Write from 'buf' into the virtual address space of 'map' starting at
 * 'vaddr' for size 'count'. To do this, you will need to find the correct
 * vmareas to write into, then find the correct pframes within those vmareas,
 * and finally write into the physical addresses that those pframes correspond
 * to. You should not check permissions of the areas you use. Assume (KASSERT)
 * that all the areas you are accessing exist. Remember to dirty pages!
 * Returns 0 on success, -errno on error.
 */
int
vmmap_write(vmmap_t *map, void *vaddr, const void *buf, size_t count)
{
        /*NOT_YET_IMPLEMENTED("VM: vmmap_write");*/

	/*once read is correctly implemented then this is the same as above*/

	/*size_t destaddr = (size_t)buf;*/

	/*once read is correctly implemented then this is the same as above*/
	size_t startvaddr = (size_t)vaddr;

	uint32_t start_vfn=ADDR_TO_PN(startvaddr);

 	size_t endvaddr = startvaddr + count;

	uint32_t end_vfn=ADDR_TO_PN(endvaddr);

	uint32_t new_offset=PAGE_OFFSET(startvaddr);

	vmarea_t *itr_vmarea=vmmap_lookup(map,start_vfn);

	KASSERT(NULL != itr_vmarea);

	uint32_t off = itr_vmarea->vma_off;

	uint32_t diff = start_vfn - itr_vmarea->vma_start;

	uint32_t phypage_no = off + diff;

	pframe_t *result=NULL;

	int retvar;


	if(end_vfn < itr_vmarea->vma_end)
	{
		dbg(DBG_PRINT,"(GRADING3B 1)\n");  
		while(1)
		{
		retvar = pframe_lookup(itr_vmarea->vma_obj,phypage_no,1,&result);
		if(retvar==0)
		{
			dbg(DBG_PRINT,"(GRADING3B 1)\n");  
			pframe_dirty(result);
			if(count < PAGE_SIZE)
			{
				dbg(DBG_PRINT,"(GRADING3B 1)\n");  
				memcpy((void *)((uint32_t)(result->pf_addr)+new_offset),buf,count);
				return 0;
			}
			/*else
			{
				dbg(DBG_PRINT,"(GRADING3B 1vm53)\n");  
				memcpy((void *)((uint32_t)(result->pf_addr)+new_offset),buf,(PAGE_SIZE-new_offset));
				count -= (PAGE_SIZE-new_offset);
				destaddr = destaddr + (PAGE_SIZE-new_offset);
				buf = (void*) ((PAGE_SIZE-new_offset) + (uint32_t)buf);
				new_offset=0;
				phypage_no++;
			}*/
		}
		/*else
		{
			dbg(DBG_PRINT,"(GRADING3B 1vm54)\n");  
			return retvar;
		}*/
		}
	}

	else
	{
		/*while(end_vfn > itr_vmarea->vma_end)
		{

			off = itr_vmarea->vma_off;
			diff = start_vfn - itr_vmarea->vma_start;
			phypage_no = off + diff;

			uint32_t vf=start_vfn;

			while(vf <= itr_vmarea->vma_end)
			{

				retvar = pframe_lookup(itr_vmarea->vma_obj,phypage_no,1,&result);
				if(retvar==0)
				{
					dbg(DBG_PRINT,"(GRADING3B 1vm55)\n");  
					pframe_dirty(result);
					if(count < PAGE_SIZE)
					{
						dbg(DBG_PRINT,"(GRADING3B 1vm56)\n");  
						memcpy((void *)((uint32_t)(result->pf_addr)+new_offset),buf,count);
						return 0;
					}
					else
					{
						dbg(DBG_PRINT,"(GRADING3B 1vm57)\n");  
						memcpy((void *)((uint32_t)(result->pf_addr)+new_offset),buf,(PAGE_SIZE-new_offset));
						count -= (PAGE_SIZE-new_offset);
						destaddr = destaddr + (PAGE_SIZE-new_offset);
						buf = (void*) ((PAGE_SIZE-new_offset) + (uint32_t)buf);
						new_offset=0;
						phypage_no++;
					}
				}
				else
				{
					dbg(DBG_PRINT,"(GRADING3B 1vm58)\n");  
					return retvar;
				}
				vf++;
			}


			itr_vmarea = list_item(itr_vmarea->vma_plink.l_next,vmarea_t,vma_plink);
			start_vfn = itr_vmarea->vma_start;
		}
		*/
                /*last vmarea*/

		off = itr_vmarea->vma_off;
		phypage_no = off;


		while(1)
		{
		retvar = pframe_lookup(itr_vmarea->vma_obj,phypage_no,1,&result);
		if(retvar==0)
		{
			dbg(DBG_PRINT,"(GRADING3B 1)\n");  
			pframe_dirty(result);
			if(count < PAGE_SIZE)
			{
				dbg(DBG_PRINT,"(GRADING3B 1)\n");  
				memcpy((void *)((uint32_t)(result->pf_addr)+new_offset),buf,count);
				return 0;
			}
			/*else
			{
				dbg(DBG_PRINT,"(GRADING3B 1vm61)\n");  
				memcpy(result->pf_addr,buf,PAGE_SIZE);
				count -= PAGE_SIZE;
				destaddr = destaddr + PAGE_SIZE;
				buf = (void*) (PAGE_SIZE + (uint32_t)buf);
				phypage_no++;
			}*/
		}
		/*else
		{
			dbg(DBG_PRINT,"(GRADING3B 1vm62)\n");  
			return retvar;
		}*/
		}



	}
	return retvar;

}
