import subprocess
import random
import os

BIN_DIR = "./cmake-build-debug"
MAX_THREADS = 8

MONTECARLO_X = "./montecarlo.exe"
MONTECARLO_MAX_TRIALS_EXP = 4

MANDELBROT_X = "./mandelbrot.exe"
MANDELBROT_POINTS_MUL = [1, 10, 100]


def run_montecarlo_test(threads: int, throws: int) -> None:
    result = subprocess.run([MONTECARLO_X, str(threads), str(throws)], capture_output=True, text=True)
    time = result.stdout.split("\n")[1].split()[2]
    print(f"[{threads}] {time}", end="\t")


def run_montecarlo() -> None:
    print("Run monte-carlo suite...")
    for j in range(MONTECARLO_MAX_TRIALS_EXP):
        throws = random.randint(10000 * (10 ** j), 100000 * (10 ** j))
        print(f"(#{j + 1}/{MONTECARLO_MAX_TRIALS_EXP}) {throws} trails:")
        for i in range(1, MAX_THREADS + 1):
            run_montecarlo_test(i, throws)
        print("")


def run_mandelbrot_test(threads: int, points: int) -> None:
    result = subprocess.run([MANDELBROT_X, str(threads), str(points)], capture_output=True, text=True)
    time = result.stdout.split("\n")[1].split()[2]
    print(f"[{threads}] {time}", end="\t")


def run_mondelbrot() -> None:
    print("Run mandelbrot suite...")
    for k, j in enumerate(MANDELBROT_POINTS_MUL, start=1):
        pnum = random.randint(j * 1000, j * 10000)
        print(f"(#{k}/{len(MANDELBROT_POINTS_MUL)}) {pnum} points:")
        for i in range(1, MAX_THREADS + 1):
            run_mandelbrot_test(i, pnum)
        print("")


def main() -> None:
    os.chdir(BIN_DIR)
    print("Time-measurement tests")
    print(f"Suites: monte-carlo, mandelbrot")
    print("Run tests...\n")
    run_montecarlo()
    print("\n-----------\n")
    run_mondelbrot()


if __name__ == "__main__":
    main()
