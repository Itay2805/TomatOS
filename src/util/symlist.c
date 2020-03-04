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
// ffffffffc01021e0 T get_next_component
    {0xffffffffc01021e0ull, "get_next_component"},
// ffffffffc0102200 T get_primary
    {0xffffffffc0102200ull, "get_primary"},
// ffffffffc0102300 T set_primary
    {0xffffffffc0102300ull, "set_primary"},
// ffffffffc0102340 T file_read
    {0xffffffffc0102340ull, "file_read"},
// ffffffffc0102480 T file_seek
    {0xffffffffc0102480ull, "file_seek"},
// ffffffffc0102550 T file_close
    {0xffffffffc0102550ull, "file_close"},
// ffffffffc0102610 T tar_tell
    {0xffffffffc0102610ull, "tar_tell"},
// ffffffffc0102720 T tar_read
    {0xffffffffc0102720ull, "tar_read"},
// ffffffffc0102940 T create_initrd_fs
    {0xffffffffc0102940ull, "create_initrd_fs"},
// ffffffffc0102aa0 t tar_size
    {0xffffffffc0102aa0ull, "tar_size"},
// ffffffffc0102b20 t tar_open
    {0xffffffffc0102b20ull, "tar_open"},
// ffffffffc0102d20 t tar_is_readonly
    {0xffffffffc0102d20ull, "tar_is_readonly"},
// ffffffffc0102da0 t tar_close
    {0xffffffffc0102da0ull, "tar_close"},
// ffffffffc0102e20 t tar_eof
    {0xffffffffc0102e20ull, "tar_eof"},
// ffffffffc0102f40 t tar_seek
    {0xffffffffc0102f40ull, "tar_seek"},
// ffffffffc01031d0 T sha1_transform
    {0xffffffffc01031d0ull, "sha1_transform"},
// ffffffffc0103580 T sha1_init
    {0xffffffffc0103580ull, "sha1_init"},
// ffffffffc0103610 T sha1_update
    {0xffffffffc0103610ull, "sha1_update"},
// ffffffffc01036c0 T sha1_final
    {0xffffffffc01036c0ull, "sha1_final"},
// ffffffffc0103980 T events_init
    {0xffffffffc0103980ull, "events_init"},
// ffffffffc01039f0 T raise_tpl
    {0xffffffffc01039f0ull, "raise_tpl"},
// ffffffffc0103a80 T set_tpl
    {0xffffffffc0103a80ull, "set_tpl"},
// ffffffffc0103ae0 T restore_tpl
    {0xffffffffc0103ae0ull, "restore_tpl"},
// ffffffffc0103bd0 t dispatch_event_notifies
    {0xffffffffc0103bd0ull, "dispatch_event_notifies"},
// ffffffffc0103d20 T get_tpl
    {0xffffffffc0103d20ull, "get_tpl"},
// ffffffffc0103d80 T create_event
    {0xffffffffc0103d80ull, "create_event"},
// ffffffffc0103ea0 T signal_event
    {0xffffffffc0103ea0ull, "signal_event"},
// ffffffffc0103ff0 T check_event
    {0xffffffffc0103ff0ull, "check_event"},
// ffffffffc01040c0 T wait_for_event
    {0xffffffffc01040c0ull, "wait_for_event"},
// ffffffffc01043f0 T close_event
    {0xffffffffc01043f0ull, "close_event"},
// ffffffffc01044a0 t aquire_event_lock
    {0xffffffffc01044a0ull, "aquire_event_lock"},
// ffffffffc01044c0 t release_event_lock
    {0xffffffffc01044c0ull, "release_event_lock"},
// ffffffffc01044e0 T init_pit8254
    {0xffffffffc01044e0ull, "init_pit8254"},
// ffffffffc0104530 t set_timer_period
    {0xffffffffc0104530ull, "set_timer_period"},
// ffffffffc01045e0 t interrupt_handler
    {0xffffffffc01045e0ull, "interrupt_handler"},
// ffffffffc0104610 t set_pit_count
    {0xffffffffc0104610ull, "set_pit_count"},
// ffffffffc0104670 T init_timer
    {0xffffffffc0104670ull, "init_timer"},
// ffffffffc01047c0 t check_timers
    {0xffffffffc01047c0ull, "check_timers"},
// ffffffffc0104910 T timer_tick
    {0xffffffffc0104910ull, "timer_tick"},
// ffffffffc0104980 T get_current_system_time
    {0xffffffffc0104980ull, "get_current_system_time"},
// ffffffffc01049a0 T set_timer
    {0xffffffffc01049a0ull, "set_timer"},
// ffffffffc0104b70 t insert_event_timer
    {0xffffffffc0104b70ull, "insert_event_timer"},
// ffffffffc0104c10 T ioapic_add
    {0xffffffffc0104c10ull, "ioapic_add"},
// ffffffffc0104d30 t ioapic_read
    {0xffffffffc0104d30ull, "ioapic_read"},
// ffffffffc0104db0 T ioapic_add_iso
    {0xffffffffc0104db0ull, "ioapic_add_iso"},
// ffffffffc0104e40 T ioapic_enable_interrupt
    {0xffffffffc0104e40ull, "ioapic_enable_interrupt"},
// ffffffffc0104fc0 t get_ioapic
    {0xffffffffc0104fc0ull, "get_ioapic"},
// ffffffffc01050e0 t ioapic_write
    {0xffffffffc01050e0ull, "ioapic_write"},
// ffffffffc0105160 T ioapic_configure_interrupt
    {0xffffffffc0105160ull, "ioapic_configure_interrupt"},
// ffffffffc0105450 T get_apic_id
    {0xffffffffc0105450ull, "get_apic_id"},
// ffffffffc0105470 t lapic_read
    {0xffffffffc0105470ull, "lapic_read"},
// ffffffffc01054a0 T lapic_init
    {0xffffffffc01054a0ull, "lapic_init"},
// ffffffffc01055c0 t lapic_write
    {0xffffffffc01055c0ull, "lapic_write"},
// ffffffffc01055f0 T send_fixed_ipi
    {0xffffffffc01055f0ull, "send_fixed_ipi"},
// ffffffffc0105610 T send_apic_eoi
    {0xffffffffc0105610ull, "send_apic_eoi"},
// ffffffffc0105630 t spurious_interrupt_handler
    {0xffffffffc0105630ull, "spurious_interrupt_handler"},
// ffffffffc0105650 T idt_init
    {0xffffffffc0105650ull, "idt_init"},
// ffffffffc0106970 t set_idt_entry
    {0xffffffffc0106970ull, "set_idt_entry"},
// ffffffffc0106b40 T idt_post_tss_init
    {0xffffffffc0106b40ull, "idt_post_tss_init"},
// ffffffffc0106cd0 T interrupts_init
    {0xffffffffc0106cd0ull, "interrupts_init"},
// ffffffffc0106d40 T common_interrupt_handler
    {0xffffffffc0106d40ull, "common_interrupt_handler"},
// ffffffffc0106ee0 t default_interrupt_handler
    {0xffffffffc0106ee0ull, "default_interrupt_handler"},
// ffffffffc0107740 T interrupt_register
    {0xffffffffc0107740ull, "interrupt_register"},
// ffffffffc01078d0 T interrupt_free
    {0xffffffffc01078d0ull, "interrupt_free"},
// ffffffffc0107960 t interrupt_allocate
    {0xffffffffc0107960ull, "interrupt_allocate"},
// ffffffffc0107ad0 T kernel_main
    {0xffffffffc0107ad0ull, "kernel_main"},
// ffffffffc0107c70 t main_thread
    {0xffffffffc0107c70ull, "main_thread"},
// ffffffffc0107e00 T init_tss_for_cpu
    {0xffffffffc0107e00ull, "init_tss_for_cpu"},
// ffffffffc0107ee0 T kernel_tlsf_map
    {0xffffffffc0107ee0ull, "kernel_tlsf_map"},
// ffffffffc0107f80 T kernel_tlsf_unmap
    {0xffffffffc0107f80ull, "kernel_tlsf_unmap"},
// ffffffffc0107fc0 T mm_init
    {0xffffffffc0107fc0ull, "mm_init"},
// ffffffffc0108040 T mm_allocate
    {0xffffffffc0108040ull, "mm_allocate"},
// ffffffffc0108070 t tlsf_calloc
    {0xffffffffc0108070ull, "tlsf_calloc"},
// ffffffffc01080a0 T mm_reallocate
    {0xffffffffc01080a0ull, "mm_reallocate"},
// ffffffffc01080d0 t tlsf_realloc
    {0xffffffffc01080d0ull, "tlsf_realloc"},
// ffffffffc0108110 T mm_free
    {0xffffffffc0108110ull, "mm_free"},
// ffffffffc0108140 T pmm_init
    {0xffffffffc0108140ull, "pmm_init"},
// ffffffffc01082c0 t add_range
    {0xffffffffc01082c0ull, "add_range"},
// ffffffffc0108530 T pmm_post_vmm
    {0xffffffffc0108530ull, "pmm_post_vmm"},
// ffffffffc0108780 T pmm_allocate_pages
    {0xffffffffc0108780ull, "pmm_allocate_pages"},
// ffffffffc0108b10 t find_free_pages
    {0xffffffffc0108b10ull, "find_free_pages"},
// ffffffffc0108d20 t convert_page
    {0xffffffffc0108d20ull, "convert_page"},
// ffffffffc0109290 T pmm_free_pages
    {0xffffffffc0109290ull, "pmm_free_pages"},
// ffffffffc01093a0 t remove_mem_map_entry
    {0xffffffffc01093a0ull, "remove_mem_map_entry"},
// ffffffffc01093f0 t move_temp_entries
    {0xffffffffc01093f0ull, "move_temp_entries"},
// ffffffffc01095b0 t allocate_memory_map_entry
    {0xffffffffc01095b0ull, "allocate_memory_map_entry"},
// ffffffffc0109680 t allocate_pool_pages
    {0xffffffffc0109680ull, "allocate_pool_pages"},
// ffffffffc0109720 T allocate_stack
    {0xffffffffc0109720ull, "allocate_stack"},
// ffffffffc0109920 T free_stack
    {0xffffffffc0109920ull, "free_stack"},
// ffffffffc0109a30 t acquire_alloc_lock
    {0xffffffffc0109a30ull, "acquire_alloc_lock"},
// ffffffffc0109a50 t release_alloc_lock
    {0xffffffffc0109a50ull, "release_alloc_lock"},
// ffffffffc0109a70 T vmm_user_allocate
    {0xffffffffc0109a70ull, "vmm_user_allocate"},
// ffffffffc0109ec0 t vmm_umem_find_free_pages
    {0xffffffffc0109ec0ull, "vmm_umem_find_free_pages"},
// ffffffffc010a0d0 t vmm_umem_convert_page
    {0xffffffffc010a0d0ull, "vmm_umem_convert_page"},
// ffffffffc010a580 T vmm_user_free
    {0xffffffffc010a580ull, "vmm_user_free"},
// ffffffffc010a7a0 T vmm_create_user_mem
    {0xffffffffc010a7a0ull, "vmm_create_user_mem"},
// ffffffffc010a810 t vmm_umem_add_range
    {0xffffffffc010a810ull, "vmm_umem_add_range"},
// ffffffffc010a9e0 T vmm_destroy_user_mem
    {0xffffffffc010a9e0ull, "vmm_destroy_user_mem"},
// ffffffffc010ab00 t vmm_umem_remove_memory_map_entry
    {0xffffffffc010ab00ull, "vmm_umem_remove_memory_map_entry"},
// ffffffffc010ab70 t allocate_block
    {0xffffffffc010ab70ull, "allocate_block"},
// ffffffffc010acc0 T vmm_init
    {0xffffffffc010acc0ull, "vmm_init"},
// ffffffffc010aea0 T vmm_map
    {0xffffffffc010aea0ull, "vmm_map"},
// ffffffffc010b110 T vmm_set_handle
    {0xffffffffc010b110ull, "vmm_set_handle"},
// ffffffffc010b150 T vmm_enable_cpu_features
    {0xffffffffc010b150ull, "vmm_enable_cpu_features"},
// ffffffffc010b300 T vmm_virtual_to_physical
    {0xffffffffc010b300ull, "vmm_virtual_to_physical"},
// ffffffffc010b500 t get_entry_at_virtual_address
    {0xffffffffc010b500ull, "get_entry_at_virtual_address"},
// ffffffffc010b780 T vmm_is_mapped
    {0xffffffffc010b780ull, "vmm_is_mapped"},
// ffffffffc010b900 t page_size_by_type
    {0xffffffffc010b900ull, "page_size_by_type"},
// ffffffffc010b9b0 T vmm_unmap
    {0xffffffffc010b9b0ull, "vmm_unmap"},
// ffffffffc010bbf0 t set_pte
    {0xffffffffc010bbf0ull, "set_pte"},
// ffffffffc010c790 T vmm_set_perms
    {0xffffffffc010c790ull, "vmm_set_perms"},
// ffffffffc010c960 T vmm_get_handle
    {0xffffffffc010c960ull, "vmm_get_handle"},
// ffffffffc010c970 T vmm_create_address_space
    {0xffffffffc010c970ull, "vmm_create_address_space"},
// ffffffffc010caa0 T vmm_destroy_page_table
    {0xffffffffc010caa0ull, "vmm_destroy_page_table"},
// ffffffffc010ce20 T load_elf
    {0xffffffffc010ce20ull, "load_elf"},
// ffffffffc010d450 T init_kernel_process
    {0xffffffffc010d450ull, "init_kernel_process"},
// ffffffffc010d470 T create_process
    {0xffffffffc010d470ull, "create_process"},
// ffffffffc010d5b0 T spawn_process
    {0xffffffffc010d5b0ull, "spawn_process"},
// ffffffffc010d8c0 T yield
    {0xffffffffc010d8c0ull, "yield"},
// ffffffffc010d920 T init_sched
    {0xffffffffc010d920ull, "init_sched"},
// ffffffffc010da70 t preemption_callback
    {0xffffffffc010da70ull, "preemption_callback"},
// ffffffffc010da90 T queue_thread
    {0xffffffffc010da90ull, "queue_thread"},
// ffffffffc010db50 T sched_tick
    {0xffffffffc010db50ull, "sched_tick"},
// ffffffffc010ddb0 t yield_interrupt
    {0xffffffffc010ddb0ull, "yield_interrupt"},
// ffffffffc010de20 t idle_thread_func
    {0xffffffffc010de20ull, "idle_thread_func"},
// ffffffffc010de50 t sched_lock
    {0xffffffffc010de50ull, "sched_lock"},
// ffffffffc010de90 t sched_unlock
    {0xffffffffc010de90ull, "sched_unlock"},
// ffffffffc010deb0 T spawn_thread
    {0xffffffffc010deb0ull, "spawn_thread"},
// ffffffffc010e190 T pcpu_init_for_cpu
    {0xffffffffc010e190ull, "pcpu_init_for_cpu"},
// ffffffffc010e1c0 T pcpu_init_for_bsp
    {0xffffffffc010e1c0ull, "pcpu_init_for_bsp"},
// ffffffffc010e1e0 T get_pcpu_base
    {0xffffffffc010e1e0ull, "get_pcpu_base"},
// ffffffffc010e1f0 T spinlock_acquire_high_tpl
    {0xffffffffc010e1f0ull, "spinlock_acquire_high_tpl"},
// ffffffffc010e260 T spinlock_acquire
    {0xffffffffc010e260ull, "spinlock_acquire"},
// ffffffffc010e2b0 T spinlock_release
    {0xffffffffc010e2b0ull, "spinlock_release"},
// ffffffffc010e2f0 T strerror
    {0xffffffffc010e2f0ull, "strerror"},
// ffffffffc010e340 T is_node_in_list
    {0xffffffffc010e340ull, "is_node_in_list"},
// ffffffffc010e440 t is_list_valid
    {0xffffffffc010e440ull, "is_list_valid"},
// ffffffffc010e550 T initialize_list_had
    {0xffffffffc010e550ull, "initialize_list_had"},
// ffffffffc010e5d0 T insert_head_list
    {0xffffffffc010e5d0ull, "insert_head_list"},
// ffffffffc010e670 T insert_tail_list
    {0xffffffffc010e670ull, "insert_tail_list"},
// ffffffffc010e710 T get_first_node
    {0xffffffffc010e710ull, "get_first_node"},
// ffffffffc010e780 T get_next_node
    {0xffffffffc010e780ull, "get_next_node"},
// ffffffffc010e7f0 T get_prev_node
    {0xffffffffc010e7f0ull, "get_prev_node"},
// ffffffffc010e860 T is_list_empty
    {0xffffffffc010e860ull, "is_list_empty"},
// ffffffffc010e8e0 T is_null
    {0xffffffffc010e8e0ull, "is_null"},
// ffffffffc010e910 T is_node_at_end
    {0xffffffffc010e910ull, "is_node_at_end"},
// ffffffffc010e960 T swap_list_entries
    {0xffffffffc010e960ull, "swap_list_entries"},
// ffffffffc010ea80 T remove_entry_list
    {0xffffffffc010ea80ull, "remove_entry_list"},
// ffffffffc010eb20 T serial_init
    {0xffffffffc010eb20ull, "serial_init"},
// ffffffffc010eba0 T serial_write
    {0xffffffffc010eba0ull, "serial_write"},
// ffffffffc010ec70 T serial_read
    {0xffffffffc010ec70ull, "serial_read"},
// ffffffffc010ecf0 T serial_poll
    {0xffffffffc010ecf0ull, "serial_poll"},
// ffffffffc010ed20 T trace
    {0xffffffffc010ed20ull, "trace"},
// ffffffffc010ee90 t common_stub
    {0xffffffffc010ee90ull, "common_stub"},
// ffffffffc010eedc T handle_interrupt_request_00
    {0xffffffffc010eedcull, "handle_interrupt_request_00"},
// ffffffffc010eee2 T handle_interrupt_request_01
    {0xffffffffc010eee2ull, "handle_interrupt_request_01"},
// ffffffffc010eee8 T handle_interrupt_request_02
    {0xffffffffc010eee8ull, "handle_interrupt_request_02"},
// ffffffffc010eeee T handle_interrupt_request_03
    {0xffffffffc010eeeeull, "handle_interrupt_request_03"},
// ffffffffc010eef4 T handle_interrupt_request_04
    {0xffffffffc010eef4ull, "handle_interrupt_request_04"},
// ffffffffc010eefa T handle_interrupt_request_05
    {0xffffffffc010eefaull, "handle_interrupt_request_05"},
// ffffffffc010ef00 T handle_interrupt_request_06
    {0xffffffffc010ef00ull, "handle_interrupt_request_06"},
// ffffffffc010ef06 T handle_interrupt_request_07
    {0xffffffffc010ef06ull, "handle_interrupt_request_07"},
// ffffffffc010ef0c T handle_interrupt_request_08
    {0xffffffffc010ef0cull, "handle_interrupt_request_08"},
// ffffffffc010ef10 T handle_interrupt_request_09
    {0xffffffffc010ef10ull, "handle_interrupt_request_09"},
// ffffffffc010ef19 T handle_interrupt_request_0a
    {0xffffffffc010ef19ull, "handle_interrupt_request_0a"},
// ffffffffc010ef20 T handle_interrupt_request_0b
    {0xffffffffc010ef20ull, "handle_interrupt_request_0b"},
// ffffffffc010ef27 T handle_interrupt_request_0c
    {0xffffffffc010ef27ull, "handle_interrupt_request_0c"},
// ffffffffc010ef2e T handle_interrupt_request_0d
    {0xffffffffc010ef2eull, "handle_interrupt_request_0d"},
// ffffffffc010ef35 T handle_interrupt_request_0e
    {0xffffffffc010ef35ull, "handle_interrupt_request_0e"},
// ffffffffc010ef3c T handle_interrupt_request_0f
    {0xffffffffc010ef3cull, "handle_interrupt_request_0f"},
// ffffffffc010ef45 T handle_interrupt_request_10
    {0xffffffffc010ef45ull, "handle_interrupt_request_10"},
// ffffffffc010ef4c T handle_interrupt_request_11
    {0xffffffffc010ef4cull, "handle_interrupt_request_11"},
// ffffffffc010ef55 T handle_interrupt_request_12
    {0xffffffffc010ef55ull, "handle_interrupt_request_12"},
// ffffffffc010ef5e T handle_interrupt_request_13
    {0xffffffffc010ef5eull, "handle_interrupt_request_13"},
// ffffffffc010ef67 T handle_interrupt_request_14
    {0xffffffffc010ef67ull, "handle_interrupt_request_14"},
// ffffffffc010ef70 T handle_interrupt_request_15
    {0xffffffffc010ef70ull, "handle_interrupt_request_15"},
// ffffffffc010ef79 T handle_interrupt_request_16
    {0xffffffffc010ef79ull, "handle_interrupt_request_16"},
// ffffffffc010ef82 T handle_interrupt_request_17
    {0xffffffffc010ef82ull, "handle_interrupt_request_17"},
// ffffffffc010ef8b T handle_interrupt_request_18
    {0xffffffffc010ef8bull, "handle_interrupt_request_18"},
// ffffffffc010ef94 T handle_interrupt_request_19
    {0xffffffffc010ef94ull, "handle_interrupt_request_19"},
// ffffffffc010ef9d T handle_interrupt_request_1a
    {0xffffffffc010ef9dull, "handle_interrupt_request_1a"},
// ffffffffc010efa6 T handle_interrupt_request_1b
    {0xffffffffc010efa6ull, "handle_interrupt_request_1b"},
// ffffffffc010efaf T handle_interrupt_request_1c
    {0xffffffffc010efafull, "handle_interrupt_request_1c"},
// ffffffffc010efb8 T handle_interrupt_request_1d
    {0xffffffffc010efb8ull, "handle_interrupt_request_1d"},
// ffffffffc010efc1 T handle_interrupt_request_1e
    {0xffffffffc010efc1ull, "handle_interrupt_request_1e"},
// ffffffffc010efc8 T handle_interrupt_request_1f
    {0xffffffffc010efc8ull, "handle_interrupt_request_1f"},
// ffffffffc010efd1 T handle_interrupt_request_20
    {0xffffffffc010efd1ull, "handle_interrupt_request_20"},
// ffffffffc010efda T handle_interrupt_request_21
    {0xffffffffc010efdaull, "handle_interrupt_request_21"},
// ffffffffc010efe3 T handle_interrupt_request_22
    {0xffffffffc010efe3ull, "handle_interrupt_request_22"},
// ffffffffc010efec T handle_interrupt_request_23
    {0xffffffffc010efecull, "handle_interrupt_request_23"},
// ffffffffc010eff5 T handle_interrupt_request_24
    {0xffffffffc010eff5ull, "handle_interrupt_request_24"},
// ffffffffc010effe T handle_interrupt_request_25
    {0xffffffffc010effeull, "handle_interrupt_request_25"},
// ffffffffc010f007 T handle_interrupt_request_26
    {0xffffffffc010f007ull, "handle_interrupt_request_26"},
// ffffffffc010f010 T handle_interrupt_request_27
    {0xffffffffc010f010ull, "handle_interrupt_request_27"},
// ffffffffc010f019 T handle_interrupt_request_28
    {0xffffffffc010f019ull, "handle_interrupt_request_28"},
// ffffffffc010f022 T handle_interrupt_request_29
    {0xffffffffc010f022ull, "handle_interrupt_request_29"},
// ffffffffc010f02b T handle_interrupt_request_2a
    {0xffffffffc010f02bull, "handle_interrupt_request_2a"},
// ffffffffc010f034 T handle_interrupt_request_2b
    {0xffffffffc010f034ull, "handle_interrupt_request_2b"},
// ffffffffc010f03d T handle_interrupt_request_2c
    {0xffffffffc010f03dull, "handle_interrupt_request_2c"},
// ffffffffc010f046 T handle_interrupt_request_2d
    {0xffffffffc010f046ull, "handle_interrupt_request_2d"},
// ffffffffc010f04f T handle_interrupt_request_2e
    {0xffffffffc010f04full, "handle_interrupt_request_2e"},
// ffffffffc010f058 T handle_interrupt_request_2f
    {0xffffffffc010f058ull, "handle_interrupt_request_2f"},
// ffffffffc010f061 T handle_interrupt_request_30
    {0xffffffffc010f061ull, "handle_interrupt_request_30"},
// ffffffffc010f06a T handle_interrupt_request_31
    {0xffffffffc010f06aull, "handle_interrupt_request_31"},
// ffffffffc010f073 T handle_interrupt_request_32
    {0xffffffffc010f073ull, "handle_interrupt_request_32"},
// ffffffffc010f07c T handle_interrupt_request_33
    {0xffffffffc010f07cull, "handle_interrupt_request_33"},
// ffffffffc010f085 T handle_interrupt_request_34
    {0xffffffffc010f085ull, "handle_interrupt_request_34"},
// ffffffffc010f08e T handle_interrupt_request_35
    {0xffffffffc010f08eull, "handle_interrupt_request_35"},
// ffffffffc010f097 T handle_interrupt_request_36
    {0xffffffffc010f097ull, "handle_interrupt_request_36"},
// ffffffffc010f0a0 T handle_interrupt_request_37
    {0xffffffffc010f0a0ull, "handle_interrupt_request_37"},
// ffffffffc010f0a9 T handle_interrupt_request_38
    {0xffffffffc010f0a9ull, "handle_interrupt_request_38"},
// ffffffffc010f0b2 T handle_interrupt_request_39
    {0xffffffffc010f0b2ull, "handle_interrupt_request_39"},
// ffffffffc010f0bb T handle_interrupt_request_3a
    {0xffffffffc010f0bbull, "handle_interrupt_request_3a"},
// ffffffffc010f0c4 T handle_interrupt_request_3b
    {0xffffffffc010f0c4ull, "handle_interrupt_request_3b"},
// ffffffffc010f0cd T handle_interrupt_request_3c
    {0xffffffffc010f0cdull, "handle_interrupt_request_3c"},
// ffffffffc010f0d6 T handle_interrupt_request_3d
    {0xffffffffc010f0d6ull, "handle_interrupt_request_3d"},
// ffffffffc010f0df T handle_interrupt_request_3e
    {0xffffffffc010f0dfull, "handle_interrupt_request_3e"},
// ffffffffc010f0e8 T handle_interrupt_request_3f
    {0xffffffffc010f0e8ull, "handle_interrupt_request_3f"},
// ffffffffc010f0f1 T handle_interrupt_request_40
    {0xffffffffc010f0f1ull, "handle_interrupt_request_40"},
// ffffffffc010f0fa T handle_interrupt_request_41
    {0xffffffffc010f0faull, "handle_interrupt_request_41"},
// ffffffffc010f103 T handle_interrupt_request_42
    {0xffffffffc010f103ull, "handle_interrupt_request_42"},
// ffffffffc010f10c T handle_interrupt_request_43
    {0xffffffffc010f10cull, "handle_interrupt_request_43"},
// ffffffffc010f115 T handle_interrupt_request_44
    {0xffffffffc010f115ull, "handle_interrupt_request_44"},
// ffffffffc010f11e T handle_interrupt_request_45
    {0xffffffffc010f11eull, "handle_interrupt_request_45"},
// ffffffffc010f127 T handle_interrupt_request_46
    {0xffffffffc010f127ull, "handle_interrupt_request_46"},
// ffffffffc010f130 T handle_interrupt_request_47
    {0xffffffffc010f130ull, "handle_interrupt_request_47"},
// ffffffffc010f139 T handle_interrupt_request_48
    {0xffffffffc010f139ull, "handle_interrupt_request_48"},
// ffffffffc010f142 T handle_interrupt_request_49
    {0xffffffffc010f142ull, "handle_interrupt_request_49"},
// ffffffffc010f14b T handle_interrupt_request_4a
    {0xffffffffc010f14bull, "handle_interrupt_request_4a"},
// ffffffffc010f154 T handle_interrupt_request_4b
    {0xffffffffc010f154ull, "handle_interrupt_request_4b"},
// ffffffffc010f15d T handle_interrupt_request_4c
    {0xffffffffc010f15dull, "handle_interrupt_request_4c"},
// ffffffffc010f166 T handle_interrupt_request_4d
    {0xffffffffc010f166ull, "handle_interrupt_request_4d"},
// ffffffffc010f16f T handle_interrupt_request_4e
    {0xffffffffc010f16full, "handle_interrupt_request_4e"},
// ffffffffc010f178 T handle_interrupt_request_4f
    {0xffffffffc010f178ull, "handle_interrupt_request_4f"},
// ffffffffc010f181 T handle_interrupt_request_50
    {0xffffffffc010f181ull, "handle_interrupt_request_50"},
// ffffffffc010f18a T handle_interrupt_request_51
    {0xffffffffc010f18aull, "handle_interrupt_request_51"},
// ffffffffc010f193 T handle_interrupt_request_52
    {0xffffffffc010f193ull, "handle_interrupt_request_52"},
// ffffffffc010f19c T handle_interrupt_request_53
    {0xffffffffc010f19cull, "handle_interrupt_request_53"},
// ffffffffc010f1a5 T handle_interrupt_request_54
    {0xffffffffc010f1a5ull, "handle_interrupt_request_54"},
// ffffffffc010f1ae T handle_interrupt_request_55
    {0xffffffffc010f1aeull, "handle_interrupt_request_55"},
// ffffffffc010f1b7 T handle_interrupt_request_56
    {0xffffffffc010f1b7ull, "handle_interrupt_request_56"},
// ffffffffc010f1c0 T handle_interrupt_request_57
    {0xffffffffc010f1c0ull, "handle_interrupt_request_57"},
// ffffffffc010f1c9 T handle_interrupt_request_58
    {0xffffffffc010f1c9ull, "handle_interrupt_request_58"},
// ffffffffc010f1d2 T handle_interrupt_request_59
    {0xffffffffc010f1d2ull, "handle_interrupt_request_59"},
// ffffffffc010f1db T handle_interrupt_request_5a
    {0xffffffffc010f1dbull, "handle_interrupt_request_5a"},
// ffffffffc010f1e4 T handle_interrupt_request_5b
    {0xffffffffc010f1e4ull, "handle_interrupt_request_5b"},
// ffffffffc010f1ed T handle_interrupt_request_5c
    {0xffffffffc010f1edull, "handle_interrupt_request_5c"},
// ffffffffc010f1f6 T handle_interrupt_request_5d
    {0xffffffffc010f1f6ull, "handle_interrupt_request_5d"},
// ffffffffc010f1ff T handle_interrupt_request_5e
    {0xffffffffc010f1ffull, "handle_interrupt_request_5e"},
// ffffffffc010f208 T handle_interrupt_request_5f
    {0xffffffffc010f208ull, "handle_interrupt_request_5f"},
// ffffffffc010f211 T handle_interrupt_request_60
    {0xffffffffc010f211ull, "handle_interrupt_request_60"},
// ffffffffc010f21a T handle_interrupt_request_61
    {0xffffffffc010f21aull, "handle_interrupt_request_61"},
// ffffffffc010f223 T handle_interrupt_request_62
    {0xffffffffc010f223ull, "handle_interrupt_request_62"},
// ffffffffc010f22c T handle_interrupt_request_63
    {0xffffffffc010f22cull, "handle_interrupt_request_63"},
// ffffffffc010f235 T handle_interrupt_request_64
    {0xffffffffc010f235ull, "handle_interrupt_request_64"},
// ffffffffc010f23e T handle_interrupt_request_65
    {0xffffffffc010f23eull, "handle_interrupt_request_65"},
// ffffffffc010f247 T handle_interrupt_request_66
    {0xffffffffc010f247ull, "handle_interrupt_request_66"},
// ffffffffc010f250 T handle_interrupt_request_67
    {0xffffffffc010f250ull, "handle_interrupt_request_67"},
// ffffffffc010f259 T handle_interrupt_request_68
    {0xffffffffc010f259ull, "handle_interrupt_request_68"},
// ffffffffc010f262 T handle_interrupt_request_69
    {0xffffffffc010f262ull, "handle_interrupt_request_69"},
// ffffffffc010f26b T handle_interrupt_request_6a
    {0xffffffffc010f26bull, "handle_interrupt_request_6a"},
// ffffffffc010f274 T handle_interrupt_request_6b
    {0xffffffffc010f274ull, "handle_interrupt_request_6b"},
// ffffffffc010f27d T handle_interrupt_request_6c
    {0xffffffffc010f27dull, "handle_interrupt_request_6c"},
// ffffffffc010f286 T handle_interrupt_request_6d
    {0xffffffffc010f286ull, "handle_interrupt_request_6d"},
// ffffffffc010f28f T handle_interrupt_request_6e
    {0xffffffffc010f28full, "handle_interrupt_request_6e"},
// ffffffffc010f298 T handle_interrupt_request_6f
    {0xffffffffc010f298ull, "handle_interrupt_request_6f"},
// ffffffffc010f2a1 T handle_interrupt_request_70
    {0xffffffffc010f2a1ull, "handle_interrupt_request_70"},
// ffffffffc010f2aa T handle_interrupt_request_71
    {0xffffffffc010f2aaull, "handle_interrupt_request_71"},
// ffffffffc010f2b3 T handle_interrupt_request_72
    {0xffffffffc010f2b3ull, "handle_interrupt_request_72"},
// ffffffffc010f2bc T handle_interrupt_request_73
    {0xffffffffc010f2bcull, "handle_interrupt_request_73"},
// ffffffffc010f2c5 T handle_interrupt_request_74
    {0xffffffffc010f2c5ull, "handle_interrupt_request_74"},
// ffffffffc010f2ce T handle_interrupt_request_75
    {0xffffffffc010f2ceull, "handle_interrupt_request_75"},
// ffffffffc010f2d7 T handle_interrupt_request_76
    {0xffffffffc010f2d7ull, "handle_interrupt_request_76"},
// ffffffffc010f2e0 T handle_interrupt_request_77
    {0xffffffffc010f2e0ull, "handle_interrupt_request_77"},
// ffffffffc010f2e9 T handle_interrupt_request_78
    {0xffffffffc010f2e9ull, "handle_interrupt_request_78"},
// ffffffffc010f2f2 T handle_interrupt_request_79
    {0xffffffffc010f2f2ull, "handle_interrupt_request_79"},
// ffffffffc010f2fb T handle_interrupt_request_7a
    {0xffffffffc010f2fbull, "handle_interrupt_request_7a"},
// ffffffffc010f304 T handle_interrupt_request_7b
    {0xffffffffc010f304ull, "handle_interrupt_request_7b"},
// ffffffffc010f30d T handle_interrupt_request_7c
    {0xffffffffc010f30dull, "handle_interrupt_request_7c"},
// ffffffffc010f316 T handle_interrupt_request_7d
    {0xffffffffc010f316ull, "handle_interrupt_request_7d"},
// ffffffffc010f31f T handle_interrupt_request_7e
    {0xffffffffc010f31full, "handle_interrupt_request_7e"},
// ffffffffc010f328 T handle_interrupt_request_7f
    {0xffffffffc010f328ull, "handle_interrupt_request_7f"},
// ffffffffc010f331 T handle_interrupt_request_80
    {0xffffffffc010f331ull, "handle_interrupt_request_80"},
// ffffffffc010f33d T handle_interrupt_request_81
    {0xffffffffc010f33dull, "handle_interrupt_request_81"},
// ffffffffc010f349 T handle_interrupt_request_82
    {0xffffffffc010f349ull, "handle_interrupt_request_82"},
// ffffffffc010f355 T handle_interrupt_request_83
    {0xffffffffc010f355ull, "handle_interrupt_request_83"},
// ffffffffc010f361 T handle_interrupt_request_84
    {0xffffffffc010f361ull, "handle_interrupt_request_84"},
// ffffffffc010f36d T handle_interrupt_request_85
    {0xffffffffc010f36dull, "handle_interrupt_request_85"},
// ffffffffc010f379 T handle_interrupt_request_86
    {0xffffffffc010f379ull, "handle_interrupt_request_86"},
// ffffffffc010f385 T handle_interrupt_request_87
    {0xffffffffc010f385ull, "handle_interrupt_request_87"},
// ffffffffc010f391 T handle_interrupt_request_88
    {0xffffffffc010f391ull, "handle_interrupt_request_88"},
// ffffffffc010f39d T handle_interrupt_request_89
    {0xffffffffc010f39dull, "handle_interrupt_request_89"},
// ffffffffc010f3a9 T handle_interrupt_request_8a
    {0xffffffffc010f3a9ull, "handle_interrupt_request_8a"},
// ffffffffc010f3b5 T handle_interrupt_request_8b
    {0xffffffffc010f3b5ull, "handle_interrupt_request_8b"},
// ffffffffc010f3c1 T handle_interrupt_request_8c
    {0xffffffffc010f3c1ull, "handle_interrupt_request_8c"},
// ffffffffc010f3cd T handle_interrupt_request_8d
    {0xffffffffc010f3cdull, "handle_interrupt_request_8d"},
// ffffffffc010f3d9 T handle_interrupt_request_8e
    {0xffffffffc010f3d9ull, "handle_interrupt_request_8e"},
// ffffffffc010f3e5 T handle_interrupt_request_8f
    {0xffffffffc010f3e5ull, "handle_interrupt_request_8f"},
// ffffffffc010f3f1 T handle_interrupt_request_90
    {0xffffffffc010f3f1ull, "handle_interrupt_request_90"},
// ffffffffc010f3fd T handle_interrupt_request_91
    {0xffffffffc010f3fdull, "handle_interrupt_request_91"},
// ffffffffc010f409 T handle_interrupt_request_92
    {0xffffffffc010f409ull, "handle_interrupt_request_92"},
// ffffffffc010f415 T handle_interrupt_request_93
    {0xffffffffc010f415ull, "handle_interrupt_request_93"},
// ffffffffc010f421 T handle_interrupt_request_94
    {0xffffffffc010f421ull, "handle_interrupt_request_94"},
// ffffffffc010f42d T handle_interrupt_request_95
    {0xffffffffc010f42dull, "handle_interrupt_request_95"},
// ffffffffc010f439 T handle_interrupt_request_96
    {0xffffffffc010f439ull, "handle_interrupt_request_96"},
// ffffffffc010f445 T handle_interrupt_request_97
    {0xffffffffc010f445ull, "handle_interrupt_request_97"},
// ffffffffc010f451 T handle_interrupt_request_98
    {0xffffffffc010f451ull, "handle_interrupt_request_98"},
// ffffffffc010f45d T handle_interrupt_request_99
    {0xffffffffc010f45dull, "handle_interrupt_request_99"},
// ffffffffc010f469 T handle_interrupt_request_9a
    {0xffffffffc010f469ull, "handle_interrupt_request_9a"},
// ffffffffc010f475 T handle_interrupt_request_9b
    {0xffffffffc010f475ull, "handle_interrupt_request_9b"},
// ffffffffc010f481 T handle_interrupt_request_9c
    {0xffffffffc010f481ull, "handle_interrupt_request_9c"},
// ffffffffc010f48d T handle_interrupt_request_9d
    {0xffffffffc010f48dull, "handle_interrupt_request_9d"},
// ffffffffc010f499 T handle_interrupt_request_9e
    {0xffffffffc010f499ull, "handle_interrupt_request_9e"},
// ffffffffc010f4a5 T handle_interrupt_request_9f
    {0xffffffffc010f4a5ull, "handle_interrupt_request_9f"},
// ffffffffc010f4b1 T handle_interrupt_request_a0
    {0xffffffffc010f4b1ull, "handle_interrupt_request_a0"},
// ffffffffc010f4bd T handle_interrupt_request_a1
    {0xffffffffc010f4bdull, "handle_interrupt_request_a1"},
// ffffffffc010f4c9 T handle_interrupt_request_a2
    {0xffffffffc010f4c9ull, "handle_interrupt_request_a2"},
// ffffffffc010f4d5 T handle_interrupt_request_a3
    {0xffffffffc010f4d5ull, "handle_interrupt_request_a3"},
// ffffffffc010f4e1 T handle_interrupt_request_a4
    {0xffffffffc010f4e1ull, "handle_interrupt_request_a4"},
// ffffffffc010f4ed T handle_interrupt_request_a5
    {0xffffffffc010f4edull, "handle_interrupt_request_a5"},
// ffffffffc010f4f9 T handle_interrupt_request_a6
    {0xffffffffc010f4f9ull, "handle_interrupt_request_a6"},
// ffffffffc010f505 T handle_interrupt_request_a7
    {0xffffffffc010f505ull, "handle_interrupt_request_a7"},
// ffffffffc010f511 T handle_interrupt_request_a8
    {0xffffffffc010f511ull, "handle_interrupt_request_a8"},
// ffffffffc010f51d T handle_interrupt_request_a9
    {0xffffffffc010f51dull, "handle_interrupt_request_a9"},
// ffffffffc010f529 T handle_interrupt_request_aa
    {0xffffffffc010f529ull, "handle_interrupt_request_aa"},
// ffffffffc010f535 T handle_interrupt_request_ab
    {0xffffffffc010f535ull, "handle_interrupt_request_ab"},
// ffffffffc010f541 T handle_interrupt_request_ac
    {0xffffffffc010f541ull, "handle_interrupt_request_ac"},
// ffffffffc010f54d T handle_interrupt_request_ad
    {0xffffffffc010f54dull, "handle_interrupt_request_ad"},
// ffffffffc010f559 T handle_interrupt_request_ae
    {0xffffffffc010f559ull, "handle_interrupt_request_ae"},
// ffffffffc010f565 T handle_interrupt_request_af
    {0xffffffffc010f565ull, "handle_interrupt_request_af"},
// ffffffffc010f571 T handle_interrupt_request_b0
    {0xffffffffc010f571ull, "handle_interrupt_request_b0"},
// ffffffffc010f57d T handle_interrupt_request_b1
    {0xffffffffc010f57dull, "handle_interrupt_request_b1"},
// ffffffffc010f589 T handle_interrupt_request_b2
    {0xffffffffc010f589ull, "handle_interrupt_request_b2"},
// ffffffffc010f595 T handle_interrupt_request_b3
    {0xffffffffc010f595ull, "handle_interrupt_request_b3"},
// ffffffffc010f5a1 T handle_interrupt_request_b4
    {0xffffffffc010f5a1ull, "handle_interrupt_request_b4"},
// ffffffffc010f5ad T handle_interrupt_request_b5
    {0xffffffffc010f5adull, "handle_interrupt_request_b5"},
// ffffffffc010f5b9 T handle_interrupt_request_b6
    {0xffffffffc010f5b9ull, "handle_interrupt_request_b6"},
// ffffffffc010f5c5 T handle_interrupt_request_b7
    {0xffffffffc010f5c5ull, "handle_interrupt_request_b7"},
// ffffffffc010f5d1 T handle_interrupt_request_b8
    {0xffffffffc010f5d1ull, "handle_interrupt_request_b8"},
// ffffffffc010f5dd T handle_interrupt_request_b9
    {0xffffffffc010f5ddull, "handle_interrupt_request_b9"},
// ffffffffc010f5e9 T handle_interrupt_request_ba
    {0xffffffffc010f5e9ull, "handle_interrupt_request_ba"},
// ffffffffc010f5f5 T handle_interrupt_request_bb
    {0xffffffffc010f5f5ull, "handle_interrupt_request_bb"},
// ffffffffc010f601 T handle_interrupt_request_bc
    {0xffffffffc010f601ull, "handle_interrupt_request_bc"},
// ffffffffc010f60d T handle_interrupt_request_bd
    {0xffffffffc010f60dull, "handle_interrupt_request_bd"},
// ffffffffc010f619 T handle_interrupt_request_be
    {0xffffffffc010f619ull, "handle_interrupt_request_be"},
// ffffffffc010f625 T handle_interrupt_request_bf
    {0xffffffffc010f625ull, "handle_interrupt_request_bf"},
// ffffffffc010f631 T handle_interrupt_request_c0
    {0xffffffffc010f631ull, "handle_interrupt_request_c0"},
// ffffffffc010f63d T handle_interrupt_request_c1
    {0xffffffffc010f63dull, "handle_interrupt_request_c1"},
// ffffffffc010f649 T handle_interrupt_request_c2
    {0xffffffffc010f649ull, "handle_interrupt_request_c2"},
// ffffffffc010f655 T handle_interrupt_request_c3
    {0xffffffffc010f655ull, "handle_interrupt_request_c3"},
// ffffffffc010f661 T handle_interrupt_request_c4
    {0xffffffffc010f661ull, "handle_interrupt_request_c4"},
// ffffffffc010f66d T handle_interrupt_request_c5
    {0xffffffffc010f66dull, "handle_interrupt_request_c5"},
// ffffffffc010f679 T handle_interrupt_request_c6
    {0xffffffffc010f679ull, "handle_interrupt_request_c6"},
// ffffffffc010f685 T handle_interrupt_request_c7
    {0xffffffffc010f685ull, "handle_interrupt_request_c7"},
// ffffffffc010f691 T handle_interrupt_request_c8
    {0xffffffffc010f691ull, "handle_interrupt_request_c8"},
// ffffffffc010f69d T handle_interrupt_request_c9
    {0xffffffffc010f69dull, "handle_interrupt_request_c9"},
// ffffffffc010f6a9 T handle_interrupt_request_ca
    {0xffffffffc010f6a9ull, "handle_interrupt_request_ca"},
// ffffffffc010f6b5 T handle_interrupt_request_cb
    {0xffffffffc010f6b5ull, "handle_interrupt_request_cb"},
// ffffffffc010f6c1 T handle_interrupt_request_cc
    {0xffffffffc010f6c1ull, "handle_interrupt_request_cc"},
// ffffffffc010f6cd T handle_interrupt_request_cd
    {0xffffffffc010f6cdull, "handle_interrupt_request_cd"},
// ffffffffc010f6d9 T handle_interrupt_request_ce
    {0xffffffffc010f6d9ull, "handle_interrupt_request_ce"},
// ffffffffc010f6e5 T handle_interrupt_request_cf
    {0xffffffffc010f6e5ull, "handle_interrupt_request_cf"},
// ffffffffc010f6f1 T handle_interrupt_request_d0
    {0xffffffffc010f6f1ull, "handle_interrupt_request_d0"},
// ffffffffc010f6fd T handle_interrupt_request_d1
    {0xffffffffc010f6fdull, "handle_interrupt_request_d1"},
// ffffffffc010f709 T handle_interrupt_request_d2
    {0xffffffffc010f709ull, "handle_interrupt_request_d2"},
// ffffffffc010f715 T handle_interrupt_request_d3
    {0xffffffffc010f715ull, "handle_interrupt_request_d3"},
// ffffffffc010f721 T handle_interrupt_request_d4
    {0xffffffffc010f721ull, "handle_interrupt_request_d4"},
// ffffffffc010f72d T handle_interrupt_request_d5
    {0xffffffffc010f72dull, "handle_interrupt_request_d5"},
// ffffffffc010f739 T handle_interrupt_request_d6
    {0xffffffffc010f739ull, "handle_interrupt_request_d6"},
// ffffffffc010f745 T handle_interrupt_request_d7
    {0xffffffffc010f745ull, "handle_interrupt_request_d7"},
// ffffffffc010f751 T handle_interrupt_request_d8
    {0xffffffffc010f751ull, "handle_interrupt_request_d8"},
// ffffffffc010f75d T handle_interrupt_request_d9
    {0xffffffffc010f75dull, "handle_interrupt_request_d9"},
// ffffffffc010f769 T handle_interrupt_request_da
    {0xffffffffc010f769ull, "handle_interrupt_request_da"},
// ffffffffc010f775 T handle_interrupt_request_db
    {0xffffffffc010f775ull, "handle_interrupt_request_db"},
// ffffffffc010f781 T handle_interrupt_request_dc
    {0xffffffffc010f781ull, "handle_interrupt_request_dc"},
// ffffffffc010f78d T handle_interrupt_request_dd
    {0xffffffffc010f78dull, "handle_interrupt_request_dd"},
// ffffffffc010f799 T handle_interrupt_request_de
    {0xffffffffc010f799ull, "handle_interrupt_request_de"},
// ffffffffc010f7a5 T handle_interrupt_request_df
    {0xffffffffc010f7a5ull, "handle_interrupt_request_df"},
// ffffffffc010f7b1 T handle_interrupt_request_e0
    {0xffffffffc010f7b1ull, "handle_interrupt_request_e0"},
// ffffffffc010f7bd T handle_interrupt_request_e1
    {0xffffffffc010f7bdull, "handle_interrupt_request_e1"},
// ffffffffc010f7c9 T handle_interrupt_request_e2
    {0xffffffffc010f7c9ull, "handle_interrupt_request_e2"},
// ffffffffc010f7d5 T handle_interrupt_request_e3
    {0xffffffffc010f7d5ull, "handle_interrupt_request_e3"},
// ffffffffc010f7e1 T handle_interrupt_request_e4
    {0xffffffffc010f7e1ull, "handle_interrupt_request_e4"},
// ffffffffc010f7ed T handle_interrupt_request_e5
    {0xffffffffc010f7edull, "handle_interrupt_request_e5"},
// ffffffffc010f7f9 T handle_interrupt_request_e6
    {0xffffffffc010f7f9ull, "handle_interrupt_request_e6"},
// ffffffffc010f805 T handle_interrupt_request_e7
    {0xffffffffc010f805ull, "handle_interrupt_request_e7"},
// ffffffffc010f811 T handle_interrupt_request_e8
    {0xffffffffc010f811ull, "handle_interrupt_request_e8"},
// ffffffffc010f81d T handle_interrupt_request_e9
    {0xffffffffc010f81dull, "handle_interrupt_request_e9"},
// ffffffffc010f829 T handle_interrupt_request_ea
    {0xffffffffc010f829ull, "handle_interrupt_request_ea"},
// ffffffffc010f835 T handle_interrupt_request_eb
    {0xffffffffc010f835ull, "handle_interrupt_request_eb"},
// ffffffffc010f841 T handle_interrupt_request_ec
    {0xffffffffc010f841ull, "handle_interrupt_request_ec"},
// ffffffffc010f84d T handle_interrupt_request_ed
    {0xffffffffc010f84dull, "handle_interrupt_request_ed"},
// ffffffffc010f859 T handle_interrupt_request_ee
    {0xffffffffc010f859ull, "handle_interrupt_request_ee"},
// ffffffffc010f865 T handle_interrupt_request_ef
    {0xffffffffc010f865ull, "handle_interrupt_request_ef"},
// ffffffffc010f871 T handle_interrupt_request_f0
    {0xffffffffc010f871ull, "handle_interrupt_request_f0"},
// ffffffffc010f87d T handle_interrupt_request_f1
    {0xffffffffc010f87dull, "handle_interrupt_request_f1"},
// ffffffffc010f889 T handle_interrupt_request_f2
    {0xffffffffc010f889ull, "handle_interrupt_request_f2"},
// ffffffffc010f895 T handle_interrupt_request_f3
    {0xffffffffc010f895ull, "handle_interrupt_request_f3"},
// ffffffffc010f8a1 T handle_interrupt_request_f4
    {0xffffffffc010f8a1ull, "handle_interrupt_request_f4"},
// ffffffffc010f8ad T handle_interrupt_request_f5
    {0xffffffffc010f8adull, "handle_interrupt_request_f5"},
// ffffffffc010f8b9 T handle_interrupt_request_f6
    {0xffffffffc010f8b9ull, "handle_interrupt_request_f6"},
// ffffffffc010f8c5 T handle_interrupt_request_f7
    {0xffffffffc010f8c5ull, "handle_interrupt_request_f7"},
// ffffffffc010f8d1 T handle_interrupt_request_f8
    {0xffffffffc010f8d1ull, "handle_interrupt_request_f8"},
// ffffffffc010f8dd T handle_interrupt_request_f9
    {0xffffffffc010f8ddull, "handle_interrupt_request_f9"},
// ffffffffc010f8e9 T handle_interrupt_request_fa
    {0xffffffffc010f8e9ull, "handle_interrupt_request_fa"},
// ffffffffc010f8f5 T handle_interrupt_request_fb
    {0xffffffffc010f8f5ull, "handle_interrupt_request_fb"},
// ffffffffc010f901 T handle_interrupt_request_fc
    {0xffffffffc010f901ull, "handle_interrupt_request_fc"},
// ffffffffc010f90d T handle_interrupt_request_fd
    {0xffffffffc010f90dull, "handle_interrupt_request_fd"},
// ffffffffc010f919 T handle_interrupt_request_fe
    {0xffffffffc010f919ull, "handle_interrupt_request_fe"},
// ffffffffc010f925 T handle_interrupt_request_ff
    {0xffffffffc010f925ull, "handle_interrupt_request_ff"},
// ffffffffc010f940 t fix_data_segment
    {0xffffffffc010f940ull, "fix_data_segment"},
// ffffffffc010f951 T gdt_fix_segments
    {0xffffffffc010f951ull, "gdt_fix_segments"},
// ffffffffc010f970 T memset
    {0xffffffffc010f970ull, "memset"},
// ffffffffc010f9d0 T memcmp
    {0xffffffffc010f9d0ull, "memcmp"},
// ffffffffc010fa70 T memcpy
    {0xffffffffc010fa70ull, "memcpy"},
// ffffffffc010faf0 T memmove
    {0xffffffffc010faf0ull, "memmove"},
// ffffffffc010fbf0 T strcmp
    {0xffffffffc010fbf0ull, "strcmp"},
// ffffffffc010fc80 T strlen
    {0xffffffffc010fc80ull, "strlen"},
// ffffffffc010fcd0 T stbsp_set_separators
    {0xffffffffc010fcd0ull, "stbsp_set_separators"},
// ffffffffc010fd00 T stbsp_vsprintfcb
    {0xffffffffc010fd00ull, "stbsp_vsprintfcb"},
// ffffffffc01120f0 t stbsp__lead_sign
    {0xffffffffc01120f0ull, "stbsp__lead_sign"},
// ffffffffc0112180 T stbsp_sprintf
    {0xffffffffc0112180ull, "stbsp_sprintf"},
// ffffffffc01121f0 T stbsp_vsnprintf
    {0xffffffffc01121f0ull, "stbsp_vsnprintf"},
// ffffffffc0112330 t stbsp__count_clamp_callback
    {0xffffffffc0112330ull, "stbsp__count_clamp_callback"},
// ffffffffc0112370 t stbsp__clamp_callback
    {0xffffffffc0112370ull, "stbsp__clamp_callback"},
// ffffffffc01124b0 T stbsp_snprintf
    {0xffffffffc01124b0ull, "stbsp_snprintf"},
// ffffffffc0112510 T stbsp_vsprintf
    {0xffffffffc0112510ull, "stbsp_vsprintf"},
// ffffffffc0112560 T stbds_arrgrowf
    {0xffffffffc0112560ull, "stbds_arrgrowf"},
// ffffffffc0112760 T stbds_rand_seed
    {0xffffffffc0112760ull, "stbds_rand_seed"},
// ffffffffc0112780 T stbds_hash_string
    {0xffffffffc0112780ull, "stbds_hash_string"},
// ffffffffc0112890 T stbds_hash_bytes
    {0xffffffffc0112890ull, "stbds_hash_bytes"},
// ffffffffc0112ae0 t stbds_siphash_bytes
    {0xffffffffc0112ae0ull, "stbds_siphash_bytes"},
// ffffffffc0113090 T stbds_hmfree_func
    {0xffffffffc0113090ull, "stbds_hmfree_func"},
// ffffffffc0113160 T stbds_strreset
    {0xffffffffc0113160ull, "stbds_strreset"},
// ffffffffc01131d0 T stbds_hmget_key_ts
    {0xffffffffc01131d0ull, "stbds_hmget_key_ts"},
// ffffffffc0113340 t stbds_hm_find_slot
    {0xffffffffc0113340ull, "stbds_hm_find_slot"},
// ffffffffc01135f0 T stbds_hmget_key
    {0xffffffffc01135f0ull, "stbds_hmget_key"},
// ffffffffc0113650 T stbds_hmput_default
    {0xffffffffc0113650ull, "stbds_hmput_default"},
// ffffffffc0113730 T stbds_hmput_key
    {0xffffffffc0113730ull, "stbds_hmput_key"},
// ffffffffc0113e80 t stbds_make_hash_index
    {0xffffffffc0113e80ull, "stbds_make_hash_index"},
// ffffffffc0114410 t stbds_probe_position
    {0xffffffffc0114410ull, "stbds_probe_position"},
// ffffffffc0114450 t stbds_is_key_equal
    {0xffffffffc0114450ull, "stbds_is_key_equal"},
// ffffffffc01144f0 t stbds_strdup
    {0xffffffffc01144f0ull, "stbds_strdup"},
// ffffffffc0114550 T stbds_stralloc
    {0xffffffffc0114550ull, "stbds_stralloc"},
// ffffffffc0114720 T stbds_shmode_func
    {0xffffffffc0114720ull, "stbds_shmode_func"},
// ffffffffc01147c0 T stbds_hmdel_key
    {0xffffffffc01147c0ull, "stbds_hmdel_key"},
// ffffffffc0114b20 t stbds_log2
    {0xffffffffc0114b20ull, "stbds_log2"},
// ffffffffc0114b70 T tlsf_create
    {0xffffffffc0114b70ull, "tlsf_create"},
// ffffffffc0114cd0 t add_pool
    {0xffffffffc0114cd0ull, "add_pool"},
// ffffffffc0114d70 T tlsf_destroy
    {0xffffffffc0114d70ull, "tlsf_destroy"},
// ffffffffc0114df0 t block_size
    {0xffffffffc0114df0ull, "block_size"},
// ffffffffc0114e20 T tlsf_mallocx
    {0xffffffffc0114e20ull, "tlsf_mallocx"},
// ffffffffc0114f70 t adjust_size
    {0xffffffffc0114f70ull, "adjust_size"},
// ffffffffc0114fc0 t block_locate_free
    {0xffffffffc0114fc0ull, "block_locate_free"},
// ffffffffc0115040 t round_block_size
    {0xffffffffc0115040ull, "round_block_size"},
// ffffffffc01150b0 t block_trim_free
    {0xffffffffc01150b0ull, "block_trim_free"},
// ffffffffc0115120 t block_set_free
    {0xffffffffc0115120ull, "block_set_free"},
// ffffffffc01151a0 t block_to_ptr
    {0xffffffffc01151a0ull, "block_to_ptr"},
// ffffffffc01151c0 T tlsf_free
    {0xffffffffc01151c0ull, "tlsf_free"},
// ffffffffc0115290 t block_from_ptr
    {0xffffffffc0115290ull, "block_from_ptr"},
// ffffffffc01152b0 t block_merge_prev
    {0xffffffffc01152b0ull, "block_merge_prev"},
// ffffffffc0115310 t block_merge_next
    {0xffffffffc0115310ull, "block_merge_next"},
// ffffffffc0115370 t block_is_last
    {0xffffffffc0115370ull, "block_is_last"},
// ffffffffc01153a0 t block_next
    {0xffffffffc01153a0ull, "block_next"},
// ffffffffc01153e0 t remove_pool
    {0xffffffffc01153e0ull, "remove_pool"},
// ffffffffc0115440 t block_insert
    {0xffffffffc0115440ull, "block_insert"},
// ffffffffc0115490 T tlsf_reallocx
    {0xffffffffc0115490ull, "tlsf_reallocx"},
// ffffffffc01156b0 t block_is_free
    {0xffffffffc01156b0ull, "block_is_free"},
// ffffffffc01156f0 t block_set_prev_free
    {0xffffffffc01156f0ull, "block_set_prev_free"},
// ffffffffc0115750 t block_trim_used
    {0xffffffffc0115750ull, "block_trim_used"},
// ffffffffc01157d0 t block_link_next
    {0xffffffffc01157d0ull, "block_link_next"},
// ffffffffc0115800 t align_up
    {0xffffffffc0115800ull, "align_up"},
// ffffffffc0115820 t mapping_insert
    {0xffffffffc0115820ull, "mapping_insert"},
// ffffffffc01158b0 t search_suitable_block
    {0xffffffffc01158b0ull, "search_suitable_block"},
// ffffffffc01159a0 t remove_free_block
    {0xffffffffc01159a0ull, "remove_free_block"},
// ffffffffc0115ab0 t tlsf_fls
    {0xffffffffc0115ab0ull, "tlsf_fls"},
// ffffffffc0115b10 t tlsf_ffs
    {0xffffffffc0115b10ull, "tlsf_ffs"},
// ffffffffc0115b40 t block_can_split
    {0xffffffffc0115b40ull, "block_can_split"},
// ffffffffc0115b80 t block_split
    {0xffffffffc0115b80ull, "block_split"},
// ffffffffc0115c00 t block_set_size
    {0xffffffffc0115c00ull, "block_set_size"},
// ffffffffc0115c40 t block_is_prev_free
    {0xffffffffc0115c40ull, "block_is_prev_free"},
// ffffffffc0115c80 t block_prev
    {0xffffffffc0115c80ull, "block_prev"},
// ffffffffc0115ca0 t block_remove
    {0xffffffffc0115ca0ull, "block_remove"},
// ffffffffc0115cf0 t block_absorb
    {0xffffffffc0115cf0ull, "block_absorb"},
// ffffffffc0115d40 t insert_free_block
    {0xffffffffc0115d40ull, "insert_free_block"},
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

