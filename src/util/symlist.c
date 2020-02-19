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
// 000000000019b1d4 B kernel_physical_end
// ffffffffc0101000 T acpi_tables_init
    {0xffffffffc0101000ull, "acpi_tables_init"},
// ffffffffc0101040 T madt_init
    {0xffffffffc0101040ull, "madt_init"},
// ffffffffc01012b0 T madt_get_next
    {0xffffffffc01012b0ull, "madt_get_next"},
// ffffffffc0101380 T rsdp_init
    {0xffffffffc0101380ull, "rsdp_init"},
// ffffffffc01014a0 T rsdt_init
    {0xffffffffc01014a0ull, "rsdt_init"},
// ffffffffc01015d0 T rsdt_search
    {0xffffffffc01015d0ull, "rsdt_search"},
// ffffffffc0101770 T trace_table
    {0xffffffffc0101770ull, "trace_table"},
// ffffffffc0101810 T __outbyte
    {0xffffffffc0101810ull, "__outbyte"},
// ffffffffc0101830 T __outdword
    {0xffffffffc0101830ull, "__outdword"},
// ffffffffc0101850 T __outword
    {0xffffffffc0101850ull, "__outword"},
// ffffffffc0101870 T __inbyte
    {0xffffffffc0101870ull, "__inbyte"},
// ffffffffc0101890 T __inword
    {0xffffffffc0101890ull, "__inword"},
// ffffffffc01018b0 T __indword
    {0xffffffffc01018b0ull, "__indword"},
// ffffffffc01018d0 T __readcr8
    {0xffffffffc01018d0ull, "__readcr8"},
// ffffffffc01018f0 T __writecr8
    {0xffffffffc01018f0ull, "__writecr8"},
// ffffffffc0101910 T __readcr4
    {0xffffffffc0101910ull, "__readcr4"},
// ffffffffc0101940 T __writecr4
    {0xffffffffc0101940ull, "__writecr4"},
// ffffffffc0101960 T __readcr3
    {0xffffffffc0101960ull, "__readcr3"},
// ffffffffc0101980 T __writecr3
    {0xffffffffc0101980ull, "__writecr3"},
// ffffffffc01019a0 T __readcr2
    {0xffffffffc01019a0ull, "__readcr2"},
// ffffffffc01019c0 T __writecr2
    {0xffffffffc01019c0ull, "__writecr2"},
// ffffffffc01019e0 T __readcr0
    {0xffffffffc01019e0ull, "__readcr0"},
// ffffffffc0101a10 T __writecr0
    {0xffffffffc0101a10ull, "__writecr0"},
// ffffffffc0101a30 T __readmsr
    {0xffffffffc0101a30ull, "__readmsr"},
// ffffffffc0101a70 T __writemsr
    {0xffffffffc0101a70ull, "__writemsr"},
// ffffffffc0101ab0 T __cpuid
    {0xffffffffc0101ab0ull, "__cpuid"},
// ffffffffc0101b10 T __cpuidex
    {0xffffffffc0101b10ull, "__cpuidex"},
// ffffffffc0101b70 T cpuid
    {0xffffffffc0101b70ull, "cpuid"},
// ffffffffc0101c10 T __invlpg
    {0xffffffffc0101c10ull, "__invlpg"},
// ffffffffc0101c30 T __halt
    {0xffffffffc0101c30ull, "__halt"},
// ffffffffc0101c40 T __nop
    {0xffffffffc0101c40ull, "__nop"},
// ffffffffc0101c50 T _BitScanReverse
    {0xffffffffc0101c50ull, "_BitScanReverse"},
// ffffffffc0101c90 T _BitScanReverse64
    {0xffffffffc0101c90ull, "_BitScanReverse64"},
// ffffffffc0101cd0 T __readmsr_efer
    {0xffffffffc0101cd0ull, "__readmsr_efer"},
// ffffffffc0101cf0 T __writemsr_efer
    {0xffffffffc0101cf0ull, "__writemsr_efer"},
// ffffffffc0101d10 T __readmsr_apic_base
    {0xffffffffc0101d10ull, "__readmsr_apic_base"},
// ffffffffc0101d30 T events_init
    {0xffffffffc0101d30ull, "events_init"},
// ffffffffc0101da0 T raise_tpl
    {0xffffffffc0101da0ull, "raise_tpl"},
// ffffffffc0101e20 T set_tpl
    {0xffffffffc0101e20ull, "set_tpl"},
// ffffffffc0101e80 T restore_tpl
    {0xffffffffc0101e80ull, "restore_tpl"},
// ffffffffc0101f70 t dispatch_event_notifies
    {0xffffffffc0101f70ull, "dispatch_event_notifies"},
// ffffffffc01020c0 T get_tpl
    {0xffffffffc01020c0ull, "get_tpl"},
// ffffffffc0102120 T create_event
    {0xffffffffc0102120ull, "create_event"},
// ffffffffc0102240 T signal_event
    {0xffffffffc0102240ull, "signal_event"},
// ffffffffc0102390 T check_event
    {0xffffffffc0102390ull, "check_event"},
// ffffffffc0102460 T wait_for_event
    {0xffffffffc0102460ull, "wait_for_event"},
// ffffffffc0102790 T close_event
    {0xffffffffc0102790ull, "close_event"},
// ffffffffc0102840 t aquire_event_lock
    {0xffffffffc0102840ull, "aquire_event_lock"},
// ffffffffc0102870 t release_event_lock
    {0xffffffffc0102870ull, "release_event_lock"},
// ffffffffc0102890 T init_pit8254
    {0xffffffffc0102890ull, "init_pit8254"},
// ffffffffc01028e0 t set_timer_period
    {0xffffffffc01028e0ull, "set_timer_period"},
// ffffffffc0102990 t interrupt_handler
    {0xffffffffc0102990ull, "interrupt_handler"},
// ffffffffc01029c0 t set_pit_count
    {0xffffffffc01029c0ull, "set_pit_count"},
// ffffffffc0102a20 T init_timer
    {0xffffffffc0102a20ull, "init_timer"},
// ffffffffc0102b60 t check_timers
    {0xffffffffc0102b60ull, "check_timers"},
// ffffffffc0102cc0 T timer_tick
    {0xffffffffc0102cc0ull, "timer_tick"},
// ffffffffc0102d30 T get_current_system_time
    {0xffffffffc0102d30ull, "get_current_system_time"},
// ffffffffc0102d50 T set_timer
    {0xffffffffc0102d50ull, "set_timer"},
// ffffffffc0102f40 t insert_event_timer
    {0xffffffffc0102f40ull, "insert_event_timer"},
// ffffffffc0102fe0 T ioapic_add
    {0xffffffffc0102fe0ull, "ioapic_add"},
// ffffffffc0103100 t ioapic_read
    {0xffffffffc0103100ull, "ioapic_read"},
// ffffffffc0103180 T ioapic_add_iso
    {0xffffffffc0103180ull, "ioapic_add_iso"},
// ffffffffc0103210 T ioapic_enable_interrupt
    {0xffffffffc0103210ull, "ioapic_enable_interrupt"},
// ffffffffc0103390 t get_ioapic
    {0xffffffffc0103390ull, "get_ioapic"},
// ffffffffc01034b0 t ioapic_write
    {0xffffffffc01034b0ull, "ioapic_write"},
// ffffffffc0103530 T ioapic_configure_interrupt
    {0xffffffffc0103530ull, "ioapic_configure_interrupt"},
// ffffffffc0103810 T get_apic_id
    {0xffffffffc0103810ull, "get_apic_id"},
// ffffffffc0103830 t lapic_read
    {0xffffffffc0103830ull, "lapic_read"},
// ffffffffc0103860 T lapic_init
    {0xffffffffc0103860ull, "lapic_init"},
// ffffffffc0103980 t lapic_write
    {0xffffffffc0103980ull, "lapic_write"},
// ffffffffc01039b0 T send_fixed_ipi
    {0xffffffffc01039b0ull, "send_fixed_ipi"},
// ffffffffc01039d0 T send_apic_eoi
    {0xffffffffc01039d0ull, "send_apic_eoi"},
// ffffffffc01039f0 t spurious_interrupt_handler
    {0xffffffffc01039f0ull, "spurious_interrupt_handler"},
// ffffffffc0103a10 T idt_init
    {0xffffffffc0103a10ull, "idt_init"},
// ffffffffc0104d30 t set_idt_entry
    {0xffffffffc0104d30ull, "set_idt_entry"},
// ffffffffc0104f00 T interrupts_init
    {0xffffffffc0104f00ull, "interrupts_init"},
// ffffffffc0104f70 T common_interrupt_handler
    {0xffffffffc0104f70ull, "common_interrupt_handler"},
// ffffffffc0105100 t default_interrupt_handler
    {0xffffffffc0105100ull, "default_interrupt_handler"},
// ffffffffc0105a20 T interrupt_register
    {0xffffffffc0105a20ull, "interrupt_register"},
// ffffffffc0105b90 T interrupt_free
    {0xffffffffc0105b90ull, "interrupt_free"},
// ffffffffc0105c20 t interrupt_allocate
    {0xffffffffc0105c20ull, "interrupt_allocate"},
// ffffffffc0105d90 T kernel_main
    {0xffffffffc0105d90ull, "kernel_main"},
// ffffffffc0105f20 t main_thread
    {0xffffffffc0105f20ull, "main_thread"},
// ffffffffc0106060 T init_tss_for_cpu
    {0xffffffffc0106060ull, "init_tss_for_cpu"},
// ffffffffc0106130 T mm_init
    {0xffffffffc0106130ull, "mm_init"},
// ffffffffc0106140 T mm_allocate
    {0xffffffffc0106140ull, "mm_allocate"},
// ffffffffc0106210 t tlsf_malloc
    {0xffffffffc0106210ull, "tlsf_malloc"},
// ffffffffc0106250 t tlsf_add_pool
    {0xffffffffc0106250ull, "tlsf_add_pool"},
// ffffffffc01063a0 T mm_reallocate
    {0xffffffffc01063a0ull, "mm_reallocate"},
// ffffffffc0106470 t tlsf_realloc
    {0xffffffffc0106470ull, "tlsf_realloc"},
// ffffffffc0106660 T mm_free
    {0xffffffffc0106660ull, "mm_free"},
// ffffffffc0106680 t tlsf_free
    {0xffffffffc0106680ull, "tlsf_free"},
// ffffffffc0106730 t tlsf_create
    {0xffffffffc0106730ull, "tlsf_create"},
// ffffffffc0106740 t control_construct
    {0xffffffffc0106740ull, "control_construct"},
// ffffffffc01067e0 t adjust_request_size
    {0xffffffffc01067e0ull, "adjust_request_size"},
// ffffffffc0106870 t block_locate_free
    {0xffffffffc0106870ull, "block_locate_free"},
// ffffffffc0106940 t block_prepare_used
    {0xffffffffc0106940ull, "block_prepare_used"},
// ffffffffc01069f0 t align_up
    {0xffffffffc01069f0ull, "align_up"},
// ffffffffc0106aa0 t mapping_search
    {0xffffffffc0106aa0ull, "mapping_search"},
// ffffffffc0106b10 t search_suitable_block
    {0xffffffffc0106b10ull, "search_suitable_block"},
// ffffffffc0106c50 t block_size
    {0xffffffffc0106c50ull, "block_size"},
// ffffffffc0106c70 t remove_free_block
    {0xffffffffc0106c70ull, "remove_free_block"},
// ffffffffc0106e00 t tlsf_fls_sizet
    {0xffffffffc0106e00ull, "tlsf_fls_sizet"},
// ffffffffc0106e60 t mapping_insert
    {0xffffffffc0106e60ull, "mapping_insert"},
// ffffffffc0106ef0 t tlsf_fls
    {0xffffffffc0106ef0ull, "tlsf_fls"},
// ffffffffc0106f40 t tlsf_ffs
    {0xffffffffc0106f40ull, "tlsf_ffs"},
// ffffffffc0106f70 t block_trim_free
    {0xffffffffc0106f70ull, "block_trim_free"},
// ffffffffc0107030 t block_mark_as_used
    {0xffffffffc0107030ull, "block_mark_as_used"},
// ffffffffc0107070 t block_to_ptr
    {0xffffffffc0107070ull, "block_to_ptr"},
// ffffffffc0107090 t block_is_free
    {0xffffffffc0107090ull, "block_is_free"},
// ffffffffc01070b0 t block_can_split
    {0xffffffffc01070b0ull, "block_can_split"},
// ffffffffc01070f0 t block_split
    {0xffffffffc01070f0ull, "block_split"},
// ffffffffc01072a0 t block_link_next
    {0xffffffffc01072a0ull, "block_link_next"},
// ffffffffc01072d0 t block_set_prev_free
    {0xffffffffc01072d0ull, "block_set_prev_free"},
// ffffffffc0107300 t block_insert
    {0xffffffffc0107300ull, "block_insert"},
// ffffffffc0107340 t offset_to_block
    {0xffffffffc0107340ull, "offset_to_block"},
// ffffffffc0107360 t align_ptr
    {0xffffffffc0107360ull, "align_ptr"},
// ffffffffc0107410 t block_set_size
    {0xffffffffc0107410ull, "block_set_size"},
// ffffffffc0107450 t block_mark_as_free
    {0xffffffffc0107450ull, "block_mark_as_free"},
// ffffffffc0107490 t block_set_free
    {0xffffffffc0107490ull, "block_set_free"},
// ffffffffc01074c0 t block_next
    {0xffffffffc01074c0ull, "block_next"},
// ffffffffc0107550 t block_is_last
    {0xffffffffc0107550ull, "block_is_last"},
// ffffffffc0107580 t insert_free_block
    {0xffffffffc0107580ull, "insert_free_block"},
// ffffffffc0107740 t block_set_prev_used
    {0xffffffffc0107740ull, "block_set_prev_used"},
// ffffffffc0107770 t block_set_used
    {0xffffffffc0107770ull, "block_set_used"},
// ffffffffc01077a0 t tlsf_pool_overhead
    {0xffffffffc01077a0ull, "tlsf_pool_overhead"},
// ffffffffc01077b0 t align_down
    {0xffffffffc01077b0ull, "align_down"},
// ffffffffc0107850 t block_from_ptr
    {0xffffffffc0107850ull, "block_from_ptr"},
// ffffffffc0107870 t block_merge_next
    {0xffffffffc0107870ull, "block_merge_next"},
// ffffffffc0107970 t block_trim_used
    {0xffffffffc0107970ull, "block_trim_used"},
// ffffffffc0107a30 t block_remove
    {0xffffffffc0107a30ull, "block_remove"},
// ffffffffc0107a70 t block_absorb
    {0xffffffffc0107a70ull, "block_absorb"},
// ffffffffc0107b10 t block_merge_prev
    {0xffffffffc0107b10ull, "block_merge_prev"},
// ffffffffc0107c10 t block_is_prev_free
    {0xffffffffc0107c10ull, "block_is_prev_free"},
// ffffffffc0107c30 t block_prev
    {0xffffffffc0107c30ull, "block_prev"},
// ffffffffc0107ca0 T pmm_init
    {0xffffffffc0107ca0ull, "pmm_init"},
// ffffffffc0107dc0 t add_range
    {0xffffffffc0107dc0ull, "add_range"},
// ffffffffc0108010 T pmm_post_vmm
    {0xffffffffc0108010ull, "pmm_post_vmm"},
// ffffffffc0108260 T pmm_allocate_pages
    {0xffffffffc0108260ull, "pmm_allocate_pages"},
// ffffffffc01085c0 t find_free_pages
    {0xffffffffc01085c0ull, "find_free_pages"},
// ffffffffc01087d0 t convert_page
    {0xffffffffc01087d0ull, "convert_page"},
// ffffffffc0108d00 T pmm_free_pages
    {0xffffffffc0108d00ull, "pmm_free_pages"},
// ffffffffc0108e20 t remove_mem_map_entry
    {0xffffffffc0108e20ull, "remove_mem_map_entry"},
// ffffffffc0108e70 t move_temp_entries
    {0xffffffffc0108e70ull, "move_temp_entries"},
// ffffffffc0109030 t allocate_memory_map_entry
    {0xffffffffc0109030ull, "allocate_memory_map_entry"},
// ffffffffc0109100 t allocate_pool_pages
    {0xffffffffc0109100ull, "allocate_pool_pages"},
// ffffffffc01091a0 T allocate_stack
    {0xffffffffc01091a0ull, "allocate_stack"},
// ffffffffc0109390 T free_stack
    {0xffffffffc0109390ull, "free_stack"},
// ffffffffc01094a0 t acquire_alloc_lock
    {0xffffffffc01094a0ull, "acquire_alloc_lock"},
// ffffffffc01094d0 t release_alloc_lock
    {0xffffffffc01094d0ull, "release_alloc_lock"},
// ffffffffc01094f0 T vmm_init
    {0xffffffffc01094f0ull, "vmm_init"},
// ffffffffc01096d0 T vmm_map
    {0xffffffffc01096d0ull, "vmm_map"},
// ffffffffc0109950 T vmm_set_handle
    {0xffffffffc0109950ull, "vmm_set_handle"},
// ffffffffc0109990 T vmm_enable_cpu_features
    {0xffffffffc0109990ull, "vmm_enable_cpu_features"},
// ffffffffc0109b30 T vmm_virtual_to_physical
    {0xffffffffc0109b30ull, "vmm_virtual_to_physical"},
// ffffffffc0109d20 t get_entry_at_virtual_address
    {0xffffffffc0109d20ull, "get_entry_at_virtual_address"},
// ffffffffc0109fa0 T vmm_is_mapped
    {0xffffffffc0109fa0ull, "vmm_is_mapped"},
// ffffffffc010a110 t page_size_by_type
    {0xffffffffc010a110ull, "page_size_by_type"},
// ffffffffc010a1c0 T vmm_unmap
    {0xffffffffc010a1c0ull, "vmm_unmap"},
// ffffffffc010a3f0 t set_pte
    {0xffffffffc010a3f0ull, "set_pte"},
// ffffffffc010aea0 T init_kernel_process
    {0xffffffffc010aea0ull, "init_kernel_process"},
// ffffffffc010aec0 T yield
    {0xffffffffc010aec0ull, "yield"},
// ffffffffc010aed0 T init_sched
    {0xffffffffc010aed0ull, "init_sched"},
// ffffffffc010b020 t preemption_callback
    {0xffffffffc010b020ull, "preemption_callback"},
// ffffffffc010b040 T queue_thread
    {0xffffffffc010b040ull, "queue_thread"},
// ffffffffc010b100 T sched_tick
    {0xffffffffc010b100ull, "sched_tick"},
// ffffffffc010b360 t yield_interrupt
    {0xffffffffc010b360ull, "yield_interrupt"},
// ffffffffc010b3d0 t idle_thread_func
    {0xffffffffc010b3d0ull, "idle_thread_func"},
// ffffffffc010b400 t sched_lock
    {0xffffffffc010b400ull, "sched_lock"},
// ffffffffc010b440 t sched_unlock
    {0xffffffffc010b440ull, "sched_unlock"},
// ffffffffc010b460 T spawn_thread
    {0xffffffffc010b460ull, "spawn_thread"},
// ffffffffc010b750 T pcpu_init_for_cpu
    {0xffffffffc010b750ull, "pcpu_init_for_cpu"},
// ffffffffc010b780 T pcpu_init_for_bsp
    {0xffffffffc010b780ull, "pcpu_init_for_bsp"},
// ffffffffc010b7a0 T get_pcpu_base
    {0xffffffffc010b7a0ull, "get_pcpu_base"},
// ffffffffc010b7b0 T spinlock_acquire_high_tpl_raw
    {0xffffffffc010b7b0ull, "spinlock_acquire_high_tpl_raw"},
// ffffffffc010b840 T spinlock_acquire_raw
    {0xffffffffc010b840ull, "spinlock_acquire_raw"},
// ffffffffc010b8c0 T spinlock_release
    {0xffffffffc010b8c0ull, "spinlock_release"},
// ffffffffc010b920 T strerror
    {0xffffffffc010b920ull, "strerror"},
// ffffffffc010b970 T panic
    {0xffffffffc010b970ull, "panic"},
// ffffffffc010b990 T is_node_in_list
    {0xffffffffc010b990ull, "is_node_in_list"},
// ffffffffc010ba90 t is_list_valid
    {0xffffffffc010ba90ull, "is_list_valid"},
// ffffffffc010bb80 T initialize_list_had
    {0xffffffffc010bb80ull, "initialize_list_had"},
// ffffffffc010bbf0 T insert_head_list
    {0xffffffffc010bbf0ull, "insert_head_list"},
// ffffffffc010bc90 T insert_tail_list
    {0xffffffffc010bc90ull, "insert_tail_list"},
// ffffffffc010bd30 T get_first_node
    {0xffffffffc010bd30ull, "get_first_node"},
// ffffffffc010bd90 T get_next_node
    {0xffffffffc010bd90ull, "get_next_node"},
// ffffffffc010bdf0 T get_prev_node
    {0xffffffffc010bdf0ull, "get_prev_node"},
// ffffffffc010be60 T is_list_empty
    {0xffffffffc010be60ull, "is_list_empty"},
// ffffffffc010bed0 T is_null
    {0xffffffffc010bed0ull, "is_null"},
// ffffffffc010bf00 T is_node_at_end
    {0xffffffffc010bf00ull, "is_node_at_end"},
// ffffffffc010bf50 T swap_list_entries
    {0xffffffffc010bf50ull, "swap_list_entries"},
// ffffffffc010c060 T remove_entry_list
    {0xffffffffc010c060ull, "remove_entry_list"},
// ffffffffc010c0f0 T serial_init
    {0xffffffffc010c0f0ull, "serial_init"},
// ffffffffc010c170 T serial_write
    {0xffffffffc010c170ull, "serial_write"},
// ffffffffc010c230 T serial_read
    {0xffffffffc010c230ull, "serial_read"},
// ffffffffc010c2b0 T serial_poll
    {0xffffffffc010c2b0ull, "serial_poll"},
// ffffffffc010c2e0 T trace
    {0xffffffffc010c2e0ull, "trace"},
// ffffffffc010c3c0 t stb_print_callback
    {0xffffffffc010c3c0ull, "stb_print_callback"},
// ffffffffc010c440 t common_stub
    {0xffffffffc010c440ull, "common_stub"},
// ffffffffc010c48d T handle_interrupt_request_00
    {0xffffffffc010c48dull, "handle_interrupt_request_00"},
// ffffffffc010c493 T handle_interrupt_request_01
    {0xffffffffc010c493ull, "handle_interrupt_request_01"},
// ffffffffc010c499 T handle_interrupt_request_02
    {0xffffffffc010c499ull, "handle_interrupt_request_02"},
// ffffffffc010c49f T handle_interrupt_request_03
    {0xffffffffc010c49full, "handle_interrupt_request_03"},
// ffffffffc010c4a5 T handle_interrupt_request_04
    {0xffffffffc010c4a5ull, "handle_interrupt_request_04"},
// ffffffffc010c4ab T handle_interrupt_request_05
    {0xffffffffc010c4abull, "handle_interrupt_request_05"},
// ffffffffc010c4b1 T handle_interrupt_request_06
    {0xffffffffc010c4b1ull, "handle_interrupt_request_06"},
// ffffffffc010c4b7 T handle_interrupt_request_07
    {0xffffffffc010c4b7ull, "handle_interrupt_request_07"},
// ffffffffc010c4bd T handle_interrupt_request_08
    {0xffffffffc010c4bdull, "handle_interrupt_request_08"},
// ffffffffc010c4c4 T handle_interrupt_request_09
    {0xffffffffc010c4c4ull, "handle_interrupt_request_09"},
// ffffffffc010c4cd T handle_interrupt_request_0a
    {0xffffffffc010c4cdull, "handle_interrupt_request_0a"},
// ffffffffc010c4d4 T handle_interrupt_request_0b
    {0xffffffffc010c4d4ull, "handle_interrupt_request_0b"},
// ffffffffc010c4db T handle_interrupt_request_0c
    {0xffffffffc010c4dbull, "handle_interrupt_request_0c"},
// ffffffffc010c4e2 T handle_interrupt_request_0d
    {0xffffffffc010c4e2ull, "handle_interrupt_request_0d"},
// ffffffffc010c4e9 T handle_interrupt_request_0e
    {0xffffffffc010c4e9ull, "handle_interrupt_request_0e"},
// ffffffffc010c4f0 T handle_interrupt_request_0f
    {0xffffffffc010c4f0ull, "handle_interrupt_request_0f"},
// ffffffffc010c4f9 T handle_interrupt_request_10
    {0xffffffffc010c4f9ull, "handle_interrupt_request_10"},
// ffffffffc010c500 T handle_interrupt_request_11
    {0xffffffffc010c500ull, "handle_interrupt_request_11"},
// ffffffffc010c509 T handle_interrupt_request_12
    {0xffffffffc010c509ull, "handle_interrupt_request_12"},
// ffffffffc010c512 T handle_interrupt_request_13
    {0xffffffffc010c512ull, "handle_interrupt_request_13"},
// ffffffffc010c51b T handle_interrupt_request_14
    {0xffffffffc010c51bull, "handle_interrupt_request_14"},
// ffffffffc010c524 T handle_interrupt_request_15
    {0xffffffffc010c524ull, "handle_interrupt_request_15"},
// ffffffffc010c52d T handle_interrupt_request_16
    {0xffffffffc010c52dull, "handle_interrupt_request_16"},
// ffffffffc010c536 T handle_interrupt_request_17
    {0xffffffffc010c536ull, "handle_interrupt_request_17"},
// ffffffffc010c53f T handle_interrupt_request_18
    {0xffffffffc010c53full, "handle_interrupt_request_18"},
// ffffffffc010c548 T handle_interrupt_request_19
    {0xffffffffc010c548ull, "handle_interrupt_request_19"},
// ffffffffc010c551 T handle_interrupt_request_1a
    {0xffffffffc010c551ull, "handle_interrupt_request_1a"},
// ffffffffc010c55a T handle_interrupt_request_1b
    {0xffffffffc010c55aull, "handle_interrupt_request_1b"},
// ffffffffc010c563 T handle_interrupt_request_1c
    {0xffffffffc010c563ull, "handle_interrupt_request_1c"},
// ffffffffc010c56c T handle_interrupt_request_1d
    {0xffffffffc010c56cull, "handle_interrupt_request_1d"},
// ffffffffc010c575 T handle_interrupt_request_1e
    {0xffffffffc010c575ull, "handle_interrupt_request_1e"},
// ffffffffc010c57c T handle_interrupt_request_1f
    {0xffffffffc010c57cull, "handle_interrupt_request_1f"},
// ffffffffc010c585 T handle_interrupt_request_20
    {0xffffffffc010c585ull, "handle_interrupt_request_20"},
// ffffffffc010c58e T handle_interrupt_request_21
    {0xffffffffc010c58eull, "handle_interrupt_request_21"},
// ffffffffc010c597 T handle_interrupt_request_22
    {0xffffffffc010c597ull, "handle_interrupt_request_22"},
// ffffffffc010c5a0 T handle_interrupt_request_23
    {0xffffffffc010c5a0ull, "handle_interrupt_request_23"},
// ffffffffc010c5a9 T handle_interrupt_request_24
    {0xffffffffc010c5a9ull, "handle_interrupt_request_24"},
// ffffffffc010c5b2 T handle_interrupt_request_25
    {0xffffffffc010c5b2ull, "handle_interrupt_request_25"},
// ffffffffc010c5bb T handle_interrupt_request_26
    {0xffffffffc010c5bbull, "handle_interrupt_request_26"},
// ffffffffc010c5c4 T handle_interrupt_request_27
    {0xffffffffc010c5c4ull, "handle_interrupt_request_27"},
// ffffffffc010c5cd T handle_interrupt_request_28
    {0xffffffffc010c5cdull, "handle_interrupt_request_28"},
// ffffffffc010c5d6 T handle_interrupt_request_29
    {0xffffffffc010c5d6ull, "handle_interrupt_request_29"},
// ffffffffc010c5df T handle_interrupt_request_2a
    {0xffffffffc010c5dfull, "handle_interrupt_request_2a"},
// ffffffffc010c5e8 T handle_interrupt_request_2b
    {0xffffffffc010c5e8ull, "handle_interrupt_request_2b"},
// ffffffffc010c5f1 T handle_interrupt_request_2c
    {0xffffffffc010c5f1ull, "handle_interrupt_request_2c"},
// ffffffffc010c5fa T handle_interrupt_request_2d
    {0xffffffffc010c5faull, "handle_interrupt_request_2d"},
// ffffffffc010c603 T handle_interrupt_request_2e
    {0xffffffffc010c603ull, "handle_interrupt_request_2e"},
// ffffffffc010c60c T handle_interrupt_request_2f
    {0xffffffffc010c60cull, "handle_interrupt_request_2f"},
// ffffffffc010c615 T handle_interrupt_request_30
    {0xffffffffc010c615ull, "handle_interrupt_request_30"},
// ffffffffc010c61e T handle_interrupt_request_31
    {0xffffffffc010c61eull, "handle_interrupt_request_31"},
// ffffffffc010c627 T handle_interrupt_request_32
    {0xffffffffc010c627ull, "handle_interrupt_request_32"},
// ffffffffc010c630 T handle_interrupt_request_33
    {0xffffffffc010c630ull, "handle_interrupt_request_33"},
// ffffffffc010c639 T handle_interrupt_request_34
    {0xffffffffc010c639ull, "handle_interrupt_request_34"},
// ffffffffc010c642 T handle_interrupt_request_35
    {0xffffffffc010c642ull, "handle_interrupt_request_35"},
// ffffffffc010c64b T handle_interrupt_request_36
    {0xffffffffc010c64bull, "handle_interrupt_request_36"},
// ffffffffc010c654 T handle_interrupt_request_37
    {0xffffffffc010c654ull, "handle_interrupt_request_37"},
// ffffffffc010c65d T handle_interrupt_request_38
    {0xffffffffc010c65dull, "handle_interrupt_request_38"},
// ffffffffc010c666 T handle_interrupt_request_39
    {0xffffffffc010c666ull, "handle_interrupt_request_39"},
// ffffffffc010c66f T handle_interrupt_request_3a
    {0xffffffffc010c66full, "handle_interrupt_request_3a"},
// ffffffffc010c678 T handle_interrupt_request_3b
    {0xffffffffc010c678ull, "handle_interrupt_request_3b"},
// ffffffffc010c681 T handle_interrupt_request_3c
    {0xffffffffc010c681ull, "handle_interrupt_request_3c"},
// ffffffffc010c68a T handle_interrupt_request_3d
    {0xffffffffc010c68aull, "handle_interrupt_request_3d"},
// ffffffffc010c693 T handle_interrupt_request_3e
    {0xffffffffc010c693ull, "handle_interrupt_request_3e"},
// ffffffffc010c69c T handle_interrupt_request_3f
    {0xffffffffc010c69cull, "handle_interrupt_request_3f"},
// ffffffffc010c6a5 T handle_interrupt_request_40
    {0xffffffffc010c6a5ull, "handle_interrupt_request_40"},
// ffffffffc010c6ae T handle_interrupt_request_41
    {0xffffffffc010c6aeull, "handle_interrupt_request_41"},
// ffffffffc010c6b7 T handle_interrupt_request_42
    {0xffffffffc010c6b7ull, "handle_interrupt_request_42"},
// ffffffffc010c6c0 T handle_interrupt_request_43
    {0xffffffffc010c6c0ull, "handle_interrupt_request_43"},
// ffffffffc010c6c9 T handle_interrupt_request_44
    {0xffffffffc010c6c9ull, "handle_interrupt_request_44"},
// ffffffffc010c6d2 T handle_interrupt_request_45
    {0xffffffffc010c6d2ull, "handle_interrupt_request_45"},
// ffffffffc010c6db T handle_interrupt_request_46
    {0xffffffffc010c6dbull, "handle_interrupt_request_46"},
// ffffffffc010c6e4 T handle_interrupt_request_47
    {0xffffffffc010c6e4ull, "handle_interrupt_request_47"},
// ffffffffc010c6ed T handle_interrupt_request_48
    {0xffffffffc010c6edull, "handle_interrupt_request_48"},
// ffffffffc010c6f6 T handle_interrupt_request_49
    {0xffffffffc010c6f6ull, "handle_interrupt_request_49"},
// ffffffffc010c6ff T handle_interrupt_request_4a
    {0xffffffffc010c6ffull, "handle_interrupt_request_4a"},
// ffffffffc010c708 T handle_interrupt_request_4b
    {0xffffffffc010c708ull, "handle_interrupt_request_4b"},
// ffffffffc010c711 T handle_interrupt_request_4c
    {0xffffffffc010c711ull, "handle_interrupt_request_4c"},
// ffffffffc010c71a T handle_interrupt_request_4d
    {0xffffffffc010c71aull, "handle_interrupt_request_4d"},
// ffffffffc010c723 T handle_interrupt_request_4e
    {0xffffffffc010c723ull, "handle_interrupt_request_4e"},
// ffffffffc010c72c T handle_interrupt_request_4f
    {0xffffffffc010c72cull, "handle_interrupt_request_4f"},
// ffffffffc010c735 T handle_interrupt_request_50
    {0xffffffffc010c735ull, "handle_interrupt_request_50"},
// ffffffffc010c73e T handle_interrupt_request_51
    {0xffffffffc010c73eull, "handle_interrupt_request_51"},
// ffffffffc010c747 T handle_interrupt_request_52
    {0xffffffffc010c747ull, "handle_interrupt_request_52"},
// ffffffffc010c750 T handle_interrupt_request_53
    {0xffffffffc010c750ull, "handle_interrupt_request_53"},
// ffffffffc010c759 T handle_interrupt_request_54
    {0xffffffffc010c759ull, "handle_interrupt_request_54"},
// ffffffffc010c762 T handle_interrupt_request_55
    {0xffffffffc010c762ull, "handle_interrupt_request_55"},
// ffffffffc010c76b T handle_interrupt_request_56
    {0xffffffffc010c76bull, "handle_interrupt_request_56"},
// ffffffffc010c774 T handle_interrupt_request_57
    {0xffffffffc010c774ull, "handle_interrupt_request_57"},
// ffffffffc010c77d T handle_interrupt_request_58
    {0xffffffffc010c77dull, "handle_interrupt_request_58"},
// ffffffffc010c786 T handle_interrupt_request_59
    {0xffffffffc010c786ull, "handle_interrupt_request_59"},
// ffffffffc010c78f T handle_interrupt_request_5a
    {0xffffffffc010c78full, "handle_interrupt_request_5a"},
// ffffffffc010c798 T handle_interrupt_request_5b
    {0xffffffffc010c798ull, "handle_interrupt_request_5b"},
// ffffffffc010c7a1 T handle_interrupt_request_5c
    {0xffffffffc010c7a1ull, "handle_interrupt_request_5c"},
// ffffffffc010c7aa T handle_interrupt_request_5d
    {0xffffffffc010c7aaull, "handle_interrupt_request_5d"},
// ffffffffc010c7b3 T handle_interrupt_request_5e
    {0xffffffffc010c7b3ull, "handle_interrupt_request_5e"},
// ffffffffc010c7bc T handle_interrupt_request_5f
    {0xffffffffc010c7bcull, "handle_interrupt_request_5f"},
// ffffffffc010c7c5 T handle_interrupt_request_60
    {0xffffffffc010c7c5ull, "handle_interrupt_request_60"},
// ffffffffc010c7ce T handle_interrupt_request_61
    {0xffffffffc010c7ceull, "handle_interrupt_request_61"},
// ffffffffc010c7d7 T handle_interrupt_request_62
    {0xffffffffc010c7d7ull, "handle_interrupt_request_62"},
// ffffffffc010c7e0 T handle_interrupt_request_63
    {0xffffffffc010c7e0ull, "handle_interrupt_request_63"},
// ffffffffc010c7e9 T handle_interrupt_request_64
    {0xffffffffc010c7e9ull, "handle_interrupt_request_64"},
// ffffffffc010c7f2 T handle_interrupt_request_65
    {0xffffffffc010c7f2ull, "handle_interrupt_request_65"},
// ffffffffc010c7fb T handle_interrupt_request_66
    {0xffffffffc010c7fbull, "handle_interrupt_request_66"},
// ffffffffc010c804 T handle_interrupt_request_67
    {0xffffffffc010c804ull, "handle_interrupt_request_67"},
// ffffffffc010c80d T handle_interrupt_request_68
    {0xffffffffc010c80dull, "handle_interrupt_request_68"},
// ffffffffc010c816 T handle_interrupt_request_69
    {0xffffffffc010c816ull, "handle_interrupt_request_69"},
// ffffffffc010c81f T handle_interrupt_request_6a
    {0xffffffffc010c81full, "handle_interrupt_request_6a"},
// ffffffffc010c828 T handle_interrupt_request_6b
    {0xffffffffc010c828ull, "handle_interrupt_request_6b"},
// ffffffffc010c831 T handle_interrupt_request_6c
    {0xffffffffc010c831ull, "handle_interrupt_request_6c"},
// ffffffffc010c83a T handle_interrupt_request_6d
    {0xffffffffc010c83aull, "handle_interrupt_request_6d"},
// ffffffffc010c843 T handle_interrupt_request_6e
    {0xffffffffc010c843ull, "handle_interrupt_request_6e"},
// ffffffffc010c84c T handle_interrupt_request_6f
    {0xffffffffc010c84cull, "handle_interrupt_request_6f"},
// ffffffffc010c855 T handle_interrupt_request_70
    {0xffffffffc010c855ull, "handle_interrupt_request_70"},
// ffffffffc010c85e T handle_interrupt_request_71
    {0xffffffffc010c85eull, "handle_interrupt_request_71"},
// ffffffffc010c867 T handle_interrupt_request_72
    {0xffffffffc010c867ull, "handle_interrupt_request_72"},
// ffffffffc010c870 T handle_interrupt_request_73
    {0xffffffffc010c870ull, "handle_interrupt_request_73"},
// ffffffffc010c879 T handle_interrupt_request_74
    {0xffffffffc010c879ull, "handle_interrupt_request_74"},
// ffffffffc010c882 T handle_interrupt_request_75
    {0xffffffffc010c882ull, "handle_interrupt_request_75"},
// ffffffffc010c88b T handle_interrupt_request_76
    {0xffffffffc010c88bull, "handle_interrupt_request_76"},
// ffffffffc010c894 T handle_interrupt_request_77
    {0xffffffffc010c894ull, "handle_interrupt_request_77"},
// ffffffffc010c89d T handle_interrupt_request_78
    {0xffffffffc010c89dull, "handle_interrupt_request_78"},
// ffffffffc010c8a6 T handle_interrupt_request_79
    {0xffffffffc010c8a6ull, "handle_interrupt_request_79"},
// ffffffffc010c8af T handle_interrupt_request_7a
    {0xffffffffc010c8afull, "handle_interrupt_request_7a"},
// ffffffffc010c8b8 T handle_interrupt_request_7b
    {0xffffffffc010c8b8ull, "handle_interrupt_request_7b"},
// ffffffffc010c8c1 T handle_interrupt_request_7c
    {0xffffffffc010c8c1ull, "handle_interrupt_request_7c"},
// ffffffffc010c8ca T handle_interrupt_request_7d
    {0xffffffffc010c8caull, "handle_interrupt_request_7d"},
// ffffffffc010c8d3 T handle_interrupt_request_7e
    {0xffffffffc010c8d3ull, "handle_interrupt_request_7e"},
// ffffffffc010c8dc T handle_interrupt_request_7f
    {0xffffffffc010c8dcull, "handle_interrupt_request_7f"},
// ffffffffc010c8e5 T handle_interrupt_request_80
    {0xffffffffc010c8e5ull, "handle_interrupt_request_80"},
// ffffffffc010c8f1 T handle_interrupt_request_81
    {0xffffffffc010c8f1ull, "handle_interrupt_request_81"},
// ffffffffc010c8fd T handle_interrupt_request_82
    {0xffffffffc010c8fdull, "handle_interrupt_request_82"},
// ffffffffc010c909 T handle_interrupt_request_83
    {0xffffffffc010c909ull, "handle_interrupt_request_83"},
// ffffffffc010c915 T handle_interrupt_request_84
    {0xffffffffc010c915ull, "handle_interrupt_request_84"},
// ffffffffc010c921 T handle_interrupt_request_85
    {0xffffffffc010c921ull, "handle_interrupt_request_85"},
// ffffffffc010c92d T handle_interrupt_request_86
    {0xffffffffc010c92dull, "handle_interrupt_request_86"},
// ffffffffc010c939 T handle_interrupt_request_87
    {0xffffffffc010c939ull, "handle_interrupt_request_87"},
// ffffffffc010c945 T handle_interrupt_request_88
    {0xffffffffc010c945ull, "handle_interrupt_request_88"},
// ffffffffc010c951 T handle_interrupt_request_89
    {0xffffffffc010c951ull, "handle_interrupt_request_89"},
// ffffffffc010c95d T handle_interrupt_request_8a
    {0xffffffffc010c95dull, "handle_interrupt_request_8a"},
// ffffffffc010c969 T handle_interrupt_request_8b
    {0xffffffffc010c969ull, "handle_interrupt_request_8b"},
// ffffffffc010c975 T handle_interrupt_request_8c
    {0xffffffffc010c975ull, "handle_interrupt_request_8c"},
// ffffffffc010c981 T handle_interrupt_request_8d
    {0xffffffffc010c981ull, "handle_interrupt_request_8d"},
// ffffffffc010c98d T handle_interrupt_request_8e
    {0xffffffffc010c98dull, "handle_interrupt_request_8e"},
// ffffffffc010c999 T handle_interrupt_request_8f
    {0xffffffffc010c999ull, "handle_interrupt_request_8f"},
// ffffffffc010c9a5 T handle_interrupt_request_90
    {0xffffffffc010c9a5ull, "handle_interrupt_request_90"},
// ffffffffc010c9b1 T handle_interrupt_request_91
    {0xffffffffc010c9b1ull, "handle_interrupt_request_91"},
// ffffffffc010c9bd T handle_interrupt_request_92
    {0xffffffffc010c9bdull, "handle_interrupt_request_92"},
// ffffffffc010c9c9 T handle_interrupt_request_93
    {0xffffffffc010c9c9ull, "handle_interrupt_request_93"},
// ffffffffc010c9d5 T handle_interrupt_request_94
    {0xffffffffc010c9d5ull, "handle_interrupt_request_94"},
// ffffffffc010c9e1 T handle_interrupt_request_95
    {0xffffffffc010c9e1ull, "handle_interrupt_request_95"},
// ffffffffc010c9ed T handle_interrupt_request_96
    {0xffffffffc010c9edull, "handle_interrupt_request_96"},
// ffffffffc010c9f9 T handle_interrupt_request_97
    {0xffffffffc010c9f9ull, "handle_interrupt_request_97"},
// ffffffffc010ca05 T handle_interrupt_request_98
    {0xffffffffc010ca05ull, "handle_interrupt_request_98"},
// ffffffffc010ca11 T handle_interrupt_request_99
    {0xffffffffc010ca11ull, "handle_interrupt_request_99"},
// ffffffffc010ca1d T handle_interrupt_request_9a
    {0xffffffffc010ca1dull, "handle_interrupt_request_9a"},
// ffffffffc010ca29 T handle_interrupt_request_9b
    {0xffffffffc010ca29ull, "handle_interrupt_request_9b"},
// ffffffffc010ca35 T handle_interrupt_request_9c
    {0xffffffffc010ca35ull, "handle_interrupt_request_9c"},
// ffffffffc010ca41 T handle_interrupt_request_9d
    {0xffffffffc010ca41ull, "handle_interrupt_request_9d"},
// ffffffffc010ca4d T handle_interrupt_request_9e
    {0xffffffffc010ca4dull, "handle_interrupt_request_9e"},
// ffffffffc010ca59 T handle_interrupt_request_9f
    {0xffffffffc010ca59ull, "handle_interrupt_request_9f"},
// ffffffffc010ca65 T handle_interrupt_request_a0
    {0xffffffffc010ca65ull, "handle_interrupt_request_a0"},
// ffffffffc010ca71 T handle_interrupt_request_a1
    {0xffffffffc010ca71ull, "handle_interrupt_request_a1"},
// ffffffffc010ca7d T handle_interrupt_request_a2
    {0xffffffffc010ca7dull, "handle_interrupt_request_a2"},
// ffffffffc010ca89 T handle_interrupt_request_a3
    {0xffffffffc010ca89ull, "handle_interrupt_request_a3"},
// ffffffffc010ca95 T handle_interrupt_request_a4
    {0xffffffffc010ca95ull, "handle_interrupt_request_a4"},
// ffffffffc010caa1 T handle_interrupt_request_a5
    {0xffffffffc010caa1ull, "handle_interrupt_request_a5"},
// ffffffffc010caad T handle_interrupt_request_a6
    {0xffffffffc010caadull, "handle_interrupt_request_a6"},
// ffffffffc010cab9 T handle_interrupt_request_a7
    {0xffffffffc010cab9ull, "handle_interrupt_request_a7"},
// ffffffffc010cac5 T handle_interrupt_request_a8
    {0xffffffffc010cac5ull, "handle_interrupt_request_a8"},
// ffffffffc010cad1 T handle_interrupt_request_a9
    {0xffffffffc010cad1ull, "handle_interrupt_request_a9"},
// ffffffffc010cadd T handle_interrupt_request_aa
    {0xffffffffc010caddull, "handle_interrupt_request_aa"},
// ffffffffc010cae9 T handle_interrupt_request_ab
    {0xffffffffc010cae9ull, "handle_interrupt_request_ab"},
// ffffffffc010caf5 T handle_interrupt_request_ac
    {0xffffffffc010caf5ull, "handle_interrupt_request_ac"},
// ffffffffc010cb01 T handle_interrupt_request_ad
    {0xffffffffc010cb01ull, "handle_interrupt_request_ad"},
// ffffffffc010cb0d T handle_interrupt_request_ae
    {0xffffffffc010cb0dull, "handle_interrupt_request_ae"},
// ffffffffc010cb19 T handle_interrupt_request_af
    {0xffffffffc010cb19ull, "handle_interrupt_request_af"},
// ffffffffc010cb25 T handle_interrupt_request_b0
    {0xffffffffc010cb25ull, "handle_interrupt_request_b0"},
// ffffffffc010cb31 T handle_interrupt_request_b1
    {0xffffffffc010cb31ull, "handle_interrupt_request_b1"},
// ffffffffc010cb3d T handle_interrupt_request_b2
    {0xffffffffc010cb3dull, "handle_interrupt_request_b2"},
// ffffffffc010cb49 T handle_interrupt_request_b3
    {0xffffffffc010cb49ull, "handle_interrupt_request_b3"},
// ffffffffc010cb55 T handle_interrupt_request_b4
    {0xffffffffc010cb55ull, "handle_interrupt_request_b4"},
// ffffffffc010cb61 T handle_interrupt_request_b5
    {0xffffffffc010cb61ull, "handle_interrupt_request_b5"},
// ffffffffc010cb6d T handle_interrupt_request_b6
    {0xffffffffc010cb6dull, "handle_interrupt_request_b6"},
// ffffffffc010cb79 T handle_interrupt_request_b7
    {0xffffffffc010cb79ull, "handle_interrupt_request_b7"},
// ffffffffc010cb85 T handle_interrupt_request_b8
    {0xffffffffc010cb85ull, "handle_interrupt_request_b8"},
// ffffffffc010cb91 T handle_interrupt_request_b9
    {0xffffffffc010cb91ull, "handle_interrupt_request_b9"},
// ffffffffc010cb9d T handle_interrupt_request_ba
    {0xffffffffc010cb9dull, "handle_interrupt_request_ba"},
// ffffffffc010cba9 T handle_interrupt_request_bb
    {0xffffffffc010cba9ull, "handle_interrupt_request_bb"},
// ffffffffc010cbb5 T handle_interrupt_request_bc
    {0xffffffffc010cbb5ull, "handle_interrupt_request_bc"},
// ffffffffc010cbc1 T handle_interrupt_request_bd
    {0xffffffffc010cbc1ull, "handle_interrupt_request_bd"},
// ffffffffc010cbcd T handle_interrupt_request_be
    {0xffffffffc010cbcdull, "handle_interrupt_request_be"},
// ffffffffc010cbd9 T handle_interrupt_request_bf
    {0xffffffffc010cbd9ull, "handle_interrupt_request_bf"},
// ffffffffc010cbe5 T handle_interrupt_request_c0
    {0xffffffffc010cbe5ull, "handle_interrupt_request_c0"},
// ffffffffc010cbf1 T handle_interrupt_request_c1
    {0xffffffffc010cbf1ull, "handle_interrupt_request_c1"},
// ffffffffc010cbfd T handle_interrupt_request_c2
    {0xffffffffc010cbfdull, "handle_interrupt_request_c2"},
// ffffffffc010cc09 T handle_interrupt_request_c3
    {0xffffffffc010cc09ull, "handle_interrupt_request_c3"},
// ffffffffc010cc15 T handle_interrupt_request_c4
    {0xffffffffc010cc15ull, "handle_interrupt_request_c4"},
// ffffffffc010cc21 T handle_interrupt_request_c5
    {0xffffffffc010cc21ull, "handle_interrupt_request_c5"},
// ffffffffc010cc2d T handle_interrupt_request_c6
    {0xffffffffc010cc2dull, "handle_interrupt_request_c6"},
// ffffffffc010cc39 T handle_interrupt_request_c7
    {0xffffffffc010cc39ull, "handle_interrupt_request_c7"},
// ffffffffc010cc45 T handle_interrupt_request_c8
    {0xffffffffc010cc45ull, "handle_interrupt_request_c8"},
// ffffffffc010cc51 T handle_interrupt_request_c9
    {0xffffffffc010cc51ull, "handle_interrupt_request_c9"},
// ffffffffc010cc5d T handle_interrupt_request_ca
    {0xffffffffc010cc5dull, "handle_interrupt_request_ca"},
// ffffffffc010cc69 T handle_interrupt_request_cb
    {0xffffffffc010cc69ull, "handle_interrupt_request_cb"},
// ffffffffc010cc75 T handle_interrupt_request_cc
    {0xffffffffc010cc75ull, "handle_interrupt_request_cc"},
// ffffffffc010cc81 T handle_interrupt_request_cd
    {0xffffffffc010cc81ull, "handle_interrupt_request_cd"},
// ffffffffc010cc8d T handle_interrupt_request_ce
    {0xffffffffc010cc8dull, "handle_interrupt_request_ce"},
// ffffffffc010cc99 T handle_interrupt_request_cf
    {0xffffffffc010cc99ull, "handle_interrupt_request_cf"},
// ffffffffc010cca5 T handle_interrupt_request_d0
    {0xffffffffc010cca5ull, "handle_interrupt_request_d0"},
// ffffffffc010ccb1 T handle_interrupt_request_d1
    {0xffffffffc010ccb1ull, "handle_interrupt_request_d1"},
// ffffffffc010ccbd T handle_interrupt_request_d2
    {0xffffffffc010ccbdull, "handle_interrupt_request_d2"},
// ffffffffc010ccc9 T handle_interrupt_request_d3
    {0xffffffffc010ccc9ull, "handle_interrupt_request_d3"},
// ffffffffc010ccd5 T handle_interrupt_request_d4
    {0xffffffffc010ccd5ull, "handle_interrupt_request_d4"},
// ffffffffc010cce1 T handle_interrupt_request_d5
    {0xffffffffc010cce1ull, "handle_interrupt_request_d5"},
// ffffffffc010cced T handle_interrupt_request_d6
    {0xffffffffc010ccedull, "handle_interrupt_request_d6"},
// ffffffffc010ccf9 T handle_interrupt_request_d7
    {0xffffffffc010ccf9ull, "handle_interrupt_request_d7"},
// ffffffffc010cd05 T handle_interrupt_request_d8
    {0xffffffffc010cd05ull, "handle_interrupt_request_d8"},
// ffffffffc010cd11 T handle_interrupt_request_d9
    {0xffffffffc010cd11ull, "handle_interrupt_request_d9"},
// ffffffffc010cd1d T handle_interrupt_request_da
    {0xffffffffc010cd1dull, "handle_interrupt_request_da"},
// ffffffffc010cd29 T handle_interrupt_request_db
    {0xffffffffc010cd29ull, "handle_interrupt_request_db"},
// ffffffffc010cd35 T handle_interrupt_request_dc
    {0xffffffffc010cd35ull, "handle_interrupt_request_dc"},
// ffffffffc010cd41 T handle_interrupt_request_dd
    {0xffffffffc010cd41ull, "handle_interrupt_request_dd"},
// ffffffffc010cd4d T handle_interrupt_request_de
    {0xffffffffc010cd4dull, "handle_interrupt_request_de"},
// ffffffffc010cd59 T handle_interrupt_request_df
    {0xffffffffc010cd59ull, "handle_interrupt_request_df"},
// ffffffffc010cd65 T handle_interrupt_request_e0
    {0xffffffffc010cd65ull, "handle_interrupt_request_e0"},
// ffffffffc010cd71 T handle_interrupt_request_e1
    {0xffffffffc010cd71ull, "handle_interrupt_request_e1"},
// ffffffffc010cd7d T handle_interrupt_request_e2
    {0xffffffffc010cd7dull, "handle_interrupt_request_e2"},
// ffffffffc010cd89 T handle_interrupt_request_e3
    {0xffffffffc010cd89ull, "handle_interrupt_request_e3"},
// ffffffffc010cd95 T handle_interrupt_request_e4
    {0xffffffffc010cd95ull, "handle_interrupt_request_e4"},
// ffffffffc010cda1 T handle_interrupt_request_e5
    {0xffffffffc010cda1ull, "handle_interrupt_request_e5"},
// ffffffffc010cdad T handle_interrupt_request_e6
    {0xffffffffc010cdadull, "handle_interrupt_request_e6"},
// ffffffffc010cdb9 T handle_interrupt_request_e7
    {0xffffffffc010cdb9ull, "handle_interrupt_request_e7"},
// ffffffffc010cdc5 T handle_interrupt_request_e8
    {0xffffffffc010cdc5ull, "handle_interrupt_request_e8"},
// ffffffffc010cdd1 T handle_interrupt_request_e9
    {0xffffffffc010cdd1ull, "handle_interrupt_request_e9"},
// ffffffffc010cddd T handle_interrupt_request_ea
    {0xffffffffc010cdddull, "handle_interrupt_request_ea"},
// ffffffffc010cde9 T handle_interrupt_request_eb
    {0xffffffffc010cde9ull, "handle_interrupt_request_eb"},
// ffffffffc010cdf5 T handle_interrupt_request_ec
    {0xffffffffc010cdf5ull, "handle_interrupt_request_ec"},
// ffffffffc010ce01 T handle_interrupt_request_ed
    {0xffffffffc010ce01ull, "handle_interrupt_request_ed"},
// ffffffffc010ce0d T handle_interrupt_request_ee
    {0xffffffffc010ce0dull, "handle_interrupt_request_ee"},
// ffffffffc010ce19 T handle_interrupt_request_ef
    {0xffffffffc010ce19ull, "handle_interrupt_request_ef"},
// ffffffffc010ce25 T handle_interrupt_request_f0
    {0xffffffffc010ce25ull, "handle_interrupt_request_f0"},
// ffffffffc010ce31 T handle_interrupt_request_f1
    {0xffffffffc010ce31ull, "handle_interrupt_request_f1"},
// ffffffffc010ce3d T handle_interrupt_request_f2
    {0xffffffffc010ce3dull, "handle_interrupt_request_f2"},
// ffffffffc010ce49 T handle_interrupt_request_f3
    {0xffffffffc010ce49ull, "handle_interrupt_request_f3"},
// ffffffffc010ce55 T handle_interrupt_request_f4
    {0xffffffffc010ce55ull, "handle_interrupt_request_f4"},
// ffffffffc010ce61 T handle_interrupt_request_f5
    {0xffffffffc010ce61ull, "handle_interrupt_request_f5"},
// ffffffffc010ce6d T handle_interrupt_request_f6
    {0xffffffffc010ce6dull, "handle_interrupt_request_f6"},
// ffffffffc010ce79 T handle_interrupt_request_f7
    {0xffffffffc010ce79ull, "handle_interrupt_request_f7"},
// ffffffffc010ce85 T handle_interrupt_request_f8
    {0xffffffffc010ce85ull, "handle_interrupt_request_f8"},
// ffffffffc010ce91 T handle_interrupt_request_f9
    {0xffffffffc010ce91ull, "handle_interrupt_request_f9"},
// ffffffffc010ce9d T handle_interrupt_request_fa
    {0xffffffffc010ce9dull, "handle_interrupt_request_fa"},
// ffffffffc010cea9 T handle_interrupt_request_fb
    {0xffffffffc010cea9ull, "handle_interrupt_request_fb"},
// ffffffffc010ceb5 T handle_interrupt_request_fc
    {0xffffffffc010ceb5ull, "handle_interrupt_request_fc"},
// ffffffffc010cec1 T handle_interrupt_request_fd
    {0xffffffffc010cec1ull, "handle_interrupt_request_fd"},
// ffffffffc010cecd T handle_interrupt_request_fe
    {0xffffffffc010cecdull, "handle_interrupt_request_fe"},
// ffffffffc010ced9 T handle_interrupt_request_ff
    {0xffffffffc010ced9ull, "handle_interrupt_request_ff"},
// ffffffffc010cef0 t fix_data_segment
    {0xffffffffc010cef0ull, "fix_data_segment"},
// ffffffffc010cf01 T gdt_fix_segments
    {0xffffffffc010cf01ull, "gdt_fix_segments"},
// ffffffffc010cf20 T memset
    {0xffffffffc010cf20ull, "memset"},
// ffffffffc010cf80 T memcmp
    {0xffffffffc010cf80ull, "memcmp"},
// ffffffffc010d020 T memcpy
    {0xffffffffc010d020ull, "memcpy"},
// ffffffffc010d0a0 T memmove
    {0xffffffffc010d0a0ull, "memmove"},
// ffffffffc010d1a0 T strcmp
    {0xffffffffc010d1a0ull, "strcmp"},
// ffffffffc010d230 T strlen
    {0xffffffffc010d230ull, "strlen"},
// ffffffffc010d280 T stbsp_set_separators
    {0xffffffffc010d280ull, "stbsp_set_separators"},
// ffffffffc010d2b0 T stbsp_vsprintfcb
    {0xffffffffc010d2b0ull, "stbsp_vsprintfcb"},
// ffffffffc010f6a0 t stbsp__lead_sign
    {0xffffffffc010f6a0ull, "stbsp__lead_sign"},
// ffffffffc010f730 T stbsp_sprintf
    {0xffffffffc010f730ull, "stbsp_sprintf"},
// ffffffffc010f7a0 T stbsp_vsnprintf
    {0xffffffffc010f7a0ull, "stbsp_vsnprintf"},
// ffffffffc010f8e0 t stbsp__count_clamp_callback
    {0xffffffffc010f8e0ull, "stbsp__count_clamp_callback"},
// ffffffffc010f920 t stbsp__clamp_callback
    {0xffffffffc010f920ull, "stbsp__clamp_callback"},
// ffffffffc010fa60 T stbsp_snprintf
    {0xffffffffc010fa60ull, "stbsp_snprintf"},
// ffffffffc010fac0 T stbsp_vsprintf
    {0xffffffffc010fac0ull, "stbsp_vsprintf"},
// ffffffffc010fb10 T stbds_arrgrowf
    {0xffffffffc010fb10ull, "stbds_arrgrowf"},
// ffffffffc010fd10 T stbds_rand_seed
    {0xffffffffc010fd10ull, "stbds_rand_seed"},
// ffffffffc010fd30 T stbds_hash_string
    {0xffffffffc010fd30ull, "stbds_hash_string"},
// ffffffffc010fe40 T stbds_hash_bytes
    {0xffffffffc010fe40ull, "stbds_hash_bytes"},
// ffffffffc0110090 t stbds_siphash_bytes
    {0xffffffffc0110090ull, "stbds_siphash_bytes"},
// ffffffffc0110640 T stbds_hmfree_func
    {0xffffffffc0110640ull, "stbds_hmfree_func"},
// ffffffffc0110710 T stbds_strreset
    {0xffffffffc0110710ull, "stbds_strreset"},
// ffffffffc0110780 T stbds_hmget_key_ts
    {0xffffffffc0110780ull, "stbds_hmget_key_ts"},
// ffffffffc01108f0 t stbds_hm_find_slot
    {0xffffffffc01108f0ull, "stbds_hm_find_slot"},
// ffffffffc0110ba0 T stbds_hmget_key
    {0xffffffffc0110ba0ull, "stbds_hmget_key"},
// ffffffffc0110c00 T stbds_hmput_default
    {0xffffffffc0110c00ull, "stbds_hmput_default"},
// ffffffffc0110ce0 T stbds_hmput_key
    {0xffffffffc0110ce0ull, "stbds_hmput_key"},
// ffffffffc0111430 t stbds_make_hash_index
    {0xffffffffc0111430ull, "stbds_make_hash_index"},
// ffffffffc01119c0 t stbds_probe_position
    {0xffffffffc01119c0ull, "stbds_probe_position"},
// ffffffffc0111a00 t stbds_is_key_equal
    {0xffffffffc0111a00ull, "stbds_is_key_equal"},
// ffffffffc0111aa0 t stbds_strdup
    {0xffffffffc0111aa0ull, "stbds_strdup"},
// ffffffffc0111b00 T stbds_stralloc
    {0xffffffffc0111b00ull, "stbds_stralloc"},
// ffffffffc0111cd0 T stbds_shmode_func
    {0xffffffffc0111cd0ull, "stbds_shmode_func"},
// ffffffffc0111d70 T stbds_hmdel_key
    {0xffffffffc0111d70ull, "stbds_hmdel_key"},
// ffffffffc01120d0 t stbds_log2
    {0xffffffffc01120d0ull, "stbds_log2"},
// ffffffffc0116000 d early_current_tpl
// ffffffffc0116008 d handler
// ffffffffc0116038 D pit8254
// ffffffffc0116040 d timers_list
// ffffffffc0116050 d sv_handler
// ffffffffc0116080 D idt
// ffffffffc0116090 d ISR_NAMES
// ffffffffc0116140 d PROCESSOR_NAME
// ffffffffc0116150 d TABLE_NAME
// ffffffffc0116170 d PRESENT_NAMES
// ffffffffc0116180 d USER_NAME
// ffffffffc0116190 d OPERATION_NAME
// ffffffffc01161a0 d gdt_entries
// ffffffffc01161d8 D gdt
// ffffffffc01161f0 d tboot_mmap_names
// ffffffffc0116218 d mem_map
// ffffffffc0116228 d free_entries_list
// ffffffffc0116238 d free_blocks
// ffffffffc0116248 d used_blocks
// ffffffffc0116258 d stack_allocation
// ffffffffc0116260 D process_list
// ffffffffc0116270 D kernel_process
// ffffffffc01162e0 d handler
// ffffffffc0116310 d threads_queue
// ffffffffc0116320 d errors
// ffffffffc0116350 d stbsp__period
// ffffffffc0116351 d stbsp__comma
// ffffffffc0116360 d stbsp_vsprintfcb.hex
// ffffffffc0116380 d stbsp_vsprintfcb.hexu
// ffffffffc0116394 d stbsp__digitpair
// ffffffffc0116460 d stbds_hash_seed
// ffffffffc0116470 D symlist
// ffffffffc0116480 D bsp_pcpu_data
// ffffffffc0117000 B acpi_madt
// ffffffffc0117008 B acpi_rsdp
// ffffffffc0117010 B acpi_xsdp
// ffffffffc0117018 B acpi_rsdt
// ffffffffc0117020 B acpi_xsdt
// ffffffffc0117030 b notify_queues
// ffffffffc0117230 b notify_queues_pending
// ffffffffc0117238 b event_lock
// ffffffffc0117258 b period
// ffffffffc0117260 b timer
// ffffffffc0117268 b check_timers_event
// ffffffffc0117270 b current_system_time
// ffffffffc0117278 b timer_lock
// ffffffffc0117298 b ioapics
// ffffffffc01172a0 b isos
// ffffffffc01172a8 b initialized_once
// ffffffffc01172b0 b idt_entries
// ffffffffc01182b0 b interrupt_handlers
// ffffffffc01192b0 b interrupt_vector
// ffffffffc0119624 b index
// ffffffffc0119628 b control
// ffffffffc011afb0 b mem_memory_top
// ffffffffc011afb8 b memory_base
// ffffffffc011afc0 b lock
// ffffffffc011afe0 b temp_entries
// ffffffffc011b170 b temp_entries_count
// ffffffffc011b178 b freeing
// ffffffffc011b180 b lock
// ffffffffc011b1a0 b memory_base
// ffffffffc011b1a8 b support_1gb
// ffffffffc011b1a9 b support_global
// ffffffffc011b1b0 B process_lock
// ffffffffc011b1d0 b flag
// ffffffffc019b1d4 B kernel_virtual_end
// ffffffffc019b1d4 b tmp_kernel_stack
// 
//not enough values to unpack (expected 3, got 1)
    {0xffffffffffffffff, ""}
};

