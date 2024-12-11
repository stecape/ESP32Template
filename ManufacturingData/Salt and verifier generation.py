import hashlib
import os
import argparse
from colorama import Fore, Style, init

def get_args():
    parser = argparse.ArgumentParser(description="Manufacturing binary image partition creation")
    parser.add_argument('--namespace', type=str, help='Namespace')
    parser.add_argument('--user', type=str, help='Username')
    parser.add_argument('--password', type=str, help='Password')
    parser.add_argument('--serial_no', type=str, help='Serial number')
    args = parser.parse_args()
    return args

def generate_salt(length=16):
    return os.urandom(length)

def get_user_credentials():
    args = get_args()
    namespace = args.namespace if args.namespace is not None else input("Namespace: ")
    user = args.user if args.user is not None else input("Username: ")
    password = args.password if args.password is not None else input("Password: ")
    serial_no = args.serial_no if args.serial_no is not None else input("Serial number: ")
    return namespace, user, password, serial_no

def sha512(data):
    return hashlib.sha512(data).digest()

def calculate_x(salt, username, password):
    # Concatenate username and password with ':'
    user_password = f"{username}:{password}".encode()
    
    # Calculate SHA512(username | ":" | password)
    hash_ip = sha512(user_password)
    
    # Concatenate salt and hash_ip
    salt_hash_ip = salt + hash_ip
    
    # Calculate SHA512(salt | SHA512(username | ":" | password))
    x = sha512(salt_hash_ip)
    
    return int.from_bytes(x, 'big')

def calculate_v(x, g, N):
    return pow(g, x, N)

if __name__ == "__main__":
    # Init colorama
    init()

    #Get credentials and Manufactoring data
    namespace, user, password, serial_no = get_user_credentials()
    print(f"namespace: {namespace}")
    print(f"user: {user}")
    print(f"password: {password}")
    print(f"serial_no: {serial_no}")
    
    salt = generate_salt() # 16 bytes Salt

    # SRP Params
    g = 5  # Base
    N_hex = ("FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD129024E088A67CC74020BBEA63B139B22514A08798E3404DD"
             "EF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7ED"
             "EE386BFB5A899FA5AE9F24117C4B1FE649286651ECE45B3DC2007CB8A163BF0598DA48361C55D39A69163FA8FD24CF5F"
             "83655D23DCA3AD961C62F356208552BB9ED529077096966D670C354E4ABC9804F1746C08CA18217C32905E462E36CE3B"
             "E39E772C180E86039B2783A2EC07A28FB5C55DF06F4C52C9DE2BCBF6955817183995497CEA956AE515D2261898FA0510"
             "15728E5A8AAAC42DAD33170D04507A33A85521ABDF1CBA64ECFB850458DBEF0A8AEA71575D060C7DB3970F85A6E1E4C7"
             "ABF5AE8CDB0933D71E8C94E04A25619DCEE3D2261AD2EE6BF12FFA06D98A0864D87602733EC86A64521F2B18177B200C"
             "BBE117577A615D6C770988C0BAD946E208E24FA074E5AB3143DB5BFCE0FD108E4B82D120A93AD2CAFFFFFFFFFFFFFFFF")

    N = int(N_hex, 16)  # Mod

    x = calculate_x(salt, user, password)
    verifier = calculate_v(x, g, N)
    verifier = verifier.to_bytes((verifier.bit_length() + 7) // 8, 'big')

    salt_hex = salt.hex()
    verifier_hex = verifier.hex()
    #print(f"salt: {salt}")
    #print(f"salt (hex): {salt_hex}")
    #print(f"verifier: {verifier}")
    #print(f"verifier (hex): {verifier_hex}")
    print(Fore.GREEN + Style.BRIGHT + "config.csv" + Style.RESET_ALL)
    print(f"{namespace},namespace,")
    print(f"serial_no,data,string")
    print(f"salt,data,hex2bin")
    print(f"verifier,data,hex2bin")
    print(Fore.GREEN + Style.BRIGHT + "values.csv" + Style.RESET_ALL)
    print(f"serial_no,salt,verifier")
    print(f"{serial_no},{salt_hex},{verifier_hex}")
