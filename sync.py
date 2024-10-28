import os, json
from hashlib import sha256

PATH1 = 'client/lib'
PATH2 = 'server/lib'
EXCEPTIONS = ['application.h']

def get_hashes(path):
    hashes = {}
    for file in os.listdir(path):
        descriptor = os.path.join(path, file)
        if os.path.isfile(descriptor) and file not in EXCEPTIONS:
            with open(descriptor, 'rb') as f:
                hashes[file] = sha256(f.read()).hexdigest()

    return hashes


def copyfile(path1, path2):
    with open(path1, 'rb') as f1:
        with open(path2, 'wb') as f2:
            f2.write(
                f1.read()
            )

def copyfiles(path1, path2):
    for file in os.listdir(path1):
        if file not in EXCEPTIONS:
            copyfile(
                os.path.join(path1, file),
                os.path.join(path2, file)
            )

def create_std():
    if get_hashes(PATH1) == get_hashes(PATH2):
        with open('sync_std', 'w') as f:
            json.dump(get_hashes(PATH1), f)

def get_std():
    with open('sync_std') as f:
        return json.load(f)

if __name__ == '__main__':
    print('SYNC')
    std = get_std()

    if get_hashes(PATH1) != std:
        copyfiles(PATH1, PATH2)
        create_std()
        print(f'{PATH1} -> {PATH2}.')
    elif get_hashes(PATH2) != std:
        copyfiles(PATH2, PATH1)
        create_std()
        print(f'{PATH2} -> {PATH1}.')
    else:
        print('NO CHANGES')