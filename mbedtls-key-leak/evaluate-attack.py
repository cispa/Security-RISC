#! /usr/bin/env python3

import os
import sys
import random
import string
import shutil

NO_OF_DIFFERENT_KEYS = 1
REPETITIONS_PER_MSG = 100
MSG_LEN = 100

DEFAULT_THRESHOLD = 25000

LEAKAGE_FNAME = "./key_leak.log"
KEY_FNAME = "./key.log"
STORED_LEAKAGE_FOLDER = "./risc-attack-eval"
KEYS_FOLDER = "./keys"


# ATTENTION: has to be called from ./mbedtls-key-leak

def create_keys(no_of_keys):
    try:
        os.mkdir(KEYS_FOLDER)
    except FileExistsError:
        pass

    src_priv_key = "./rsa_priv.txt"
    src_pub_key = "./rsa_priv.txt"
    for key_no in range(no_of_keys):
        dst_priv_key = f"{KEYS_FOLDER}/rsa_priv{key_no}.txt"
        dst_pub_key = f"{KEYS_FOLDER}/rsa_pub{key_no}.txt"

        # ATTENTION: to change the key generation to 2048 bytes, you need to set KEY_SIZE in
        # rsa_genkey.c
        os.system("LD_LIBRARY_PATH=./mbedtls/library ./mbedtls/programs/pkey/rsa_genkey")
        shutil.copyfile(src_priv_key, dst_priv_key)
        shutil.copyfile(src_pub_key, dst_pub_key)


def main():
    get_leakage = False
    post_process = False
    generate_keys = False
    if len(sys.argv) == 1:
        print(f"USAGE: {sys.argv[0]} <get-leakage|post-process|generate-keys|> <threshold for post processing>")
    if len(sys.argv) > 1 and sys.argv[1] == "get-leakage":
        get_leakage = True
    if len(sys.argv) > 1 and sys.argv[1] == "post-process":
        post_process = True
    if len(sys.argv) > 1 and sys.argv[1] == "generate-keys":
        generate_keys = True
    if len(sys.argv) > 2:
        user_threshold = int(sys.argv[2])

    if generate_keys:
        create_keys(NO_OF_DIFFERENT_KEYS)
        exit()

    # compile
    if generate_keys or get_leakage:
        os.system("make clean")
        os.system("make decrypt")
        os.system("make encrypt")
    try:
        os.mkdir(STORED_LEAKAGE_FOLDER)
    except FileExistsError:
        pass
    correct_bits_sum = 0
    for keys_no in range(NO_OF_DIFFERENT_KEYS):
        print(f"key {keys_no+1}/{NO_OF_DIFFERENT_KEYS}")
        curr_leak_fname = f"{STORED_LEAKAGE_FOLDER}/{LEAKAGE_FNAME[:-4]}_{keys_no}.csv"
        # attack the crypto
        if get_leakage:
            # change keys
            shutil.copyfile(f"{KEYS_FOLDER}/rsa_priv{keys_no}.txt", "./rsa_priv.txt")
            shutil.copyfile(f"{KEYS_FOLDER}/rsa_pub{keys_no}.txt", "./rsa_pub.txt")

            # generate message
            rand_msg = "".join([random.choice(string.ascii_letters) for _ in range(MSG_LEN)])
            os.system(f"LD_LIBRARY_PATH=./mbedtls/library ./encrypt \"{rand_msg}\" >> log_encrypt")

            try:
                os.remove(LEAKAGE_FNAME)
            except FileNotFoundError:
                pass

            # get decryption traces
            for rep in range(REPETITIONS_PER_MSG):
                os.system("LD_LIBRARY_PATH=./mbedtls/library taskset ./decrypt >> log_decrypt")
            shutil.copyfile(LEAKAGE_FNAME, curr_leak_fname)
            # store used key
            shutil.copyfile("./rsa_priv.txt", f"{STORED_LEAKAGE_FOLDER}/key_{keys_no}.priv")
            shutil.copyfile("./rsa_pub.txt", f"{STORED_LEAKAGE_FOLDER}/key_{keys_no}.pub")

        # analyze leakage
        if post_process:
            import pandas as pd
            import numpy as np
            # skip first 16 rows for the first 16 indices as they are noisy
            df = pd.read_csv(curr_leak_fname, names=["idx", "realbit", "one", "zero"], skiprows=16)
            df_aggr = pd.DataFrame()
            df_aggr['one-min'] = df.groupby("idx")["one"].min()
            df_aggr['one-median'] = df.groupby("idx")["one"].median()
            df_aggr['one-mean'] = df.groupby("idx")["one"].mean()


            if user_threshold != 0:
                threshold = user_threshold
            else:
                threshold = DEFAULT_THRESHOLD
            df_aggr['prediction'] = np.where(df_aggr["one-median"] < threshold, 1, 0)


            df_aggr['realbit'] = df.groupby("idx")["realbit"].min()
            correct_bits = np.where(df_aggr['realbit'] == df_aggr['prediction'], 1, 0).sum()

            #with pd.option_context('display.max_rows', None, 'display.max_columns', None):  # more options can be specified also
            #     print(df_aggr)
            print("Median of 1-bits:", df_aggr.loc[df_aggr['realbit'] == 1, 'one-mean'].median())
            print("Median of 0-bits:", df_aggr.loc[df_aggr['realbit'] == 0, 'one-mean'].median())
            print("Correctly leaked bits: ", correct_bits)
            print(f"Threshold used: {threshold}")
            correct_bits_sum += correct_bits
    if post_process:
        print(f"Correctly leaked bits (avg): {correct_bits_sum/NO_OF_DIFFERENT_KEYS}")
        print(f"Correctly leaked bits (avg): {((correct_bits_sum/NO_OF_DIFFERENT_KEYS)/2048) * 100}%")


if __name__ == "__main__":
    main()
