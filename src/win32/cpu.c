#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "xmon.h"

typedef struct {
	LARGE_INTEGER idle;
	LARGE_INTEGER kernel;
	LARGE_INTEGER user;
	LARGE_INTEGER rsvd1[2];
	unsigned long rsvd2;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

enum sys_info_class { SYS_PROC_PERF_INFO = 8 };

typedef int (WINAPI *ntqsysinfo_func)(enum sys_info_class, void*, unsigned long, unsigned long*);

static HINSTANCE ntdll;
static ntqsysinfo_func NtQuerySystemInformation;

static SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION *sppinfo, *prev;
static unsigned long sppinfo_size;


int cpu_init(void)
{
	unsigned int i;
	SYSTEM_INFO si;
	unsigned long retsz;

	GetSystemInfo(&si);

	smon.num_cpus = si.dwNumberOfProcessors;
	if(!(smon.cpu = calloc(smon.num_cpus, sizeof *smon.cpu))) {
		char buf[256];
		sprintf(buf, "Failed to allocate memory for per-cpu usage (%u)", smon.num_cpus);
		MessageBox(0, buf, "Fatal", MB_OK);
		return -1;
	}

	/* looking for CPU stat gathering methods should be the last thing
	 * in this function because the first available one will return success
	 * immediately, leaving the fallthrough case for failure.
	 */

	/* Try NtQuerySystemInformation, should be available on all NT versions */
	if((ntdll = LoadLibrary("ntdll.dll"))) {
		NtQuerySystemInformation = (ntqsysinfo_func)GetProcAddress(ntdll, "NtQuerySystemInformation");
		if(NtQuerySystemInformation) {
			sppinfo_size = sizeof *sppinfo * smon.num_cpus;
			if(!(sppinfo = malloc(sppinfo_size * 2))) {
				MessageBox(0, "Failed to allocate cpu usage query buffer", "Fatal", MB_OK);
				return -1;
			}
			prev = sppinfo + smon.num_cpus;

			for(;;) {
				NtQuerySystemInformation(SYS_PROC_PERF_INFO, sppinfo, sppinfo_size, &retsz);
				if(retsz <= sppinfo_size) break;

				sppinfo_size <<= 1;
				free(sppinfo);
				if(!(sppinfo = malloc(sppinfo_size * 2))) {
					MessageBox(0, "Failed to allocate cpu usage query buffer", "Fatal", MB_OK);
					return -1;
				}
				prev = sppinfo + smon.num_cpus;
			}

			for(i=0; i<smon.num_cpus; i++) {
				prev[i] = sppinfo[i];
			}

			return 0;
		}
	}

	/* ... try more methods ... */
			
	/* we found no supported CPU stat gathering method */
	return -1;
}

void cpu_update(void)
{
	unsigned int i;

	if(NtQuerySystemInformation) {
		int status;
		unsigned long retsz;
		unsigned long idle, sum, allidle, allsum;

		status = NtQuerySystemInformation(SYS_PROC_PERF_INFO, sppinfo, sppinfo_size, &retsz);

		allidle = allsum = 0;
		for(i=0; i<smon.num_cpus; i++) {
			idle = (unsigned long)(sppinfo[i].idle.QuadPart - prev[i].idle.QuadPart);
			sum = (unsigned long)((sppinfo[i].user.QuadPart - prev[i].user.QuadPart) +
					(sppinfo[i].kernel.QuadPart - prev[i].kernel.QuadPart));
			smon.cpu[i] = ((sum - idle) << 7) / sum;
			prev[i] = sppinfo[i];

			allidle += idle;
			allsum += sum;
		}

		smon.single = ((allsum - allidle) << 7) / allsum;
	}
}
