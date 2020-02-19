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
// 000000000019c190 B kernel_physical_end
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
// ffffffffc0101c70 T _BitScanReverse
    {0xffffffffc0101c70ull, "_BitScanReverse"},
// ffffffffc0101cb0 T _BitScanReverse64
    {0xffffffffc0101cb0ull, "_BitScanReverse64"},
// ffffffffc0101cf0 T __readmsr_efer
    {0xffffffffc0101cf0ull, "__readmsr_efer"},
// ffffffffc0101d10 T __writemsr_efer
    {0xffffffffc0101d10ull, "__writemsr_efer"},
// ffffffffc0101d30 T __readmsr_apic_base
    {0xffffffffc0101d30ull, "__readmsr_apic_base"},
// ffffffffc0101d50 T generate_device_address
    {0xffffffffc0101d50ull, "generate_device_address"},
// ffffffffc0101e90 T register_component
    {0xffffffffc0101e90ull, "register_component"},
// ffffffffc0102060 T get_component
    {0xffffffffc0102060ull, "get_component"},
// ffffffffc01021b0 T get_next_component
    {0xffffffffc01021b0ull, "get_next_component"},
// ffffffffc01021d0 T get_primary
    {0xffffffffc01021d0ull, "get_primary"},
// ffffffffc01022d0 T set_primary
    {0xffffffffc01022d0ull, "set_primary"},
// ffffffffc0102310 T tar_tell
    {0xffffffffc0102310ull, "tar_tell"},
// ffffffffc0102420 T tar_read
    {0xffffffffc0102420ull, "tar_read"},
// ffffffffc0102630 T create_initrd_fs
    {0xffffffffc0102630ull, "create_initrd_fs"},
// ffffffffc01027b0 t tar_size
    {0xffffffffc01027b0ull, "tar_size"},
// ffffffffc0102830 t tar_open
    {0xffffffffc0102830ull, "tar_open"},
// ffffffffc0102a00 t tar_is_readonly
    {0xffffffffc0102a00ull, "tar_is_readonly"},
// ffffffffc0102a80 t tar_eof
    {0xffffffffc0102a80ull, "tar_eof"},
// ffffffffc0102ba0 t tar_seek
    {0xffffffffc0102ba0ull, "tar_seek"},
// ffffffffc0102e30 t tar_close
    {0xffffffffc0102e30ull, "tar_close"},
// ffffffffc0102eb0 T sha1_transform
    {0xffffffffc0102eb0ull, "sha1_transform"},
// ffffffffc0103260 T sha1_init
    {0xffffffffc0103260ull, "sha1_init"},
// ffffffffc01032f0 T sha1_update
    {0xffffffffc01032f0ull, "sha1_update"},
// ffffffffc01033a0 T sha1_final
    {0xffffffffc01033a0ull, "sha1_final"},
// ffffffffc0103660 T events_init
    {0xffffffffc0103660ull, "events_init"},
// ffffffffc01036d0 T raise_tpl
    {0xffffffffc01036d0ull, "raise_tpl"},
// ffffffffc0103760 T set_tpl
    {0xffffffffc0103760ull, "set_tpl"},
// ffffffffc01037c0 T restore_tpl
    {0xffffffffc01037c0ull, "restore_tpl"},
// ffffffffc01038b0 t dispatch_event_notifies
    {0xffffffffc01038b0ull, "dispatch_event_notifies"},
// ffffffffc0103a00 T get_tpl
    {0xffffffffc0103a00ull, "get_tpl"},
// ffffffffc0103a60 T create_event
    {0xffffffffc0103a60ull, "create_event"},
// ffffffffc0103b80 T signal_event
    {0xffffffffc0103b80ull, "signal_event"},
// ffffffffc0103cd0 T check_event
    {0xffffffffc0103cd0ull, "check_event"},
// ffffffffc0103da0 T wait_for_event
    {0xffffffffc0103da0ull, "wait_for_event"},
// ffffffffc01040d0 T close_event
    {0xffffffffc01040d0ull, "close_event"},
// ffffffffc0104180 t aquire_event_lock
    {0xffffffffc0104180ull, "aquire_event_lock"},
// ffffffffc01041a0 t release_event_lock
    {0xffffffffc01041a0ull, "release_event_lock"},
// ffffffffc01041c0 T init_pit8254
    {0xffffffffc01041c0ull, "init_pit8254"},
// ffffffffc0104210 t set_timer_period
    {0xffffffffc0104210ull, "set_timer_period"},
// ffffffffc01042c0 t interrupt_handler
    {0xffffffffc01042c0ull, "interrupt_handler"},
// ffffffffc01042f0 t set_pit_count
    {0xffffffffc01042f0ull, "set_pit_count"},
// ffffffffc0104350 T init_timer
    {0xffffffffc0104350ull, "init_timer"},
// ffffffffc01044a0 t check_timers
    {0xffffffffc01044a0ull, "check_timers"},
// ffffffffc01045f0 T timer_tick
    {0xffffffffc01045f0ull, "timer_tick"},
// ffffffffc0104660 T get_current_system_time
    {0xffffffffc0104660ull, "get_current_system_time"},
// ffffffffc0104680 T set_timer
    {0xffffffffc0104680ull, "set_timer"},
// ffffffffc0104850 t insert_event_timer
    {0xffffffffc0104850ull, "insert_event_timer"},
// ffffffffc01048f0 T ioapic_add
    {0xffffffffc01048f0ull, "ioapic_add"},
// ffffffffc0104a10 t ioapic_read
    {0xffffffffc0104a10ull, "ioapic_read"},
// ffffffffc0104a90 T ioapic_add_iso
    {0xffffffffc0104a90ull, "ioapic_add_iso"},
// ffffffffc0104b20 T ioapic_enable_interrupt
    {0xffffffffc0104b20ull, "ioapic_enable_interrupt"},
// ffffffffc0104ca0 t get_ioapic
    {0xffffffffc0104ca0ull, "get_ioapic"},
// ffffffffc0104dc0 t ioapic_write
    {0xffffffffc0104dc0ull, "ioapic_write"},
// ffffffffc0104e40 T ioapic_configure_interrupt
    {0xffffffffc0104e40ull, "ioapic_configure_interrupt"},
// ffffffffc0105130 T get_apic_id
    {0xffffffffc0105130ull, "get_apic_id"},
// ffffffffc0105150 t lapic_read
    {0xffffffffc0105150ull, "lapic_read"},
// ffffffffc0105180 T lapic_init
    {0xffffffffc0105180ull, "lapic_init"},
// ffffffffc01052a0 t lapic_write
    {0xffffffffc01052a0ull, "lapic_write"},
// ffffffffc01052d0 T send_fixed_ipi
    {0xffffffffc01052d0ull, "send_fixed_ipi"},
// ffffffffc01052f0 T send_apic_eoi
    {0xffffffffc01052f0ull, "send_apic_eoi"},
// ffffffffc0105310 t spurious_interrupt_handler
    {0xffffffffc0105310ull, "spurious_interrupt_handler"},
// ffffffffc0105330 T idt_init
    {0xffffffffc0105330ull, "idt_init"},
// ffffffffc0106650 t set_idt_entry
    {0xffffffffc0106650ull, "set_idt_entry"},
// ffffffffc0106820 T interrupts_init
    {0xffffffffc0106820ull, "interrupts_init"},
// ffffffffc0106890 T common_interrupt_handler
    {0xffffffffc0106890ull, "common_interrupt_handler"},
// ffffffffc0106a20 t default_interrupt_handler
    {0xffffffffc0106a20ull, "default_interrupt_handler"},
// ffffffffc0107360 T interrupt_register
    {0xffffffffc0107360ull, "interrupt_register"},
// ffffffffc01074f0 T interrupt_free
    {0xffffffffc01074f0ull, "interrupt_free"},
// ffffffffc0107580 t interrupt_allocate
    {0xffffffffc0107580ull, "interrupt_allocate"},
// ffffffffc01076f0 T kernel_main
    {0xffffffffc01076f0ull, "kernel_main"},
// ffffffffc0107880 t main_thread
    {0xffffffffc0107880ull, "main_thread"},
// ffffffffc0107aa0 T init_tss_for_cpu
    {0xffffffffc0107aa0ull, "init_tss_for_cpu"},
// ffffffffc0107b70 T mm_init
    {0xffffffffc0107b70ull, "mm_init"},
// ffffffffc0107b80 T mm_allocate
    {0xffffffffc0107b80ull, "mm_allocate"},
// ffffffffc0107c50 t tlsf_malloc
    {0xffffffffc0107c50ull, "tlsf_malloc"},
// ffffffffc0107c90 t tlsf_add_pool
    {0xffffffffc0107c90ull, "tlsf_add_pool"},
// ffffffffc0107de0 T mm_reallocate
    {0xffffffffc0107de0ull, "mm_reallocate"},
// ffffffffc0107eb0 t tlsf_realloc
    {0xffffffffc0107eb0ull, "tlsf_realloc"},
// ffffffffc01080b0 T mm_free
    {0xffffffffc01080b0ull, "mm_free"},
// ffffffffc01080d0 t tlsf_free
    {0xffffffffc01080d0ull, "tlsf_free"},
// ffffffffc0108190 t tlsf_create
    {0xffffffffc0108190ull, "tlsf_create"},
// ffffffffc01081a0 t control_construct
    {0xffffffffc01081a0ull, "control_construct"},
// ffffffffc0108240 t adjust_request_size
    {0xffffffffc0108240ull, "adjust_request_size"},
// ffffffffc01082d0 t block_locate_free
    {0xffffffffc01082d0ull, "block_locate_free"},
// ffffffffc01083b0 t block_prepare_used
    {0xffffffffc01083b0ull, "block_prepare_used"},
// ffffffffc0108460 t align_up
    {0xffffffffc0108460ull, "align_up"},
// ffffffffc0108510 t mapping_search
    {0xffffffffc0108510ull, "mapping_search"},
// ffffffffc0108580 t search_suitable_block
    {0xffffffffc0108580ull, "search_suitable_block"},
// ffffffffc01086c0 t block_size
    {0xffffffffc01086c0ull, "block_size"},
// ffffffffc01086e0 t remove_free_block
    {0xffffffffc01086e0ull, "remove_free_block"},
// ffffffffc0108880 t tlsf_fls_sizet
    {0xffffffffc0108880ull, "tlsf_fls_sizet"},
// ffffffffc01088e0 t mapping_insert
    {0xffffffffc01088e0ull, "mapping_insert"},
// ffffffffc0108970 t tlsf_fls
    {0xffffffffc0108970ull, "tlsf_fls"},
// ffffffffc01089c0 t tlsf_ffs
    {0xffffffffc01089c0ull, "tlsf_ffs"},
// ffffffffc01089f0 t block_trim_free
    {0xffffffffc01089f0ull, "block_trim_free"},
// ffffffffc0108ab0 t block_mark_as_used
    {0xffffffffc0108ab0ull, "block_mark_as_used"},
// ffffffffc0108af0 t block_to_ptr
    {0xffffffffc0108af0ull, "block_to_ptr"},
// ffffffffc0108b10 t block_is_free
    {0xffffffffc0108b10ull, "block_is_free"},
// ffffffffc0108b30 t block_can_split
    {0xffffffffc0108b30ull, "block_can_split"},
// ffffffffc0108b70 t block_split
    {0xffffffffc0108b70ull, "block_split"},
// ffffffffc0108d30 t block_link_next
    {0xffffffffc0108d30ull, "block_link_next"},
// ffffffffc0108d60 t block_set_prev_free
    {0xffffffffc0108d60ull, "block_set_prev_free"},
// ffffffffc0108d90 t block_insert
    {0xffffffffc0108d90ull, "block_insert"},
// ffffffffc0108dd0 t offset_to_block
    {0xffffffffc0108dd0ull, "offset_to_block"},
// ffffffffc0108df0 t align_ptr
    {0xffffffffc0108df0ull, "align_ptr"},
// ffffffffc0108ea0 t block_set_size
    {0xffffffffc0108ea0ull, "block_set_size"},
// ffffffffc0108ee0 t block_mark_as_free
    {0xffffffffc0108ee0ull, "block_mark_as_free"},
// ffffffffc0108f20 t block_set_free
    {0xffffffffc0108f20ull, "block_set_free"},
// ffffffffc0108f50 t block_next
    {0xffffffffc0108f50ull, "block_next"},
// ffffffffc0108ff0 t block_is_last
    {0xffffffffc0108ff0ull, "block_is_last"},
// ffffffffc0109020 t insert_free_block
    {0xffffffffc0109020ull, "insert_free_block"},
// ffffffffc01091f0 t block_set_prev_used
    {0xffffffffc01091f0ull, "block_set_prev_used"},
// ffffffffc0109220 t block_set_used
    {0xffffffffc0109220ull, "block_set_used"},
// ffffffffc0109250 t tlsf_pool_overhead
    {0xffffffffc0109250ull, "tlsf_pool_overhead"},
// ffffffffc0109260 t align_down
    {0xffffffffc0109260ull, "align_down"},
// ffffffffc0109300 t block_from_ptr
    {0xffffffffc0109300ull, "block_from_ptr"},
// ffffffffc0109320 t block_merge_next
    {0xffffffffc0109320ull, "block_merge_next"},
// ffffffffc0109430 t block_trim_used
    {0xffffffffc0109430ull, "block_trim_used"},
// ffffffffc01094f0 t block_remove
    {0xffffffffc01094f0ull, "block_remove"},
// ffffffffc0109530 t block_absorb
    {0xffffffffc0109530ull, "block_absorb"},
// ffffffffc01095e0 t block_merge_prev
    {0xffffffffc01095e0ull, "block_merge_prev"},
// ffffffffc01096f0 t block_is_prev_free
    {0xffffffffc01096f0ull, "block_is_prev_free"},
// ffffffffc0109710 t block_prev
    {0xffffffffc0109710ull, "block_prev"},
// ffffffffc0109790 T pmm_init
    {0xffffffffc0109790ull, "pmm_init"},
// ffffffffc0109910 t add_range
    {0xffffffffc0109910ull, "add_range"},
// ffffffffc0109b80 T pmm_post_vmm
    {0xffffffffc0109b80ull, "pmm_post_vmm"},
// ffffffffc0109dd0 T pmm_allocate_pages
    {0xffffffffc0109dd0ull, "pmm_allocate_pages"},
// ffffffffc010a160 t find_free_pages
    {0xffffffffc010a160ull, "find_free_pages"},
// ffffffffc010a370 t convert_page
    {0xffffffffc010a370ull, "convert_page"},
// ffffffffc010a8e0 T pmm_free_pages
    {0xffffffffc010a8e0ull, "pmm_free_pages"},
// ffffffffc010a9f0 t remove_mem_map_entry
    {0xffffffffc010a9f0ull, "remove_mem_map_entry"},
// ffffffffc010aa40 t move_temp_entries
    {0xffffffffc010aa40ull, "move_temp_entries"},
// ffffffffc010ac00 t allocate_memory_map_entry
    {0xffffffffc010ac00ull, "allocate_memory_map_entry"},
// ffffffffc010acd0 t allocate_pool_pages
    {0xffffffffc010acd0ull, "allocate_pool_pages"},
// ffffffffc010ad70 T allocate_stack
    {0xffffffffc010ad70ull, "allocate_stack"},
// ffffffffc010af70 T free_stack
    {0xffffffffc010af70ull, "free_stack"},
// ffffffffc010b080 t acquire_alloc_lock
    {0xffffffffc010b080ull, "acquire_alloc_lock"},
// ffffffffc010b0a0 t release_alloc_lock
    {0xffffffffc010b0a0ull, "release_alloc_lock"},
// ffffffffc010b0c0 T vmm_init
    {0xffffffffc010b0c0ull, "vmm_init"},
// ffffffffc010b2a0 T vmm_map
    {0xffffffffc010b2a0ull, "vmm_map"},
// ffffffffc010b510 T vmm_set_handle
    {0xffffffffc010b510ull, "vmm_set_handle"},
// ffffffffc010b550 T vmm_enable_cpu_features
    {0xffffffffc010b550ull, "vmm_enable_cpu_features"},
// ffffffffc010b700 T vmm_virtual_to_physical
    {0xffffffffc010b700ull, "vmm_virtual_to_physical"},
// ffffffffc010b900 t get_entry_at_virtual_address
    {0xffffffffc010b900ull, "get_entry_at_virtual_address"},
// ffffffffc010bb80 T vmm_is_mapped
    {0xffffffffc010bb80ull, "vmm_is_mapped"},
// ffffffffc010bd00 t page_size_by_type
    {0xffffffffc010bd00ull, "page_size_by_type"},
// ffffffffc010bdb0 T vmm_unmap
    {0xffffffffc010bdb0ull, "vmm_unmap"},
// ffffffffc010bff0 t set_pte
    {0xffffffffc010bff0ull, "set_pte"},
// ffffffffc010cab0 T init_kernel_process
    {0xffffffffc010cab0ull, "init_kernel_process"},
// ffffffffc010cad0 T yield
    {0xffffffffc010cad0ull, "yield"},
// ffffffffc010cae0 T init_sched
    {0xffffffffc010cae0ull, "init_sched"},
// ffffffffc010cc30 t preemption_callback
    {0xffffffffc010cc30ull, "preemption_callback"},
// ffffffffc010cc50 T queue_thread
    {0xffffffffc010cc50ull, "queue_thread"},
// ffffffffc010cd10 T sched_tick
    {0xffffffffc010cd10ull, "sched_tick"},
// ffffffffc010cf70 t yield_interrupt
    {0xffffffffc010cf70ull, "yield_interrupt"},
// ffffffffc010cfe0 t idle_thread_func
    {0xffffffffc010cfe0ull, "idle_thread_func"},
// ffffffffc010d010 t sched_lock
    {0xffffffffc010d010ull, "sched_lock"},
// ffffffffc010d050 t sched_unlock
    {0xffffffffc010d050ull, "sched_unlock"},
// ffffffffc010d070 T spawn_thread
    {0xffffffffc010d070ull, "spawn_thread"},
// ffffffffc010d350 T pcpu_init_for_cpu
    {0xffffffffc010d350ull, "pcpu_init_for_cpu"},
// ffffffffc010d380 T pcpu_init_for_bsp
    {0xffffffffc010d380ull, "pcpu_init_for_bsp"},
// ffffffffc010d3a0 T get_pcpu_base
    {0xffffffffc010d3a0ull, "get_pcpu_base"},
// ffffffffc010d3b0 T spinlock_acquire_high_tpl
    {0xffffffffc010d3b0ull, "spinlock_acquire_high_tpl"},
// ffffffffc010d420 T spinlock_acquire
    {0xffffffffc010d420ull, "spinlock_acquire"},
// ffffffffc010d470 T spinlock_release
    {0xffffffffc010d470ull, "spinlock_release"},
// ffffffffc010d4b0 T strerror
    {0xffffffffc010d4b0ull, "strerror"},
// ffffffffc010d500 T is_node_in_list
    {0xffffffffc010d500ull, "is_node_in_list"},
// ffffffffc010d600 t is_list_valid
    {0xffffffffc010d600ull, "is_list_valid"},
// ffffffffc010d710 T initialize_list_had
    {0xffffffffc010d710ull, "initialize_list_had"},
// ffffffffc010d790 T insert_head_list
    {0xffffffffc010d790ull, "insert_head_list"},
// ffffffffc010d830 T insert_tail_list
    {0xffffffffc010d830ull, "insert_tail_list"},
// ffffffffc010d8d0 T get_first_node
    {0xffffffffc010d8d0ull, "get_first_node"},
// ffffffffc010d940 T get_next_node
    {0xffffffffc010d940ull, "get_next_node"},
// ffffffffc010d9b0 T get_prev_node
    {0xffffffffc010d9b0ull, "get_prev_node"},
// ffffffffc010da20 T is_list_empty
    {0xffffffffc010da20ull, "is_list_empty"},
// ffffffffc010daa0 T is_null
    {0xffffffffc010daa0ull, "is_null"},
// ffffffffc010dad0 T is_node_at_end
    {0xffffffffc010dad0ull, "is_node_at_end"},
// ffffffffc010db20 T swap_list_entries
    {0xffffffffc010db20ull, "swap_list_entries"},
// ffffffffc010dc40 T remove_entry_list
    {0xffffffffc010dc40ull, "remove_entry_list"},
// ffffffffc010dce0 T serial_init
    {0xffffffffc010dce0ull, "serial_init"},
// ffffffffc010dd60 T serial_write
    {0xffffffffc010dd60ull, "serial_write"},
// ffffffffc010de30 T serial_read
    {0xffffffffc010de30ull, "serial_read"},
// ffffffffc010deb0 T serial_poll
    {0xffffffffc010deb0ull, "serial_poll"},
// ffffffffc010dee0 T trace
    {0xffffffffc010dee0ull, "trace"},
// ffffffffc010e050 t common_stub
    {0xffffffffc010e050ull, "common_stub"},
// ffffffffc010e09d T handle_interrupt_request_00
    {0xffffffffc010e09dull, "handle_interrupt_request_00"},
// ffffffffc010e0a3 T handle_interrupt_request_01
    {0xffffffffc010e0a3ull, "handle_interrupt_request_01"},
// ffffffffc010e0a9 T handle_interrupt_request_02
    {0xffffffffc010e0a9ull, "handle_interrupt_request_02"},
// ffffffffc010e0af T handle_interrupt_request_03
    {0xffffffffc010e0afull, "handle_interrupt_request_03"},
// ffffffffc010e0b5 T handle_interrupt_request_04
    {0xffffffffc010e0b5ull, "handle_interrupt_request_04"},
// ffffffffc010e0bb T handle_interrupt_request_05
    {0xffffffffc010e0bbull, "handle_interrupt_request_05"},
// ffffffffc010e0c1 T handle_interrupt_request_06
    {0xffffffffc010e0c1ull, "handle_interrupt_request_06"},
// ffffffffc010e0c7 T handle_interrupt_request_07
    {0xffffffffc010e0c7ull, "handle_interrupt_request_07"},
// ffffffffc010e0cd T handle_interrupt_request_08
    {0xffffffffc010e0cdull, "handle_interrupt_request_08"},
// ffffffffc010e0d4 T handle_interrupt_request_09
    {0xffffffffc010e0d4ull, "handle_interrupt_request_09"},
// ffffffffc010e0dd T handle_interrupt_request_0a
    {0xffffffffc010e0ddull, "handle_interrupt_request_0a"},
// ffffffffc010e0e4 T handle_interrupt_request_0b
    {0xffffffffc010e0e4ull, "handle_interrupt_request_0b"},
// ffffffffc010e0eb T handle_interrupt_request_0c
    {0xffffffffc010e0ebull, "handle_interrupt_request_0c"},
// ffffffffc010e0f2 T handle_interrupt_request_0d
    {0xffffffffc010e0f2ull, "handle_interrupt_request_0d"},
// ffffffffc010e0f9 T handle_interrupt_request_0e
    {0xffffffffc010e0f9ull, "handle_interrupt_request_0e"},
// ffffffffc010e100 T handle_interrupt_request_0f
    {0xffffffffc010e100ull, "handle_interrupt_request_0f"},
// ffffffffc010e109 T handle_interrupt_request_10
    {0xffffffffc010e109ull, "handle_interrupt_request_10"},
// ffffffffc010e110 T handle_interrupt_request_11
    {0xffffffffc010e110ull, "handle_interrupt_request_11"},
// ffffffffc010e119 T handle_interrupt_request_12
    {0xffffffffc010e119ull, "handle_interrupt_request_12"},
// ffffffffc010e122 T handle_interrupt_request_13
    {0xffffffffc010e122ull, "handle_interrupt_request_13"},
// ffffffffc010e12b T handle_interrupt_request_14
    {0xffffffffc010e12bull, "handle_interrupt_request_14"},
// ffffffffc010e134 T handle_interrupt_request_15
    {0xffffffffc010e134ull, "handle_interrupt_request_15"},
// ffffffffc010e13d T handle_interrupt_request_16
    {0xffffffffc010e13dull, "handle_interrupt_request_16"},
// ffffffffc010e146 T handle_interrupt_request_17
    {0xffffffffc010e146ull, "handle_interrupt_request_17"},
// ffffffffc010e14f T handle_interrupt_request_18
    {0xffffffffc010e14full, "handle_interrupt_request_18"},
// ffffffffc010e158 T handle_interrupt_request_19
    {0xffffffffc010e158ull, "handle_interrupt_request_19"},
// ffffffffc010e161 T handle_interrupt_request_1a
    {0xffffffffc010e161ull, "handle_interrupt_request_1a"},
// ffffffffc010e16a T handle_interrupt_request_1b
    {0xffffffffc010e16aull, "handle_interrupt_request_1b"},
// ffffffffc010e173 T handle_interrupt_request_1c
    {0xffffffffc010e173ull, "handle_interrupt_request_1c"},
// ffffffffc010e17c T handle_interrupt_request_1d
    {0xffffffffc010e17cull, "handle_interrupt_request_1d"},
// ffffffffc010e185 T handle_interrupt_request_1e
    {0xffffffffc010e185ull, "handle_interrupt_request_1e"},
// ffffffffc010e18c T handle_interrupt_request_1f
    {0xffffffffc010e18cull, "handle_interrupt_request_1f"},
// ffffffffc010e195 T handle_interrupt_request_20
    {0xffffffffc010e195ull, "handle_interrupt_request_20"},
// ffffffffc010e19e T handle_interrupt_request_21
    {0xffffffffc010e19eull, "handle_interrupt_request_21"},
// ffffffffc010e1a7 T handle_interrupt_request_22
    {0xffffffffc010e1a7ull, "handle_interrupt_request_22"},
// ffffffffc010e1b0 T handle_interrupt_request_23
    {0xffffffffc010e1b0ull, "handle_interrupt_request_23"},
// ffffffffc010e1b9 T handle_interrupt_request_24
    {0xffffffffc010e1b9ull, "handle_interrupt_request_24"},
// ffffffffc010e1c2 T handle_interrupt_request_25
    {0xffffffffc010e1c2ull, "handle_interrupt_request_25"},
// ffffffffc010e1cb T handle_interrupt_request_26
    {0xffffffffc010e1cbull, "handle_interrupt_request_26"},
// ffffffffc010e1d4 T handle_interrupt_request_27
    {0xffffffffc010e1d4ull, "handle_interrupt_request_27"},
// ffffffffc010e1dd T handle_interrupt_request_28
    {0xffffffffc010e1ddull, "handle_interrupt_request_28"},
// ffffffffc010e1e6 T handle_interrupt_request_29
    {0xffffffffc010e1e6ull, "handle_interrupt_request_29"},
// ffffffffc010e1ef T handle_interrupt_request_2a
    {0xffffffffc010e1efull, "handle_interrupt_request_2a"},
// ffffffffc010e1f8 T handle_interrupt_request_2b
    {0xffffffffc010e1f8ull, "handle_interrupt_request_2b"},
// ffffffffc010e201 T handle_interrupt_request_2c
    {0xffffffffc010e201ull, "handle_interrupt_request_2c"},
// ffffffffc010e20a T handle_interrupt_request_2d
    {0xffffffffc010e20aull, "handle_interrupt_request_2d"},
// ffffffffc010e213 T handle_interrupt_request_2e
    {0xffffffffc010e213ull, "handle_interrupt_request_2e"},
// ffffffffc010e21c T handle_interrupt_request_2f
    {0xffffffffc010e21cull, "handle_interrupt_request_2f"},
// ffffffffc010e225 T handle_interrupt_request_30
    {0xffffffffc010e225ull, "handle_interrupt_request_30"},
// ffffffffc010e22e T handle_interrupt_request_31
    {0xffffffffc010e22eull, "handle_interrupt_request_31"},
// ffffffffc010e237 T handle_interrupt_request_32
    {0xffffffffc010e237ull, "handle_interrupt_request_32"},
// ffffffffc010e240 T handle_interrupt_request_33
    {0xffffffffc010e240ull, "handle_interrupt_request_33"},
// ffffffffc010e249 T handle_interrupt_request_34
    {0xffffffffc010e249ull, "handle_interrupt_request_34"},
// ffffffffc010e252 T handle_interrupt_request_35
    {0xffffffffc010e252ull, "handle_interrupt_request_35"},
// ffffffffc010e25b T handle_interrupt_request_36
    {0xffffffffc010e25bull, "handle_interrupt_request_36"},
// ffffffffc010e264 T handle_interrupt_request_37
    {0xffffffffc010e264ull, "handle_interrupt_request_37"},
// ffffffffc010e26d T handle_interrupt_request_38
    {0xffffffffc010e26dull, "handle_interrupt_request_38"},
// ffffffffc010e276 T handle_interrupt_request_39
    {0xffffffffc010e276ull, "handle_interrupt_request_39"},
// ffffffffc010e27f T handle_interrupt_request_3a
    {0xffffffffc010e27full, "handle_interrupt_request_3a"},
// ffffffffc010e288 T handle_interrupt_request_3b
    {0xffffffffc010e288ull, "handle_interrupt_request_3b"},
// ffffffffc010e291 T handle_interrupt_request_3c
    {0xffffffffc010e291ull, "handle_interrupt_request_3c"},
// ffffffffc010e29a T handle_interrupt_request_3d
    {0xffffffffc010e29aull, "handle_interrupt_request_3d"},
// ffffffffc010e2a3 T handle_interrupt_request_3e
    {0xffffffffc010e2a3ull, "handle_interrupt_request_3e"},
// ffffffffc010e2ac T handle_interrupt_request_3f
    {0xffffffffc010e2acull, "handle_interrupt_request_3f"},
// ffffffffc010e2b5 T handle_interrupt_request_40
    {0xffffffffc010e2b5ull, "handle_interrupt_request_40"},
// ffffffffc010e2be T handle_interrupt_request_41
    {0xffffffffc010e2beull, "handle_interrupt_request_41"},
// ffffffffc010e2c7 T handle_interrupt_request_42
    {0xffffffffc010e2c7ull, "handle_interrupt_request_42"},
// ffffffffc010e2d0 T handle_interrupt_request_43
    {0xffffffffc010e2d0ull, "handle_interrupt_request_43"},
// ffffffffc010e2d9 T handle_interrupt_request_44
    {0xffffffffc010e2d9ull, "handle_interrupt_request_44"},
// ffffffffc010e2e2 T handle_interrupt_request_45
    {0xffffffffc010e2e2ull, "handle_interrupt_request_45"},
// ffffffffc010e2eb T handle_interrupt_request_46
    {0xffffffffc010e2ebull, "handle_interrupt_request_46"},
// ffffffffc010e2f4 T handle_interrupt_request_47
    {0xffffffffc010e2f4ull, "handle_interrupt_request_47"},
// ffffffffc010e2fd T handle_interrupt_request_48
    {0xffffffffc010e2fdull, "handle_interrupt_request_48"},
// ffffffffc010e306 T handle_interrupt_request_49
    {0xffffffffc010e306ull, "handle_interrupt_request_49"},
// ffffffffc010e30f T handle_interrupt_request_4a
    {0xffffffffc010e30full, "handle_interrupt_request_4a"},
// ffffffffc010e318 T handle_interrupt_request_4b
    {0xffffffffc010e318ull, "handle_interrupt_request_4b"},
// ffffffffc010e321 T handle_interrupt_request_4c
    {0xffffffffc010e321ull, "handle_interrupt_request_4c"},
// ffffffffc010e32a T handle_interrupt_request_4d
    {0xffffffffc010e32aull, "handle_interrupt_request_4d"},
// ffffffffc010e333 T handle_interrupt_request_4e
    {0xffffffffc010e333ull, "handle_interrupt_request_4e"},
// ffffffffc010e33c T handle_interrupt_request_4f
    {0xffffffffc010e33cull, "handle_interrupt_request_4f"},
// ffffffffc010e345 T handle_interrupt_request_50
    {0xffffffffc010e345ull, "handle_interrupt_request_50"},
// ffffffffc010e34e T handle_interrupt_request_51
    {0xffffffffc010e34eull, "handle_interrupt_request_51"},
// ffffffffc010e357 T handle_interrupt_request_52
    {0xffffffffc010e357ull, "handle_interrupt_request_52"},
// ffffffffc010e360 T handle_interrupt_request_53
    {0xffffffffc010e360ull, "handle_interrupt_request_53"},
// ffffffffc010e369 T handle_interrupt_request_54
    {0xffffffffc010e369ull, "handle_interrupt_request_54"},
// ffffffffc010e372 T handle_interrupt_request_55
    {0xffffffffc010e372ull, "handle_interrupt_request_55"},
// ffffffffc010e37b T handle_interrupt_request_56
    {0xffffffffc010e37bull, "handle_interrupt_request_56"},
// ffffffffc010e384 T handle_interrupt_request_57
    {0xffffffffc010e384ull, "handle_interrupt_request_57"},
// ffffffffc010e38d T handle_interrupt_request_58
    {0xffffffffc010e38dull, "handle_interrupt_request_58"},
// ffffffffc010e396 T handle_interrupt_request_59
    {0xffffffffc010e396ull, "handle_interrupt_request_59"},
// ffffffffc010e39f T handle_interrupt_request_5a
    {0xffffffffc010e39full, "handle_interrupt_request_5a"},
// ffffffffc010e3a8 T handle_interrupt_request_5b
    {0xffffffffc010e3a8ull, "handle_interrupt_request_5b"},
// ffffffffc010e3b1 T handle_interrupt_request_5c
    {0xffffffffc010e3b1ull, "handle_interrupt_request_5c"},
// ffffffffc010e3ba T handle_interrupt_request_5d
    {0xffffffffc010e3baull, "handle_interrupt_request_5d"},
// ffffffffc010e3c3 T handle_interrupt_request_5e
    {0xffffffffc010e3c3ull, "handle_interrupt_request_5e"},
// ffffffffc010e3cc T handle_interrupt_request_5f
    {0xffffffffc010e3ccull, "handle_interrupt_request_5f"},
// ffffffffc010e3d5 T handle_interrupt_request_60
    {0xffffffffc010e3d5ull, "handle_interrupt_request_60"},
// ffffffffc010e3de T handle_interrupt_request_61
    {0xffffffffc010e3deull, "handle_interrupt_request_61"},
// ffffffffc010e3e7 T handle_interrupt_request_62
    {0xffffffffc010e3e7ull, "handle_interrupt_request_62"},
// ffffffffc010e3f0 T handle_interrupt_request_63
    {0xffffffffc010e3f0ull, "handle_interrupt_request_63"},
// ffffffffc010e3f9 T handle_interrupt_request_64
    {0xffffffffc010e3f9ull, "handle_interrupt_request_64"},
// ffffffffc010e402 T handle_interrupt_request_65
    {0xffffffffc010e402ull, "handle_interrupt_request_65"},
// ffffffffc010e40b T handle_interrupt_request_66
    {0xffffffffc010e40bull, "handle_interrupt_request_66"},
// ffffffffc010e414 T handle_interrupt_request_67
    {0xffffffffc010e414ull, "handle_interrupt_request_67"},
// ffffffffc010e41d T handle_interrupt_request_68
    {0xffffffffc010e41dull, "handle_interrupt_request_68"},
// ffffffffc010e426 T handle_interrupt_request_69
    {0xffffffffc010e426ull, "handle_interrupt_request_69"},
// ffffffffc010e42f T handle_interrupt_request_6a
    {0xffffffffc010e42full, "handle_interrupt_request_6a"},
// ffffffffc010e438 T handle_interrupt_request_6b
    {0xffffffffc010e438ull, "handle_interrupt_request_6b"},
// ffffffffc010e441 T handle_interrupt_request_6c
    {0xffffffffc010e441ull, "handle_interrupt_request_6c"},
// ffffffffc010e44a T handle_interrupt_request_6d
    {0xffffffffc010e44aull, "handle_interrupt_request_6d"},
// ffffffffc010e453 T handle_interrupt_request_6e
    {0xffffffffc010e453ull, "handle_interrupt_request_6e"},
// ffffffffc010e45c T handle_interrupt_request_6f
    {0xffffffffc010e45cull, "handle_interrupt_request_6f"},
// ffffffffc010e465 T handle_interrupt_request_70
    {0xffffffffc010e465ull, "handle_interrupt_request_70"},
// ffffffffc010e46e T handle_interrupt_request_71
    {0xffffffffc010e46eull, "handle_interrupt_request_71"},
// ffffffffc010e477 T handle_interrupt_request_72
    {0xffffffffc010e477ull, "handle_interrupt_request_72"},
// ffffffffc010e480 T handle_interrupt_request_73
    {0xffffffffc010e480ull, "handle_interrupt_request_73"},
// ffffffffc010e489 T handle_interrupt_request_74
    {0xffffffffc010e489ull, "handle_interrupt_request_74"},
// ffffffffc010e492 T handle_interrupt_request_75
    {0xffffffffc010e492ull, "handle_interrupt_request_75"},
// ffffffffc010e49b T handle_interrupt_request_76
    {0xffffffffc010e49bull, "handle_interrupt_request_76"},
// ffffffffc010e4a4 T handle_interrupt_request_77
    {0xffffffffc010e4a4ull, "handle_interrupt_request_77"},
// ffffffffc010e4ad T handle_interrupt_request_78
    {0xffffffffc010e4adull, "handle_interrupt_request_78"},
// ffffffffc010e4b6 T handle_interrupt_request_79
    {0xffffffffc010e4b6ull, "handle_interrupt_request_79"},
// ffffffffc010e4bf T handle_interrupt_request_7a
    {0xffffffffc010e4bfull, "handle_interrupt_request_7a"},
// ffffffffc010e4c8 T handle_interrupt_request_7b
    {0xffffffffc010e4c8ull, "handle_interrupt_request_7b"},
// ffffffffc010e4d1 T handle_interrupt_request_7c
    {0xffffffffc010e4d1ull, "handle_interrupt_request_7c"},
// ffffffffc010e4da T handle_interrupt_request_7d
    {0xffffffffc010e4daull, "handle_interrupt_request_7d"},
// ffffffffc010e4e3 T handle_interrupt_request_7e
    {0xffffffffc010e4e3ull, "handle_interrupt_request_7e"},
// ffffffffc010e4ec T handle_interrupt_request_7f
    {0xffffffffc010e4ecull, "handle_interrupt_request_7f"},
// ffffffffc010e4f5 T handle_interrupt_request_80
    {0xffffffffc010e4f5ull, "handle_interrupt_request_80"},
// ffffffffc010e501 T handle_interrupt_request_81
    {0xffffffffc010e501ull, "handle_interrupt_request_81"},
// ffffffffc010e50d T handle_interrupt_request_82
    {0xffffffffc010e50dull, "handle_interrupt_request_82"},
// ffffffffc010e519 T handle_interrupt_request_83
    {0xffffffffc010e519ull, "handle_interrupt_request_83"},
// ffffffffc010e525 T handle_interrupt_request_84
    {0xffffffffc010e525ull, "handle_interrupt_request_84"},
// ffffffffc010e531 T handle_interrupt_request_85
    {0xffffffffc010e531ull, "handle_interrupt_request_85"},
// ffffffffc010e53d T handle_interrupt_request_86
    {0xffffffffc010e53dull, "handle_interrupt_request_86"},
// ffffffffc010e549 T handle_interrupt_request_87
    {0xffffffffc010e549ull, "handle_interrupt_request_87"},
// ffffffffc010e555 T handle_interrupt_request_88
    {0xffffffffc010e555ull, "handle_interrupt_request_88"},
// ffffffffc010e561 T handle_interrupt_request_89
    {0xffffffffc010e561ull, "handle_interrupt_request_89"},
// ffffffffc010e56d T handle_interrupt_request_8a
    {0xffffffffc010e56dull, "handle_interrupt_request_8a"},
// ffffffffc010e579 T handle_interrupt_request_8b
    {0xffffffffc010e579ull, "handle_interrupt_request_8b"},
// ffffffffc010e585 T handle_interrupt_request_8c
    {0xffffffffc010e585ull, "handle_interrupt_request_8c"},
// ffffffffc010e591 T handle_interrupt_request_8d
    {0xffffffffc010e591ull, "handle_interrupt_request_8d"},
// ffffffffc010e59d T handle_interrupt_request_8e
    {0xffffffffc010e59dull, "handle_interrupt_request_8e"},
// ffffffffc010e5a9 T handle_interrupt_request_8f
    {0xffffffffc010e5a9ull, "handle_interrupt_request_8f"},
// ffffffffc010e5b5 T handle_interrupt_request_90
    {0xffffffffc010e5b5ull, "handle_interrupt_request_90"},
// ffffffffc010e5c1 T handle_interrupt_request_91
    {0xffffffffc010e5c1ull, "handle_interrupt_request_91"},
// ffffffffc010e5cd T handle_interrupt_request_92
    {0xffffffffc010e5cdull, "handle_interrupt_request_92"},
// ffffffffc010e5d9 T handle_interrupt_request_93
    {0xffffffffc010e5d9ull, "handle_interrupt_request_93"},
// ffffffffc010e5e5 T handle_interrupt_request_94
    {0xffffffffc010e5e5ull, "handle_interrupt_request_94"},
// ffffffffc010e5f1 T handle_interrupt_request_95
    {0xffffffffc010e5f1ull, "handle_interrupt_request_95"},
// ffffffffc010e5fd T handle_interrupt_request_96
    {0xffffffffc010e5fdull, "handle_interrupt_request_96"},
// ffffffffc010e609 T handle_interrupt_request_97
    {0xffffffffc010e609ull, "handle_interrupt_request_97"},
// ffffffffc010e615 T handle_interrupt_request_98
    {0xffffffffc010e615ull, "handle_interrupt_request_98"},
// ffffffffc010e621 T handle_interrupt_request_99
    {0xffffffffc010e621ull, "handle_interrupt_request_99"},
// ffffffffc010e62d T handle_interrupt_request_9a
    {0xffffffffc010e62dull, "handle_interrupt_request_9a"},
// ffffffffc010e639 T handle_interrupt_request_9b
    {0xffffffffc010e639ull, "handle_interrupt_request_9b"},
// ffffffffc010e645 T handle_interrupt_request_9c
    {0xffffffffc010e645ull, "handle_interrupt_request_9c"},
// ffffffffc010e651 T handle_interrupt_request_9d
    {0xffffffffc010e651ull, "handle_interrupt_request_9d"},
// ffffffffc010e65d T handle_interrupt_request_9e
    {0xffffffffc010e65dull, "handle_interrupt_request_9e"},
// ffffffffc010e669 T handle_interrupt_request_9f
    {0xffffffffc010e669ull, "handle_interrupt_request_9f"},
// ffffffffc010e675 T handle_interrupt_request_a0
    {0xffffffffc010e675ull, "handle_interrupt_request_a0"},
// ffffffffc010e681 T handle_interrupt_request_a1
    {0xffffffffc010e681ull, "handle_interrupt_request_a1"},
// ffffffffc010e68d T handle_interrupt_request_a2
    {0xffffffffc010e68dull, "handle_interrupt_request_a2"},
// ffffffffc010e699 T handle_interrupt_request_a3
    {0xffffffffc010e699ull, "handle_interrupt_request_a3"},
// ffffffffc010e6a5 T handle_interrupt_request_a4
    {0xffffffffc010e6a5ull, "handle_interrupt_request_a4"},
// ffffffffc010e6b1 T handle_interrupt_request_a5
    {0xffffffffc010e6b1ull, "handle_interrupt_request_a5"},
// ffffffffc010e6bd T handle_interrupt_request_a6
    {0xffffffffc010e6bdull, "handle_interrupt_request_a6"},
// ffffffffc010e6c9 T handle_interrupt_request_a7
    {0xffffffffc010e6c9ull, "handle_interrupt_request_a7"},
// ffffffffc010e6d5 T handle_interrupt_request_a8
    {0xffffffffc010e6d5ull, "handle_interrupt_request_a8"},
// ffffffffc010e6e1 T handle_interrupt_request_a9
    {0xffffffffc010e6e1ull, "handle_interrupt_request_a9"},
// ffffffffc010e6ed T handle_interrupt_request_aa
    {0xffffffffc010e6edull, "handle_interrupt_request_aa"},
// ffffffffc010e6f9 T handle_interrupt_request_ab
    {0xffffffffc010e6f9ull, "handle_interrupt_request_ab"},
// ffffffffc010e705 T handle_interrupt_request_ac
    {0xffffffffc010e705ull, "handle_interrupt_request_ac"},
// ffffffffc010e711 T handle_interrupt_request_ad
    {0xffffffffc010e711ull, "handle_interrupt_request_ad"},
// ffffffffc010e71d T handle_interrupt_request_ae
    {0xffffffffc010e71dull, "handle_interrupt_request_ae"},
// ffffffffc010e729 T handle_interrupt_request_af
    {0xffffffffc010e729ull, "handle_interrupt_request_af"},
// ffffffffc010e735 T handle_interrupt_request_b0
    {0xffffffffc010e735ull, "handle_interrupt_request_b0"},
// ffffffffc010e741 T handle_interrupt_request_b1
    {0xffffffffc010e741ull, "handle_interrupt_request_b1"},
// ffffffffc010e74d T handle_interrupt_request_b2
    {0xffffffffc010e74dull, "handle_interrupt_request_b2"},
// ffffffffc010e759 T handle_interrupt_request_b3
    {0xffffffffc010e759ull, "handle_interrupt_request_b3"},
// ffffffffc010e765 T handle_interrupt_request_b4
    {0xffffffffc010e765ull, "handle_interrupt_request_b4"},
// ffffffffc010e771 T handle_interrupt_request_b5
    {0xffffffffc010e771ull, "handle_interrupt_request_b5"},
// ffffffffc010e77d T handle_interrupt_request_b6
    {0xffffffffc010e77dull, "handle_interrupt_request_b6"},
// ffffffffc010e789 T handle_interrupt_request_b7
    {0xffffffffc010e789ull, "handle_interrupt_request_b7"},
// ffffffffc010e795 T handle_interrupt_request_b8
    {0xffffffffc010e795ull, "handle_interrupt_request_b8"},
// ffffffffc010e7a1 T handle_interrupt_request_b9
    {0xffffffffc010e7a1ull, "handle_interrupt_request_b9"},
// ffffffffc010e7ad T handle_interrupt_request_ba
    {0xffffffffc010e7adull, "handle_interrupt_request_ba"},
// ffffffffc010e7b9 T handle_interrupt_request_bb
    {0xffffffffc010e7b9ull, "handle_interrupt_request_bb"},
// ffffffffc010e7c5 T handle_interrupt_request_bc
    {0xffffffffc010e7c5ull, "handle_interrupt_request_bc"},
// ffffffffc010e7d1 T handle_interrupt_request_bd
    {0xffffffffc010e7d1ull, "handle_interrupt_request_bd"},
// ffffffffc010e7dd T handle_interrupt_request_be
    {0xffffffffc010e7ddull, "handle_interrupt_request_be"},
// ffffffffc010e7e9 T handle_interrupt_request_bf
    {0xffffffffc010e7e9ull, "handle_interrupt_request_bf"},
// ffffffffc010e7f5 T handle_interrupt_request_c0
    {0xffffffffc010e7f5ull, "handle_interrupt_request_c0"},
// ffffffffc010e801 T handle_interrupt_request_c1
    {0xffffffffc010e801ull, "handle_interrupt_request_c1"},
// ffffffffc010e80d T handle_interrupt_request_c2
    {0xffffffffc010e80dull, "handle_interrupt_request_c2"},
// ffffffffc010e819 T handle_interrupt_request_c3
    {0xffffffffc010e819ull, "handle_interrupt_request_c3"},
// ffffffffc010e825 T handle_interrupt_request_c4
    {0xffffffffc010e825ull, "handle_interrupt_request_c4"},
// ffffffffc010e831 T handle_interrupt_request_c5
    {0xffffffffc010e831ull, "handle_interrupt_request_c5"},
// ffffffffc010e83d T handle_interrupt_request_c6
    {0xffffffffc010e83dull, "handle_interrupt_request_c6"},
// ffffffffc010e849 T handle_interrupt_request_c7
    {0xffffffffc010e849ull, "handle_interrupt_request_c7"},
// ffffffffc010e855 T handle_interrupt_request_c8
    {0xffffffffc010e855ull, "handle_interrupt_request_c8"},
// ffffffffc010e861 T handle_interrupt_request_c9
    {0xffffffffc010e861ull, "handle_interrupt_request_c9"},
// ffffffffc010e86d T handle_interrupt_request_ca
    {0xffffffffc010e86dull, "handle_interrupt_request_ca"},
// ffffffffc010e879 T handle_interrupt_request_cb
    {0xffffffffc010e879ull, "handle_interrupt_request_cb"},
// ffffffffc010e885 T handle_interrupt_request_cc
    {0xffffffffc010e885ull, "handle_interrupt_request_cc"},
// ffffffffc010e891 T handle_interrupt_request_cd
    {0xffffffffc010e891ull, "handle_interrupt_request_cd"},
// ffffffffc010e89d T handle_interrupt_request_ce
    {0xffffffffc010e89dull, "handle_interrupt_request_ce"},
// ffffffffc010e8a9 T handle_interrupt_request_cf
    {0xffffffffc010e8a9ull, "handle_interrupt_request_cf"},
// ffffffffc010e8b5 T handle_interrupt_request_d0
    {0xffffffffc010e8b5ull, "handle_interrupt_request_d0"},
// ffffffffc010e8c1 T handle_interrupt_request_d1
    {0xffffffffc010e8c1ull, "handle_interrupt_request_d1"},
// ffffffffc010e8cd T handle_interrupt_request_d2
    {0xffffffffc010e8cdull, "handle_interrupt_request_d2"},
// ffffffffc010e8d9 T handle_interrupt_request_d3
    {0xffffffffc010e8d9ull, "handle_interrupt_request_d3"},
// ffffffffc010e8e5 T handle_interrupt_request_d4
    {0xffffffffc010e8e5ull, "handle_interrupt_request_d4"},
// ffffffffc010e8f1 T handle_interrupt_request_d5
    {0xffffffffc010e8f1ull, "handle_interrupt_request_d5"},
// ffffffffc010e8fd T handle_interrupt_request_d6
    {0xffffffffc010e8fdull, "handle_interrupt_request_d6"},
// ffffffffc010e909 T handle_interrupt_request_d7
    {0xffffffffc010e909ull, "handle_interrupt_request_d7"},
// ffffffffc010e915 T handle_interrupt_request_d8
    {0xffffffffc010e915ull, "handle_interrupt_request_d8"},
// ffffffffc010e921 T handle_interrupt_request_d9
    {0xffffffffc010e921ull, "handle_interrupt_request_d9"},
// ffffffffc010e92d T handle_interrupt_request_da
    {0xffffffffc010e92dull, "handle_interrupt_request_da"},
// ffffffffc010e939 T handle_interrupt_request_db
    {0xffffffffc010e939ull, "handle_interrupt_request_db"},
// ffffffffc010e945 T handle_interrupt_request_dc
    {0xffffffffc010e945ull, "handle_interrupt_request_dc"},
// ffffffffc010e951 T handle_interrupt_request_dd
    {0xffffffffc010e951ull, "handle_interrupt_request_dd"},
// ffffffffc010e95d T handle_interrupt_request_de
    {0xffffffffc010e95dull, "handle_interrupt_request_de"},
// ffffffffc010e969 T handle_interrupt_request_df
    {0xffffffffc010e969ull, "handle_interrupt_request_df"},
// ffffffffc010e975 T handle_interrupt_request_e0
    {0xffffffffc010e975ull, "handle_interrupt_request_e0"},
// ffffffffc010e981 T handle_interrupt_request_e1
    {0xffffffffc010e981ull, "handle_interrupt_request_e1"},
// ffffffffc010e98d T handle_interrupt_request_e2
    {0xffffffffc010e98dull, "handle_interrupt_request_e2"},
// ffffffffc010e999 T handle_interrupt_request_e3
    {0xffffffffc010e999ull, "handle_interrupt_request_e3"},
// ffffffffc010e9a5 T handle_interrupt_request_e4
    {0xffffffffc010e9a5ull, "handle_interrupt_request_e4"},
// ffffffffc010e9b1 T handle_interrupt_request_e5
    {0xffffffffc010e9b1ull, "handle_interrupt_request_e5"},
// ffffffffc010e9bd T handle_interrupt_request_e6
    {0xffffffffc010e9bdull, "handle_interrupt_request_e6"},
// ffffffffc010e9c9 T handle_interrupt_request_e7
    {0xffffffffc010e9c9ull, "handle_interrupt_request_e7"},
// ffffffffc010e9d5 T handle_interrupt_request_e8
    {0xffffffffc010e9d5ull, "handle_interrupt_request_e8"},
// ffffffffc010e9e1 T handle_interrupt_request_e9
    {0xffffffffc010e9e1ull, "handle_interrupt_request_e9"},
// ffffffffc010e9ed T handle_interrupt_request_ea
    {0xffffffffc010e9edull, "handle_interrupt_request_ea"},
// ffffffffc010e9f9 T handle_interrupt_request_eb
    {0xffffffffc010e9f9ull, "handle_interrupt_request_eb"},
// ffffffffc010ea05 T handle_interrupt_request_ec
    {0xffffffffc010ea05ull, "handle_interrupt_request_ec"},
// ffffffffc010ea11 T handle_interrupt_request_ed
    {0xffffffffc010ea11ull, "handle_interrupt_request_ed"},
// ffffffffc010ea1d T handle_interrupt_request_ee
    {0xffffffffc010ea1dull, "handle_interrupt_request_ee"},
// ffffffffc010ea29 T handle_interrupt_request_ef
    {0xffffffffc010ea29ull, "handle_interrupt_request_ef"},
// ffffffffc010ea35 T handle_interrupt_request_f0
    {0xffffffffc010ea35ull, "handle_interrupt_request_f0"},
// ffffffffc010ea41 T handle_interrupt_request_f1
    {0xffffffffc010ea41ull, "handle_interrupt_request_f1"},
// ffffffffc010ea4d T handle_interrupt_request_f2
    {0xffffffffc010ea4dull, "handle_interrupt_request_f2"},
// ffffffffc010ea59 T handle_interrupt_request_f3
    {0xffffffffc010ea59ull, "handle_interrupt_request_f3"},
// ffffffffc010ea65 T handle_interrupt_request_f4
    {0xffffffffc010ea65ull, "handle_interrupt_request_f4"},
// ffffffffc010ea71 T handle_interrupt_request_f5
    {0xffffffffc010ea71ull, "handle_interrupt_request_f5"},
// ffffffffc010ea7d T handle_interrupt_request_f6
    {0xffffffffc010ea7dull, "handle_interrupt_request_f6"},
// ffffffffc010ea89 T handle_interrupt_request_f7
    {0xffffffffc010ea89ull, "handle_interrupt_request_f7"},
// ffffffffc010ea95 T handle_interrupt_request_f8
    {0xffffffffc010ea95ull, "handle_interrupt_request_f8"},
// ffffffffc010eaa1 T handle_interrupt_request_f9
    {0xffffffffc010eaa1ull, "handle_interrupt_request_f9"},
// ffffffffc010eaad T handle_interrupt_request_fa
    {0xffffffffc010eaadull, "handle_interrupt_request_fa"},
// ffffffffc010eab9 T handle_interrupt_request_fb
    {0xffffffffc010eab9ull, "handle_interrupt_request_fb"},
// ffffffffc010eac5 T handle_interrupt_request_fc
    {0xffffffffc010eac5ull, "handle_interrupt_request_fc"},
// ffffffffc010ead1 T handle_interrupt_request_fd
    {0xffffffffc010ead1ull, "handle_interrupt_request_fd"},
// ffffffffc010eadd T handle_interrupt_request_fe
    {0xffffffffc010eaddull, "handle_interrupt_request_fe"},
// ffffffffc010eae9 T handle_interrupt_request_ff
    {0xffffffffc010eae9ull, "handle_interrupt_request_ff"},
// ffffffffc010eb00 t fix_data_segment
    {0xffffffffc010eb00ull, "fix_data_segment"},
// ffffffffc010eb11 T gdt_fix_segments
    {0xffffffffc010eb11ull, "gdt_fix_segments"},
// ffffffffc010eb30 T memset
    {0xffffffffc010eb30ull, "memset"},
// ffffffffc010eb90 T memcmp
    {0xffffffffc010eb90ull, "memcmp"},
// ffffffffc010ec30 T memcpy
    {0xffffffffc010ec30ull, "memcpy"},
// ffffffffc010ecb0 T memmove
    {0xffffffffc010ecb0ull, "memmove"},
// ffffffffc010edb0 T strcmp
    {0xffffffffc010edb0ull, "strcmp"},
// ffffffffc010ee40 T strlen
    {0xffffffffc010ee40ull, "strlen"},
// ffffffffc010ee90 T stbsp_set_separators
    {0xffffffffc010ee90ull, "stbsp_set_separators"},
// ffffffffc010eec0 T stbsp_vsprintfcb
    {0xffffffffc010eec0ull, "stbsp_vsprintfcb"},
// ffffffffc01112b0 t stbsp__lead_sign
    {0xffffffffc01112b0ull, "stbsp__lead_sign"},
// ffffffffc0111340 T stbsp_sprintf
    {0xffffffffc0111340ull, "stbsp_sprintf"},
// ffffffffc01113b0 T stbsp_vsnprintf
    {0xffffffffc01113b0ull, "stbsp_vsnprintf"},
// ffffffffc01114f0 t stbsp__count_clamp_callback
    {0xffffffffc01114f0ull, "stbsp__count_clamp_callback"},
// ffffffffc0111530 t stbsp__clamp_callback
    {0xffffffffc0111530ull, "stbsp__clamp_callback"},
// ffffffffc0111670 T stbsp_snprintf
    {0xffffffffc0111670ull, "stbsp_snprintf"},
// ffffffffc01116d0 T stbsp_vsprintf
    {0xffffffffc01116d0ull, "stbsp_vsprintf"},
// ffffffffc0111720 T stbds_arrgrowf
    {0xffffffffc0111720ull, "stbds_arrgrowf"},
// ffffffffc0111920 T stbds_rand_seed
    {0xffffffffc0111920ull, "stbds_rand_seed"},
// ffffffffc0111940 T stbds_hash_string
    {0xffffffffc0111940ull, "stbds_hash_string"},
// ffffffffc0111a50 T stbds_hash_bytes
    {0xffffffffc0111a50ull, "stbds_hash_bytes"},
// ffffffffc0111ca0 t stbds_siphash_bytes
    {0xffffffffc0111ca0ull, "stbds_siphash_bytes"},
// ffffffffc0112250 T stbds_hmfree_func
    {0xffffffffc0112250ull, "stbds_hmfree_func"},
// ffffffffc0112320 T stbds_strreset
    {0xffffffffc0112320ull, "stbds_strreset"},
// ffffffffc0112390 T stbds_hmget_key_ts
    {0xffffffffc0112390ull, "stbds_hmget_key_ts"},
// ffffffffc0112500 t stbds_hm_find_slot
    {0xffffffffc0112500ull, "stbds_hm_find_slot"},
// ffffffffc01127b0 T stbds_hmget_key
    {0xffffffffc01127b0ull, "stbds_hmget_key"},
// ffffffffc0112810 T stbds_hmput_default
    {0xffffffffc0112810ull, "stbds_hmput_default"},
// ffffffffc01128f0 T stbds_hmput_key
    {0xffffffffc01128f0ull, "stbds_hmput_key"},
// ffffffffc0113040 t stbds_make_hash_index
    {0xffffffffc0113040ull, "stbds_make_hash_index"},
// ffffffffc01135d0 t stbds_probe_position
    {0xffffffffc01135d0ull, "stbds_probe_position"},
// ffffffffc0113610 t stbds_is_key_equal
    {0xffffffffc0113610ull, "stbds_is_key_equal"},
// ffffffffc01136b0 t stbds_strdup
    {0xffffffffc01136b0ull, "stbds_strdup"},
// ffffffffc0113710 T stbds_stralloc
    {0xffffffffc0113710ull, "stbds_stralloc"},
// ffffffffc01138e0 T stbds_shmode_func
    {0xffffffffc01138e0ull, "stbds_shmode_func"},
// ffffffffc0113980 T stbds_hmdel_key
    {0xffffffffc0113980ull, "stbds_hmdel_key"},
// ffffffffc0113ce0 t stbds_log2
    {0xffffffffc0113ce0ull, "stbds_log2"},
// ffffffffc0117000 d fs_components
// ffffffffc0117010 d all_components
// ffffffffc0117020 d type_to_name
// ffffffffc0117028 d component
// ffffffffc0117098 d early_current_tpl
// ffffffffc01170a0 d handler
// ffffffffc01170d0 D pit8254
// ffffffffc01170d8 d timers_list
// ffffffffc01170e8 d sv_handler
// ffffffffc0117118 D idt
// ffffffffc0117130 d ISR_NAMES
// ffffffffc01171e0 d PROCESSOR_NAME
// ffffffffc01171f0 d TABLE_NAME
// ffffffffc0117210 d PRESENT_NAMES
// ffffffffc0117220 d USER_NAME
// ffffffffc0117230 d OPERATION_NAME
// ffffffffc0117240 d gdt_entries
// ffffffffc0117278 D gdt
// ffffffffc0117290 d tboot_mmap_names
// ffffffffc01172b8 d mem_map
// ffffffffc01172c8 d free_entries_list
// ffffffffc01172d8 d free_blocks
// ffffffffc01172e8 d used_blocks
// ffffffffc01172f8 d stack_allocation
// ffffffffc0117300 D process_list
// ffffffffc0117310 D kernel_process
// ffffffffc0117350 d handler
// ffffffffc0117380 d threads_queue
// ffffffffc0117390 d errors
// ffffffffc01173d0 d stbsp__period
// ffffffffc01173d1 d stbsp__comma
// ffffffffc01173e0 d stbsp_vsprintfcb.hex
// ffffffffc0117400 d stbsp_vsprintfcb.hexu
// ffffffffc0117414 d stbsp__digitpair
// ffffffffc01174e0 d stbds_hash_seed
// ffffffffc01174f0 D symlist
// ffffffffc0117500 D bsp_pcpu_data
// ffffffffc0118000 B acpi_madt
// ffffffffc0118008 B acpi_rsdp
// ffffffffc0118010 B acpi_xsdp
// ffffffffc0118018 B acpi_rsdt
// ffffffffc0118020 B acpi_xsdt
// ffffffffc0118028 b components_lock
// ffffffffc0118030 b primary_components
// ffffffffc0118038 b initrd_files
// ffffffffc0118040 b lock
// ffffffffc0118050 b notify_queues
// ffffffffc0118250 b notify_queues_pending
// ffffffffc0118254 b event_lock
// ffffffffc0118260 b period
// ffffffffc0118268 b timer
// ffffffffc0118270 b check_timers_event
// ffffffffc0118278 b current_system_time
// ffffffffc0118280 b timer_lock
// ffffffffc0118288 b ioapics
// ffffffffc0118290 b isos
// ffffffffc0118298 b initialized_once
// ffffffffc01182a0 b idt_entries
// ffffffffc01192a0 b interrupt_handlers
// ffffffffc011a2a0 b interrupt_vector
// ffffffffc011a614 b index
// ffffffffc011a618 b g_info
// ffffffffc011a620 b control
// ffffffffc011bfb0 b mem_memory_top
// ffffffffc011bfb8 b memory_base
// ffffffffc011bfc0 b lock
// ffffffffc011bfd0 b temp_entries
// ffffffffc011c160 b temp_entries_count
// ffffffffc011c168 b freeing
// ffffffffc011c16c b lock
// ffffffffc011c178 b memory_base
// ffffffffc011c180 b support_1gb
// ffffffffc011c181 b support_global
// ffffffffc011c184 B process_lock
// ffffffffc011c18c b flag
// ffffffffc019c190 B kernel_virtual_end
// ffffffffc019c190 b tmp_kernel_stack
// 
//not enough values to unpack (expected 3, got 1)
    {0xffffffffc019c190, "__kernel_end"}
};

