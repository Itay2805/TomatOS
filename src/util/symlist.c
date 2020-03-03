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
// 000000000019c818 B kernel_physical_end
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
// ffffffffc0101c90 T _BitScanReverse
    {0xffffffffc0101c90ull, "_BitScanReverse"},
// ffffffffc0101cd0 T _BitScanReverse64
    {0xffffffffc0101cd0ull, "_BitScanReverse64"},
// ffffffffc0101d10 T __readmsr_efer
    {0xffffffffc0101d10ull, "__readmsr_efer"},
// ffffffffc0101d30 T __writemsr_efer
    {0xffffffffc0101d30ull, "__writemsr_efer"},
// ffffffffc0101d50 T __readmsr_apic_base
    {0xffffffffc0101d50ull, "__readmsr_apic_base"},
// ffffffffc0101d70 T generate_device_address
    {0xffffffffc0101d70ull, "generate_device_address"},
// ffffffffc0101eb0 T register_component
    {0xffffffffc0101eb0ull, "register_component"},
// ffffffffc0102080 T get_component
    {0xffffffffc0102080ull, "get_component"},
// ffffffffc01021d0 T get_next_component
    {0xffffffffc01021d0ull, "get_next_component"},
// ffffffffc01021f0 T get_primary
    {0xffffffffc01021f0ull, "get_primary"},
// ffffffffc01022f0 T set_primary
    {0xffffffffc01022f0ull, "set_primary"},
// ffffffffc0102330 T file_read
    {0xffffffffc0102330ull, "file_read"},
// ffffffffc0102470 T file_seek
    {0xffffffffc0102470ull, "file_seek"},
// ffffffffc0102550 T file_close
    {0xffffffffc0102550ull, "file_close"},
// ffffffffc0102610 T tar_tell
    {0xffffffffc0102610ull, "tar_tell"},
// ffffffffc0102720 T tar_read
    {0xffffffffc0102720ull, "tar_read"},
// ffffffffc0102930 T create_initrd_fs
    {0xffffffffc0102930ull, "create_initrd_fs"},
// ffffffffc0102a90 t tar_size
    {0xffffffffc0102a90ull, "tar_size"},
// ffffffffc0102b10 t tar_open
    {0xffffffffc0102b10ull, "tar_open"},
// ffffffffc0102d10 t tar_is_readonly
    {0xffffffffc0102d10ull, "tar_is_readonly"},
// ffffffffc0102d90 t tar_close
    {0xffffffffc0102d90ull, "tar_close"},
// ffffffffc0102e10 t tar_eof
    {0xffffffffc0102e10ull, "tar_eof"},
// ffffffffc0102f30 t tar_seek
    {0xffffffffc0102f30ull, "tar_seek"},
// ffffffffc01031c0 T sha1_transform
    {0xffffffffc01031c0ull, "sha1_transform"},
// ffffffffc0103570 T sha1_init
    {0xffffffffc0103570ull, "sha1_init"},
// ffffffffc0103600 T sha1_update
    {0xffffffffc0103600ull, "sha1_update"},
// ffffffffc01036b0 T sha1_final
    {0xffffffffc01036b0ull, "sha1_final"},
// ffffffffc0103970 T events_init
    {0xffffffffc0103970ull, "events_init"},
// ffffffffc01039e0 T raise_tpl
    {0xffffffffc01039e0ull, "raise_tpl"},
// ffffffffc0103a70 T set_tpl
    {0xffffffffc0103a70ull, "set_tpl"},
// ffffffffc0103ad0 T restore_tpl
    {0xffffffffc0103ad0ull, "restore_tpl"},
// ffffffffc0103bc0 t dispatch_event_notifies
    {0xffffffffc0103bc0ull, "dispatch_event_notifies"},
// ffffffffc0103d10 T get_tpl
    {0xffffffffc0103d10ull, "get_tpl"},
// ffffffffc0103d70 T create_event
    {0xffffffffc0103d70ull, "create_event"},
// ffffffffc0103e90 T signal_event
    {0xffffffffc0103e90ull, "signal_event"},
// ffffffffc0103fe0 T check_event
    {0xffffffffc0103fe0ull, "check_event"},
// ffffffffc01040b0 T wait_for_event
    {0xffffffffc01040b0ull, "wait_for_event"},
// ffffffffc01043e0 T close_event
    {0xffffffffc01043e0ull, "close_event"},
// ffffffffc0104490 t aquire_event_lock
    {0xffffffffc0104490ull, "aquire_event_lock"},
// ffffffffc01044b0 t release_event_lock
    {0xffffffffc01044b0ull, "release_event_lock"},
// ffffffffc01044d0 T init_pit8254
    {0xffffffffc01044d0ull, "init_pit8254"},
// ffffffffc0104520 t set_timer_period
    {0xffffffffc0104520ull, "set_timer_period"},
// ffffffffc01045d0 t interrupt_handler
    {0xffffffffc01045d0ull, "interrupt_handler"},
// ffffffffc0104600 t set_pit_count
    {0xffffffffc0104600ull, "set_pit_count"},
// ffffffffc0104660 T init_timer
    {0xffffffffc0104660ull, "init_timer"},
// ffffffffc01047b0 t check_timers
    {0xffffffffc01047b0ull, "check_timers"},
// ffffffffc0104900 T timer_tick
    {0xffffffffc0104900ull, "timer_tick"},
// ffffffffc0104970 T get_current_system_time
    {0xffffffffc0104970ull, "get_current_system_time"},
// ffffffffc0104990 T set_timer
    {0xffffffffc0104990ull, "set_timer"},
// ffffffffc0104b60 t insert_event_timer
    {0xffffffffc0104b60ull, "insert_event_timer"},
// ffffffffc0104c00 T ioapic_add
    {0xffffffffc0104c00ull, "ioapic_add"},
// ffffffffc0104d20 t ioapic_read
    {0xffffffffc0104d20ull, "ioapic_read"},
// ffffffffc0104da0 T ioapic_add_iso
    {0xffffffffc0104da0ull, "ioapic_add_iso"},
// ffffffffc0104e30 T ioapic_enable_interrupt
    {0xffffffffc0104e30ull, "ioapic_enable_interrupt"},
// ffffffffc0104fb0 t get_ioapic
    {0xffffffffc0104fb0ull, "get_ioapic"},
// ffffffffc01050d0 t ioapic_write
    {0xffffffffc01050d0ull, "ioapic_write"},
// ffffffffc0105150 T ioapic_configure_interrupt
    {0xffffffffc0105150ull, "ioapic_configure_interrupt"},
// ffffffffc0105440 T get_apic_id
    {0xffffffffc0105440ull, "get_apic_id"},
// ffffffffc0105460 t lapic_read
    {0xffffffffc0105460ull, "lapic_read"},
// ffffffffc0105490 T lapic_init
    {0xffffffffc0105490ull, "lapic_init"},
// ffffffffc01055b0 t lapic_write
    {0xffffffffc01055b0ull, "lapic_write"},
// ffffffffc01055e0 T send_fixed_ipi
    {0xffffffffc01055e0ull, "send_fixed_ipi"},
// ffffffffc0105600 T send_apic_eoi
    {0xffffffffc0105600ull, "send_apic_eoi"},
// ffffffffc0105620 t spurious_interrupt_handler
    {0xffffffffc0105620ull, "spurious_interrupt_handler"},
// ffffffffc0105640 T idt_init
    {0xffffffffc0105640ull, "idt_init"},
// ffffffffc0106960 t set_idt_entry
    {0xffffffffc0106960ull, "set_idt_entry"},
// ffffffffc0106b30 T idt_post_tss_init
    {0xffffffffc0106b30ull, "idt_post_tss_init"},
// ffffffffc0106cc0 T interrupts_init
    {0xffffffffc0106cc0ull, "interrupts_init"},
// ffffffffc0106d30 T common_interrupt_handler
    {0xffffffffc0106d30ull, "common_interrupt_handler"},
// ffffffffc0106ea0 t default_interrupt_handler
    {0xffffffffc0106ea0ull, "default_interrupt_handler"},
// ffffffffc0107680 T interrupt_register
    {0xffffffffc0107680ull, "interrupt_register"},
// ffffffffc0107810 T interrupt_free
    {0xffffffffc0107810ull, "interrupt_free"},
// ffffffffc01078a0 t interrupt_allocate
    {0xffffffffc01078a0ull, "interrupt_allocate"},
// ffffffffc0107a10 T kernel_main
    {0xffffffffc0107a10ull, "kernel_main"},
// ffffffffc0107bb0 t main_thread
    {0xffffffffc0107bb0ull, "main_thread"},
// ffffffffc0107d40 T init_tss_for_cpu
    {0xffffffffc0107d40ull, "init_tss_for_cpu"},
// ffffffffc0107e20 T kernel_tlsf_map
    {0xffffffffc0107e20ull, "kernel_tlsf_map"},
// ffffffffc0107ec0 T kernel_tlsf_unmap
    {0xffffffffc0107ec0ull, "kernel_tlsf_unmap"},
// ffffffffc0107f00 T mm_init
    {0xffffffffc0107f00ull, "mm_init"},
// ffffffffc0107f80 T mm_allocate
    {0xffffffffc0107f80ull, "mm_allocate"},
// ffffffffc0107fb0 t tlsf_calloc
    {0xffffffffc0107fb0ull, "tlsf_calloc"},
// ffffffffc0107fe0 T mm_reallocate
    {0xffffffffc0107fe0ull, "mm_reallocate"},
// ffffffffc0108010 t tlsf_realloc
    {0xffffffffc0108010ull, "tlsf_realloc"},
// ffffffffc0108050 T mm_free
    {0xffffffffc0108050ull, "mm_free"},
// ffffffffc0108080 T pmm_init
    {0xffffffffc0108080ull, "pmm_init"},
// ffffffffc0108200 t add_range
    {0xffffffffc0108200ull, "add_range"},
// ffffffffc0108470 T pmm_post_vmm
    {0xffffffffc0108470ull, "pmm_post_vmm"},
// ffffffffc01086c0 T pmm_allocate_pages
    {0xffffffffc01086c0ull, "pmm_allocate_pages"},
// ffffffffc0108a50 t find_free_pages
    {0xffffffffc0108a50ull, "find_free_pages"},
// ffffffffc0108c60 t convert_page
    {0xffffffffc0108c60ull, "convert_page"},
// ffffffffc01091d0 T pmm_free_pages
    {0xffffffffc01091d0ull, "pmm_free_pages"},
// ffffffffc01092e0 t remove_mem_map_entry
    {0xffffffffc01092e0ull, "remove_mem_map_entry"},
// ffffffffc0109330 t move_temp_entries
    {0xffffffffc0109330ull, "move_temp_entries"},
// ffffffffc01094f0 t allocate_memory_map_entry
    {0xffffffffc01094f0ull, "allocate_memory_map_entry"},
// ffffffffc01095c0 t allocate_pool_pages
    {0xffffffffc01095c0ull, "allocate_pool_pages"},
// ffffffffc0109660 T allocate_stack
    {0xffffffffc0109660ull, "allocate_stack"},
// ffffffffc0109860 T free_stack
    {0xffffffffc0109860ull, "free_stack"},
// ffffffffc0109970 t acquire_alloc_lock
    {0xffffffffc0109970ull, "acquire_alloc_lock"},
// ffffffffc0109990 t release_alloc_lock
    {0xffffffffc0109990ull, "release_alloc_lock"},
// ffffffffc01099b0 T vmm_user_allocate
    {0xffffffffc01099b0ull, "vmm_user_allocate"},
// ffffffffc0109e00 t vmm_umem_find_free_pages
    {0xffffffffc0109e00ull, "vmm_umem_find_free_pages"},
// ffffffffc010a010 t vmm_umem_convert_page
    {0xffffffffc010a010ull, "vmm_umem_convert_page"},
// ffffffffc010a4c0 T vmm_user_free
    {0xffffffffc010a4c0ull, "vmm_user_free"},
// ffffffffc010a6e0 T vmm_create_user_mem
    {0xffffffffc010a6e0ull, "vmm_create_user_mem"},
// ffffffffc010a750 t vmm_umem_add_range
    {0xffffffffc010a750ull, "vmm_umem_add_range"},
// ffffffffc010a920 T vmm_destroy_user_mem
    {0xffffffffc010a920ull, "vmm_destroy_user_mem"},
// ffffffffc010aa40 t vmm_umem_remove_memory_map_entry
    {0xffffffffc010aa40ull, "vmm_umem_remove_memory_map_entry"},
// ffffffffc010aab0 t allocate_block
    {0xffffffffc010aab0ull, "allocate_block"},
// ffffffffc010ac00 T vmm_init
    {0xffffffffc010ac00ull, "vmm_init"},
// ffffffffc010ade0 T vmm_map
    {0xffffffffc010ade0ull, "vmm_map"},
// ffffffffc010b050 T vmm_set_handle
    {0xffffffffc010b050ull, "vmm_set_handle"},
// ffffffffc010b090 T vmm_enable_cpu_features
    {0xffffffffc010b090ull, "vmm_enable_cpu_features"},
// ffffffffc010b240 T vmm_virtual_to_physical
    {0xffffffffc010b240ull, "vmm_virtual_to_physical"},
// ffffffffc010b440 t get_entry_at_virtual_address
    {0xffffffffc010b440ull, "get_entry_at_virtual_address"},
// ffffffffc010b6c0 T vmm_is_mapped
    {0xffffffffc010b6c0ull, "vmm_is_mapped"},
// ffffffffc010b840 t page_size_by_type
    {0xffffffffc010b840ull, "page_size_by_type"},
// ffffffffc010b8f0 T vmm_unmap
    {0xffffffffc010b8f0ull, "vmm_unmap"},
// ffffffffc010bb30 t set_pte
    {0xffffffffc010bb30ull, "set_pte"},
// ffffffffc010c6d0 T vmm_set_perms
    {0xffffffffc010c6d0ull, "vmm_set_perms"},
// ffffffffc010c8a0 T vmm_get_handle
    {0xffffffffc010c8a0ull, "vmm_get_handle"},
// ffffffffc010c8b0 T vmm_create_address_space
    {0xffffffffc010c8b0ull, "vmm_create_address_space"},
// ffffffffc010c9e0 T vmm_destroy_page_table
    {0xffffffffc010c9e0ull, "vmm_destroy_page_table"},
// ffffffffc010cd60 T load_elf
    {0xffffffffc010cd60ull, "load_elf"},
// ffffffffc010d360 T init_kernel_process
    {0xffffffffc010d360ull, "init_kernel_process"},
// ffffffffc010d380 T create_process
    {0xffffffffc010d380ull, "create_process"},
// ffffffffc010d4c0 T spawn_process
    {0xffffffffc010d4c0ull, "spawn_process"},
// ffffffffc010d7d0 T yield
    {0xffffffffc010d7d0ull, "yield"},
// ffffffffc010d830 T init_sched
    {0xffffffffc010d830ull, "init_sched"},
// ffffffffc010d980 t preemption_callback
    {0xffffffffc010d980ull, "preemption_callback"},
// ffffffffc010d9a0 T queue_thread
    {0xffffffffc010d9a0ull, "queue_thread"},
// ffffffffc010da60 T sched_tick
    {0xffffffffc010da60ull, "sched_tick"},
// ffffffffc010dcc0 t yield_interrupt
    {0xffffffffc010dcc0ull, "yield_interrupt"},
// ffffffffc010dd30 t idle_thread_func
    {0xffffffffc010dd30ull, "idle_thread_func"},
// ffffffffc010dd60 t sched_lock
    {0xffffffffc010dd60ull, "sched_lock"},
// ffffffffc010dda0 t sched_unlock
    {0xffffffffc010dda0ull, "sched_unlock"},
// ffffffffc010ddc0 T spawn_thread
    {0xffffffffc010ddc0ull, "spawn_thread"},
// ffffffffc010e0a0 T pcpu_init_for_cpu
    {0xffffffffc010e0a0ull, "pcpu_init_for_cpu"},
// ffffffffc010e0d0 T pcpu_init_for_bsp
    {0xffffffffc010e0d0ull, "pcpu_init_for_bsp"},
// ffffffffc010e0f0 T get_pcpu_base
    {0xffffffffc010e0f0ull, "get_pcpu_base"},
// ffffffffc010e100 T spinlock_acquire_high_tpl
    {0xffffffffc010e100ull, "spinlock_acquire_high_tpl"},
// ffffffffc010e170 T spinlock_acquire
    {0xffffffffc010e170ull, "spinlock_acquire"},
// ffffffffc010e1c0 T spinlock_release
    {0xffffffffc010e1c0ull, "spinlock_release"},
// ffffffffc010e200 T strerror
    {0xffffffffc010e200ull, "strerror"},
// ffffffffc010e250 T is_node_in_list
    {0xffffffffc010e250ull, "is_node_in_list"},
// ffffffffc010e350 t is_list_valid
    {0xffffffffc010e350ull, "is_list_valid"},
// ffffffffc010e460 T initialize_list_had
    {0xffffffffc010e460ull, "initialize_list_had"},
// ffffffffc010e4e0 T insert_head_list
    {0xffffffffc010e4e0ull, "insert_head_list"},
// ffffffffc010e580 T insert_tail_list
    {0xffffffffc010e580ull, "insert_tail_list"},
// ffffffffc010e620 T get_first_node
    {0xffffffffc010e620ull, "get_first_node"},
// ffffffffc010e690 T get_next_node
    {0xffffffffc010e690ull, "get_next_node"},
// ffffffffc010e700 T get_prev_node
    {0xffffffffc010e700ull, "get_prev_node"},
// ffffffffc010e770 T is_list_empty
    {0xffffffffc010e770ull, "is_list_empty"},
// ffffffffc010e7f0 T is_null
    {0xffffffffc010e7f0ull, "is_null"},
// ffffffffc010e820 T is_node_at_end
    {0xffffffffc010e820ull, "is_node_at_end"},
// ffffffffc010e870 T swap_list_entries
    {0xffffffffc010e870ull, "swap_list_entries"},
// ffffffffc010e990 T remove_entry_list
    {0xffffffffc010e990ull, "remove_entry_list"},
// ffffffffc010ea30 T serial_init
    {0xffffffffc010ea30ull, "serial_init"},
// ffffffffc010eab0 T serial_write
    {0xffffffffc010eab0ull, "serial_write"},
// ffffffffc010eb80 T serial_read
    {0xffffffffc010eb80ull, "serial_read"},
// ffffffffc010ec00 T serial_poll
    {0xffffffffc010ec00ull, "serial_poll"},
// ffffffffc010ec30 T trace
    {0xffffffffc010ec30ull, "trace"},
// ffffffffc010eda0 t common_stub
    {0xffffffffc010eda0ull, "common_stub"},
// ffffffffc010edec T handle_interrupt_request_00
    {0xffffffffc010edecull, "handle_interrupt_request_00"},
// ffffffffc010edf2 T handle_interrupt_request_01
    {0xffffffffc010edf2ull, "handle_interrupt_request_01"},
// ffffffffc010edf8 T handle_interrupt_request_02
    {0xffffffffc010edf8ull, "handle_interrupt_request_02"},
// ffffffffc010edfe T handle_interrupt_request_03
    {0xffffffffc010edfeull, "handle_interrupt_request_03"},
// ffffffffc010ee04 T handle_interrupt_request_04
    {0xffffffffc010ee04ull, "handle_interrupt_request_04"},
// ffffffffc010ee0a T handle_interrupt_request_05
    {0xffffffffc010ee0aull, "handle_interrupt_request_05"},
// ffffffffc010ee10 T handle_interrupt_request_06
    {0xffffffffc010ee10ull, "handle_interrupt_request_06"},
// ffffffffc010ee16 T handle_interrupt_request_07
    {0xffffffffc010ee16ull, "handle_interrupt_request_07"},
// ffffffffc010ee1c T handle_interrupt_request_08
    {0xffffffffc010ee1cull, "handle_interrupt_request_08"},
// ffffffffc010ee20 T handle_interrupt_request_09
    {0xffffffffc010ee20ull, "handle_interrupt_request_09"},
// ffffffffc010ee29 T handle_interrupt_request_0a
    {0xffffffffc010ee29ull, "handle_interrupt_request_0a"},
// ffffffffc010ee30 T handle_interrupt_request_0b
    {0xffffffffc010ee30ull, "handle_interrupt_request_0b"},
// ffffffffc010ee37 T handle_interrupt_request_0c
    {0xffffffffc010ee37ull, "handle_interrupt_request_0c"},
// ffffffffc010ee3e T handle_interrupt_request_0d
    {0xffffffffc010ee3eull, "handle_interrupt_request_0d"},
// ffffffffc010ee45 T handle_interrupt_request_0e
    {0xffffffffc010ee45ull, "handle_interrupt_request_0e"},
// ffffffffc010ee4c T handle_interrupt_request_0f
    {0xffffffffc010ee4cull, "handle_interrupt_request_0f"},
// ffffffffc010ee55 T handle_interrupt_request_10
    {0xffffffffc010ee55ull, "handle_interrupt_request_10"},
// ffffffffc010ee5c T handle_interrupt_request_11
    {0xffffffffc010ee5cull, "handle_interrupt_request_11"},
// ffffffffc010ee65 T handle_interrupt_request_12
    {0xffffffffc010ee65ull, "handle_interrupt_request_12"},
// ffffffffc010ee6e T handle_interrupt_request_13
    {0xffffffffc010ee6eull, "handle_interrupt_request_13"},
// ffffffffc010ee77 T handle_interrupt_request_14
    {0xffffffffc010ee77ull, "handle_interrupt_request_14"},
// ffffffffc010ee80 T handle_interrupt_request_15
    {0xffffffffc010ee80ull, "handle_interrupt_request_15"},
// ffffffffc010ee89 T handle_interrupt_request_16
    {0xffffffffc010ee89ull, "handle_interrupt_request_16"},
// ffffffffc010ee92 T handle_interrupt_request_17
    {0xffffffffc010ee92ull, "handle_interrupt_request_17"},
// ffffffffc010ee9b T handle_interrupt_request_18
    {0xffffffffc010ee9bull, "handle_interrupt_request_18"},
// ffffffffc010eea4 T handle_interrupt_request_19
    {0xffffffffc010eea4ull, "handle_interrupt_request_19"},
// ffffffffc010eead T handle_interrupt_request_1a
    {0xffffffffc010eeadull, "handle_interrupt_request_1a"},
// ffffffffc010eeb6 T handle_interrupt_request_1b
    {0xffffffffc010eeb6ull, "handle_interrupt_request_1b"},
// ffffffffc010eebf T handle_interrupt_request_1c
    {0xffffffffc010eebfull, "handle_interrupt_request_1c"},
// ffffffffc010eec8 T handle_interrupt_request_1d
    {0xffffffffc010eec8ull, "handle_interrupt_request_1d"},
// ffffffffc010eed1 T handle_interrupt_request_1e
    {0xffffffffc010eed1ull, "handle_interrupt_request_1e"},
// ffffffffc010eed8 T handle_interrupt_request_1f
    {0xffffffffc010eed8ull, "handle_interrupt_request_1f"},
// ffffffffc010eee1 T handle_interrupt_request_20
    {0xffffffffc010eee1ull, "handle_interrupt_request_20"},
// ffffffffc010eeea T handle_interrupt_request_21
    {0xffffffffc010eeeaull, "handle_interrupt_request_21"},
// ffffffffc010eef3 T handle_interrupt_request_22
    {0xffffffffc010eef3ull, "handle_interrupt_request_22"},
// ffffffffc010eefc T handle_interrupt_request_23
    {0xffffffffc010eefcull, "handle_interrupt_request_23"},
// ffffffffc010ef05 T handle_interrupt_request_24
    {0xffffffffc010ef05ull, "handle_interrupt_request_24"},
// ffffffffc010ef0e T handle_interrupt_request_25
    {0xffffffffc010ef0eull, "handle_interrupt_request_25"},
// ffffffffc010ef17 T handle_interrupt_request_26
    {0xffffffffc010ef17ull, "handle_interrupt_request_26"},
// ffffffffc010ef20 T handle_interrupt_request_27
    {0xffffffffc010ef20ull, "handle_interrupt_request_27"},
// ffffffffc010ef29 T handle_interrupt_request_28
    {0xffffffffc010ef29ull, "handle_interrupt_request_28"},
// ffffffffc010ef32 T handle_interrupt_request_29
    {0xffffffffc010ef32ull, "handle_interrupt_request_29"},
// ffffffffc010ef3b T handle_interrupt_request_2a
    {0xffffffffc010ef3bull, "handle_interrupt_request_2a"},
// ffffffffc010ef44 T handle_interrupt_request_2b
    {0xffffffffc010ef44ull, "handle_interrupt_request_2b"},
// ffffffffc010ef4d T handle_interrupt_request_2c
    {0xffffffffc010ef4dull, "handle_interrupt_request_2c"},
// ffffffffc010ef56 T handle_interrupt_request_2d
    {0xffffffffc010ef56ull, "handle_interrupt_request_2d"},
// ffffffffc010ef5f T handle_interrupt_request_2e
    {0xffffffffc010ef5full, "handle_interrupt_request_2e"},
// ffffffffc010ef68 T handle_interrupt_request_2f
    {0xffffffffc010ef68ull, "handle_interrupt_request_2f"},
// ffffffffc010ef71 T handle_interrupt_request_30
    {0xffffffffc010ef71ull, "handle_interrupt_request_30"},
// ffffffffc010ef7a T handle_interrupt_request_31
    {0xffffffffc010ef7aull, "handle_interrupt_request_31"},
// ffffffffc010ef83 T handle_interrupt_request_32
    {0xffffffffc010ef83ull, "handle_interrupt_request_32"},
// ffffffffc010ef8c T handle_interrupt_request_33
    {0xffffffffc010ef8cull, "handle_interrupt_request_33"},
// ffffffffc010ef95 T handle_interrupt_request_34
    {0xffffffffc010ef95ull, "handle_interrupt_request_34"},
// ffffffffc010ef9e T handle_interrupt_request_35
    {0xffffffffc010ef9eull, "handle_interrupt_request_35"},
// ffffffffc010efa7 T handle_interrupt_request_36
    {0xffffffffc010efa7ull, "handle_interrupt_request_36"},
// ffffffffc010efb0 T handle_interrupt_request_37
    {0xffffffffc010efb0ull, "handle_interrupt_request_37"},
// ffffffffc010efb9 T handle_interrupt_request_38
    {0xffffffffc010efb9ull, "handle_interrupt_request_38"},
// ffffffffc010efc2 T handle_interrupt_request_39
    {0xffffffffc010efc2ull, "handle_interrupt_request_39"},
// ffffffffc010efcb T handle_interrupt_request_3a
    {0xffffffffc010efcbull, "handle_interrupt_request_3a"},
// ffffffffc010efd4 T handle_interrupt_request_3b
    {0xffffffffc010efd4ull, "handle_interrupt_request_3b"},
// ffffffffc010efdd T handle_interrupt_request_3c
    {0xffffffffc010efddull, "handle_interrupt_request_3c"},
// ffffffffc010efe6 T handle_interrupt_request_3d
    {0xffffffffc010efe6ull, "handle_interrupt_request_3d"},
// ffffffffc010efef T handle_interrupt_request_3e
    {0xffffffffc010efefull, "handle_interrupt_request_3e"},
// ffffffffc010eff8 T handle_interrupt_request_3f
    {0xffffffffc010eff8ull, "handle_interrupt_request_3f"},
// ffffffffc010f001 T handle_interrupt_request_40
    {0xffffffffc010f001ull, "handle_interrupt_request_40"},
// ffffffffc010f00a T handle_interrupt_request_41
    {0xffffffffc010f00aull, "handle_interrupt_request_41"},
// ffffffffc010f013 T handle_interrupt_request_42
    {0xffffffffc010f013ull, "handle_interrupt_request_42"},
// ffffffffc010f01c T handle_interrupt_request_43
    {0xffffffffc010f01cull, "handle_interrupt_request_43"},
// ffffffffc010f025 T handle_interrupt_request_44
    {0xffffffffc010f025ull, "handle_interrupt_request_44"},
// ffffffffc010f02e T handle_interrupt_request_45
    {0xffffffffc010f02eull, "handle_interrupt_request_45"},
// ffffffffc010f037 T handle_interrupt_request_46
    {0xffffffffc010f037ull, "handle_interrupt_request_46"},
// ffffffffc010f040 T handle_interrupt_request_47
    {0xffffffffc010f040ull, "handle_interrupt_request_47"},
// ffffffffc010f049 T handle_interrupt_request_48
    {0xffffffffc010f049ull, "handle_interrupt_request_48"},
// ffffffffc010f052 T handle_interrupt_request_49
    {0xffffffffc010f052ull, "handle_interrupt_request_49"},
// ffffffffc010f05b T handle_interrupt_request_4a
    {0xffffffffc010f05bull, "handle_interrupt_request_4a"},
// ffffffffc010f064 T handle_interrupt_request_4b
    {0xffffffffc010f064ull, "handle_interrupt_request_4b"},
// ffffffffc010f06d T handle_interrupt_request_4c
    {0xffffffffc010f06dull, "handle_interrupt_request_4c"},
// ffffffffc010f076 T handle_interrupt_request_4d
    {0xffffffffc010f076ull, "handle_interrupt_request_4d"},
// ffffffffc010f07f T handle_interrupt_request_4e
    {0xffffffffc010f07full, "handle_interrupt_request_4e"},
// ffffffffc010f088 T handle_interrupt_request_4f
    {0xffffffffc010f088ull, "handle_interrupt_request_4f"},
// ffffffffc010f091 T handle_interrupt_request_50
    {0xffffffffc010f091ull, "handle_interrupt_request_50"},
// ffffffffc010f09a T handle_interrupt_request_51
    {0xffffffffc010f09aull, "handle_interrupt_request_51"},
// ffffffffc010f0a3 T handle_interrupt_request_52
    {0xffffffffc010f0a3ull, "handle_interrupt_request_52"},
// ffffffffc010f0ac T handle_interrupt_request_53
    {0xffffffffc010f0acull, "handle_interrupt_request_53"},
// ffffffffc010f0b5 T handle_interrupt_request_54
    {0xffffffffc010f0b5ull, "handle_interrupt_request_54"},
// ffffffffc010f0be T handle_interrupt_request_55
    {0xffffffffc010f0beull, "handle_interrupt_request_55"},
// ffffffffc010f0c7 T handle_interrupt_request_56
    {0xffffffffc010f0c7ull, "handle_interrupt_request_56"},
// ffffffffc010f0d0 T handle_interrupt_request_57
    {0xffffffffc010f0d0ull, "handle_interrupt_request_57"},
// ffffffffc010f0d9 T handle_interrupt_request_58
    {0xffffffffc010f0d9ull, "handle_interrupt_request_58"},
// ffffffffc010f0e2 T handle_interrupt_request_59
    {0xffffffffc010f0e2ull, "handle_interrupt_request_59"},
// ffffffffc010f0eb T handle_interrupt_request_5a
    {0xffffffffc010f0ebull, "handle_interrupt_request_5a"},
// ffffffffc010f0f4 T handle_interrupt_request_5b
    {0xffffffffc010f0f4ull, "handle_interrupt_request_5b"},
// ffffffffc010f0fd T handle_interrupt_request_5c
    {0xffffffffc010f0fdull, "handle_interrupt_request_5c"},
// ffffffffc010f106 T handle_interrupt_request_5d
    {0xffffffffc010f106ull, "handle_interrupt_request_5d"},
// ffffffffc010f10f T handle_interrupt_request_5e
    {0xffffffffc010f10full, "handle_interrupt_request_5e"},
// ffffffffc010f118 T handle_interrupt_request_5f
    {0xffffffffc010f118ull, "handle_interrupt_request_5f"},
// ffffffffc010f121 T handle_interrupt_request_60
    {0xffffffffc010f121ull, "handle_interrupt_request_60"},
// ffffffffc010f12a T handle_interrupt_request_61
    {0xffffffffc010f12aull, "handle_interrupt_request_61"},
// ffffffffc010f133 T handle_interrupt_request_62
    {0xffffffffc010f133ull, "handle_interrupt_request_62"},
// ffffffffc010f13c T handle_interrupt_request_63
    {0xffffffffc010f13cull, "handle_interrupt_request_63"},
// ffffffffc010f145 T handle_interrupt_request_64
    {0xffffffffc010f145ull, "handle_interrupt_request_64"},
// ffffffffc010f14e T handle_interrupt_request_65
    {0xffffffffc010f14eull, "handle_interrupt_request_65"},
// ffffffffc010f157 T handle_interrupt_request_66
    {0xffffffffc010f157ull, "handle_interrupt_request_66"},
// ffffffffc010f160 T handle_interrupt_request_67
    {0xffffffffc010f160ull, "handle_interrupt_request_67"},
// ffffffffc010f169 T handle_interrupt_request_68
    {0xffffffffc010f169ull, "handle_interrupt_request_68"},
// ffffffffc010f172 T handle_interrupt_request_69
    {0xffffffffc010f172ull, "handle_interrupt_request_69"},
// ffffffffc010f17b T handle_interrupt_request_6a
    {0xffffffffc010f17bull, "handle_interrupt_request_6a"},
// ffffffffc010f184 T handle_interrupt_request_6b
    {0xffffffffc010f184ull, "handle_interrupt_request_6b"},
// ffffffffc010f18d T handle_interrupt_request_6c
    {0xffffffffc010f18dull, "handle_interrupt_request_6c"},
// ffffffffc010f196 T handle_interrupt_request_6d
    {0xffffffffc010f196ull, "handle_interrupt_request_6d"},
// ffffffffc010f19f T handle_interrupt_request_6e
    {0xffffffffc010f19full, "handle_interrupt_request_6e"},
// ffffffffc010f1a8 T handle_interrupt_request_6f
    {0xffffffffc010f1a8ull, "handle_interrupt_request_6f"},
// ffffffffc010f1b1 T handle_interrupt_request_70
    {0xffffffffc010f1b1ull, "handle_interrupt_request_70"},
// ffffffffc010f1ba T handle_interrupt_request_71
    {0xffffffffc010f1baull, "handle_interrupt_request_71"},
// ffffffffc010f1c3 T handle_interrupt_request_72
    {0xffffffffc010f1c3ull, "handle_interrupt_request_72"},
// ffffffffc010f1cc T handle_interrupt_request_73
    {0xffffffffc010f1ccull, "handle_interrupt_request_73"},
// ffffffffc010f1d5 T handle_interrupt_request_74
    {0xffffffffc010f1d5ull, "handle_interrupt_request_74"},
// ffffffffc010f1de T handle_interrupt_request_75
    {0xffffffffc010f1deull, "handle_interrupt_request_75"},
// ffffffffc010f1e7 T handle_interrupt_request_76
    {0xffffffffc010f1e7ull, "handle_interrupt_request_76"},
// ffffffffc010f1f0 T handle_interrupt_request_77
    {0xffffffffc010f1f0ull, "handle_interrupt_request_77"},
// ffffffffc010f1f9 T handle_interrupt_request_78
    {0xffffffffc010f1f9ull, "handle_interrupt_request_78"},
// ffffffffc010f202 T handle_interrupt_request_79
    {0xffffffffc010f202ull, "handle_interrupt_request_79"},
// ffffffffc010f20b T handle_interrupt_request_7a
    {0xffffffffc010f20bull, "handle_interrupt_request_7a"},
// ffffffffc010f214 T handle_interrupt_request_7b
    {0xffffffffc010f214ull, "handle_interrupt_request_7b"},
// ffffffffc010f21d T handle_interrupt_request_7c
    {0xffffffffc010f21dull, "handle_interrupt_request_7c"},
// ffffffffc010f226 T handle_interrupt_request_7d
    {0xffffffffc010f226ull, "handle_interrupt_request_7d"},
// ffffffffc010f22f T handle_interrupt_request_7e
    {0xffffffffc010f22full, "handle_interrupt_request_7e"},
// ffffffffc010f238 T handle_interrupt_request_7f
    {0xffffffffc010f238ull, "handle_interrupt_request_7f"},
// ffffffffc010f241 T handle_interrupt_request_80
    {0xffffffffc010f241ull, "handle_interrupt_request_80"},
// ffffffffc010f24d T handle_interrupt_request_81
    {0xffffffffc010f24dull, "handle_interrupt_request_81"},
// ffffffffc010f259 T handle_interrupt_request_82
    {0xffffffffc010f259ull, "handle_interrupt_request_82"},
// ffffffffc010f265 T handle_interrupt_request_83
    {0xffffffffc010f265ull, "handle_interrupt_request_83"},
// ffffffffc010f271 T handle_interrupt_request_84
    {0xffffffffc010f271ull, "handle_interrupt_request_84"},
// ffffffffc010f27d T handle_interrupt_request_85
    {0xffffffffc010f27dull, "handle_interrupt_request_85"},
// ffffffffc010f289 T handle_interrupt_request_86
    {0xffffffffc010f289ull, "handle_interrupt_request_86"},
// ffffffffc010f295 T handle_interrupt_request_87
    {0xffffffffc010f295ull, "handle_interrupt_request_87"},
// ffffffffc010f2a1 T handle_interrupt_request_88
    {0xffffffffc010f2a1ull, "handle_interrupt_request_88"},
// ffffffffc010f2ad T handle_interrupt_request_89
    {0xffffffffc010f2adull, "handle_interrupt_request_89"},
// ffffffffc010f2b9 T handle_interrupt_request_8a
    {0xffffffffc010f2b9ull, "handle_interrupt_request_8a"},
// ffffffffc010f2c5 T handle_interrupt_request_8b
    {0xffffffffc010f2c5ull, "handle_interrupt_request_8b"},
// ffffffffc010f2d1 T handle_interrupt_request_8c
    {0xffffffffc010f2d1ull, "handle_interrupt_request_8c"},
// ffffffffc010f2dd T handle_interrupt_request_8d
    {0xffffffffc010f2ddull, "handle_interrupt_request_8d"},
// ffffffffc010f2e9 T handle_interrupt_request_8e
    {0xffffffffc010f2e9ull, "handle_interrupt_request_8e"},
// ffffffffc010f2f5 T handle_interrupt_request_8f
    {0xffffffffc010f2f5ull, "handle_interrupt_request_8f"},
// ffffffffc010f301 T handle_interrupt_request_90
    {0xffffffffc010f301ull, "handle_interrupt_request_90"},
// ffffffffc010f30d T handle_interrupt_request_91
    {0xffffffffc010f30dull, "handle_interrupt_request_91"},
// ffffffffc010f319 T handle_interrupt_request_92
    {0xffffffffc010f319ull, "handle_interrupt_request_92"},
// ffffffffc010f325 T handle_interrupt_request_93
    {0xffffffffc010f325ull, "handle_interrupt_request_93"},
// ffffffffc010f331 T handle_interrupt_request_94
    {0xffffffffc010f331ull, "handle_interrupt_request_94"},
// ffffffffc010f33d T handle_interrupt_request_95
    {0xffffffffc010f33dull, "handle_interrupt_request_95"},
// ffffffffc010f349 T handle_interrupt_request_96
    {0xffffffffc010f349ull, "handle_interrupt_request_96"},
// ffffffffc010f355 T handle_interrupt_request_97
    {0xffffffffc010f355ull, "handle_interrupt_request_97"},
// ffffffffc010f361 T handle_interrupt_request_98
    {0xffffffffc010f361ull, "handle_interrupt_request_98"},
// ffffffffc010f36d T handle_interrupt_request_99
    {0xffffffffc010f36dull, "handle_interrupt_request_99"},
// ffffffffc010f379 T handle_interrupt_request_9a
    {0xffffffffc010f379ull, "handle_interrupt_request_9a"},
// ffffffffc010f385 T handle_interrupt_request_9b
    {0xffffffffc010f385ull, "handle_interrupt_request_9b"},
// ffffffffc010f391 T handle_interrupt_request_9c
    {0xffffffffc010f391ull, "handle_interrupt_request_9c"},
// ffffffffc010f39d T handle_interrupt_request_9d
    {0xffffffffc010f39dull, "handle_interrupt_request_9d"},
// ffffffffc010f3a9 T handle_interrupt_request_9e
    {0xffffffffc010f3a9ull, "handle_interrupt_request_9e"},
// ffffffffc010f3b5 T handle_interrupt_request_9f
    {0xffffffffc010f3b5ull, "handle_interrupt_request_9f"},
// ffffffffc010f3c1 T handle_interrupt_request_a0
    {0xffffffffc010f3c1ull, "handle_interrupt_request_a0"},
// ffffffffc010f3cd T handle_interrupt_request_a1
    {0xffffffffc010f3cdull, "handle_interrupt_request_a1"},
// ffffffffc010f3d9 T handle_interrupt_request_a2
    {0xffffffffc010f3d9ull, "handle_interrupt_request_a2"},
// ffffffffc010f3e5 T handle_interrupt_request_a3
    {0xffffffffc010f3e5ull, "handle_interrupt_request_a3"},
// ffffffffc010f3f1 T handle_interrupt_request_a4
    {0xffffffffc010f3f1ull, "handle_interrupt_request_a4"},
// ffffffffc010f3fd T handle_interrupt_request_a5
    {0xffffffffc010f3fdull, "handle_interrupt_request_a5"},
// ffffffffc010f409 T handle_interrupt_request_a6
    {0xffffffffc010f409ull, "handle_interrupt_request_a6"},
// ffffffffc010f415 T handle_interrupt_request_a7
    {0xffffffffc010f415ull, "handle_interrupt_request_a7"},
// ffffffffc010f421 T handle_interrupt_request_a8
    {0xffffffffc010f421ull, "handle_interrupt_request_a8"},
// ffffffffc010f42d T handle_interrupt_request_a9
    {0xffffffffc010f42dull, "handle_interrupt_request_a9"},
// ffffffffc010f439 T handle_interrupt_request_aa
    {0xffffffffc010f439ull, "handle_interrupt_request_aa"},
// ffffffffc010f445 T handle_interrupt_request_ab
    {0xffffffffc010f445ull, "handle_interrupt_request_ab"},
// ffffffffc010f451 T handle_interrupt_request_ac
    {0xffffffffc010f451ull, "handle_interrupt_request_ac"},
// ffffffffc010f45d T handle_interrupt_request_ad
    {0xffffffffc010f45dull, "handle_interrupt_request_ad"},
// ffffffffc010f469 T handle_interrupt_request_ae
    {0xffffffffc010f469ull, "handle_interrupt_request_ae"},
// ffffffffc010f475 T handle_interrupt_request_af
    {0xffffffffc010f475ull, "handle_interrupt_request_af"},
// ffffffffc010f481 T handle_interrupt_request_b0
    {0xffffffffc010f481ull, "handle_interrupt_request_b0"},
// ffffffffc010f48d T handle_interrupt_request_b1
    {0xffffffffc010f48dull, "handle_interrupt_request_b1"},
// ffffffffc010f499 T handle_interrupt_request_b2
    {0xffffffffc010f499ull, "handle_interrupt_request_b2"},
// ffffffffc010f4a5 T handle_interrupt_request_b3
    {0xffffffffc010f4a5ull, "handle_interrupt_request_b3"},
// ffffffffc010f4b1 T handle_interrupt_request_b4
    {0xffffffffc010f4b1ull, "handle_interrupt_request_b4"},
// ffffffffc010f4bd T handle_interrupt_request_b5
    {0xffffffffc010f4bdull, "handle_interrupt_request_b5"},
// ffffffffc010f4c9 T handle_interrupt_request_b6
    {0xffffffffc010f4c9ull, "handle_interrupt_request_b6"},
// ffffffffc010f4d5 T handle_interrupt_request_b7
    {0xffffffffc010f4d5ull, "handle_interrupt_request_b7"},
// ffffffffc010f4e1 T handle_interrupt_request_b8
    {0xffffffffc010f4e1ull, "handle_interrupt_request_b8"},
// ffffffffc010f4ed T handle_interrupt_request_b9
    {0xffffffffc010f4edull, "handle_interrupt_request_b9"},
// ffffffffc010f4f9 T handle_interrupt_request_ba
    {0xffffffffc010f4f9ull, "handle_interrupt_request_ba"},
// ffffffffc010f505 T handle_interrupt_request_bb
    {0xffffffffc010f505ull, "handle_interrupt_request_bb"},
// ffffffffc010f511 T handle_interrupt_request_bc
    {0xffffffffc010f511ull, "handle_interrupt_request_bc"},
// ffffffffc010f51d T handle_interrupt_request_bd
    {0xffffffffc010f51dull, "handle_interrupt_request_bd"},
// ffffffffc010f529 T handle_interrupt_request_be
    {0xffffffffc010f529ull, "handle_interrupt_request_be"},
// ffffffffc010f535 T handle_interrupt_request_bf
    {0xffffffffc010f535ull, "handle_interrupt_request_bf"},
// ffffffffc010f541 T handle_interrupt_request_c0
    {0xffffffffc010f541ull, "handle_interrupt_request_c0"},
// ffffffffc010f54d T handle_interrupt_request_c1
    {0xffffffffc010f54dull, "handle_interrupt_request_c1"},
// ffffffffc010f559 T handle_interrupt_request_c2
    {0xffffffffc010f559ull, "handle_interrupt_request_c2"},
// ffffffffc010f565 T handle_interrupt_request_c3
    {0xffffffffc010f565ull, "handle_interrupt_request_c3"},
// ffffffffc010f571 T handle_interrupt_request_c4
    {0xffffffffc010f571ull, "handle_interrupt_request_c4"},
// ffffffffc010f57d T handle_interrupt_request_c5
    {0xffffffffc010f57dull, "handle_interrupt_request_c5"},
// ffffffffc010f589 T handle_interrupt_request_c6
    {0xffffffffc010f589ull, "handle_interrupt_request_c6"},
// ffffffffc010f595 T handle_interrupt_request_c7
    {0xffffffffc010f595ull, "handle_interrupt_request_c7"},
// ffffffffc010f5a1 T handle_interrupt_request_c8
    {0xffffffffc010f5a1ull, "handle_interrupt_request_c8"},
// ffffffffc010f5ad T handle_interrupt_request_c9
    {0xffffffffc010f5adull, "handle_interrupt_request_c9"},
// ffffffffc010f5b9 T handle_interrupt_request_ca
    {0xffffffffc010f5b9ull, "handle_interrupt_request_ca"},
// ffffffffc010f5c5 T handle_interrupt_request_cb
    {0xffffffffc010f5c5ull, "handle_interrupt_request_cb"},
// ffffffffc010f5d1 T handle_interrupt_request_cc
    {0xffffffffc010f5d1ull, "handle_interrupt_request_cc"},
// ffffffffc010f5dd T handle_interrupt_request_cd
    {0xffffffffc010f5ddull, "handle_interrupt_request_cd"},
// ffffffffc010f5e9 T handle_interrupt_request_ce
    {0xffffffffc010f5e9ull, "handle_interrupt_request_ce"},
// ffffffffc010f5f5 T handle_interrupt_request_cf
    {0xffffffffc010f5f5ull, "handle_interrupt_request_cf"},
// ffffffffc010f601 T handle_interrupt_request_d0
    {0xffffffffc010f601ull, "handle_interrupt_request_d0"},
// ffffffffc010f60d T handle_interrupt_request_d1
    {0xffffffffc010f60dull, "handle_interrupt_request_d1"},
// ffffffffc010f619 T handle_interrupt_request_d2
    {0xffffffffc010f619ull, "handle_interrupt_request_d2"},
// ffffffffc010f625 T handle_interrupt_request_d3
    {0xffffffffc010f625ull, "handle_interrupt_request_d3"},
// ffffffffc010f631 T handle_interrupt_request_d4
    {0xffffffffc010f631ull, "handle_interrupt_request_d4"},
// ffffffffc010f63d T handle_interrupt_request_d5
    {0xffffffffc010f63dull, "handle_interrupt_request_d5"},
// ffffffffc010f649 T handle_interrupt_request_d6
    {0xffffffffc010f649ull, "handle_interrupt_request_d6"},
// ffffffffc010f655 T handle_interrupt_request_d7
    {0xffffffffc010f655ull, "handle_interrupt_request_d7"},
// ffffffffc010f661 T handle_interrupt_request_d8
    {0xffffffffc010f661ull, "handle_interrupt_request_d8"},
// ffffffffc010f66d T handle_interrupt_request_d9
    {0xffffffffc010f66dull, "handle_interrupt_request_d9"},
// ffffffffc010f679 T handle_interrupt_request_da
    {0xffffffffc010f679ull, "handle_interrupt_request_da"},
// ffffffffc010f685 T handle_interrupt_request_db
    {0xffffffffc010f685ull, "handle_interrupt_request_db"},
// ffffffffc010f691 T handle_interrupt_request_dc
    {0xffffffffc010f691ull, "handle_interrupt_request_dc"},
// ffffffffc010f69d T handle_interrupt_request_dd
    {0xffffffffc010f69dull, "handle_interrupt_request_dd"},
// ffffffffc010f6a9 T handle_interrupt_request_de
    {0xffffffffc010f6a9ull, "handle_interrupt_request_de"},
// ffffffffc010f6b5 T handle_interrupt_request_df
    {0xffffffffc010f6b5ull, "handle_interrupt_request_df"},
// ffffffffc010f6c1 T handle_interrupt_request_e0
    {0xffffffffc010f6c1ull, "handle_interrupt_request_e0"},
// ffffffffc010f6cd T handle_interrupt_request_e1
    {0xffffffffc010f6cdull, "handle_interrupt_request_e1"},
// ffffffffc010f6d9 T handle_interrupt_request_e2
    {0xffffffffc010f6d9ull, "handle_interrupt_request_e2"},
// ffffffffc010f6e5 T handle_interrupt_request_e3
    {0xffffffffc010f6e5ull, "handle_interrupt_request_e3"},
// ffffffffc010f6f1 T handle_interrupt_request_e4
    {0xffffffffc010f6f1ull, "handle_interrupt_request_e4"},
// ffffffffc010f6fd T handle_interrupt_request_e5
    {0xffffffffc010f6fdull, "handle_interrupt_request_e5"},
// ffffffffc010f709 T handle_interrupt_request_e6
    {0xffffffffc010f709ull, "handle_interrupt_request_e6"},
// ffffffffc010f715 T handle_interrupt_request_e7
    {0xffffffffc010f715ull, "handle_interrupt_request_e7"},
// ffffffffc010f721 T handle_interrupt_request_e8
    {0xffffffffc010f721ull, "handle_interrupt_request_e8"},
// ffffffffc010f72d T handle_interrupt_request_e9
    {0xffffffffc010f72dull, "handle_interrupt_request_e9"},
// ffffffffc010f739 T handle_interrupt_request_ea
    {0xffffffffc010f739ull, "handle_interrupt_request_ea"},
// ffffffffc010f745 T handle_interrupt_request_eb
    {0xffffffffc010f745ull, "handle_interrupt_request_eb"},
// ffffffffc010f751 T handle_interrupt_request_ec
    {0xffffffffc010f751ull, "handle_interrupt_request_ec"},
// ffffffffc010f75d T handle_interrupt_request_ed
    {0xffffffffc010f75dull, "handle_interrupt_request_ed"},
// ffffffffc010f769 T handle_interrupt_request_ee
    {0xffffffffc010f769ull, "handle_interrupt_request_ee"},
// ffffffffc010f775 T handle_interrupt_request_ef
    {0xffffffffc010f775ull, "handle_interrupt_request_ef"},
// ffffffffc010f781 T handle_interrupt_request_f0
    {0xffffffffc010f781ull, "handle_interrupt_request_f0"},
// ffffffffc010f78d T handle_interrupt_request_f1
    {0xffffffffc010f78dull, "handle_interrupt_request_f1"},
// ffffffffc010f799 T handle_interrupt_request_f2
    {0xffffffffc010f799ull, "handle_interrupt_request_f2"},
// ffffffffc010f7a5 T handle_interrupt_request_f3
    {0xffffffffc010f7a5ull, "handle_interrupt_request_f3"},
// ffffffffc010f7b1 T handle_interrupt_request_f4
    {0xffffffffc010f7b1ull, "handle_interrupt_request_f4"},
// ffffffffc010f7bd T handle_interrupt_request_f5
    {0xffffffffc010f7bdull, "handle_interrupt_request_f5"},
// ffffffffc010f7c9 T handle_interrupt_request_f6
    {0xffffffffc010f7c9ull, "handle_interrupt_request_f6"},
// ffffffffc010f7d5 T handle_interrupt_request_f7
    {0xffffffffc010f7d5ull, "handle_interrupt_request_f7"},
// ffffffffc010f7e1 T handle_interrupt_request_f8
    {0xffffffffc010f7e1ull, "handle_interrupt_request_f8"},
// ffffffffc010f7ed T handle_interrupt_request_f9
    {0xffffffffc010f7edull, "handle_interrupt_request_f9"},
// ffffffffc010f7f9 T handle_interrupt_request_fa
    {0xffffffffc010f7f9ull, "handle_interrupt_request_fa"},
// ffffffffc010f805 T handle_interrupt_request_fb
    {0xffffffffc010f805ull, "handle_interrupt_request_fb"},
// ffffffffc010f811 T handle_interrupt_request_fc
    {0xffffffffc010f811ull, "handle_interrupt_request_fc"},
// ffffffffc010f81d T handle_interrupt_request_fd
    {0xffffffffc010f81dull, "handle_interrupt_request_fd"},
// ffffffffc010f829 T handle_interrupt_request_fe
    {0xffffffffc010f829ull, "handle_interrupt_request_fe"},
// ffffffffc010f835 T handle_interrupt_request_ff
    {0xffffffffc010f835ull, "handle_interrupt_request_ff"},
// ffffffffc010f850 t fix_data_segment
    {0xffffffffc010f850ull, "fix_data_segment"},
// ffffffffc010f861 T gdt_fix_segments
    {0xffffffffc010f861ull, "gdt_fix_segments"},
// ffffffffc010f880 T memset
    {0xffffffffc010f880ull, "memset"},
// ffffffffc010f8e0 T memcmp
    {0xffffffffc010f8e0ull, "memcmp"},
// ffffffffc010f980 T memcpy
    {0xffffffffc010f980ull, "memcpy"},
// ffffffffc010fa00 T memmove
    {0xffffffffc010fa00ull, "memmove"},
// ffffffffc010fb00 T strcmp
    {0xffffffffc010fb00ull, "strcmp"},
// ffffffffc010fb90 T strlen
    {0xffffffffc010fb90ull, "strlen"},
// ffffffffc010fbe0 T stbsp_set_separators
    {0xffffffffc010fbe0ull, "stbsp_set_separators"},
// ffffffffc010fc10 T stbsp_vsprintfcb
    {0xffffffffc010fc10ull, "stbsp_vsprintfcb"},
// ffffffffc0112000 t stbsp__lead_sign
    {0xffffffffc0112000ull, "stbsp__lead_sign"},
// ffffffffc0112090 T stbsp_sprintf
    {0xffffffffc0112090ull, "stbsp_sprintf"},
// ffffffffc0112100 T stbsp_vsnprintf
    {0xffffffffc0112100ull, "stbsp_vsnprintf"},
// ffffffffc0112240 t stbsp__count_clamp_callback
    {0xffffffffc0112240ull, "stbsp__count_clamp_callback"},
// ffffffffc0112280 t stbsp__clamp_callback
    {0xffffffffc0112280ull, "stbsp__clamp_callback"},
// ffffffffc01123c0 T stbsp_snprintf
    {0xffffffffc01123c0ull, "stbsp_snprintf"},
// ffffffffc0112420 T stbsp_vsprintf
    {0xffffffffc0112420ull, "stbsp_vsprintf"},
// ffffffffc0112470 T stbds_arrgrowf
    {0xffffffffc0112470ull, "stbds_arrgrowf"},
// ffffffffc0112670 T stbds_rand_seed
    {0xffffffffc0112670ull, "stbds_rand_seed"},
// ffffffffc0112690 T stbds_hash_string
    {0xffffffffc0112690ull, "stbds_hash_string"},
// ffffffffc01127a0 T stbds_hash_bytes
    {0xffffffffc01127a0ull, "stbds_hash_bytes"},
// ffffffffc01129f0 t stbds_siphash_bytes
    {0xffffffffc01129f0ull, "stbds_siphash_bytes"},
// ffffffffc0112fa0 T stbds_hmfree_func
    {0xffffffffc0112fa0ull, "stbds_hmfree_func"},
// ffffffffc0113070 T stbds_strreset
    {0xffffffffc0113070ull, "stbds_strreset"},
// ffffffffc01130e0 T stbds_hmget_key_ts
    {0xffffffffc01130e0ull, "stbds_hmget_key_ts"},
// ffffffffc0113250 t stbds_hm_find_slot
    {0xffffffffc0113250ull, "stbds_hm_find_slot"},
// ffffffffc0113500 T stbds_hmget_key
    {0xffffffffc0113500ull, "stbds_hmget_key"},
// ffffffffc0113560 T stbds_hmput_default
    {0xffffffffc0113560ull, "stbds_hmput_default"},
// ffffffffc0113640 T stbds_hmput_key
    {0xffffffffc0113640ull, "stbds_hmput_key"},
// ffffffffc0113d90 t stbds_make_hash_index
    {0xffffffffc0113d90ull, "stbds_make_hash_index"},
// ffffffffc0114320 t stbds_probe_position
    {0xffffffffc0114320ull, "stbds_probe_position"},
// ffffffffc0114360 t stbds_is_key_equal
    {0xffffffffc0114360ull, "stbds_is_key_equal"},
// ffffffffc0114400 t stbds_strdup
    {0xffffffffc0114400ull, "stbds_strdup"},
// ffffffffc0114460 T stbds_stralloc
    {0xffffffffc0114460ull, "stbds_stralloc"},
// ffffffffc0114630 T stbds_shmode_func
    {0xffffffffc0114630ull, "stbds_shmode_func"},
// ffffffffc01146d0 T stbds_hmdel_key
    {0xffffffffc01146d0ull, "stbds_hmdel_key"},
// ffffffffc0114a30 t stbds_log2
    {0xffffffffc0114a30ull, "stbds_log2"},
// ffffffffc0114a80 T tlsf_create
    {0xffffffffc0114a80ull, "tlsf_create"},
// ffffffffc0114be0 t add_pool
    {0xffffffffc0114be0ull, "add_pool"},
// ffffffffc0114c80 T tlsf_destroy
    {0xffffffffc0114c80ull, "tlsf_destroy"},
// ffffffffc0114d00 t block_size
    {0xffffffffc0114d00ull, "block_size"},
// ffffffffc0114d30 T tlsf_mallocx
    {0xffffffffc0114d30ull, "tlsf_mallocx"},
// ffffffffc0114e80 t adjust_size
    {0xffffffffc0114e80ull, "adjust_size"},
// ffffffffc0114ed0 t block_locate_free
    {0xffffffffc0114ed0ull, "block_locate_free"},
// ffffffffc0114f50 t round_block_size
    {0xffffffffc0114f50ull, "round_block_size"},
// ffffffffc0114fc0 t block_trim_free
    {0xffffffffc0114fc0ull, "block_trim_free"},
// ffffffffc0115030 t block_set_free
    {0xffffffffc0115030ull, "block_set_free"},
// ffffffffc01150b0 t block_to_ptr
    {0xffffffffc01150b0ull, "block_to_ptr"},
// ffffffffc01150d0 T tlsf_free
    {0xffffffffc01150d0ull, "tlsf_free"},
// ffffffffc01151a0 t block_from_ptr
    {0xffffffffc01151a0ull, "block_from_ptr"},
// ffffffffc01151c0 t block_merge_prev
    {0xffffffffc01151c0ull, "block_merge_prev"},
// ffffffffc0115220 t block_merge_next
    {0xffffffffc0115220ull, "block_merge_next"},
// ffffffffc0115280 t block_is_last
    {0xffffffffc0115280ull, "block_is_last"},
// ffffffffc01152b0 t block_next
    {0xffffffffc01152b0ull, "block_next"},
// ffffffffc01152f0 t remove_pool
    {0xffffffffc01152f0ull, "remove_pool"},
// ffffffffc0115350 t block_insert
    {0xffffffffc0115350ull, "block_insert"},
// ffffffffc01153a0 T tlsf_reallocx
    {0xffffffffc01153a0ull, "tlsf_reallocx"},
// ffffffffc01155c0 t block_is_free
    {0xffffffffc01155c0ull, "block_is_free"},
// ffffffffc0115600 t block_set_prev_free
    {0xffffffffc0115600ull, "block_set_prev_free"},
// ffffffffc0115660 t block_trim_used
    {0xffffffffc0115660ull, "block_trim_used"},
// ffffffffc01156e0 t block_link_next
    {0xffffffffc01156e0ull, "block_link_next"},
// ffffffffc0115710 t align_up
    {0xffffffffc0115710ull, "align_up"},
// ffffffffc0115730 t mapping_insert
    {0xffffffffc0115730ull, "mapping_insert"},
// ffffffffc01157c0 t search_suitable_block
    {0xffffffffc01157c0ull, "search_suitable_block"},
// ffffffffc01158b0 t remove_free_block
    {0xffffffffc01158b0ull, "remove_free_block"},
// ffffffffc01159c0 t tlsf_fls
    {0xffffffffc01159c0ull, "tlsf_fls"},
// ffffffffc0115a20 t tlsf_ffs
    {0xffffffffc0115a20ull, "tlsf_ffs"},
// ffffffffc0115a50 t block_can_split
    {0xffffffffc0115a50ull, "block_can_split"},
// ffffffffc0115a90 t block_split
    {0xffffffffc0115a90ull, "block_split"},
// ffffffffc0115b10 t block_set_size
    {0xffffffffc0115b10ull, "block_set_size"},
// ffffffffc0115b50 t block_is_prev_free
    {0xffffffffc0115b50ull, "block_is_prev_free"},
// ffffffffc0115b90 t block_prev
    {0xffffffffc0115b90ull, "block_prev"},
// ffffffffc0115bb0 t block_remove
    {0xffffffffc0115bb0ull, "block_remove"},
// ffffffffc0115c00 t block_absorb
    {0xffffffffc0115c00ull, "block_absorb"},
// ffffffffc0115c50 t insert_free_block
    {0xffffffffc0115c50ull, "insert_free_block"},
// ffffffffc0119000 d fs_components
// ffffffffc0119010 d all_components
// ffffffffc0119020 d type_to_name
// ffffffffc0119028 d component
// ffffffffc0119070 d early_current_tpl
// ffffffffc0119078 d handler
// ffffffffc01190a8 D pit8254
// ffffffffc01190b0 d timers_list
// ffffffffc01190c0 d sv_handler
// ffffffffc01190f0 D idt
// ffffffffc0119100 d ISR_NAMES
// ffffffffc01191b0 d PROCESSOR_NAME
// ffffffffc01191c0 d TABLE_NAME
// ffffffffc01191e0 d PRESENT_NAMES
// ffffffffc01191f0 d USER_NAME
// ffffffffc0119200 d OPERATION_NAME
// ffffffffc0119210 d gdt_entries
// ffffffffc0119248 D gdt
// ffffffffc0119260 d tboot_mmap_names
// ffffffffc0119288 d mem_map
// ffffffffc0119298 d free_entries_list
// ffffffffc01192a8 d free_blocks
// ffffffffc01192b8 d used_blocks
// ffffffffc01192c8 d stack_allocation
// ffffffffc01192d0 d free_block_list
// ffffffffc01192e0 D process_list
// ffffffffc01192f0 D kernel_process
// ffffffffc0119340 d g_pid_gen
// ffffffffc0119348 d handler
// ffffffffc0119378 d threads_queue
// ffffffffc0119390 d errors
// ffffffffc01193d0 d stbsp__period
// ffffffffc01193d1 d stbsp__comma
// ffffffffc01193e0 d stbsp_vsprintfcb.hex
// ffffffffc0119400 d stbsp_vsprintfcb.hexu
// ffffffffc0119414 d stbsp__digitpair
// ffffffffc01194e0 d stbds_hash_seed
// ffffffffc01194f0 D symlist
// ffffffffc0119500 D bsp_pcpu_data
// ffffffffc011a000 B acpi_madt
// ffffffffc011a008 B acpi_rsdp
// ffffffffc011a010 B acpi_xsdp
// ffffffffc011a018 B acpi_rsdt
// ffffffffc011a020 B acpi_xsdt
// ffffffffc011a028 b components_lock
// ffffffffc011a030 b primary_components
// ffffffffc011a038 b initrd_files
// ffffffffc011a040 b lock
// ffffffffc011a050 b notify_queues
// ffffffffc011a250 b notify_queues_pending
// ffffffffc011a254 b event_lock
// ffffffffc011a260 b period
// ffffffffc011a268 b timer
// ffffffffc011a270 b check_timers_event
// ffffffffc011a278 b current_system_time
// ffffffffc011a280 b timer_lock
// ffffffffc011a288 b ioapics
// ffffffffc011a290 b isos
// ffffffffc011a298 b initialized_once
// ffffffffc011a2a0 b idt_entries
// ffffffffc011b2a0 b interrupt_handlers
// ffffffffc011c2a0 b interrupt_vector
// ffffffffc011c614 b index
// ffffffffc011c618 b g_info
// ffffffffc011c620 b g_kernel_tlsf
// ffffffffc011c630 b mem_memory_top
// ffffffffc011c638 b memory_base
// ffffffffc011c640 b lock
// ffffffffc011c650 b temp_entries
// ffffffffc011c7e0 b temp_entries_count
// ffffffffc011c7e8 b freeing
// ffffffffc011c7ec b lock
// ffffffffc011c7f4 b free_block_list_lock
// ffffffffc011c800 b memory_base
// ffffffffc011c808 b support_1gb
// ffffffffc011c809 b support_global
// ffffffffc011c80c B process_lock
// ffffffffc011c814 b flag
// ffffffffc019c818 B kernel_virtual_end
// ffffffffc019c818 b tmp_kernel_stack
// 
//not enough values to unpack (expected 3, got 1)
    {0xffffffffc019c818, "__kernel_end"}
};

