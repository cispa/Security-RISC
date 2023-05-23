#! /usr/bin/env python3


from pwn import *

#LIB = "./mbedtls/library/libmbedtls.so"
LIB = "./libmbedtls.so.8"

def get_offsets(libpath):
    context.log_level = 'error'
    context.arch = 'riscv'
    e = ELF(LIB)
    ctr_drbg_init_offset = e.functions['ctr_drbg_init'].address
    print(f"[+] CTR_DRBG_INIT_OFFSET @ 0x{ctr_drbg_init_offset:x}")

    mpi_exp_mod_offset = e.functions['mpi_exp_mod'].address
    print(f"[+] MPI_EXP_MOD_OFFSET @ 0x{mpi_exp_mod_offset:x}")

    # c.nop, c.nop, c.nop, c.nop, unimpl, c.nop
    pattern_bytes = b"\x01\x00\x01\x00\x01\x00\x01\x00\x00\x00\x01\x00"

    patterns_found = e.search(pattern_bytes, executable=True)
    one_offset = next(patterns_found, None)
    if one_offset is None:
        print("\033[91m[-] Could not find ONE_OFFSET address\033[0m")
        exit(1)
    one_offset += 8  # increase to the the actual point of failure
    # assert that we only found one possible address
    assert next(patterns_found, None)  == None
    print(f"[+] ONE_OFFSET @ 0x{one_offset:x}")
    return ctr_drbg_init_offset, mpi_exp_mod_offset, one_offset
    
def main():
    get_offsets(LIB)
    



if __name__ == "__main__":
    main()
