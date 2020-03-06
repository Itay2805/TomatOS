#include <util/symlist.h>

struct symlist_t symlist[] = {
// 0000000000000000 B current_cpu_index
// 0000000000000008 a GDT_KERNEL_CODE
// 0000000000000008 a GDT_KERNEL_CODE
// 0000000000000008 B kernel_stack
// 0000000000000010 a GDT_KERNEL_DATA
// 0000000000000010 a GDT_KERNEL_DATA
// 0000000000000010 B tss
// 000000000000001b a GDT_USER_DATA
// 000000000000001b a GDT_USER_DATA
// 0000000000000023 a GDT_USER_CODE
// 0000000000000023 a GDT_USER_CODE
// 0000000000000078 B current_vmm_handle
// 0000000000000080 b idle_thread
// 0000000000000088 B current_thread
// 0000000000000090 B kernel_pcpu_data_size
// 0000000000100000 B kernel_physical_start
// 0000000000100000 R tboot_main
// 000000000019d818 B kernel_physical_end
// ffffffffc0101000 T acpi_tables_init
    {0xffffffffc0101000ull, "acpi_tables_init"},
// ffffffffc0101040 T madt_init
    {0xffffffffc0101040ull, "madt_init"},
// ffffffffc01012c0 T madt_get_next
    {0xffffffffc01012c0ull, "madt_get_next"},
// ffffffffc0101390 T rsdp_init
    {0xffffffffc0101390ull, "rsdp_init"},
// ffffffffc01014b0 T rsdt_init
    {0xffffffffc01014b0ull, "rsdt_init"},
// ffffffffc01015f0 T rsdt_search
    {0xffffffffc01015f0ull, "rsdt_search"},
// ffffffffc0101790 T trace_table
    {0xffffffffc0101790ull, "trace_table"},
// ffffffffc0101830 T __outbyte
    {0xffffffffc0101830ull, "__outbyte"},
// ffffffffc0101850 T __outdword
    {0xffffffffc0101850ull, "__outdword"},
// ffffffffc0101870 T __outword
    {0xffffffffc0101870ull, "__outword"},
// ffffffffc0101890 T __inbyte
    {0xffffffffc0101890ull, "__inbyte"},
// ffffffffc01018b0 T __inword
    {0xffffffffc01018b0ull, "__inword"},
// ffffffffc01018d0 T __indword
    {0xffffffffc01018d0ull, "__indword"},
// ffffffffc01018f0 T __readcr8
    {0xffffffffc01018f0ull, "__readcr8"},
// ffffffffc0101910 T __writecr8
    {0xffffffffc0101910ull, "__writecr8"},
// ffffffffc0101930 T __readcr4
    {0xffffffffc0101930ull, "__readcr4"},
// ffffffffc0101960 T __writecr4
    {0xffffffffc0101960ull, "__writecr4"},
// ffffffffc0101980 T __readcr3
    {0xffffffffc0101980ull, "__readcr3"},
// ffffffffc01019a0 T __writecr3
    {0xffffffffc01019a0ull, "__writecr3"},
// ffffffffc01019c0 T __readcr2
    {0xffffffffc01019c0ull, "__readcr2"},
// ffffffffc01019e0 T __writecr2
    {0xffffffffc01019e0ull, "__writecr2"},
// ffffffffc0101a00 T __readcr0
    {0xffffffffc0101a00ull, "__readcr0"},
// ffffffffc0101a30 T __writecr0
    {0xffffffffc0101a30ull, "__writecr0"},
// ffffffffc0101a50 T __readmsr
    {0xffffffffc0101a50ull, "__readmsr"},
// ffffffffc0101a90 T __writemsr
    {0xffffffffc0101a90ull, "__writemsr"},
// ffffffffc0101ad0 T __cpuid
    {0xffffffffc0101ad0ull, "__cpuid"},
// ffffffffc0101b30 T __cpuidex
    {0xffffffffc0101b30ull, "__cpuidex"},
// ffffffffc0101b90 T cpuid
    {0xffffffffc0101b90ull, "cpuid"},
// ffffffffc0101c30 T __invlpg
    {0xffffffffc0101c30ull, "__invlpg"},
// ffffffffc0101c50 T __halt
    {0xffffffffc0101c50ull, "__halt"},
// ffffffffc0101c60 T __nop
    {0xffffffffc0101c60ull, "__nop"},
// ffffffffc0101c70 T __ltr
    {0xffffffffc0101c70ull, "__ltr"},
// ffffffffc0101c90 T __swapgs
    {0xffffffffc0101c90ull, "__swapgs"},
// ffffffffc0101ca0 T _BitScanReverse
    {0xffffffffc0101ca0ull, "_BitScanReverse"},
// ffffffffc0101ce0 T _BitScanReverse64
    {0xffffffffc0101ce0ull, "_BitScanReverse64"},
// ffffffffc0101d20 T __readmsr_efer
    {0xffffffffc0101d20ull, "__readmsr_efer"},
// ffffffffc0101d40 T __writemsr_efer
    {0xffffffffc0101d40ull, "__writemsr_efer"},
// ffffffffc0101d60 T __readmsr_apic_base
    {0xffffffffc0101d60ull, "__readmsr_apic_base"},
// ffffffffc0101d80 T generate_device_address
    {0xffffffffc0101d80ull, "generate_device_address"},
// ffffffffc0101ec0 T register_component
    {0xffffffffc0101ec0ull, "register_component"},
// ffffffffc0102090 T get_component
    {0xffffffffc0102090ull, "get_component"},
// ffffffffc0102200 T release_component
    {0xffffffffc0102200ull, "release_component"},
// ffffffffc0102320 T get_next_component
    {0xffffffffc0102320ull, "get_next_component"},
// ffffffffc0102370 T get_primary
    {0xffffffffc0102370ull, "get_primary"},
// ffffffffc0102490 T set_primary
    {0xffffffffc0102490ull, "set_primary"},
// ffffffffc01024d0 T fs_open
    {0xffffffffc01024d0ull, "fs_open"},
// ffffffffc0102650 T fs_is_readonly
    {0xffffffffc0102650ull, "fs_is_readonly"},
// ffffffffc0102770 T file_read
    {0xffffffffc0102770ull, "file_read"},
// ffffffffc0102980 T file_read_ex
    {0xffffffffc0102980ull, "file_read_ex"},
// ffffffffc0102ce0 T file_seek
    {0xffffffffc0102ce0ull, "file_seek"},
// ffffffffc0102db0 T file_close
    {0xffffffffc0102db0ull, "file_close"},
// ffffffffc0102e70 T tar_tell
    {0xffffffffc0102e70ull, "tar_tell"},
// ffffffffc0102f20 T tar_read
    {0xffffffffc0102f20ull, "tar_read"},
// ffffffffc0103060 T create_initrd_fs
    {0xffffffffc0103060ull, "create_initrd_fs"},
// ffffffffc01031c0 t tar_size
    {0xffffffffc01031c0ull, "tar_size"},
// ffffffffc0103240 t tar_open
    {0xffffffffc0103240ull, "tar_open"},
// ffffffffc01033a0 t tar_is_readonly
    {0xffffffffc01033a0ull, "tar_is_readonly"},
// ffffffffc01033d0 t tar_close
    {0xffffffffc01033d0ull, "tar_close"},
// ffffffffc0103400 t tar_eof
    {0xffffffffc0103400ull, "tar_eof"},
// ffffffffc01034c0 t tar_seek
    {0xffffffffc01034c0ull, "tar_seek"},
// ffffffffc01036f0 T vfs_open
    {0xffffffffc01036f0ull, "vfs_open"},
// ffffffffc0103810 T vfs_resolve
    {0xffffffffc0103810ull, "vfs_resolve"},
// ffffffffc01039f0 T vfs_mount
    {0xffffffffc01039f0ull, "vfs_mount"},
// ffffffffc0103a40 T sha1_transform
    {0xffffffffc0103a40ull, "sha1_transform"},
// ffffffffc0103df0 T sha1_init
    {0xffffffffc0103df0ull, "sha1_init"},
// ffffffffc0103e80 T sha1_update
    {0xffffffffc0103e80ull, "sha1_update"},
// ffffffffc0103f30 T sha1_final
    {0xffffffffc0103f30ull, "sha1_final"},
// ffffffffc01041f0 T events_init
    {0xffffffffc01041f0ull, "events_init"},
// ffffffffc0104260 T raise_tpl
    {0xffffffffc0104260ull, "raise_tpl"},
// ffffffffc01042f0 T set_tpl
    {0xffffffffc01042f0ull, "set_tpl"},
// ffffffffc0104350 T restore_tpl
    {0xffffffffc0104350ull, "restore_tpl"},
// ffffffffc0104440 t dispatch_event_notifies
    {0xffffffffc0104440ull, "dispatch_event_notifies"},
// ffffffffc0104590 T get_tpl
    {0xffffffffc0104590ull, "get_tpl"},
// ffffffffc01045f0 T create_event
    {0xffffffffc01045f0ull, "create_event"},
// ffffffffc0104710 T signal_event
    {0xffffffffc0104710ull, "signal_event"},
// ffffffffc0104860 T check_event
    {0xffffffffc0104860ull, "check_event"},
// ffffffffc0104930 T wait_for_event
    {0xffffffffc0104930ull, "wait_for_event"},
// ffffffffc0104c80 T close_event
    {0xffffffffc0104c80ull, "close_event"},
// ffffffffc0104d30 t aquire_event_lock
    {0xffffffffc0104d30ull, "aquire_event_lock"},
// ffffffffc0104d50 t release_event_lock
    {0xffffffffc0104d50ull, "release_event_lock"},
// ffffffffc0104d70 T init_pit8254
    {0xffffffffc0104d70ull, "init_pit8254"},
// ffffffffc0104dc0 t set_timer_period
    {0xffffffffc0104dc0ull, "set_timer_period"},
// ffffffffc0104e70 t interrupt_handler
    {0xffffffffc0104e70ull, "interrupt_handler"},
// ffffffffc0104ea0 t set_pit_count
    {0xffffffffc0104ea0ull, "set_pit_count"},
// ffffffffc0104f00 T init_timer
    {0xffffffffc0104f00ull, "init_timer"},
// ffffffffc0105050 t check_timers
    {0xffffffffc0105050ull, "check_timers"},
// ffffffffc01051a0 T timer_tick
    {0xffffffffc01051a0ull, "timer_tick"},
// ffffffffc0105210 T get_current_system_time
    {0xffffffffc0105210ull, "get_current_system_time"},
// ffffffffc0105230 T set_timer
    {0xffffffffc0105230ull, "set_timer"},
// ffffffffc0105400 t insert_event_timer
    {0xffffffffc0105400ull, "insert_event_timer"},
// ffffffffc01054a0 T ioapic_add
    {0xffffffffc01054a0ull, "ioapic_add"},
// ffffffffc01055c0 t ioapic_read
    {0xffffffffc01055c0ull, "ioapic_read"},
// ffffffffc0105640 T ioapic_add_iso
    {0xffffffffc0105640ull, "ioapic_add_iso"},
// ffffffffc01056d0 T ioapic_enable_interrupt
    {0xffffffffc01056d0ull, "ioapic_enable_interrupt"},
// ffffffffc0105850 t get_ioapic
    {0xffffffffc0105850ull, "get_ioapic"},
// ffffffffc0105970 t ioapic_write
    {0xffffffffc0105970ull, "ioapic_write"},
// ffffffffc01059f0 T ioapic_configure_interrupt
    {0xffffffffc01059f0ull, "ioapic_configure_interrupt"},
// ffffffffc0105ce0 T get_apic_id
    {0xffffffffc0105ce0ull, "get_apic_id"},
// ffffffffc0105d00 t lapic_read
    {0xffffffffc0105d00ull, "lapic_read"},
// ffffffffc0105d30 T lapic_init
    {0xffffffffc0105d30ull, "lapic_init"},
// ffffffffc0105e50 t lapic_write
    {0xffffffffc0105e50ull, "lapic_write"},
// ffffffffc0105e80 T send_fixed_ipi
    {0xffffffffc0105e80ull, "send_fixed_ipi"},
// ffffffffc0105ea0 T send_apic_eoi
    {0xffffffffc0105ea0ull, "send_apic_eoi"},
// ffffffffc0105ec0 t spurious_interrupt_handler
    {0xffffffffc0105ec0ull, "spurious_interrupt_handler"},
// ffffffffc0105ee0 T idt_init
    {0xffffffffc0105ee0ull, "idt_init"},
// ffffffffc0107200 t set_idt_entry
    {0xffffffffc0107200ull, "set_idt_entry"},
// ffffffffc01073d0 T idt_post_tss_init
    {0xffffffffc01073d0ull, "idt_post_tss_init"},
// ffffffffc0107560 T interrupts_init
    {0xffffffffc0107560ull, "interrupts_init"},
// ffffffffc01075d0 T common_interrupt_handler
    {0xffffffffc01075d0ull, "common_interrupt_handler"},
// ffffffffc0107770 t default_interrupt_handler
    {0xffffffffc0107770ull, "default_interrupt_handler"},
// ffffffffc0107fd0 T interrupt_register
    {0xffffffffc0107fd0ull, "interrupt_register"},
// ffffffffc0108160 T interrupt_free
    {0xffffffffc0108160ull, "interrupt_free"},
// ffffffffc01081f0 t interrupt_allocate
    {0xffffffffc01081f0ull, "interrupt_allocate"},
// ffffffffc0108360 T kernel_main
    {0xffffffffc0108360ull, "kernel_main"},
// ffffffffc0108500 t main_thread
    {0xffffffffc0108500ull, "main_thread"},
// ffffffffc0108650 T init_tss_for_cpu
    {0xffffffffc0108650ull, "init_tss_for_cpu"},
// ffffffffc0108730 T kernel_tlsf_map
    {0xffffffffc0108730ull, "kernel_tlsf_map"},
// ffffffffc01087d0 T kernel_tlsf_unmap
    {0xffffffffc01087d0ull, "kernel_tlsf_unmap"},
// ffffffffc0108810 T mm_init
    {0xffffffffc0108810ull, "mm_init"},
// ffffffffc0108890 T mm_allocate
    {0xffffffffc0108890ull, "mm_allocate"},
// ffffffffc01088c0 t tlsf_calloc
    {0xffffffffc01088c0ull, "tlsf_calloc"},
// ffffffffc01088f0 T mm_reallocate
    {0xffffffffc01088f0ull, "mm_reallocate"},
// ffffffffc0108920 t tlsf_realloc
    {0xffffffffc0108920ull, "tlsf_realloc"},
// ffffffffc0108960 T mm_free
    {0xffffffffc0108960ull, "mm_free"},
// ffffffffc0108990 T pmm_init
    {0xffffffffc0108990ull, "pmm_init"},
// ffffffffc0108b10 t add_range
    {0xffffffffc0108b10ull, "add_range"},
// ffffffffc0108d80 T pmm_post_vmm
    {0xffffffffc0108d80ull, "pmm_post_vmm"},
// ffffffffc0108fd0 T pmm_allocate_pages
    {0xffffffffc0108fd0ull, "pmm_allocate_pages"},
// ffffffffc0109360 t find_free_pages
    {0xffffffffc0109360ull, "find_free_pages"},
// ffffffffc0109570 t convert_page
    {0xffffffffc0109570ull, "convert_page"},
// ffffffffc0109ae0 T pmm_free_pages
    {0xffffffffc0109ae0ull, "pmm_free_pages"},
// ffffffffc0109bf0 t remove_mem_map_entry
    {0xffffffffc0109bf0ull, "remove_mem_map_entry"},
// ffffffffc0109c40 t move_temp_entries
    {0xffffffffc0109c40ull, "move_temp_entries"},
// ffffffffc0109e00 t allocate_memory_map_entry
    {0xffffffffc0109e00ull, "allocate_memory_map_entry"},
// ffffffffc0109ed0 t allocate_pool_pages
    {0xffffffffc0109ed0ull, "allocate_pool_pages"},
// ffffffffc0109f70 T allocate_stack
    {0xffffffffc0109f70ull, "allocate_stack"},
// ffffffffc010a170 T free_stack
    {0xffffffffc010a170ull, "free_stack"},
// ffffffffc010a280 t acquire_alloc_lock
    {0xffffffffc010a280ull, "acquire_alloc_lock"},
// ffffffffc010a2a0 t release_alloc_lock
    {0xffffffffc010a2a0ull, "release_alloc_lock"},
// ffffffffc010a2c0 T vmm_user_allocate
    {0xffffffffc010a2c0ull, "vmm_user_allocate"},
// ffffffffc010a710 t vmm_umem_find_free_pages
    {0xffffffffc010a710ull, "vmm_umem_find_free_pages"},
// ffffffffc010a920 t vmm_umem_convert_page
    {0xffffffffc010a920ull, "vmm_umem_convert_page"},
// ffffffffc010add0 T vmm_user_free
    {0xffffffffc010add0ull, "vmm_user_free"},
// ffffffffc010aff0 T vmm_create_user_mem
    {0xffffffffc010aff0ull, "vmm_create_user_mem"},
// ffffffffc010b060 t vmm_umem_add_range
    {0xffffffffc010b060ull, "vmm_umem_add_range"},
// ffffffffc010b230 T vmm_destroy_user_mem
    {0xffffffffc010b230ull, "vmm_destroy_user_mem"},
// ffffffffc010b350 t vmm_umem_remove_memory_map_entry
    {0xffffffffc010b350ull, "vmm_umem_remove_memory_map_entry"},
// ffffffffc010b3c0 t allocate_block
    {0xffffffffc010b3c0ull, "allocate_block"},
// ffffffffc010b510 T vmm_init
    {0xffffffffc010b510ull, "vmm_init"},
// ffffffffc010b6f0 T vmm_map
    {0xffffffffc010b6f0ull, "vmm_map"},
// ffffffffc010b960 T vmm_set_handle
    {0xffffffffc010b960ull, "vmm_set_handle"},
// ffffffffc010b9a0 T vmm_enable_cpu_features
    {0xffffffffc010b9a0ull, "vmm_enable_cpu_features"},
// ffffffffc010bb50 T vmm_virtual_to_physical
    {0xffffffffc010bb50ull, "vmm_virtual_to_physical"},
// ffffffffc010bd50 t get_entry_at_virtual_address
    {0xffffffffc010bd50ull, "get_entry_at_virtual_address"},
// ffffffffc010bfd0 T vmm_is_mapped
    {0xffffffffc010bfd0ull, "vmm_is_mapped"},
// ffffffffc010c150 t page_size_by_type
    {0xffffffffc010c150ull, "page_size_by_type"},
// ffffffffc010c200 T vmm_unmap
    {0xffffffffc010c200ull, "vmm_unmap"},
// ffffffffc010c440 t set_pte
    {0xffffffffc010c440ull, "set_pte"},
// ffffffffc010cfe0 T vmm_set_perms
    {0xffffffffc010cfe0ull, "vmm_set_perms"},
// ffffffffc010d1b0 T vmm_get_handle
    {0xffffffffc010d1b0ull, "vmm_get_handle"},
// ffffffffc010d1c0 T vmm_create_address_space
    {0xffffffffc010d1c0ull, "vmm_create_address_space"},
// ffffffffc010d2f0 T vmm_destroy_page_table
    {0xffffffffc010d2f0ull, "vmm_destroy_page_table"},
// ffffffffc010d670 T load_elf
    {0xffffffffc010d670ull, "load_elf"},
// ffffffffc010dcd0 T init_kernel_process
    {0xffffffffc010dcd0ull, "init_kernel_process"},
// ffffffffc010dcf0 T create_process
    {0xffffffffc010dcf0ull, "create_process"},
// ffffffffc010de30 T spawn_process
    {0xffffffffc010de30ull, "spawn_process"},
// ffffffffc010e110 T yield
    {0xffffffffc010e110ull, "yield"},
// ffffffffc010e170 T init_sched
    {0xffffffffc010e170ull, "init_sched"},
// ffffffffc010e2c0 t preemption_callback
    {0xffffffffc010e2c0ull, "preemption_callback"},
// ffffffffc010e2e0 T queue_thread
    {0xffffffffc010e2e0ull, "queue_thread"},
// ffffffffc010e3a0 T sched_tick
    {0xffffffffc010e3a0ull, "sched_tick"},
// ffffffffc010e600 t yield_interrupt
    {0xffffffffc010e600ull, "yield_interrupt"},
// ffffffffc010e670 t idle_thread_func
    {0xffffffffc010e670ull, "idle_thread_func"},
// ffffffffc010e6a0 t sched_lock
    {0xffffffffc010e6a0ull, "sched_lock"},
// ffffffffc010e6e0 t sched_unlock
    {0xffffffffc010e6e0ull, "sched_unlock"},
// ffffffffc010e700 T spawn_thread
    {0xffffffffc010e700ull, "spawn_thread"},
// ffffffffc010e9e0 T pcpu_init_for_cpu
    {0xffffffffc010e9e0ull, "pcpu_init_for_cpu"},
// ffffffffc010ea10 T pcpu_init_for_bsp
    {0xffffffffc010ea10ull, "pcpu_init_for_bsp"},
// ffffffffc010ea30 T get_pcpu_base
    {0xffffffffc010ea30ull, "get_pcpu_base"},
// ffffffffc010ea40 T spinlock_acquire_high_tpl
    {0xffffffffc010ea40ull, "spinlock_acquire_high_tpl"},
// ffffffffc010eab0 T spinlock_acquire
    {0xffffffffc010eab0ull, "spinlock_acquire"},
// ffffffffc010eb00 T spinlock_release
    {0xffffffffc010eb00ull, "spinlock_release"},
// ffffffffc010eb40 T strerror
    {0xffffffffc010eb40ull, "strerror"},
// ffffffffc010eb90 T is_node_in_list
    {0xffffffffc010eb90ull, "is_node_in_list"},
// ffffffffc010ec90 t is_list_valid
    {0xffffffffc010ec90ull, "is_list_valid"},
// ffffffffc010eda0 T initialize_list_had
    {0xffffffffc010eda0ull, "initialize_list_had"},
// ffffffffc010ee20 T insert_head_list
    {0xffffffffc010ee20ull, "insert_head_list"},
// ffffffffc010eec0 T insert_tail_list
    {0xffffffffc010eec0ull, "insert_tail_list"},
// ffffffffc010ef60 T get_first_node
    {0xffffffffc010ef60ull, "get_first_node"},
// ffffffffc010efd0 T get_next_node
    {0xffffffffc010efd0ull, "get_next_node"},
// ffffffffc010f040 T get_prev_node
    {0xffffffffc010f040ull, "get_prev_node"},
// ffffffffc010f0b0 T is_list_empty
    {0xffffffffc010f0b0ull, "is_list_empty"},
// ffffffffc010f130 T is_null
    {0xffffffffc010f130ull, "is_null"},
// ffffffffc010f160 T is_node_at_end
    {0xffffffffc010f160ull, "is_node_at_end"},
// ffffffffc010f1b0 T swap_list_entries
    {0xffffffffc010f1b0ull, "swap_list_entries"},
// ffffffffc010f2d0 T remove_entry_list
    {0xffffffffc010f2d0ull, "remove_entry_list"},
// ffffffffc010f370 T serial_init
    {0xffffffffc010f370ull, "serial_init"},
// ffffffffc010f3f0 T serial_write
    {0xffffffffc010f3f0ull, "serial_write"},
// ffffffffc010f4c0 T serial_read
    {0xffffffffc010f4c0ull, "serial_read"},
// ffffffffc010f540 T serial_poll
    {0xffffffffc010f540ull, "serial_poll"},
// ffffffffc010f570 T trace
    {0xffffffffc010f570ull, "trace"},
// ffffffffc010f6e0 t common_stub
    {0xffffffffc010f6e0ull, "common_stub"},
// ffffffffc010f72c T handle_interrupt_request_00
    {0xffffffffc010f72cull, "handle_interrupt_request_00"},
// ffffffffc010f732 T handle_interrupt_request_01
    {0xffffffffc010f732ull, "handle_interrupt_request_01"},
// ffffffffc010f738 T handle_interrupt_request_02
    {0xffffffffc010f738ull, "handle_interrupt_request_02"},
// ffffffffc010f73e T handle_interrupt_request_03
    {0xffffffffc010f73eull, "handle_interrupt_request_03"},
// ffffffffc010f744 T handle_interrupt_request_04
    {0xffffffffc010f744ull, "handle_interrupt_request_04"},
// ffffffffc010f74a T handle_interrupt_request_05
    {0xffffffffc010f74aull, "handle_interrupt_request_05"},
// ffffffffc010f750 T handle_interrupt_request_06
    {0xffffffffc010f750ull, "handle_interrupt_request_06"},
// ffffffffc010f756 T handle_interrupt_request_07
    {0xffffffffc010f756ull, "handle_interrupt_request_07"},
// ffffffffc010f75c T handle_interrupt_request_08
    {0xffffffffc010f75cull, "handle_interrupt_request_08"},
// ffffffffc010f760 T handle_interrupt_request_09
    {0xffffffffc010f760ull, "handle_interrupt_request_09"},
// ffffffffc010f769 T handle_interrupt_request_0a
    {0xffffffffc010f769ull, "handle_interrupt_request_0a"},
// ffffffffc010f770 T handle_interrupt_request_0b
    {0xffffffffc010f770ull, "handle_interrupt_request_0b"},
// ffffffffc010f777 T handle_interrupt_request_0c
    {0xffffffffc010f777ull, "handle_interrupt_request_0c"},
// ffffffffc010f77e T handle_interrupt_request_0d
    {0xffffffffc010f77eull, "handle_interrupt_request_0d"},
// ffffffffc010f785 T handle_interrupt_request_0e
    {0xffffffffc010f785ull, "handle_interrupt_request_0e"},
// ffffffffc010f78c T handle_interrupt_request_0f
    {0xffffffffc010f78cull, "handle_interrupt_request_0f"},
// ffffffffc010f795 T handle_interrupt_request_10
    {0xffffffffc010f795ull, "handle_interrupt_request_10"},
// ffffffffc010f79c T handle_interrupt_request_11
    {0xffffffffc010f79cull, "handle_interrupt_request_11"},
// ffffffffc010f7a5 T handle_interrupt_request_12
    {0xffffffffc010f7a5ull, "handle_interrupt_request_12"},
// ffffffffc010f7ae T handle_interrupt_request_13
    {0xffffffffc010f7aeull, "handle_interrupt_request_13"},
// ffffffffc010f7b7 T handle_interrupt_request_14
    {0xffffffffc010f7b7ull, "handle_interrupt_request_14"},
// ffffffffc010f7c0 T handle_interrupt_request_15
    {0xffffffffc010f7c0ull, "handle_interrupt_request_15"},
// ffffffffc010f7c9 T handle_interrupt_request_16
    {0xffffffffc010f7c9ull, "handle_interrupt_request_16"},
// ffffffffc010f7d2 T handle_interrupt_request_17
    {0xffffffffc010f7d2ull, "handle_interrupt_request_17"},
// ffffffffc010f7db T handle_interrupt_request_18
    {0xffffffffc010f7dbull, "handle_interrupt_request_18"},
// ffffffffc010f7e4 T handle_interrupt_request_19
    {0xffffffffc010f7e4ull, "handle_interrupt_request_19"},
// ffffffffc010f7ed T handle_interrupt_request_1a
    {0xffffffffc010f7edull, "handle_interrupt_request_1a"},
// ffffffffc010f7f6 T handle_interrupt_request_1b
    {0xffffffffc010f7f6ull, "handle_interrupt_request_1b"},
// ffffffffc010f7ff T handle_interrupt_request_1c
    {0xffffffffc010f7ffull, "handle_interrupt_request_1c"},
// ffffffffc010f808 T handle_interrupt_request_1d
    {0xffffffffc010f808ull, "handle_interrupt_request_1d"},
// ffffffffc010f811 T handle_interrupt_request_1e
    {0xffffffffc010f811ull, "handle_interrupt_request_1e"},
// ffffffffc010f818 T handle_interrupt_request_1f
    {0xffffffffc010f818ull, "handle_interrupt_request_1f"},
// ffffffffc010f821 T handle_interrupt_request_20
    {0xffffffffc010f821ull, "handle_interrupt_request_20"},
// ffffffffc010f82a T handle_interrupt_request_21
    {0xffffffffc010f82aull, "handle_interrupt_request_21"},
// ffffffffc010f833 T handle_interrupt_request_22
    {0xffffffffc010f833ull, "handle_interrupt_request_22"},
// ffffffffc010f83c T handle_interrupt_request_23
    {0xffffffffc010f83cull, "handle_interrupt_request_23"},
// ffffffffc010f845 T handle_interrupt_request_24
    {0xffffffffc010f845ull, "handle_interrupt_request_24"},
// ffffffffc010f84e T handle_interrupt_request_25
    {0xffffffffc010f84eull, "handle_interrupt_request_25"},
// ffffffffc010f857 T handle_interrupt_request_26
    {0xffffffffc010f857ull, "handle_interrupt_request_26"},
// ffffffffc010f860 T handle_interrupt_request_27
    {0xffffffffc010f860ull, "handle_interrupt_request_27"},
// ffffffffc010f869 T handle_interrupt_request_28
    {0xffffffffc010f869ull, "handle_interrupt_request_28"},
// ffffffffc010f872 T handle_interrupt_request_29
    {0xffffffffc010f872ull, "handle_interrupt_request_29"},
// ffffffffc010f87b T handle_interrupt_request_2a
    {0xffffffffc010f87bull, "handle_interrupt_request_2a"},
// ffffffffc010f884 T handle_interrupt_request_2b
    {0xffffffffc010f884ull, "handle_interrupt_request_2b"},
// ffffffffc010f88d T handle_interrupt_request_2c
    {0xffffffffc010f88dull, "handle_interrupt_request_2c"},
// ffffffffc010f896 T handle_interrupt_request_2d
    {0xffffffffc010f896ull, "handle_interrupt_request_2d"},
// ffffffffc010f89f T handle_interrupt_request_2e
    {0xffffffffc010f89full, "handle_interrupt_request_2e"},
// ffffffffc010f8a8 T handle_interrupt_request_2f
    {0xffffffffc010f8a8ull, "handle_interrupt_request_2f"},
// ffffffffc010f8b1 T handle_interrupt_request_30
    {0xffffffffc010f8b1ull, "handle_interrupt_request_30"},
// ffffffffc010f8ba T handle_interrupt_request_31
    {0xffffffffc010f8baull, "handle_interrupt_request_31"},
// ffffffffc010f8c3 T handle_interrupt_request_32
    {0xffffffffc010f8c3ull, "handle_interrupt_request_32"},
// ffffffffc010f8cc T handle_interrupt_request_33
    {0xffffffffc010f8ccull, "handle_interrupt_request_33"},
// ffffffffc010f8d5 T handle_interrupt_request_34
    {0xffffffffc010f8d5ull, "handle_interrupt_request_34"},
// ffffffffc010f8de T handle_interrupt_request_35
    {0xffffffffc010f8deull, "handle_interrupt_request_35"},
// ffffffffc010f8e7 T handle_interrupt_request_36
    {0xffffffffc010f8e7ull, "handle_interrupt_request_36"},
// ffffffffc010f8f0 T handle_interrupt_request_37
    {0xffffffffc010f8f0ull, "handle_interrupt_request_37"},
// ffffffffc010f8f9 T handle_interrupt_request_38
    {0xffffffffc010f8f9ull, "handle_interrupt_request_38"},
// ffffffffc010f902 T handle_interrupt_request_39
    {0xffffffffc010f902ull, "handle_interrupt_request_39"},
// ffffffffc010f90b T handle_interrupt_request_3a
    {0xffffffffc010f90bull, "handle_interrupt_request_3a"},
// ffffffffc010f914 T handle_interrupt_request_3b
    {0xffffffffc010f914ull, "handle_interrupt_request_3b"},
// ffffffffc010f91d T handle_interrupt_request_3c
    {0xffffffffc010f91dull, "handle_interrupt_request_3c"},
// ffffffffc010f926 T handle_interrupt_request_3d
    {0xffffffffc010f926ull, "handle_interrupt_request_3d"},
// ffffffffc010f92f T handle_interrupt_request_3e
    {0xffffffffc010f92full, "handle_interrupt_request_3e"},
// ffffffffc010f938 T handle_interrupt_request_3f
    {0xffffffffc010f938ull, "handle_interrupt_request_3f"},
// ffffffffc010f941 T handle_interrupt_request_40
    {0xffffffffc010f941ull, "handle_interrupt_request_40"},
// ffffffffc010f94a T handle_interrupt_request_41
    {0xffffffffc010f94aull, "handle_interrupt_request_41"},
// ffffffffc010f953 T handle_interrupt_request_42
    {0xffffffffc010f953ull, "handle_interrupt_request_42"},
// ffffffffc010f95c T handle_interrupt_request_43
    {0xffffffffc010f95cull, "handle_interrupt_request_43"},
// ffffffffc010f965 T handle_interrupt_request_44
    {0xffffffffc010f965ull, "handle_interrupt_request_44"},
// ffffffffc010f96e T handle_interrupt_request_45
    {0xffffffffc010f96eull, "handle_interrupt_request_45"},
// ffffffffc010f977 T handle_interrupt_request_46
    {0xffffffffc010f977ull, "handle_interrupt_request_46"},
// ffffffffc010f980 T handle_interrupt_request_47
    {0xffffffffc010f980ull, "handle_interrupt_request_47"},
// ffffffffc010f989 T handle_interrupt_request_48
    {0xffffffffc010f989ull, "handle_interrupt_request_48"},
// ffffffffc010f992 T handle_interrupt_request_49
    {0xffffffffc010f992ull, "handle_interrupt_request_49"},
// ffffffffc010f99b T handle_interrupt_request_4a
    {0xffffffffc010f99bull, "handle_interrupt_request_4a"},
// ffffffffc010f9a4 T handle_interrupt_request_4b
    {0xffffffffc010f9a4ull, "handle_interrupt_request_4b"},
// ffffffffc010f9ad T handle_interrupt_request_4c
    {0xffffffffc010f9adull, "handle_interrupt_request_4c"},
// ffffffffc010f9b6 T handle_interrupt_request_4d
    {0xffffffffc010f9b6ull, "handle_interrupt_request_4d"},
// ffffffffc010f9bf T handle_interrupt_request_4e
    {0xffffffffc010f9bfull, "handle_interrupt_request_4e"},
// ffffffffc010f9c8 T handle_interrupt_request_4f
    {0xffffffffc010f9c8ull, "handle_interrupt_request_4f"},
// ffffffffc010f9d1 T handle_interrupt_request_50
    {0xffffffffc010f9d1ull, "handle_interrupt_request_50"},
// ffffffffc010f9da T handle_interrupt_request_51
    {0xffffffffc010f9daull, "handle_interrupt_request_51"},
// ffffffffc010f9e3 T handle_interrupt_request_52
    {0xffffffffc010f9e3ull, "handle_interrupt_request_52"},
// ffffffffc010f9ec T handle_interrupt_request_53
    {0xffffffffc010f9ecull, "handle_interrupt_request_53"},
// ffffffffc010f9f5 T handle_interrupt_request_54
    {0xffffffffc010f9f5ull, "handle_interrupt_request_54"},
// ffffffffc010f9fe T handle_interrupt_request_55
    {0xffffffffc010f9feull, "handle_interrupt_request_55"},
// ffffffffc010fa07 T handle_interrupt_request_56
    {0xffffffffc010fa07ull, "handle_interrupt_request_56"},
// ffffffffc010fa10 T handle_interrupt_request_57
    {0xffffffffc010fa10ull, "handle_interrupt_request_57"},
// ffffffffc010fa19 T handle_interrupt_request_58
    {0xffffffffc010fa19ull, "handle_interrupt_request_58"},
// ffffffffc010fa22 T handle_interrupt_request_59
    {0xffffffffc010fa22ull, "handle_interrupt_request_59"},
// ffffffffc010fa2b T handle_interrupt_request_5a
    {0xffffffffc010fa2bull, "handle_interrupt_request_5a"},
// ffffffffc010fa34 T handle_interrupt_request_5b
    {0xffffffffc010fa34ull, "handle_interrupt_request_5b"},
// ffffffffc010fa3d T handle_interrupt_request_5c
    {0xffffffffc010fa3dull, "handle_interrupt_request_5c"},
// ffffffffc010fa46 T handle_interrupt_request_5d
    {0xffffffffc010fa46ull, "handle_interrupt_request_5d"},
// ffffffffc010fa4f T handle_interrupt_request_5e
    {0xffffffffc010fa4full, "handle_interrupt_request_5e"},
// ffffffffc010fa58 T handle_interrupt_request_5f
    {0xffffffffc010fa58ull, "handle_interrupt_request_5f"},
// ffffffffc010fa61 T handle_interrupt_request_60
    {0xffffffffc010fa61ull, "handle_interrupt_request_60"},
// ffffffffc010fa6a T handle_interrupt_request_61
    {0xffffffffc010fa6aull, "handle_interrupt_request_61"},
// ffffffffc010fa73 T handle_interrupt_request_62
    {0xffffffffc010fa73ull, "handle_interrupt_request_62"},
// ffffffffc010fa7c T handle_interrupt_request_63
    {0xffffffffc010fa7cull, "handle_interrupt_request_63"},
// ffffffffc010fa85 T handle_interrupt_request_64
    {0xffffffffc010fa85ull, "handle_interrupt_request_64"},
// ffffffffc010fa8e T handle_interrupt_request_65
    {0xffffffffc010fa8eull, "handle_interrupt_request_65"},
// ffffffffc010fa97 T handle_interrupt_request_66
    {0xffffffffc010fa97ull, "handle_interrupt_request_66"},
// ffffffffc010faa0 T handle_interrupt_request_67
    {0xffffffffc010faa0ull, "handle_interrupt_request_67"},
// ffffffffc010faa9 T handle_interrupt_request_68
    {0xffffffffc010faa9ull, "handle_interrupt_request_68"},
// ffffffffc010fab2 T handle_interrupt_request_69
    {0xffffffffc010fab2ull, "handle_interrupt_request_69"},
// ffffffffc010fabb T handle_interrupt_request_6a
    {0xffffffffc010fabbull, "handle_interrupt_request_6a"},
// ffffffffc010fac4 T handle_interrupt_request_6b
    {0xffffffffc010fac4ull, "handle_interrupt_request_6b"},
// ffffffffc010facd T handle_interrupt_request_6c
    {0xffffffffc010facdull, "handle_interrupt_request_6c"},
// ffffffffc010fad6 T handle_interrupt_request_6d
    {0xffffffffc010fad6ull, "handle_interrupt_request_6d"},
// ffffffffc010fadf T handle_interrupt_request_6e
    {0xffffffffc010fadfull, "handle_interrupt_request_6e"},
// ffffffffc010fae8 T handle_interrupt_request_6f
    {0xffffffffc010fae8ull, "handle_interrupt_request_6f"},
// ffffffffc010faf1 T handle_interrupt_request_70
    {0xffffffffc010faf1ull, "handle_interrupt_request_70"},
// ffffffffc010fafa T handle_interrupt_request_71
    {0xffffffffc010fafaull, "handle_interrupt_request_71"},
// ffffffffc010fb03 T handle_interrupt_request_72
    {0xffffffffc010fb03ull, "handle_interrupt_request_72"},
// ffffffffc010fb0c T handle_interrupt_request_73
    {0xffffffffc010fb0cull, "handle_interrupt_request_73"},
// ffffffffc010fb15 T handle_interrupt_request_74
    {0xffffffffc010fb15ull, "handle_interrupt_request_74"},
// ffffffffc010fb1e T handle_interrupt_request_75
    {0xffffffffc010fb1eull, "handle_interrupt_request_75"},
// ffffffffc010fb27 T handle_interrupt_request_76
    {0xffffffffc010fb27ull, "handle_interrupt_request_76"},
// ffffffffc010fb30 T handle_interrupt_request_77
    {0xffffffffc010fb30ull, "handle_interrupt_request_77"},
// ffffffffc010fb39 T handle_interrupt_request_78
    {0xffffffffc010fb39ull, "handle_interrupt_request_78"},
// ffffffffc010fb42 T handle_interrupt_request_79
    {0xffffffffc010fb42ull, "handle_interrupt_request_79"},
// ffffffffc010fb4b T handle_interrupt_request_7a
    {0xffffffffc010fb4bull, "handle_interrupt_request_7a"},
// ffffffffc010fb54 T handle_interrupt_request_7b
    {0xffffffffc010fb54ull, "handle_interrupt_request_7b"},
// ffffffffc010fb5d T handle_interrupt_request_7c
    {0xffffffffc010fb5dull, "handle_interrupt_request_7c"},
// ffffffffc010fb66 T handle_interrupt_request_7d
    {0xffffffffc010fb66ull, "handle_interrupt_request_7d"},
// ffffffffc010fb6f T handle_interrupt_request_7e
    {0xffffffffc010fb6full, "handle_interrupt_request_7e"},
// ffffffffc010fb78 T handle_interrupt_request_7f
    {0xffffffffc010fb78ull, "handle_interrupt_request_7f"},
// ffffffffc010fb81 T handle_interrupt_request_80
    {0xffffffffc010fb81ull, "handle_interrupt_request_80"},
// ffffffffc010fb8d T handle_interrupt_request_81
    {0xffffffffc010fb8dull, "handle_interrupt_request_81"},
// ffffffffc010fb99 T handle_interrupt_request_82
    {0xffffffffc010fb99ull, "handle_interrupt_request_82"},
// ffffffffc010fba5 T handle_interrupt_request_83
    {0xffffffffc010fba5ull, "handle_interrupt_request_83"},
// ffffffffc010fbb1 T handle_interrupt_request_84
    {0xffffffffc010fbb1ull, "handle_interrupt_request_84"},
// ffffffffc010fbbd T handle_interrupt_request_85
    {0xffffffffc010fbbdull, "handle_interrupt_request_85"},
// ffffffffc010fbc9 T handle_interrupt_request_86
    {0xffffffffc010fbc9ull, "handle_interrupt_request_86"},
// ffffffffc010fbd5 T handle_interrupt_request_87
    {0xffffffffc010fbd5ull, "handle_interrupt_request_87"},
// ffffffffc010fbe1 T handle_interrupt_request_88
    {0xffffffffc010fbe1ull, "handle_interrupt_request_88"},
// ffffffffc010fbed T handle_interrupt_request_89
    {0xffffffffc010fbedull, "handle_interrupt_request_89"},
// ffffffffc010fbf9 T handle_interrupt_request_8a
    {0xffffffffc010fbf9ull, "handle_interrupt_request_8a"},
// ffffffffc010fc05 T handle_interrupt_request_8b
    {0xffffffffc010fc05ull, "handle_interrupt_request_8b"},
// ffffffffc010fc11 T handle_interrupt_request_8c
    {0xffffffffc010fc11ull, "handle_interrupt_request_8c"},
// ffffffffc010fc1d T handle_interrupt_request_8d
    {0xffffffffc010fc1dull, "handle_interrupt_request_8d"},
// ffffffffc010fc29 T handle_interrupt_request_8e
    {0xffffffffc010fc29ull, "handle_interrupt_request_8e"},
// ffffffffc010fc35 T handle_interrupt_request_8f
    {0xffffffffc010fc35ull, "handle_interrupt_request_8f"},
// ffffffffc010fc41 T handle_interrupt_request_90
    {0xffffffffc010fc41ull, "handle_interrupt_request_90"},
// ffffffffc010fc4d T handle_interrupt_request_91
    {0xffffffffc010fc4dull, "handle_interrupt_request_91"},
// ffffffffc010fc59 T handle_interrupt_request_92
    {0xffffffffc010fc59ull, "handle_interrupt_request_92"},
// ffffffffc010fc65 T handle_interrupt_request_93
    {0xffffffffc010fc65ull, "handle_interrupt_request_93"},
// ffffffffc010fc71 T handle_interrupt_request_94
    {0xffffffffc010fc71ull, "handle_interrupt_request_94"},
// ffffffffc010fc7d T handle_interrupt_request_95
    {0xffffffffc010fc7dull, "handle_interrupt_request_95"},
// ffffffffc010fc89 T handle_interrupt_request_96
    {0xffffffffc010fc89ull, "handle_interrupt_request_96"},
// ffffffffc010fc95 T handle_interrupt_request_97
    {0xffffffffc010fc95ull, "handle_interrupt_request_97"},
// ffffffffc010fca1 T handle_interrupt_request_98
    {0xffffffffc010fca1ull, "handle_interrupt_request_98"},
// ffffffffc010fcad T handle_interrupt_request_99
    {0xffffffffc010fcadull, "handle_interrupt_request_99"},
// ffffffffc010fcb9 T handle_interrupt_request_9a
    {0xffffffffc010fcb9ull, "handle_interrupt_request_9a"},
// ffffffffc010fcc5 T handle_interrupt_request_9b
    {0xffffffffc010fcc5ull, "handle_interrupt_request_9b"},
// ffffffffc010fcd1 T handle_interrupt_request_9c
    {0xffffffffc010fcd1ull, "handle_interrupt_request_9c"},
// ffffffffc010fcdd T handle_interrupt_request_9d
    {0xffffffffc010fcddull, "handle_interrupt_request_9d"},
// ffffffffc010fce9 T handle_interrupt_request_9e
    {0xffffffffc010fce9ull, "handle_interrupt_request_9e"},
// ffffffffc010fcf5 T handle_interrupt_request_9f
    {0xffffffffc010fcf5ull, "handle_interrupt_request_9f"},
// ffffffffc010fd01 T handle_interrupt_request_a0
    {0xffffffffc010fd01ull, "handle_interrupt_request_a0"},
// ffffffffc010fd0d T handle_interrupt_request_a1
    {0xffffffffc010fd0dull, "handle_interrupt_request_a1"},
// ffffffffc010fd19 T handle_interrupt_request_a2
    {0xffffffffc010fd19ull, "handle_interrupt_request_a2"},
// ffffffffc010fd25 T handle_interrupt_request_a3
    {0xffffffffc010fd25ull, "handle_interrupt_request_a3"},
// ffffffffc010fd31 T handle_interrupt_request_a4
    {0xffffffffc010fd31ull, "handle_interrupt_request_a4"},
// ffffffffc010fd3d T handle_interrupt_request_a5
    {0xffffffffc010fd3dull, "handle_interrupt_request_a5"},
// ffffffffc010fd49 T handle_interrupt_request_a6
    {0xffffffffc010fd49ull, "handle_interrupt_request_a6"},
// ffffffffc010fd55 T handle_interrupt_request_a7
    {0xffffffffc010fd55ull, "handle_interrupt_request_a7"},
// ffffffffc010fd61 T handle_interrupt_request_a8
    {0xffffffffc010fd61ull, "handle_interrupt_request_a8"},
// ffffffffc010fd6d T handle_interrupt_request_a9
    {0xffffffffc010fd6dull, "handle_interrupt_request_a9"},
// ffffffffc010fd79 T handle_interrupt_request_aa
    {0xffffffffc010fd79ull, "handle_interrupt_request_aa"},
// ffffffffc010fd85 T handle_interrupt_request_ab
    {0xffffffffc010fd85ull, "handle_interrupt_request_ab"},
// ffffffffc010fd91 T handle_interrupt_request_ac
    {0xffffffffc010fd91ull, "handle_interrupt_request_ac"},
// ffffffffc010fd9d T handle_interrupt_request_ad
    {0xffffffffc010fd9dull, "handle_interrupt_request_ad"},
// ffffffffc010fda9 T handle_interrupt_request_ae
    {0xffffffffc010fda9ull, "handle_interrupt_request_ae"},
// ffffffffc010fdb5 T handle_interrupt_request_af
    {0xffffffffc010fdb5ull, "handle_interrupt_request_af"},
// ffffffffc010fdc1 T handle_interrupt_request_b0
    {0xffffffffc010fdc1ull, "handle_interrupt_request_b0"},
// ffffffffc010fdcd T handle_interrupt_request_b1
    {0xffffffffc010fdcdull, "handle_interrupt_request_b1"},
// ffffffffc010fdd9 T handle_interrupt_request_b2
    {0xffffffffc010fdd9ull, "handle_interrupt_request_b2"},
// ffffffffc010fde5 T handle_interrupt_request_b3
    {0xffffffffc010fde5ull, "handle_interrupt_request_b3"},
// ffffffffc010fdf1 T handle_interrupt_request_b4
    {0xffffffffc010fdf1ull, "handle_interrupt_request_b4"},
// ffffffffc010fdfd T handle_interrupt_request_b5
    {0xffffffffc010fdfdull, "handle_interrupt_request_b5"},
// ffffffffc010fe09 T handle_interrupt_request_b6
    {0xffffffffc010fe09ull, "handle_interrupt_request_b6"},
// ffffffffc010fe15 T handle_interrupt_request_b7
    {0xffffffffc010fe15ull, "handle_interrupt_request_b7"},
// ffffffffc010fe21 T handle_interrupt_request_b8
    {0xffffffffc010fe21ull, "handle_interrupt_request_b8"},
// ffffffffc010fe2d T handle_interrupt_request_b9
    {0xffffffffc010fe2dull, "handle_interrupt_request_b9"},
// ffffffffc010fe39 T handle_interrupt_request_ba
    {0xffffffffc010fe39ull, "handle_interrupt_request_ba"},
// ffffffffc010fe45 T handle_interrupt_request_bb
    {0xffffffffc010fe45ull, "handle_interrupt_request_bb"},
// ffffffffc010fe51 T handle_interrupt_request_bc
    {0xffffffffc010fe51ull, "handle_interrupt_request_bc"},
// ffffffffc010fe5d T handle_interrupt_request_bd
    {0xffffffffc010fe5dull, "handle_interrupt_request_bd"},
// ffffffffc010fe69 T handle_interrupt_request_be
    {0xffffffffc010fe69ull, "handle_interrupt_request_be"},
// ffffffffc010fe75 T handle_interrupt_request_bf
    {0xffffffffc010fe75ull, "handle_interrupt_request_bf"},
// ffffffffc010fe81 T handle_interrupt_request_c0
    {0xffffffffc010fe81ull, "handle_interrupt_request_c0"},
// ffffffffc010fe8d T handle_interrupt_request_c1
    {0xffffffffc010fe8dull, "handle_interrupt_request_c1"},
// ffffffffc010fe99 T handle_interrupt_request_c2
    {0xffffffffc010fe99ull, "handle_interrupt_request_c2"},
// ffffffffc010fea5 T handle_interrupt_request_c3
    {0xffffffffc010fea5ull, "handle_interrupt_request_c3"},
// ffffffffc010feb1 T handle_interrupt_request_c4
    {0xffffffffc010feb1ull, "handle_interrupt_request_c4"},
// ffffffffc010febd T handle_interrupt_request_c5
    {0xffffffffc010febdull, "handle_interrupt_request_c5"},
// ffffffffc010fec9 T handle_interrupt_request_c6
    {0xffffffffc010fec9ull, "handle_interrupt_request_c6"},
// ffffffffc010fed5 T handle_interrupt_request_c7
    {0xffffffffc010fed5ull, "handle_interrupt_request_c7"},
// ffffffffc010fee1 T handle_interrupt_request_c8
    {0xffffffffc010fee1ull, "handle_interrupt_request_c8"},
// ffffffffc010feed T handle_interrupt_request_c9
    {0xffffffffc010feedull, "handle_interrupt_request_c9"},
// ffffffffc010fef9 T handle_interrupt_request_ca
    {0xffffffffc010fef9ull, "handle_interrupt_request_ca"},
// ffffffffc010ff05 T handle_interrupt_request_cb
    {0xffffffffc010ff05ull, "handle_interrupt_request_cb"},
// ffffffffc010ff11 T handle_interrupt_request_cc
    {0xffffffffc010ff11ull, "handle_interrupt_request_cc"},
// ffffffffc010ff1d T handle_interrupt_request_cd
    {0xffffffffc010ff1dull, "handle_interrupt_request_cd"},
// ffffffffc010ff29 T handle_interrupt_request_ce
    {0xffffffffc010ff29ull, "handle_interrupt_request_ce"},
// ffffffffc010ff35 T handle_interrupt_request_cf
    {0xffffffffc010ff35ull, "handle_interrupt_request_cf"},
// ffffffffc010ff41 T handle_interrupt_request_d0
    {0xffffffffc010ff41ull, "handle_interrupt_request_d0"},
// ffffffffc010ff4d T handle_interrupt_request_d1
    {0xffffffffc010ff4dull, "handle_interrupt_request_d1"},
// ffffffffc010ff59 T handle_interrupt_request_d2
    {0xffffffffc010ff59ull, "handle_interrupt_request_d2"},
// ffffffffc010ff65 T handle_interrupt_request_d3
    {0xffffffffc010ff65ull, "handle_interrupt_request_d3"},
// ffffffffc010ff71 T handle_interrupt_request_d4
    {0xffffffffc010ff71ull, "handle_interrupt_request_d4"},
// ffffffffc010ff7d T handle_interrupt_request_d5
    {0xffffffffc010ff7dull, "handle_interrupt_request_d5"},
// ffffffffc010ff89 T handle_interrupt_request_d6
    {0xffffffffc010ff89ull, "handle_interrupt_request_d6"},
// ffffffffc010ff95 T handle_interrupt_request_d7
    {0xffffffffc010ff95ull, "handle_interrupt_request_d7"},
// ffffffffc010ffa1 T handle_interrupt_request_d8
    {0xffffffffc010ffa1ull, "handle_interrupt_request_d8"},
// ffffffffc010ffad T handle_interrupt_request_d9
    {0xffffffffc010ffadull, "handle_interrupt_request_d9"},
// ffffffffc010ffb9 T handle_interrupt_request_da
    {0xffffffffc010ffb9ull, "handle_interrupt_request_da"},
// ffffffffc010ffc5 T handle_interrupt_request_db
    {0xffffffffc010ffc5ull, "handle_interrupt_request_db"},
// ffffffffc010ffd1 T handle_interrupt_request_dc
    {0xffffffffc010ffd1ull, "handle_interrupt_request_dc"},
// ffffffffc010ffdd T handle_interrupt_request_dd
    {0xffffffffc010ffddull, "handle_interrupt_request_dd"},
// ffffffffc010ffe9 T handle_interrupt_request_de
    {0xffffffffc010ffe9ull, "handle_interrupt_request_de"},
// ffffffffc010fff5 T handle_interrupt_request_df
    {0xffffffffc010fff5ull, "handle_interrupt_request_df"},
// ffffffffc0110001 T handle_interrupt_request_e0
    {0xffffffffc0110001ull, "handle_interrupt_request_e0"},
// ffffffffc011000d T handle_interrupt_request_e1
    {0xffffffffc011000dull, "handle_interrupt_request_e1"},
// ffffffffc0110019 T handle_interrupt_request_e2
    {0xffffffffc0110019ull, "handle_interrupt_request_e2"},
// ffffffffc0110025 T handle_interrupt_request_e3
    {0xffffffffc0110025ull, "handle_interrupt_request_e3"},
// ffffffffc0110031 T handle_interrupt_request_e4
    {0xffffffffc0110031ull, "handle_interrupt_request_e4"},
// ffffffffc011003d T handle_interrupt_request_e5
    {0xffffffffc011003dull, "handle_interrupt_request_e5"},
// ffffffffc0110049 T handle_interrupt_request_e6
    {0xffffffffc0110049ull, "handle_interrupt_request_e6"},
// ffffffffc0110055 T handle_interrupt_request_e7
    {0xffffffffc0110055ull, "handle_interrupt_request_e7"},
// ffffffffc0110061 T handle_interrupt_request_e8
    {0xffffffffc0110061ull, "handle_interrupt_request_e8"},
// ffffffffc011006d T handle_interrupt_request_e9
    {0xffffffffc011006dull, "handle_interrupt_request_e9"},
// ffffffffc0110079 T handle_interrupt_request_ea
    {0xffffffffc0110079ull, "handle_interrupt_request_ea"},
// ffffffffc0110085 T handle_interrupt_request_eb
    {0xffffffffc0110085ull, "handle_interrupt_request_eb"},
// ffffffffc0110091 T handle_interrupt_request_ec
    {0xffffffffc0110091ull, "handle_interrupt_request_ec"},
// ffffffffc011009d T handle_interrupt_request_ed
    {0xffffffffc011009dull, "handle_interrupt_request_ed"},
// ffffffffc01100a9 T handle_interrupt_request_ee
    {0xffffffffc01100a9ull, "handle_interrupt_request_ee"},
// ffffffffc01100b5 T handle_interrupt_request_ef
    {0xffffffffc01100b5ull, "handle_interrupt_request_ef"},
// ffffffffc01100c1 T handle_interrupt_request_f0
    {0xffffffffc01100c1ull, "handle_interrupt_request_f0"},
// ffffffffc01100cd T handle_interrupt_request_f1
    {0xffffffffc01100cdull, "handle_interrupt_request_f1"},
// ffffffffc01100d9 T handle_interrupt_request_f2
    {0xffffffffc01100d9ull, "handle_interrupt_request_f2"},
// ffffffffc01100e5 T handle_interrupt_request_f3
    {0xffffffffc01100e5ull, "handle_interrupt_request_f3"},
// ffffffffc01100f1 T handle_interrupt_request_f4
    {0xffffffffc01100f1ull, "handle_interrupt_request_f4"},
// ffffffffc01100fd T handle_interrupt_request_f5
    {0xffffffffc01100fdull, "handle_interrupt_request_f5"},
// ffffffffc0110109 T handle_interrupt_request_f6
    {0xffffffffc0110109ull, "handle_interrupt_request_f6"},
// ffffffffc0110115 T handle_interrupt_request_f7
    {0xffffffffc0110115ull, "handle_interrupt_request_f7"},
// ffffffffc0110121 T handle_interrupt_request_f8
    {0xffffffffc0110121ull, "handle_interrupt_request_f8"},
// ffffffffc011012d T handle_interrupt_request_f9
    {0xffffffffc011012dull, "handle_interrupt_request_f9"},
// ffffffffc0110139 T handle_interrupt_request_fa
    {0xffffffffc0110139ull, "handle_interrupt_request_fa"},
// ffffffffc0110145 T handle_interrupt_request_fb
    {0xffffffffc0110145ull, "handle_interrupt_request_fb"},
// ffffffffc0110151 T handle_interrupt_request_fc
    {0xffffffffc0110151ull, "handle_interrupt_request_fc"},
// ffffffffc011015d T handle_interrupt_request_fd
    {0xffffffffc011015dull, "handle_interrupt_request_fd"},
// ffffffffc0110169 T handle_interrupt_request_fe
    {0xffffffffc0110169ull, "handle_interrupt_request_fe"},
// ffffffffc0110175 T handle_interrupt_request_ff
    {0xffffffffc0110175ull, "handle_interrupt_request_ff"},
// ffffffffc0110190 t fix_data_segment
    {0xffffffffc0110190ull, "fix_data_segment"},
// ffffffffc01101a1 T gdt_fix_segments
    {0xffffffffc01101a1ull, "gdt_fix_segments"},
// ffffffffc01101c0 T memset
    {0xffffffffc01101c0ull, "memset"},
// ffffffffc0110220 T memcmp
    {0xffffffffc0110220ull, "memcmp"},
// ffffffffc01102c0 T memcpy
    {0xffffffffc01102c0ull, "memcpy"},
// ffffffffc0110340 T memmove
    {0xffffffffc0110340ull, "memmove"},
// ffffffffc0110440 T strcmp
    {0xffffffffc0110440ull, "strcmp"},
// ffffffffc01104d0 T strlen
    {0xffffffffc01104d0ull, "strlen"},
// ffffffffc0110520 T stbsp_set_separators
    {0xffffffffc0110520ull, "stbsp_set_separators"},
// ffffffffc0110550 T stbsp_vsprintfcb
    {0xffffffffc0110550ull, "stbsp_vsprintfcb"},
// ffffffffc0112940 t stbsp__lead_sign
    {0xffffffffc0112940ull, "stbsp__lead_sign"},
// ffffffffc01129d0 T stbsp_sprintf
    {0xffffffffc01129d0ull, "stbsp_sprintf"},
// ffffffffc0112a40 T stbsp_vsnprintf
    {0xffffffffc0112a40ull, "stbsp_vsnprintf"},
// ffffffffc0112b80 t stbsp__count_clamp_callback
    {0xffffffffc0112b80ull, "stbsp__count_clamp_callback"},
// ffffffffc0112bc0 t stbsp__clamp_callback
    {0xffffffffc0112bc0ull, "stbsp__clamp_callback"},
// ffffffffc0112d00 T stbsp_snprintf
    {0xffffffffc0112d00ull, "stbsp_snprintf"},
// ffffffffc0112d60 T stbsp_vsprintf
    {0xffffffffc0112d60ull, "stbsp_vsprintf"},
// ffffffffc0112db0 T stbds_arrgrowf
    {0xffffffffc0112db0ull, "stbds_arrgrowf"},
// ffffffffc0112fb0 T stbds_rand_seed
    {0xffffffffc0112fb0ull, "stbds_rand_seed"},
// ffffffffc0112fd0 T stbds_hash_string
    {0xffffffffc0112fd0ull, "stbds_hash_string"},
// ffffffffc01130e0 T stbds_hash_bytes
    {0xffffffffc01130e0ull, "stbds_hash_bytes"},
// ffffffffc0113330 t stbds_siphash_bytes
    {0xffffffffc0113330ull, "stbds_siphash_bytes"},
// ffffffffc01138e0 T stbds_hmfree_func
    {0xffffffffc01138e0ull, "stbds_hmfree_func"},
// ffffffffc01139b0 T stbds_strreset
    {0xffffffffc01139b0ull, "stbds_strreset"},
// ffffffffc0113a20 T stbds_hmget_key_ts
    {0xffffffffc0113a20ull, "stbds_hmget_key_ts"},
// ffffffffc0113b90 t stbds_hm_find_slot
    {0xffffffffc0113b90ull, "stbds_hm_find_slot"},
// ffffffffc0113e40 T stbds_hmget_key
    {0xffffffffc0113e40ull, "stbds_hmget_key"},
// ffffffffc0113ea0 T stbds_hmput_default
    {0xffffffffc0113ea0ull, "stbds_hmput_default"},
// ffffffffc0113f80 T stbds_hmput_key
    {0xffffffffc0113f80ull, "stbds_hmput_key"},
// ffffffffc01146d0 t stbds_make_hash_index
    {0xffffffffc01146d0ull, "stbds_make_hash_index"},
// ffffffffc0114c60 t stbds_probe_position
    {0xffffffffc0114c60ull, "stbds_probe_position"},
// ffffffffc0114ca0 t stbds_is_key_equal
    {0xffffffffc0114ca0ull, "stbds_is_key_equal"},
// ffffffffc0114d40 t stbds_strdup
    {0xffffffffc0114d40ull, "stbds_strdup"},
// ffffffffc0114da0 T stbds_stralloc
    {0xffffffffc0114da0ull, "stbds_stralloc"},
// ffffffffc0114f70 T stbds_shmode_func
    {0xffffffffc0114f70ull, "stbds_shmode_func"},
// ffffffffc0115010 T stbds_hmdel_key
    {0xffffffffc0115010ull, "stbds_hmdel_key"},
// ffffffffc0115370 t stbds_log2
    {0xffffffffc0115370ull, "stbds_log2"},
// ffffffffc01153c0 T tlsf_create
    {0xffffffffc01153c0ull, "tlsf_create"},
// ffffffffc0115520 t add_pool
    {0xffffffffc0115520ull, "add_pool"},
// ffffffffc01155c0 T tlsf_destroy
    {0xffffffffc01155c0ull, "tlsf_destroy"},
// ffffffffc0115640 t block_size
    {0xffffffffc0115640ull, "block_size"},
// ffffffffc0115670 T tlsf_mallocx
    {0xffffffffc0115670ull, "tlsf_mallocx"},
// ffffffffc01157c0 t adjust_size
    {0xffffffffc01157c0ull, "adjust_size"},
// ffffffffc0115810 t block_locate_free
    {0xffffffffc0115810ull, "block_locate_free"},
// ffffffffc0115890 t round_block_size
    {0xffffffffc0115890ull, "round_block_size"},
// ffffffffc0115900 t block_trim_free
    {0xffffffffc0115900ull, "block_trim_free"},
// ffffffffc0115970 t block_set_free
    {0xffffffffc0115970ull, "block_set_free"},
// ffffffffc01159f0 t block_to_ptr
    {0xffffffffc01159f0ull, "block_to_ptr"},
// ffffffffc0115a10 T tlsf_free
    {0xffffffffc0115a10ull, "tlsf_free"},
// ffffffffc0115ae0 t block_from_ptr
    {0xffffffffc0115ae0ull, "block_from_ptr"},
// ffffffffc0115b00 t block_merge_prev
    {0xffffffffc0115b00ull, "block_merge_prev"},
// ffffffffc0115b60 t block_merge_next
    {0xffffffffc0115b60ull, "block_merge_next"},
// ffffffffc0115bc0 t block_is_last
    {0xffffffffc0115bc0ull, "block_is_last"},
// ffffffffc0115bf0 t block_next
    {0xffffffffc0115bf0ull, "block_next"},
// ffffffffc0115c30 t remove_pool
    {0xffffffffc0115c30ull, "remove_pool"},
// ffffffffc0115c90 t block_insert
    {0xffffffffc0115c90ull, "block_insert"},
// ffffffffc0115ce0 T tlsf_reallocx
    {0xffffffffc0115ce0ull, "tlsf_reallocx"},
// ffffffffc0115f00 t block_is_free
    {0xffffffffc0115f00ull, "block_is_free"},
// ffffffffc0115f40 t block_set_prev_free
    {0xffffffffc0115f40ull, "block_set_prev_free"},
// ffffffffc0115fa0 t block_trim_used
    {0xffffffffc0115fa0ull, "block_trim_used"},
// ffffffffc0116020 t block_link_next
    {0xffffffffc0116020ull, "block_link_next"},
// ffffffffc0116050 t align_up
    {0xffffffffc0116050ull, "align_up"},
// ffffffffc0116070 t mapping_insert
    {0xffffffffc0116070ull, "mapping_insert"},
// ffffffffc0116100 t search_suitable_block
    {0xffffffffc0116100ull, "search_suitable_block"},
// ffffffffc01161f0 t remove_free_block
    {0xffffffffc01161f0ull, "remove_free_block"},
// ffffffffc0116300 t tlsf_fls
    {0xffffffffc0116300ull, "tlsf_fls"},
// ffffffffc0116360 t tlsf_ffs
    {0xffffffffc0116360ull, "tlsf_ffs"},
// ffffffffc0116390 t block_can_split
    {0xffffffffc0116390ull, "block_can_split"},
// ffffffffc01163d0 t block_split
    {0xffffffffc01163d0ull, "block_split"},
// ffffffffc0116450 t block_set_size
    {0xffffffffc0116450ull, "block_set_size"},
// ffffffffc0116490 t block_is_prev_free
    {0xffffffffc0116490ull, "block_is_prev_free"},
// ffffffffc01164d0 t block_prev
    {0xffffffffc01164d0ull, "block_prev"},
// ffffffffc01164f0 t block_remove
    {0xffffffffc01164f0ull, "block_remove"},
// ffffffffc0116540 t block_absorb
    {0xffffffffc0116540ull, "block_absorb"},
// ffffffffc0116590 t insert_free_block
    {0xffffffffc0116590ull, "insert_free_block"},
// ffffffffc011a000 d fs_components
// ffffffffc011a010 d all_components
// ffffffffc011a020 d type_to_name
// ffffffffc011a028 d component
// ffffffffc011a080 d early_current_tpl
// ffffffffc011a088 d handler
// ffffffffc011a0b8 D pit8254
// ffffffffc011a0c0 d timers_list
// ffffffffc011a0d0 d sv_handler
// ffffffffc011a100 D idt
// ffffffffc011a110 d ISR_NAMES
// ffffffffc011a1c0 d PROCESSOR_NAME
// ffffffffc011a1d0 d TABLE_NAME
// ffffffffc011a1f0 d PRESENT_NAMES
// ffffffffc011a200 d USER_NAME
// ffffffffc011a210 d OPERATION_NAME
// ffffffffc011a220 d gdt_entries
// ffffffffc011a258 D gdt
// ffffffffc011a270 d tboot_mmap_names
// ffffffffc011a298 d mem_map
// ffffffffc011a2a8 d free_entries_list
// ffffffffc011a2b8 d free_blocks
// ffffffffc011a2c8 d used_blocks
// ffffffffc011a2d8 d stack_allocation
// ffffffffc011a2e0 d free_block_list
// ffffffffc011a2f0 D process_list
// ffffffffc011a300 D kernel_process
// ffffffffc011a350 d g_pid_gen
// ffffffffc011a358 d handler
// ffffffffc011a388 d threads_queue
// ffffffffc011a3a0 d errors
// ffffffffc011a3e0 d stbsp__period
// ffffffffc011a3e1 d stbsp__comma
// ffffffffc011a3f0 d stbsp_vsprintfcb.hex
// ffffffffc011a410 d stbsp_vsprintfcb.hexu
// ffffffffc011a424 d stbsp__digitpair
// ffffffffc011a4f0 d stbds_hash_seed
// ffffffffc011a500 D symlist
// ffffffffc011a510 D bsp_pcpu_data
// ffffffffc011b000 B acpi_madt
// ffffffffc011b008 B acpi_rsdp
// ffffffffc011b010 B acpi_xsdp
// ffffffffc011b018 B acpi_rsdt
// ffffffffc011b020 B acpi_xsdt
// ffffffffc011b028 b components_lock
// ffffffffc011b030 b primary_components
// ffffffffc011b038 b initrd_files
// ffffffffc011b040 b lock
// ffffffffc011b050 b notify_queues
// ffffffffc011b250 b notify_queues_pending
// ffffffffc011b254 b event_lock
// ffffffffc011b260 b period
// ffffffffc011b268 b timer
// ffffffffc011b270 b check_timers_event
// ffffffffc011b278 b current_system_time
// ffffffffc011b280 b timer_lock
// ffffffffc011b288 b ioapics
// ffffffffc011b290 b isos
// ffffffffc011b298 b initialized_once
// ffffffffc011b2a0 b idt_entries
// ffffffffc011c2a0 b interrupt_handlers
// ffffffffc011d2a0 b interrupt_vector
// ffffffffc011d614 b index
// ffffffffc011d618 b g_info
// ffffffffc011d620 b g_kernel_tlsf
// ffffffffc011d630 b mem_memory_top
// ffffffffc011d638 b memory_base
// ffffffffc011d640 b lock
// ffffffffc011d650 b temp_entries
// ffffffffc011d7e0 b temp_entries_count
// ffffffffc011d7e8 b freeing
// ffffffffc011d7ec b lock
// ffffffffc011d7f4 b free_block_list_lock
// ffffffffc011d800 b memory_base
// ffffffffc011d808 b support_1gb
// ffffffffc011d809 b support_global
// ffffffffc011d80c B process_lock
// ffffffffc011d814 b flag
// ffffffffc019d818 B kernel_virtual_end
// ffffffffc019d818 b tmp_kernel_stack
// 
//not enough values to unpack (expected 3, got 1)
    {0xffffffffc019d818, "__kernel_end"}
};

