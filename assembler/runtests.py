import sys
import glob
import os

if __name__ == "__main__":
    testdir = "./asmprograms/" if len(sys.argv) < 2 else sys.argv[1]
    tests = glob.glob(testdir + "*.asm")
    anss = glob.glob(testdir + "*.bin.ans")
    print("making:")
    os.system("make")
    print("running tests:")
    keep = []
    for test in tests:
        os.system(f"(./asm.exe {test} {test.split('/')[-1].split('.')[-2]}.bin.tmp) >> ./log{test.split('/')[-1].split('.')[-2]}.tmp")
        with open(test.split('/')[-1].split('.')[-2] +".bin.tmp", 'rb') as f:
            tested = False
            for ans in anss:
                if ans.split('/')[-1].split('.')[-3] == test.split('/')[-1].split('.')[-2]:
                    tested = True
                    with open(ans, 'rb') as fa:
                        if (fa.read() == f.read()):
                            print(f"\033[92m{test} passed")
                        else:
                            print(f"\033[91m{test} failed")
                            keep.append(test.split('/')[-1].split('.')[-2] + '.bin.tmp')
                            keep.append('log'+test.split('/')[-1].split('.')[-2] + '.tmp')
            if tested:
                continue
            print(f"\033[93m{test} answer [{testdir + test.split('/')[-1].split('.')[-2]+'.bin.ans'}] not found")
    print("\033[0mcleaning up:")
    for f in glob.glob("*.tmp"):
        if f in keep:
            print(f"\033[96mkeeping failed test {f} as {'.'.join(f.split('.')[:-1])}")
            os.system(f'mv {f} {".".join(f.split(".")[:-1])}')
            continue
        print(f"\033[96mrm'ing {f}")
        os.system(f"rm {f}")
    print("\033[0mcomplete")
