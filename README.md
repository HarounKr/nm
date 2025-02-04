# nm

**nm** is a C project that replicates the behavior of the UNIX `nm` utility, which is used to examine binary files.

## Features
- Displays symbol tables of binary files
- Mimics the behavior of the original `nm` command
- Supports ELF (Executable and Linkable Format) files

## Usage

1. Clone the repository:
   ```bash
   git clone <repository_url>
   ```
2. Navigate to the project root directory:
    ```bash
   cd nm
   ```
3. Compile the project using **Make**:
    ```bash
    cd nm
    ```
4. Run : 
    ```bash
    ./ft_nm <binary_file>
    ```
## Example : 
```bash
./ft_nm tester/good_files/a.out
```

**Output**:
```bash
0000000000601028 B __bss_start
0000000000601028 b completed.7698
0000000000601018 D __data_start
0000000000601018 W data_start
00000000004003f0 t deregister_tm_clones
00000000004003e0 T _dl_relocate_static_pie
0000000000400460 t __do_global_dtors_aux
0000000000600e58 d __do_global_dtors_aux_fini_array_entry
0000000000601020 D __dso_handle
0000000000600e60 d _DYNAMIC
0000000000601028 D _edata
0000000000601030 B _end
0000000000400524 T _fini
0000000000400490 t frame_dummy
0000000000600e50 d __frame_dummy_init_array_entry
000000000040063c r __FRAME_END__
0000000000601000 d _GLOBAL_OFFSET_TABLE_
                 w __gmon_start__
0000000000400534 r __GNU_EH_FRAME_HDR
0000000000400390 T _init
0000000000600e58 d __init_array_end
0000000000600e50 d __init_array_start
0000000000400530 R _IO_stdin_used
0000000000400520 T __libc_csu_fini
00000000004004b0 T __libc_csu_init
                 U __libc_start_main@@GLIBC_2.2.5
00000000004004a0 T main
0000000000400420 t register_tm_clones
00000000004003b0 T _start
0000000000601028 D __TMC_END__
```