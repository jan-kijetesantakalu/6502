import sys
import glob
import os
import platform

if __name__ == "__main__":
    print("cleaning old log files")
    for f in glob.glob("*.log"):
        print(f"\033[096mrm'ing {f}")
        os.system(f"rm {f}")
    testdir = "./asmprograms/" if len(sys.argv) < 2 else sys.argv[1]
    tests = glob.glob(testdir + "*.asm")
    anss = glob.glob(testdir + "*.bin.ans")
    print("\033[0mmaking:")
    os.system("make")
    print("running tests:")
    keep = []
    for test in tests:
        if "windows" in platform.system().lower():
            os.system(f"(asm.exe {test} {os.path.split(test)[-1].split('.')[-2]}.bin.tmp) > {os.path.split(test)[-1].split('.')[-2]}.tmp")
        else:
            os.system(f"(./asm.exe {test} ./{os.path.split(test)[-1].split('.')[-2]}.bin.tmp) > ./{os.path.split(test)[-1].split('.')[-2]}.tmp")
        with open(os.path.split(test)[-1].split('.')[-2] +".bin.tmp", 'rb') as f:
            tested = False
            for ans in anss:
                if os.path.split(ans)[-1].split('.')[-3] == os.path.split(test)[-1].split('.')[-2]:
                    tested = True
                    with open(ans, 'rb') as fa:
                        if (fa.read() == f.read()):
                            print(f"\033[92m{test} passed")
                        else:
                            print(f"\033[91m{test} failed")
                            keep.append(os.path.split(test)[-1].split('.')[-2] + '.bin.tmp')
                            keep.append(os.path.split(test)[-1].split('.')[-2] + '.tmp')
            if tested:
                continue
            print(f"\033[93m{test} answer [{testdir + os.path.split(test)[-1].split('.')[-2]+'.bin.ans'}] not found")
    print("\033[0mcleaning up:")
    for f in glob.glob("*.tmp"):
        if f in keep:
            print(f"\033[96mkeeping failed test {f} as {'.'.join(f.split('.')[:-1])}.log")
            os.system(f'mv {f} {".".join(f.split(".")[:-1])}.log')
            continue
        print(f"\033[96mrm'ing {f}")
        os.system(f"rm {f}")
    print("\033[0mcomplete")
