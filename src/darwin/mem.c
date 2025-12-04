#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysctl.h>
#include <unistd.h>

#include "xmon.h"
#include <mach/mach.h>
#include <mach/mach_host.h>

static vm_size_t page_size;
static host_t host_port;

int mem_init(void)
{
	int64_t mem_bytes;
	size_t len = sizeof(mem_bytes);
	int mib[2] = {CTL_HW, HW_MEMSIZE};

	if (sysctl(mib, 2, &mem_bytes, &len, NULL, 0) == -1) {
		fprintf(stderr, "sysctl HW_MEMSIZE failed\n");
		return -1;
	}

	smon.mem_total = mem_bytes / 1024;

	host_port = mach_host_self();
	if (host_page_size(host_port, &page_size) != KERN_SUCCESS) {
		page_size = 4096;
	}

	return 0;
}

void mem_update(void)
{
	vm_statistics64_data_t vm_stat;
	mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
	kern_return_t kr;
	int64_t free_pages;

	kr = host_statistics64(host_port, HOST_VM_INFO64, (host_info64_t)&vm_stat, &count);
	if (kr != KERN_SUCCESS) return;

	/* approximating available memory as free + inactive + speculative */
	free_pages = vm_stat.free_count + vm_stat.inactive_count + vm_stat.speculative_count;

	smon.mem_free = (free_pages * page_size) / 1024;
}
