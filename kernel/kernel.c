#include "kernel.h"

#include <providers/zero/zero_provider.h>
#include <providers/term/term_provider.h>
#include <providers/echfs/echfs_provider.h>
#include <providers/ata/ata_provider.h>
#include <providers/ps2/ps2_provider.h>
#include <providers/echfs/echfs.h>

#include <common/string.h>
#include <common/klib.h>
#include <common/common.h>

#include <interrupts/idt.h>
#include <interrupts/irq.h>
#include <interrupts/isr.h>

#include <process/scheduler.h>
#include <process/syscall.h>
#include <process/process.h>

#include <memory/gdt.h>
#include <memory/pmm.h>
#include <memory/vmm.h>

#include <cpu/cpuid.h>
#include <cpu/msr.h>
#include <common/logging.h>

#include "graphics/term.h"

extern void* boot_pdpe;
mm_context_t kernel_memory_manager;

static void thread_kernel(void* arg) {
    uint64_t start = rdtsc();
    resource_t stdout;
    resource_descriptor_t stdout_desc = {
            .scheme = "term"
    };
    open(&stdout_desc, &stdout);
    write(stdout, "In kernel thread\n", sizeof("In kernel thread\n"), NULL);

    resource_t echfs_file;
    resource_descriptor_t ata_desc = {
            .scheme = "ata",
            .domain = "primary",
            .port = 0
    };
    resource_descriptor_t echfs_desc = {
            .scheme = "echfs",
            .path = "",
            .sub = &ata_desc
    };
    if(open(&echfs_desc, &echfs_file)) {
        echfs_directory_entry_t entry;
        fprintf(stdout, "===========\n");
        while(invoke(echfs_file, 1, &entry)) {
            fprintf(stdout, "%c> %s\n", entry.type == ECHFS_OBJECT_TYPE_FILE ? 'F' : 'D', entry.name);
        }
        fprintf(stdout, "===========\n");
        close(echfs_file);
    }else {
        fprintf(stdout, "Failed to open echfs://[ata://primary:0/]/\n");
    }

    echfs_desc.path = "file.txt";
    if(open(&echfs_desc, &echfs_file)) {
        fprintf(stdout, "=file.txt==\n");
        size_t len;
        seek(echfs_file, SEEK_END, 0);
        tell(echfs_file, &len);
        seek(echfs_file, SEEK_START, 0);
        char buffer[len];
        read(echfs_file, &buffer, (int) len, NULL);
        write(stdout, buffer, (int) len, NULL);
        fprintf(stdout, "===========\n");
        close(echfs_file);
    }else {
        fprintf(stdout, "Failed to open echfs://[ata://primary:0/]/file.txt\n");
    }

    echfs_desc.path = "dir_uwu";
    if(open(&echfs_desc, &echfs_file)) {
        echfs_directory_entry_t entry;
        fprintf(stdout, "==dir_uwu==\n");
        while(invoke(echfs_file, 1, &entry)) {
            fprintf(stdout, "%c> %s\n", entry.type == ECHFS_OBJECT_TYPE_FILE ? 'F' : 'D', entry.name);
        }
        fprintf(stdout, "===========\n");
        close(echfs_file);
    }else {
        fprintf(stdout, "Failed to open echfs://[ata://primary:0/]/dir_uwu\n");
    }

    echfs_desc.path = "dir_uwu/owo.txt";
    if(open(&echfs_desc, &echfs_file)) {
        write(stdout, "==owo.txt==\n", sizeof("==owo.txt==\n"), NULL);
        size_t len;
        seek(echfs_file, SEEK_END, 0);
        tell(echfs_file, &len);
        seek(echfs_file, SEEK_START, 0);
        char buffer[len];
        read(echfs_file, &buffer, (int) len, NULL);
        write(stdout, buffer, (int) len, NULL);
        fprintf(stdout, "===========\n");
        close(echfs_file);
    }else {
        fprintf(stdout, "Failed to open echfs://[ata://primary:0/]/dir_uwu/owo.txt\n");
    }
    uint64_t end = rdtsc();
    uint64_t total = end - start;

    resource_t stdin;
    resource_descriptor_t stdin_desc = {
            .scheme = "ps2",
            .domain = "keyboard"
    };
    open(&stdin_desc, &stdin);
    while(true) {
        wait(stdin);
        while(poll(stdin)) {
            char ch;
            read(stdin, &ch, 1, NULL);
            // fprintf(stdout, "got scancode %d\n", ch);
        }
    }
}

void kernel_main(multiboot_info_t* info) {
    error_t err = NO_ERROR;

    // early vmm initialization (maps the first 4gb)
    vmm_early_init();

    // initialize the terminal so we can log stuff
    term_init(info);
    LOG_NOTICE("successfully loaded the kernel");
    LOG_INFO("bootloader='%s'", (char *)(uintptr_t)info->boot_loader_name);
    LOG_INFO("cmdline='%s'", (char *)(uintptr_t)info->cmdline);

    // test cpuid
    int vendor;
    char name[13] = {0};
    get_cpu_vendor(&vendor, name);
    LOG_INFO("cpu vendor='%s'", name);

    // initialize isr (allows to catch exceptions)
    // we will finish initializing interrupts later
    isr_init();
    idt_init();

    // initialize memory stuff
    pmm_init(info);
    vmm_init(info);

    // kernel will be mapped to 0xffffffffffffffff - (MB(512) - GB(2) (ASLR of 1.5GB)
    // heap starts at 0xffffffff00000000 - (MB(512) - GB(2)) (ASLR of 1.5GB)
    // user mode heap will start at MB(512) - GB(2) (ASLR of 1.5GB)

    mm_context_init(&kernel_memory_manager, 0xFFFFFFFF00000000);

    // finish initializing the idt (now includes irq and syscall interrupt)
    irq_init();
    syscall_init();
    idt_init();

    /// ONLY USE ERRORS FROM HERE

    // initialize processes
    CHECK_AND_RETHROW(process_init());

    // initialize resource related stuff
    CHECK_AND_RETHROW(resource_manager_init());
    CHECK_AND_RETHROW(zero_provider_init());
    CHECK_AND_RETHROW(term_provider_init());
    CHECK_AND_RETHROW(ata_provider_init());
    CHECK_AND_RETHROW(echfs_provider_init());
    CHECK_AND_RETHROW(ps2_provider_init());

    // initlize the scheduler
    CHECK_AND_RETHROW(scheduler_init());

    // create some test processes
    process_t* pk = process_create(thread_kernel, true);
    char* kstack = mm_allocate(&kernel_memory_manager, KB(4));
    pk->threads[0]->cpu_state.rbp = (uint64_t)kstack + KB(4);
    pk->threads[0]->cpu_state.rsp = (uint64_t)kstack + KB(4);

    // kick start the system!
    LOG_NOTICE("Enabling interrupts");
    sti();

    while(true) {
        hlt();
    }

cleanup:
    KERNEL_PANIC(err);
}
