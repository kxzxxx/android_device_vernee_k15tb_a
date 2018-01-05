/*
 *  ion.c
 *
 * Memory Allocator functions for ion
 *
 *   Copyright 2011 Google, Inc
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include <cutils/log.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <ion/ion.h>
//#include <linux/mtk_ion.h>
#include <ion.h>

enum mtk_ion_heap_type {
	/* ION_HEAP_TYPE_MULTIMEDIA = ION_HEAP_TYPE_CUSTOM + 1, */
	ION_HEAP_TYPE_MULTIMEDIA = 10,
};
#define ION_HEAP_MULTIMEDIA_MASK    (1 << ION_HEAP_TYPE_MULTIMEDIA)
#define ION_NUM_HEAP_IDS		sizeof(unsigned int) * 8

/* Structure definitions */
typedef enum {
	ION_CMD_SYSTEM,
	ION_CMD_MULTIMEDIA
} ION_CMDS;
typedef enum {
	ION_MM_CONFIG_BUFFER,
	ION_MM_SET_DEBUG_INFO,
	ION_MM_GET_DEBUG_INFO,
	ION_MM_SET_SF_BUF_INFO,
	ION_MM_GET_SF_BUF_INFO
} ION_MM_CMDS;
typedef enum {
	ION_SYS_CACHE_SYNC,
	ION_SYS_GET_PHYS,
	ION_SYS_GET_CLIENT,
	ION_SYS_SET_HANDLE_BACKTRACE,
	ION_SYS_SET_CLIENT_NAME,
	ION_SYS_DMA_OP,
} ION_SYS_CMDS;
typedef enum {
	ION_CACHE_CLEAN_BY_RANGE,
	ION_CACHE_INVALID_BY_RANGE,
	ION_CACHE_FLUSH_BY_RANGE,
	ION_CACHE_CLEAN_BY_RANGE_USE_VA,
	ION_CACHE_INVALID_BY_RANGE_USE_VA,
	ION_CACHE_FLUSH_BY_RANGE_USE_VA,
	ION_CACHE_CLEAN_ALL,
	ION_CACHE_INVALID_ALL,
	ION_CACHE_FLUSH_ALL
} ION_CACHE_SYNC_TYPE;
typedef enum {
	ION_ERROR_CONFIG_LOCKED = 0x10000
} ION_ERROR_E;
typedef struct ion_sys_cache_sync_param {
	union {
		ion_user_handle_t handle;
		struct ion_handle *kernel_handle;
	};
	void *va;
	unsigned int size;
	ION_CACHE_SYNC_TYPE sync_type;
} ion_sys_cache_sync_param_t;
typedef enum {
	ION_DMA_MAP_AREA,
	ION_DMA_UNMAP_AREA,
	ION_DMA_MAP_AREA_VA,
	ION_DMA_UNMAP_AREA_VA,
	ION_DMA_CACHE_FLUSH_ALL
} ION_DMA_TYPE;
typedef enum {
	ION_DMA_FROM_DEVICE,
	ION_DMA_TO_DEVICE,
	ION_DMA_BIDIRECTIONAL,
} ION_DMA_DIR;
typedef struct ion_dma_param {
	union {
		ion_user_handle_t handle;
		void *kernel_handle;
	};
	void *va;
	unsigned int size;
	ION_DMA_TYPE dma_type;
	ION_DMA_DIR dma_dir;
} ion_sys_dma_param_t;
typedef struct ion_sys_get_phys_param {
	union {
		ion_user_handle_t handle;
		struct ion_handle *kernel_handle;
	};
	unsigned int phy_addr;
	unsigned long len;
} ion_sys_get_phys_param_t;
#define ION_MM_DBG_NAME_LEN 16
#define ION_MM_SF_BUF_INFO_LEN 16
typedef struct __ion_sys_client_name {
	char name[ION_MM_DBG_NAME_LEN];
} ion_sys_client_name_t;
typedef struct ion_sys_get_client_param {
	unsigned int client;
} ion_sys_get_client_param_t;
typedef struct ion_sys_record_param {
	pid_t group_id;
	pid_t pid;
	unsigned int action;
	unsigned int address_type;
	unsigned int address;
	unsigned int length;
	unsigned int backtrace[10];
	unsigned int backtrace_num;
	struct ion_handle *handle;
	struct ion_client *client;
	struct ion_buffer *buffer;
	struct file *file;
	int fd;
} ion_sys_record_t;
typedef struct ion_sys_data {
	ION_SYS_CMDS sys_cmd;
	union {
		ion_sys_cache_sync_param_t cache_sync_param;
		ion_sys_get_phys_param_t get_phys_param;
		ion_sys_get_client_param_t get_client_param;
		ion_sys_client_name_t client_name_param;
		ion_sys_record_t record_param;
		ion_sys_dma_param_t dma_param;
	};
} ion_sys_data_t;
typedef struct ion_mm_config_buffer_param {
	union {
		ion_user_handle_t handle;
		struct ion_handle *kernel_handle;
	};
	int eModuleID;
	unsigned int security;
	unsigned int coherent;
} ion_mm_config_buffer_param_t;
typedef struct __ion_mm_buf_debug_info {
	union {
		ion_user_handle_t handle;
		struct ion_handle *kernel_handle;
	};
	char dbg_name[ION_MM_DBG_NAME_LEN];
	unsigned int value1;
	unsigned int value2;
	unsigned int value3;
	unsigned int value4;
} ion_mm_buf_debug_info_t;
typedef struct __ion_mm_sf_buf_info {
	union {
		ion_user_handle_t handle;
		struct ion_handle *kernel_handle;
	};
	unsigned int info[ION_MM_SF_BUF_INFO_LEN];
} ion_mm_sf_buf_info_t;
typedef struct ion_mm_data {
	ION_MM_CMDS mm_cmd;
	union {
		ion_mm_config_buffer_param_t config_buffer_param;
		ion_mm_buf_debug_info_t buf_debug_info_param;
		ion_mm_sf_buf_info_t sf_buf_info_param;
	};
} ion_mm_data_t;

static int ion_set_client_name(int ion_fd, const char *name)
{
    int ret;
    ion_sys_data_t sys_data;

    sys_data.sys_cmd = ION_SYS_SET_CLIENT_NAME;

    strncpy(sys_data.client_name_param.name, name, sizeof(sys_data.client_name_param.name)-1);

    if(ion_custom_ioctl(ion_fd, ION_CMD_SYSTEM, &sys_data))
    {
        //config error
        ALOGE("[ion_dbg] ion_set_client_name error\n");
        return -1;
    }

    return 0;
}

int mt_ion_open(const char *name)
{
    int fd;
    fd = ion_open();
    if(fd < 0)
    {
        ALOGE("ion_open failed!\n");
        return fd;
    }

    ion_set_client_name(fd, name);
    return fd;
}

int ion_alloc_mm(int fd, size_t len, size_t align, unsigned int flags,
              ion_user_handle_t *handle)
{
        /*int ret;
        struct ion_allocation_data data = {
                .len = len,
                .align = align,
                .flags = flags,
                .heap_mask = ION_HEAP_MULTIMEDIA_MASK
        };

        ret = ion_ioctl(fd, ION_IOC_ALLOC, &data);
        if (ret < 0)
                return ret;
        *handle = data.handle;

        ion_alloc();

        return ret;*/

        return ion_alloc(fd, len, align, ION_HEAP_MULTIMEDIA_MASK, flags, handle);
}

int ion_alloc_syscontig(int fd, size_t len, size_t align, unsigned int flags, ion_user_handle_t *handle)
{
        /*int ret;
        struct ion_allocation_data data = {
                .len = len,
                .align = align,
                .flags = flags,
                .heap_mask = ION_HEAP_SYSTEM_CONTIG_MASK
        };

        ret = ion_ioctl(fd, ION_IOC_ALLOC, &data);
        if (ret < 0)
                return ret;
        *handle = data.handle;

        return ret;*/

        return ion_alloc(fd, len, align, ION_HEAP_SYSTEM_CONTIG_MASK, flags, handle);
}

void* ion_mmap(int fd, void *addr, size_t length, int prot, int flags, int share_fd, off_t offset)
{
    void *mapping_address = NULL;

    mapping_address =  mmap(addr, length, prot, flags, share_fd, offset);

    if (mapping_address == MAP_FAILED) {
        ALOGE("ion_mmap failed fd = %d, addr = 0x%p, len = %zu, prot = %d, flags = %d, share_fd = %d, 0x%p: %s\n", fd, addr, length,
              prot, flags, share_fd, mapping_address, strerror(errno));
    }

    return mapping_address;
}

int ion_munmap(int fd, void *addr, size_t length)
{
    int ret = munmap(addr, length);

    if (ret < 0) {
        ALOGE("ion_munmap failed fd = %d, addr = 0x%p, len = %zu, %d: %s\n", fd, addr, length,
              ret, strerror(errno));
    }
    return ret;
}

int ion_share_close(int fd, int share_fd)
{
    int ret = close(share_fd);
    if (ret < 0) {
        ALOGE("ion_share_close failed fd = %d, share_fd = %d, %d: %s\n", fd, share_fd,
              ret, strerror(errno));
    }
    return ret;
}

int ion_custom_ioctl(int fd, unsigned int cmd, void* arg)
{
    struct ion_custom_data custom_data;
    custom_data.cmd = cmd;
    custom_data.arg = (unsigned long) arg;

    int ret = ioctl(fd, ION_IOC_CUSTOM, &custom_data);
    if (ret < 0) {
        ALOGE("ion_custom_ioctl %lx failed with code %d: %s\n",(unsigned long) ION_IOC_CUSTOM,
              ret, strerror(errno));

        return -errno;
    }
    return ret;
}

int ion_cache_sync_flush_all(int fd) {
	int ret;
	struct ion_sys_data sys_data;
	sys_data.sys_cmd = ION_SYS_DMA_OP;

	sys_data.dma_param.dma_type = ION_DMA_CACHE_FLUSH_ALL;
	ret = ion_custom_ioctl(fd, ION_CMD_SYSTEM, &sys_data);
	if (ret)
		return -errno;
	return ret;
}

int ion_dma_map_area(int fd, ion_user_handle_t handle, int dir) {
	int ret;
	struct ion_sys_data sys_data;
	sys_data.sys_cmd = ION_SYS_DMA_OP;
	sys_data.dma_param.handle = handle;

	sys_data.dma_param.dma_type = ION_DMA_MAP_AREA;
	sys_data.dma_param.dma_dir = dir;
	ret = ion_custom_ioctl(fd, ION_CMD_SYSTEM, &sys_data);
	if (ret)
		return -errno;
	return ret;
}

int ion_dma_unmap_area(int fd, ion_user_handle_t handle, int dir) {
	int ret;
	struct ion_sys_data sys_data;
	sys_data.sys_cmd = ION_SYS_DMA_OP;
	sys_data.dma_param.handle = handle;

	sys_data.dma_param.dma_type = ION_DMA_UNMAP_AREA;
	sys_data.dma_param.dma_dir = dir;
	ret = ion_custom_ioctl(fd, ION_CMD_SYSTEM, &sys_data);
	if (ret)
		return -errno;
	return ret;
}

int ion_dma_map_area_va(int fd, void *addr, size_t length, int dir) {
	int ret;
	struct ion_sys_data sys_data;
	sys_data.sys_cmd = ION_SYS_DMA_OP;
	sys_data.dma_param.va = addr;

	sys_data.dma_param.dma_type = ION_DMA_MAP_AREA_VA;
	sys_data.dma_param.dma_dir = dir;
	ret = ion_custom_ioctl(fd, ION_CMD_SYSTEM, &sys_data);
	if (ret)
		return -errno;
	return ret;
}

int ion_dma_unmap_area_va(int fd, void * addr, size_t length, int dir) {
	int ret;
	struct ion_sys_data sys_data;
	sys_data.sys_cmd = ION_SYS_DMA_OP;
	sys_data.dma_param.va = addr;

	sys_data.dma_param.dma_type = ION_DMA_UNMAP_AREA_VA;
	sys_data.dma_param.dma_dir = dir;
	ret = ion_custom_ioctl(fd, ION_CMD_SYSTEM, &sys_data);
	if (ret)
		return -errno;
	return ret;
}
