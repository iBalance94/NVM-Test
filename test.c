#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>

static long
perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                int cpu, int group_fd, unsigned long flags)
{
    int ret;

    ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
                   group_fd, flags);
    return ret;
}

//每次read()得到的结构体
struct read_format
{
    //计数器数量（为2）
    uint64_t nr;
    //两个计数器的值
    uint64_t values[];
};


int
main(int argc, char **argv)
{
    struct perf_event_attr pe;
    long long count;
    int fd;

    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HW_CACHE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_CACHE_L1D | ( PERF_COUNT_HW_CACHE_OP_READ << 8) 
	        | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16);
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;
    //每次读取一个组
    //pe.read_format = PERF_FORMAT_GROUP;

    fd = perf_event_open(&pe, 0, -1, -1, 0);
    if (fd == -1) {
       fprintf(stderr, "Error opening leader %llx\n", pe.config);
       exit(EXIT_FAILURE);
    }

    ioctl(fd, PERF_EVENT_IOC_RESET, 0);

    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

    //启用（开始计数），注意PERF_IOC_FLAG_GROUP标志
    //ioctl(fd,PERF_EVENT_IOC_ENABLE,PERF_IOC_FLAG_GROUP);
    
    printf("Measuring instruction count for this printf\n");

    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);

    //struct read_format aread;
    //读取最新的计数值，每次读取一个结构体
    //read(fd,&aread,sizeof(struct read_format));
    read(fd, &count, sizeof(long long));

    printf("User mode cache misses %lld\n", count);

    close(fd);
}
