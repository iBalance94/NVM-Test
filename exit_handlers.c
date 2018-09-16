#include <rootkernel/vmcs.h>
#include <rootkernel/exit_handlers.h>
#include <rootkernel/ept.h>
#include <rootkernel/trampoline.h>
#include <rootkernel/msr-index.h>

unsigned long get_real_hardware_cr3(void);
unsigned long get_real_hardware_cr3(void){
    unsigned long cr3_value;
    asm volatile ("mov %%cr3,%0\n\t" : "=r" (cr3_value));
    return cr3_value;
}

static void save_server_cr3(int ept_id, unsigned long server_cr3)
{
    server_cr3_list[ept_id] = server_cr3;
}

static void save_server_entry(int ept_id, unsigned long entry)
{
    function_list[ept_id] = (void (*)(void))(entry);
}

extern int debug_print;
static void map_trampoline_to_server(unsigned long server_cr3)
{
    printf("Before setting server cr %lx, TRAMPOLINE_GVFN ====> %lx\n", server_cr3, get_pt_entry(TRAMPOLINE_GVFN, server_cr3, 0));
    set_pt_entry(TRAMPOLINE_GVFN, kpptr_to_paddr((void *)entry_SeFaL4_trampoline) >> PAGE_SHIFT, server_cr3);
    printf("After  setting server cr %lx, TRAMPOLINE_GVFN ====> %lx\n", server_cr3, get_pt_entry(TRAMPOLINE_GVFN, server_cr3, 0));

    printf("Before setting server cr %lx, TRAMPOLINE_C_CODE_GVFN ====> %lx\n", server_cr3, get_pt_entry(TRAMPOLINE_C_CODE_GVFN, server_cr3, 0));
    set_pt_entry(TRAMPOLINE_C_CODE_GVFN, kpptr_to_paddr((void*)server_call) >> PAGE_SHIFT, server_cr3);
    printf("After  setting server cr %lx, TRAMPOLINE_C_CODE_GVFN ====> %lx\n", server_cr3, get_pt_entry(TRAMPOLINE_C_CODE_GVFN, server_cr3, 0));

//    printf("Before setting server cr %lx, TRAMPOLINE_MIDDLE_AREA ====> %lx\n", server_cr3, get_pt_entry(TRAMPOLINE_MIDDLE_AREA, server_cr3, 0));
//    set_pt_entry(TRAMPOLINE_MIDDLE_AREA, get_pt_entry((unsigned long)trampoline_middle_area >> PAGE_SHIFT, server_cr3, 0), server_cr3);
//    printf("After  setting server cr %lx, TRAMPOLINE_MIDDLE_AREA ====> %lx\n", server_cr3, get_pt_entry(TRAMPOLINE_MIDDLE_AREA, server_cr3, 0));

    printf("Before setting server cr %lx, TRAMPOLINE_FUNCTION_LIST_GVFN ====> %lx\n", server_cr3, get_pt_entry(TRAMPOLINE_FUNCTION_LIST_GVFN, server_cr3, 0));
    set_pt_entry(TRAMPOLINE_FUNCTION_LIST_GVFN, kpptr_to_paddr((void *)function_list) >> PAGE_SHIFT, server_cr3);
    printf("After  setting server cr %lx, TRAMPOLINE_FUNCTION_LIST_GVFN ====> %lx\n", server_cr3, get_pt_entry(TRAMPOLINE_FUNCTION_LIST_GVFN, server_cr3, 0));

    printf("Before setting server cr %lx, TRAMPOLINE_ENTER_FLAGS_GVFN ====> %lx\n", server_cr3, get_pt_entry(TRAMPOLINE_ENTER_FLAGS_GVFN, server_cr3, 0));
    set_pt_entry(TRAMPOLINE_ENTER_FLAGS_GVFN, kpptr_to_paddr((void *)enter_trampoline_flags) >> PAGE_SHIFT, server_cr3);
    printf("After  setting server cr %lx, TRAMPOLINE_ENTER_FLAGS_GVFN ====> %lx\n", server_cr3, get_pt_entry(TRAMPOLINE_ENTER_FLAGS_GVFN, server_cr3, 0));
}

static void map_trampoline_to_client(unsigned long client_cr3)
{
    if(get_pt_entry(TRAMPOLINE_GVFN, client_cr3, 0) == 0){
//        printf("Before setting client cr %lx, TRAMPOLINE_GVFN ====> %lx\n", client_cr3, get_pt_entry(TRAMPOLINE_GVFN, client_cr3, 0));
        set_pt_entry(TRAMPOLINE_GVFN, kpptr_to_paddr((void *)entry_SeFaL4_trampoline) >> PAGE_SHIFT, client_cr3);
        //set_pt_entry(TRAMPOLINE_GVFN, get_pt_entry((unsigned long)entry_SeFaL4_trampoline >> PAGE_SHIFT, client_cr3, 0), client_cr3);
//        printf("After  setting client cr %lx, TRAMPOLINE_GVFN ====> %lx\n", client_cr3, get_pt_entry(TRAMPOLINE_GVFN, client_cr3, 0));

//        printf("Before setting client cr %lx, TRAMPOLINE_C_CODE_GVFN ====> %lx\n", client_cr3, get_pt_entry(TRAMPOLINE_C_CODE_GVFN, client_cr3, 0));
        set_pt_entry(TRAMPOLINE_C_CODE_GVFN, kpptr_to_paddr((void *)server_call) >> PAGE_SHIFT, client_cr3);
//        printf("After  setting client cr %lx, TRAMPOLINE_C_CODE_GVFN ====> %lx\n", client_cr3, get_pt_entry(TRAMPOLINE_C_CODE_GVFN, client_cr3, 0));

//        printf("Before setting client cr %lx, TRAMPOLINE_MIDDLE_AREA ====> %lx\n", client_cr3, get_pt_entry(TRAMPOLINE_MIDDLE_AREA, client_cr3, 0));
//        set_pt_entry(TRAMPOLINE_MIDDLE_AREA, get_pt_entry((unsigned long)trampoline_middle_area >> PAGE_SHIFT, client_cr3, 0), client_cr3);
//        printf("After  setting client cr %lx, TRAMPOLINE_MIDDLE_AREA ====> %lx\n", client_cr3, get_pt_entry(TRAMPOLINE_MIDDLE_AREA, client_cr3, 0));

//        printf("Before setting client cr %lx, TRAMPOLINE_ENTER_FLAGS_GVFN ====> %lx\n", client_cr3, get_pt_entry(TRAMPOLINE_ENTER_FLAGS_GVFN, client_cr3, 0));
        set_pt_entry(TRAMPOLINE_ENTER_FLAGS_GVFN, kpptr_to_paddr((void *)enter_trampoline_flags) >> PAGE_SHIFT, client_cr3);
//        printf("After  setting client cr %lx, TRAMPOLINE_ENTER_FLAGS_GVFN ====> %lx\n", client_cr3, get_pt_entry(TRAMPOLINE_ENTER_FLAGS_GVFN, client_cr3, 0));
    }else{
//        printf("client %lx already has mapping for entry_SeFaL4_trampoline, its gpfn = %lx\n", client_cr3, get_pt_entry(TRAMPOLINE_GVFN, client_cr3, 0));

    }
}

static void map_trampoline_middle_page_to_server(int ept_id, unsigned long server_cr3)
{
    for(int thread_id = 0; thread_id < 20; thread_id++){
        unsigned long *middle_page = (unsigned long *)alloc_dynamic_pages(1);
        if(middle_page == NULL){
            ldjprintf("allocation middle page fail\n");
        }
//                    printf("\tBefore mapping middle_page into cr3 %lx, %lx ====> %lx, will map to %lx, middle_page = %lx\n", server_cr3, TRAMPOLINE_MIDDLE_PAGE_BEGIN_GVFN + ept_id * 20 + thread_id, get_pt_entry(TRAMPOLINE_MIDDLE_PAGE_BEGIN_GVFN+ ept_id * 20 + thread_id, server_cr3, 0), kpptr_to_paddr(((unsigned long)middle_page)) >> PAGE_SHIFT, (unsigned long) middle_page);
        set_pt_entry(TRAMPOLINE_MIDDLE_PAGE_BEGIN_GVFN + ept_id * 20 + thread_id, kpptr_to_paddr(((void *)middle_page)) >> PAGE_SHIFT, server_cr3);
//                    printf("\tAfter mapping middle_page into cr3 %lx, %lx ====> %lx, will map to %lx, middle_page = %lx\n", server_cr3, TRAMPOLINE_MIDDLE_PAGE_BEGIN_GVFN + ept_id * 20 + thread_id, get_pt_entry(TRAMPOLINE_MIDDLE_PAGE_BEGIN_GVFN+ ept_id * 20 + thread_id, server_cr3, 0), kpptr_to_paddr(((unsigned long)middle_page)) >> PAGE_SHIFT, (unsigned long) middle_page);
        server_trampoline_middle_page_gva_list[ept_id * 20 + thread_id] = ((TRAMPOLINE_MIDDLE_PAGE_BEGIN_GVFN + ept_id * 20 + thread_id) << 12);
        server_trampoline_middle_page_hva_list[ept_id * 20 + thread_id] = (unsigned long)middle_page;
//        current_server_stack_gvfn += 8;
    }
    set_pt_entry(TRAMPOLINE_MIDDLE_PAGE_LOCATION_GVFN, kpptr_to_paddr((void *)server_trampoline_middle_page_gva_list) >> PAGE_SHIFT, server_cr3);
}

static void map_trampoline_middle_page_to_client(int ept_id, unsigned long client_cr3)
{
    for(int thread_id = 0; thread_id < 20; thread_id++){
        unsigned long server_middle_page = server_trampoline_middle_page_hva_list[ept_id * 20 + thread_id];
        unsigned long server_middle_page_gva_start = (server_trampoline_middle_page_gva_list[ept_id * 20 + thread_id]) >> 12;
        if(server_middle_page == 0){
            ldjprintf("fail to find server stack for ept %d\n", ept_id);
        }
//        printf("\tBefore mapping middle_page into cr3 %lx, %lx ====> %lx, will map to %lx, middle_page = %lx\n", client_cr3, server_middle_page_gva_start, get_pt_entry(server_middle_page_gva_start, client_cr3, 0), kpptr_to_paddr(((void *)server_middle_page)) >> PAGE_SHIFT, (unsigned long) server_middle_page);
        set_pt_entry(server_middle_page_gva_start, kpptr_to_paddr((void *)server_middle_page) >> PAGE_SHIFT, client_cr3);
//        printf("\tAfter mapping middle_page into cr3 %lx, %lx ====> %lx, will map to %lx, middle_page = %lx\n", client_cr3, server_middle_page_gva_start, get_pt_entry(server_middle_page_gva_start, client_cr3, 0), kpptr_to_paddr(((void *)server_middle_page)) >> PAGE_SHIFT, (unsigned long) server_middle_page);
    }
    set_pt_entry(TRAMPOLINE_MIDDLE_PAGE_LOCATION_GVFN, kpptr_to_paddr((void *)server_trampoline_middle_page_gva_list) >> PAGE_SHIFT, client_cr3);
}

//static unsigned long current_server_stack_gvfn = TRAMPOLINE_STACK_BEGIN_GVFN;
static void map_trampoline_stack_to_server(int ept_id, unsigned long server_cr3)
{
    for(int thread_id = 0; thread_id < 20; thread_id++){
        unsigned long *stack_page = (unsigned long *)alloc_dynamic_pages(8);
        if(stack_page == NULL){
            ldjprintf("allocation stack fail\n");
        }
        int i = 0;
        for(i = 0; i < 8; i++){
            //        printf("\tBefore mapping stack into cr3 %lx, %lx ====> %lx, will map to %lx, stack_page = %lx\n", server_cr3, current_server_stack_gvfn + i, get_pt_entry(current_server_stack_gvfn + i, server_cr3, 0), kpptr_to_paddr(((unsigned long)stack_page) + 4096 * i) >> PAGE_SHIFT, (unsigned long) stack_page);
            set_pt_entry(TRAMPOLINE_STACK_BEGIN_GVFN + ept_id * 20 * 8 + thread_id * 8 + i, kpptr_to_paddr((void *)((unsigned long)stack_page + 4096 * i)) >> PAGE_SHIFT, server_cr3);
            //        printf("\tAfter mapping stack into cr3 %lx, %lx ====> %lx, will map to %lx, stack_page = %lx\n", server_cr3, current_server_stack_gvfn + i, get_pt_entry(current_server_stack_gvfn + i, server_cr3, 0), kpptr_to_paddr(((unsigned long)stack_page) + 4096 * i) >> PAGE_SHIFT, (unsigned long) stack_page);
        }
        server_trampoline_stack_gva_list[ept_id * 20 + thread_id] = ((TRAMPOLINE_STACK_BEGIN_GVFN + ept_id * 20 * 8 + thread_id * 8 + i) << 12);
        server_trampoline_stack_hva_list[ept_id * 20 + thread_id] = (unsigned long)stack_page;
//        current_server_stack_gvfn += 8;
    }
    set_pt_entry(TRAMPOLINE_STACK_LOCATION_GVFN, kpptr_to_paddr((void *)server_trampoline_stack_gva_list) >> PAGE_SHIFT, server_cr3);
}

static void map_trampoline_stack_to_client(int ept_id, unsigned long client_cr3)
{
    for(int thread_id = 0; thread_id < 20; thread_id++){
        unsigned long server_stack_page = server_trampoline_stack_hva_list[ept_id * 20 + thread_id];
        unsigned long server_stack_gva_start = (server_trampoline_stack_gva_list[ept_id * 20 + thread_id] - 4096*8) >> 12;
        if(server_stack_page == 0){
            ldjprintf("fail to find server stack for ept %d\n", ept_id);
        }
        int i = 0;
        for(i = 0; i < 8; i++){
            //        printf("\tBefore mapping stack into client cr3 %lx, %lx ====> %lx, will map to %lx, stack_page = %lx\n", client_cr3, server_stack_gva_start + i, get_pt_entry(server_stack_gva_start + i, client_cr3, 0), kpptr_to_paddr(((unsigned long)server_stack_page) + 4096 * i) >> PAGE_SHIFT, (unsigned long) server_stack_page);
            set_pt_entry(server_stack_gva_start + i, kpptr_to_paddr((void *)(server_stack_page + 4096 * i)) >> PAGE_SHIFT, client_cr3);
            //        printf("\tAfter  mapping stack into client cr3 %lx, %lx ====> %lx, will map to %lx, stack_page = %lx\n", client_cr3, server_stack_gva_start + i, get_pt_entry(server_stack_gva_start + i, client_cr3, 0), kpptr_to_paddr(((unsigned long)server_stack_page) + 4096 * i) >> PAGE_SHIFT, (unsigned long) server_stack_page);
        }
    }
    set_pt_entry(TRAMPOLINE_STACK_LOCATION_GVFN, kpptr_to_paddr((void *)server_trampoline_stack_gva_list) >> PAGE_SHIFT, client_cr3);
}

static unsigned long current_server_buffer_gvfn = TRAMPOLINE_SERVER_BUFFER_BEGIN_GVFN;
static void map_trampoline_buffer_to_server(int ept_id, unsigned long server_cr3)
{
    for(int thread_id = 0; thread_id < 20; thread_id++){
        unsigned long *server_buffer = (unsigned long *)alloc_dynamic_pages(8);
        if(server_buffer == NULL){
            ldjprintf("allocation stack fail\n");
        }
        int i = 0;
        for(i = 0; i < 8; i++){
                    //printf("Before setting buffer into cr3 %lx, %lx ====> %lx, will map to %lx, server_buffer = %lx\n", server_cr3, current_server_buffer_gvfn + i, get_pt_entry(current_server_buffer_gvfn + i, server_cr3, 0), kpptr_to_paddr((void *)((unsigned long)server_buffer + 4096 * i)) >> PAGE_SHIFT, (unsigned long) server_buffer);
            set_pt_entry(current_server_buffer_gvfn + i, kpptr_to_paddr((void *)((unsigned long)server_buffer + 4096 * i)) >> PAGE_SHIFT, server_cr3);
                    //printf("After setting buffer into cr3 %lx, %lx ====> %lx\n", server_cr3, current_server_buffer_gvfn + i, kpptr_to_paddr((void *)((unsigned long)server_buffer + 4096 * i)) >> PAGE_SHIFT);
//                    printf("After  setting buffer into cr3 %lx, %lx ====> %lx\n", server_cr3, current_server_buffer_gvfn + i, get_pt_entry(current_server_buffer_gvfn + i, server_cr3, 0));
        }
        server_trampoline_buffer_gva_list[ept_id * 20 + thread_id] = ((current_server_buffer_gvfn) << 12);
        server_trampoline_buffer_hva_list[ept_id * 20 + thread_id] = (unsigned long)server_buffer;
        //    printf("server_trampoline_buffer_gva_list[%d] = %lx\n", ept_id, server_trampoline_buffer_hva_list[ept_id]);
        current_server_buffer_gvfn += 8;
    }
    set_pt_entry(TRAMPOLINE_SERVER_BUFFER_LIST_GVFN, kpptr_to_paddr((void *)server_trampoline_buffer_gva_list) >> PAGE_SHIFT, server_cr3);
}

static void map_trampoline_buffer_to_client(int ept_id, unsigned long client_cr3)
{
    for(int thread_id = 0; thread_id < 20; thread_id++){
        unsigned long server_buffer_page = server_trampoline_buffer_hva_list[ept_id * 20 + thread_id];
        unsigned long server_buffer_gva_start = (server_trampoline_buffer_gva_list[ept_id * 20 + thread_id] >> 12);
        if(server_buffer_page == 0){
            ldjprintf("fail to find server buffer for ept %d\n", ept_id);
        }
        int i = 0;
        for(i = 0; i < 8; i++){
            //        printf("Before setting buffer into cr3 %lx, %lx ====> %lx, server_buffer = %lx\n", client_cr3, server_buffer_gva_start + i, get_pt_entry(server_buffer_gva_start + i, client_cr3, 0), server_buffer_page);
            set_pt_entry(server_buffer_gva_start + i, kpptr_to_paddr((void *)(server_buffer_page + 4096 * i)) >> PAGE_SHIFT, client_cr3);
            //        printf("After  setting buffer into cr3 %lx, %lx ====> %lx\n", client_cr3, server_buffer_gva_start + i, get_pt_entry(server_buffer_gva_start + i, client_cr3, 0));
        }
    }
    set_pt_entry(TRAMPOLINE_SERVER_BUFFER_LIST_GVFN, kpptr_to_paddr((void *)server_trampoline_buffer_gva_list) >> PAGE_SHIFT, client_cr3);
}

u64 client_cr3 = 0UL;
int cpuid_flag = 0;
unsigned long fault_count_flag = 0;
unsigned long fault_count = 0;
unsigned long vmfunc_resched_count = 0;
unsigned long preempt_count = 0;
unsigned long begin_cycles = 0;
unsigned long end_cycles = 0;
unsigned long fault_type_count[256] = {
    0
};
unsigned long page_fault_address[300] = {
    0
};
unsigned long schedule_from_rip[500] = {
    0
};
unsigned long schedule_from_cr3[500] = {
    0
};
unsigned long schedule_to_rip[500] = {
    0
};
unsigned long schedule_to_cr3[500] = {
    0
};
unsigned long schedule_return_address[500] = {
    0
};
int current_page_fault_address_index = 0;
extern int debug_print;

static unsigned long dTLB_miss = 0, llc_miss = 0, iTLB_miss = 0, L1D_miss = 0, retired_inst = 0, L1I_miss = 0, L2_miss = 0;
static int miss_cnt = 0;

static int vmx_handle_cpuid(struct gregs *regs)
{
    unsigned long fn, eax, ebx, ecx, edx;
    unsigned int insn_len;
    fn = regs->rax;
    insn_len = vmcs_read32(VM_EXIT_INSTRUCTION_LEN);

    switch(fn){
        case 0x12340: // Server save CR3
            {
                unsigned long server_cr3 = vmcs_readl(GUEST_CR3);
                int ept_id = (int)(regs->rcx);
                unsigned long entry = (unsigned long)regs->rbx;
                printf("register server info: ept_id = %d, cr3 = %lx, entry = %lx\n", ept_id, server_cr3, entry);
                save_server_cr3(ept_id, server_cr3);
                save_server_entry(ept_id, entry);
                map_trampoline_to_server(server_cr3);
                map_trampoline_stack_to_server(ept_id, server_cr3);
                map_trampoline_middle_page_to_server(ept_id, server_cr3);
                map_trampoline_buffer_to_server(ept_id, server_cr3);

                break;
            }
        case 0x12342: // Client save CR3
            {
                unsigned long client_cr3 = vmcs_readl(GUEST_CR3);
                int server_id = (int)(regs->rbx);
                unsigned long server_cr3 = server_cr3_list[server_id];
                map_trampoline_to_client(client_cr3);
                printf("client cr3 value %lx, server_id = %d\n", client_cr3, server_id);
                epte_t server_eptp;
                server_eptp.eptp = ept_ptr_list[server_id];
                ept_entry_t *server_ept = (ept_entry_t *)paddr_to_pptr(server_eptp.asr << PAGE_SHIFT);
                unsigned long old_hpf = get_ept_entry(client_cr3 >> PAGE_SHIFT, server_ept, 1);
                printf("hpfn for %lx is %lx\n", client_cr3, get_ept_entry(client_cr3 >> PAGE_SHIFT, server_ept, 0));
                if(old_hpf == (client_cr3 >> PAGE_SHIFT)){
                    set_ept_entry(client_cr3 >> PAGE_SHIFT, server_cr3 >> PAGE_SHIFT, server_ept);
                    printf("hpfn for %lx is %lx\n", client_cr3, get_ept_entry(client_cr3 >> PAGE_SHIFT, server_ept, 0));
                }
                map_trampoline_stack_to_client(server_id, client_cr3);
                map_trampoline_middle_page_to_client(server_id, client_cr3);
                map_trampoline_buffer_to_client(server_id, client_cr3);
                printf("finish register client to server %d\n", server_id);
                break;
            }
        case 0x12343: //free client cr3
            {
                unsigned long client_cr3 = vmcs_readl(GUEST_CR3);
                int ept_id = (int)(regs->rbx);
                epte_t server_eptp;
                server_eptp.eptp = ept_ptr_list[ept_id];
                ept_entry_t *server_ept = (ept_entry_t *)paddr_to_pptr(server_eptp.asr << PAGE_SHIFT);
                unsigned long old_hpf = get_ept_entry(client_cr3 >> PAGE_SHIFT, server_ept, 1);
                if(old_hpf != (client_cr3 >> PAGE_SHIFT)){
                    set_ept_entry(client_cr3 >> PAGE_SHIFT, client_cr3 >> PAGE_SHIFT, server_ept);
                    delete_pt_mapping(TRAMPOLINE_GVFN, client_cr3);

                    delete_pt_mapping(TRAMPOLINE_C_CODE_GVFN, client_cr3);
                    delete_pt_mapping(TRAMPOLINE_ENTER_FLAGS_GVFN, client_cr3);
                    delete_pt_mapping(TRAMPOLINE_FUNCTION_LIST_GVFN, client_cr3);
                    delete_pt_mapping(TRAMPOLINE_STACK_LOCATION_GVFN, client_cr3);

                    //delete_pt_mapping(TRAMPOLINE_MIDDLE_AREA, client_cr3);
                }
                break;
            }
        case 0x12346:
            {
                printf("cpuid = %d, cpuid rip = %lx rsp = %lx, cr3 = %lx rbx = %lx, rcx = %lu, rdx = %lx\n", smp_processor_id(), vmcs_readl(GUEST_RIP), vmcs_readl(GUEST_RSP), vmcs_readl(GUEST_CR3), regs->rbx, regs->rcx, regs->rdx);
                break;
            }
        case 0x12347:
            {
                fault_count_flag = 1;
                fault_count = 0;
                vmfunc_resched_count = 0;
                current_page_fault_address_index = 0;
                preempt_count = 0;
                for(int i = 0; i < 300; i++){
                    page_fault_address[i] = 0;
                }

                for(int i = 0; i < 256; i++){
                    fault_type_count[i] = 0;
                }
                break;
            }
        case 0x12348:
            {
                printf("schedule_count = %lu\n", vmfunc_resched_count);
                printf("fault count = %lu\n", fault_count);
                printf("preempt count = %lu\n", preempt_count);
                for(int i = 0; i < 256; i++){
                    if(fault_type_count[i] != 0){
                        printf("fault_type_count[%d] = %lu\n", i, fault_type_count[i]);
                    }
                }
                for(int i = 0; i < 300; i++){
                    if(page_fault_address[i] != 0){
                        printf("page_fault_address[%d] = %lx\n", i, page_fault_address[i]);
                    }
                }
                for(unsigned long i = 0; i < vmfunc_resched_count; i++){
                    printf("%lu: schedule from cr3 %lx, rip %lx to cr3 %lx, rip %lx return_address = %lx\n", i, schedule_from_cr3[i], schedule_from_rip[i], schedule_to_cr3[i], schedule_to_rip[i], schedule_return_address[i]);
                }
                break;
            }
        case 0x12349:
            {
                unsigned long address = regs->rbx;
                unsigned long gvfn = address >> 12;
                debug_print = 1;
                printf("cr2 = %lx, its gpfn = %lx\n", address, get_pt_entry(gvfn, server_cr3_list[1], 0));
                debug_print = 0;
                printf("core %d, ept_pointer = %lx, eptp_list_address = %lx\n", smp_processor_id(), vmcs_readl(EPT_POINTER), vmcs_readl(EPTP_LIST_ADDR));
                break;
            }
        case 0x1234a:
            {
                printf("!!!!!!!!!!!!!!!!vmfunc to 0\n");
                break;
            }
        case 0x1234b:
            {
                printf("cr3 = %lx, cycles = %llu, core %d, ept_pointer = %lx, eptp_list_address = %lx\n", vmcs_readl(GUEST_CR3), mzy_rdtsc(), smp_processor_id(), vmcs_readl(EPT_POINTER), vmcs_readl(EPTP_LIST_ADDR));
                break;
            }
        case 0x1234c:
            {

                unsigned long rbx = regs->rbx;
                if(rbx == 1){
                    printf("will count retired instruction, icache, dcache\n");
                    x86_wrmsr(0x38F, 0x7); // IA32_PERF_GLOBAL_CTRL
                    x86_wrmsr(0xc1, 0);
                    x86_wrmsr(0xc2, 0);
                    x86_wrmsr(0xc3, 0);
                    x86_wrmsr(0x186, 0x4300c0); // retired instruction
                    x86_wrmsr(0x187, 0x430283); // icache
                    x86_wrmsr(0x188, 0x430151); // dcache
                }else if(rbx == 2){
                    printf("will count l2 cahce, l3 cache, itlb\n");
                    x86_wrmsr(0x38F, 0x7); // IA32_PERF_GLOBAL_CTRL
                    x86_wrmsr(0xc1, 0);
                    x86_wrmsr(0xc2, 0);
                    x86_wrmsr(0xc3, 0);
                    x86_wrmsr(0x186, 0x433f24); // l2 cache
                    x86_wrmsr(0x187, 0x43412e); // llc 
                    x86_wrmsr(0x188, 0x410185); // iTLB misses
                }else if(rbx == 3){
                    printf("will count dtlb\n");
                    x86_wrmsr(0x38F, 0x7); // IA32_PERF_GLOBAL_CTRL
                    x86_wrmsr(0xc1, 0);
                    x86_wrmsr(0xc2, 0);
                    x86_wrmsr(0xc3, 0);
                    x86_wrmsr(0x186, 0x410108); // TLB misses
                }
                begin_cycles = mzy_rdtsc();
                break;
            }
        case 0x1234d:
            {
                end_cycles = mzy_rdtsc();
                unsigned long count1 = 0;
                unsigned long count2 = 0;
                unsigned long count3 = 0;
                count1 = x86_rdmsr(0xc1);
                count2 = x86_rdmsr(0xc2);
                count3 = x86_rdmsr(0xc3);
                printf("cycles = %lu, count1 = %lu, count2 = %lu, count3 = %lu\n", end_cycles - begin_cycles, count1, count2, count3);
                break;
            }
        case 0x13131313: // start
            x86_wrmsr(0x38F, 0x7); // MSR_PERF_GLOBAL_CTRL
            x86_wrmsr(0x38D, 0xb); // MSR_PERF_FIXED_CTR_CTRL
            x86_wrmsr(0x309, 0xb); // MSR_PERF_FIXED_CTR0/IA32_PERF_FIXED_CTR0
            x86_wrmsr(0xC1, 0);
            x86_wrmsr(0xC2, 0);
            x86_wrmsr(0xC3, 0);
            /* prefix needs to be 0x43 to also enable OS mode, IA32_PERFEVTSELx, in doc page 3387 */
            if (miss_cnt == 0) {
                x86_wrmsr(0x186, 0x410108); // dTLB
                x86_wrmsr(0x187, 0x41412e); // llc
                x86_wrmsr(0x188, 0x4100c5); // MBI
            } else if (miss_cnt == 1) {
                x86_wrmsr(0x186, 0x410151); // L1D
                x86_wrmsr(0x187, 0x4100C0); // retired inst (0x4100C0 at 7th CPU, 0x410100 at Xeon CPU)
                x86_wrmsr(0x188, 0x410283); // L1I
            } else if (miss_cnt == 2) {
                x86_wrmsr(0x186, 0x413F24); // All l2 misses
            }
            break;
        case 0x23232323: // stop
            if (miss_cnt == 0) {
                dTLB_miss = x86_rdmsr(0xc1);
                llc_miss = x86_rdmsr(0xc2);
                iTLB_miss = x86_rdmsr(0xc3);
            } else if (miss_cnt == 1) {
                L1D_miss = x86_rdmsr(0xc1);
                retired_inst = x86_rdmsr(0xc2);
                L1I_miss = x86_rdmsr(0xc3);
            } else if (miss_cnt == 2) {
                L2_miss = x86_rdmsr(0xc1);
            }
            ++miss_cnt;
            break;
        case 0x13132323: // print
            printf("dTLB_miss = %lu, llc_miss = %lu, MBI_miss = %lu\n", dTLB_miss, llc_miss, iTLB_miss);
            printf("L1D_miss = %lu, retired_inst = %lu, L1I_miss = %lu\n", L1D_miss, retired_inst, L1I_miss);
            printf("L2_miss = %lu\n", L2_miss);
            break;
        default:
            {
                eax = fn;
                cpuid_count(fn, regs->rcx, &eax, &ebx, &ecx, &edx);

                regs->rax = eax;
                regs->rbx = ebx;
                regs->rcx = ecx;
                regs->rdx = edx;

                break;
            }
    }

    if (insn_len)
        vmcs_writel(GUEST_RIP, vmcs_readl(GUEST_RIP) + insn_len);

    return 1;
}

static int (*vmx_exit_handlers[62])(struct gregs *regs) = {
    0
};

static const int vmx_max_exit_handlers = ARRAY_SIZE(vmx_exit_handlers);


//extern "C" int rootkernel_do_vmexit(struct gregs *regs);
int rootkernel_do_vmexit(struct gregs *regs)
{
    u64 exit_reason = vmcs_readl(VM_EXIT_REASON);
    u64 exit_qualification = vmcs_readl(EXIT_QUALIFICATION);
    u64 guest_physical_address = vmcs_readl(GUEST_PHYSICAL_ADDRESS);
    u64 rip = vmcs_readl(GUEST_RIP);
    u64 rsp = vmcs_readl(GUEST_RSP);
    u16 vpid = vmcs_read16(VIRTUAL_PROCESSOR_ID);
    u64 _vmx_pin_based_exec_control = vmcs_readl(PIN_BASED_VM_EXEC_CONTROL);
    u64 _vmx_cpu_based_exec_control = vmcs_readl(CPU_BASED_VM_EXEC_CONTROL);
    u64 _vmx_secondary_exec_control = vmcs_readl(SECONDARY_VM_EXEC_CONTROL);
    u64 interrupt_info = vmcs_readl(VM_EXIT_INTR_INFO);
    int ret = 0;

    if(exit_reason != EXIT_REASON_CPUID)
        printf("CPU %d, %s called, ept_pointer = %lx, vpid = %x exit_reason = %lu, rip = %lx, rsp = %lx, rdx = %lx, qualification = %lx, guest_physical_address = %lx, pin_based_vm_control = %lx, cpu_based_vm_control = %lx secondary_exec_contro = %lx, vector = %lx, type = %d\n", smp_processor_id(), __func__, vmcs_readl(EPT_POINTER), vpid, exit_reason, rip, rsp, regs->rdx, exit_qualification, guest_physical_address, _vmx_pin_based_exec_control, _vmx_cpu_based_exec_control, _vmx_secondary_exec_control, interrupt_info & 0xff, (int)((interrupt_info >> 8) & 0x7));
    if (exit_reason < vmx_max_exit_handlers && vmx_exit_handlers[exit_reason]) {
        ret = vmx_exit_handlers[exit_reason](regs);
    }
///   ldjprintf("before return, rip = %lx\n", rip);
    return ret;
}

void init_vmexit_handlers(void)
{
    vmx_exit_handlers[EXIT_REASON_CPUID] = vmx_handle_cpuid;
}
