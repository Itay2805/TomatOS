#pragma once

#include <stddef.h>

namespace arch::paging {

    constexpr size_t page_size = 4096;
    constexpr size_t page_mask = 0xFFFu;
    constexpr size_t page_shift = 12u;

    inline constexpr size_t size_to_pages(size_t a) {
        return (((a) >> page_shift) + (((a) & page_mask) ? 1u : 0u));
    }

    inline constexpr size_t pages_to_size(size_t a) {
        return ((a) << page_shift);
    }

    //! Vol 3A, Table 4-14. Format of an IA-32e PML4 Entry (PML4E) that References a Page-Directory-Pointer Table
    typedef union _PML4E {
        uint64_t raw;
        struct {
            uint64_t present : 1;     //!< 0      Present
            uint64_t rw : 1;          //!< 1      Read/write; if 0, writes are not allowed
            uint64_t us : 1;          //!< 2      User/supervisor; if 0, user-mode access isn't allowed
            uint64_t pwt : 1;         //!< 3      Page-level write-through
            uint64_t pcd : 1;         //!< 4      Page-level cache disable
            uint64_t access : 1;      //!< 5      Accessed; indicates whether software has accessed the page
            uint64_t _ignored0 : 1;    //!< 6
            uint64_t page_size : 1;    //!< 7      Page-Size; must be 0
            uint64_t _ignored1 : 4;    //!< 8-11
            uint64_t addr : 40;       //!< 12-51  Physical address that the entry points to
            uint64_t _ignored2 : 11;   //!< 52-62
            uint64_t nx : 1;          //!< 63 If  IA32_EFER.NXE = 1, execute-disable
        };
    } PML4E;
    static_assert(sizeof(uint64_t) == sizeof(PML4E));

    //! Vol 3A, Table 4-16. Format of an IA-32e Page-Directory-Pointer-Table Entry (PDPTE) that References a Page Directory
    typedef union _PDPTE
    {
        uint64_t raw;
        struct {
            uint64_t present : 1;     //!< 0      Present
            uint64_t rw : 1;          //!< 1      Read/write; if 0, writes are not allowed
            uint64_t us : 1;          //!< 2      User/supervisor; if 0, user-mode access isn't allowed
            uint64_t pwt : 1;         //!< 3      Page-level write-through
            uint64_t pcd : 1;         //!< 4      Page-level cache disable
            uint64_t access : 1;      //!< 5      Accessed; indicates whether software has accessed the page
            uint64_t dirty : 1;       //!< 6      Dirty; indicates whether software has written to the page
            uint64_t page_size : 1;    //!< 7      Page-Size; must be 0 to refernce PDE
            uint64_t _reserved1 : 4;   //!< 8-11
            uint64_t addr : 40;       //!< 12-51  Physical address that the entry points to
            uint64_t _reserved2 : 11;  //!< 52-62
            uint64_t nx : 1;          //!< 63     If IA32_EFER.NXE = 1, execute-disable
        };
    } PDPTE;
    static_assert(sizeof(uint64_t) == sizeof(PDPTE));

    //! Vol 3A, Table 4-18. Format of an IA-32e Page-Directory Entry that References a Page Table
    typedef union _PDE
    {
        uint64_t raw;
        struct {
            uint64_t present : 1;     //!< 0      Present
            uint64_t rw : 1;          //!< 1      Read/write; if 0, writes are not allowed
            uint64_t us : 1;          //!< 2      User/supervisor; if 0, user-mode access isn't allowed
            uint64_t pwt : 1;         //!< 3      Page-level write-through
            uint64_t pcd : 1;         //!< 4      Page-level cache disable
            uint64_t access : 1;      //!< 5      Accessed; indicates whether software has accessed the page
            uint64_t reserved0 : 1;   //!< 6
            uint64_t page_size : 1;    //!< 7      Page-Size; must be 0 to reference PTE
            uint64_t reserved1 : 4;   //!< 8-11
            uint64_t addr : 40;       //!< 12-51  Physical address that the entry points to
            uint64_t reserved2 : 11;  //!< 52-62
            uint64_t nx : 1;          //!< 63     If IA32_EFER.NXE = 1, execute-disable
        };
    } PDE;
    static_assert(sizeof(uint64_t) == sizeof(PDE));

    //! Vol 3A, Table 4-19. Format of an IA-32e Page-Table Entry that Maps a 4-KByte Page
    typedef union _PTE
    {
        uint64_t qwValue;
        struct {
            uint64_t present : 1;     //!< 0      Present
            uint64_t rw : 1;          //!< 1      Read/write; if 0, writes are not allowed
            uint64_t us : 1;          //!< 2      User/supervisor; if 0, user-mode access isn't allowed
            uint64_t pwt : 1;         //!< 3      Page-level write-through
            uint64_t pcd : 1;         //!< 4      Page-level cache disable
            uint64_t access : 1;      //!< 5      Accessed; indicates whether software has accessed the page
            uint64_t dirty : 1;       //!< 6      Dirty; indicates whether software has written to the page
            uint64_t pat : 1;         //!< 7      Page Attribute Table;
            uint64_t global : 1;      //!< 8      Global; if CR4.PGE = 1, determines whether the translation is global
            uint64_t ignored0 : 3;    //!< 9-11
            uint64_t addr : 40;       //!< 12-51  Physical address that the entry points to
            uint64_t _ignored1 : 7;    //!< 52-58
            uint64_t prot_key : 4;     //!< 59-62  Protection key; if CR4.PKE = 1, determines the protection key of the page
            uint64_t nx : 1;          //!< 63     If IA32_EFER.NXE = 1, execute-disable
        };
    } PTE;
    static_assert(sizeof(uint64_t) == sizeof(PTE));

    typedef union _VA_ADDRESS
    {
        uint64_t raw;

        //! Figure 4-8. Linear-Address Translation to a 4-KByte Page using IA-32e Paging
        struct {
            uint64_t offset : 12;     //!< 0-11
            uint64_t pte : 9;    //!< 12-20
            uint64_t pde : 9;    //!< 21-29
            uint64_t pdpte : 9;  //!< 30-38
            uint64_t pml4e : 9;  //!< 39-47
            uint64_t _reserved0 : 16;  //!< 48-63
        } four_kb;

        //! Figure 4-9. Linear-Address Translation to a 2-MByte Page using IA-32e Paging
        struct {
            uint64_t offset : 21;     //!< 0-20
            uint64_t pde : 9;    //!< 21-29
            uint64_t pdpte : 9;  //!< 30-38
            uint64_t pml4e : 9;  //!< 39-47
            uint64_t _reserved0 : 16;  //!< 48-63
        } two_mb;

        //! Figure 4-10. Linear-Address Translation to a 1-GByte Page using IA-32e Paging
        struct {
            uint64_t offset : 30;     //!< 0-29
            uint64_t pdpte : 9;  //!< 30-38
            uint64_t pml4e : 9;  //!< 39-47
            uint64_t _reserved0 : 16;  //!< 48-63
        } one_gb;
    } VA_ADDRESS;
    static_assert(sizeof(uint64_t) == sizeof(VA_ADDRESS));

}
