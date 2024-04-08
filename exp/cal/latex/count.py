import re
import numpy as np
import os

result = "/pub/netdisk1/linpeng/Local-ILP/result"
samp_dir = "/pub/netdisk1/linpeng/Local-ILP/benchmark/list/open-hard-union.txt"
MAXV = 1e20


def get_liftStep(instance, solver, time):
    fstr = open(f"{result}/{solver}/log/{time}/{instance}", "r").read()
    if not len(re.findall(r"liftStep: .*", fstr)) == 0:
        res = re.findall(
            r"liftStep: .*", fstr)[-1].split()
        try:
            if res[-1] == "-nan":
                return -1
            res = float(res[-1])
            return res
        except:
            return -1
    else:
        return -1


def get_breakStep(instance, solver, time):
    fstr = open(f"{result}/{solver}/log/{time}/{instance}", "r").read()
    if not len(re.findall(r"breakStep: .*", fstr)) == 0:
        res = re.findall(
            r"breakStep: .*", fstr)[-1].split()
        try:
            res = float(res[-1])
            return res
        except:
            return -1
    else:
        return -1


def get_tightStepUnsat(instance, solver, time):
    fstr = open(f"{result}/{solver}/log/{time}/{instance}", "r").read()
    if not len(re.findall(r"tightStepUnsat: .*", fstr)) == 0:
        res = re.findall(
            r"tightStepUnsat: .*", fstr)[-1].split()
        try:
            res = float(res[-1])
            return res
        except:
            return -1
    else:
        return -1


def get_tightStepSat(instance, solver, time):
    fstr = open(f"{result}/{solver}/log/{time}/{instance}", "r").read()
    if not len(re.findall(r"tightStepSat: .*", fstr)) == 0:
        res = re.findall(
            r"tightStepSat: .*", fstr)[-1].split()
        try:
            res = float(res[-1])
            return res
        except:
            return -1
    else:
        return -1


def get_randomStep(instance, solver, time):
    fstr = open(f"{result}/{solver}/log/{time}/{instance}", "r").read()
    if not len(re.findall(r"randomStep: .*", fstr)) == 0:
        res = re.findall(
            r"randomStep: .*", fstr)[-1].split()
        try:
            res = float(res[-1])
            return res
        except:
            return -1


def get_restartTimes(instance, solver, time):
    fstr = open(f"{result}/{solver}/log/{time}/{instance}", "r").read()
    if not len(re.findall(r"restartTimes: .*", fstr)) == 0:
        res = re.findall(
            r"restartTimes: .*", fstr)[-1].split()
        try:
            res = float(res[-1])
            return res
        except:
            return -1
    else:
        return -1


def choose_samp():
    Benchmark = []
    folder_path = "/pub/netdisk1/linpeng/Local-ILP/benchmark/list/class"
    file_paths = [os.path.join(folder_path, filename)
                  for filename in os.listdir(folder_path)]
    lines_count = []
    for path in file_paths:
        with open(path, 'r') as file:
            lines_count.append((path, sum(1 for line in file)))
    sorted_file_paths = [path for path, _ in sorted(
        lines_count, key=lambda x: x[1])]
    for f in sorted_file_paths:
        Benchmark.append([f, f.split("/")[-1][:-4]])
    Benchmark.append([samp_dir, "Total"])
    return Benchmark


def cal(Solver, Benchmark):
    TimeLimit = [10, 60, 300]
    for benchmark, name in Benchmark:
        ins = 0
        for instance in open(benchmark):
            ins += 1
        print(f"{name:20s} &{ins:3d} ", end='')
        for time_limit in TimeLimit:
            liftStep = []
            breakStep = []
            tightStepUnsat = []
            tightStepSat = []
            randomStep = []
            restartTimes = []
            for instance in open(benchmark):
                instance = instance.strip()
                a = get_liftStep(instance, Solver, time_limit)
                if a == -1:
                    continue
                b = get_breakStep(instance, Solver, time_limit)
                if b == -1:
                    continue
                c = get_tightStepUnsat(instance, Solver, time_limit)
                if c == -1:
                    continue
                d = get_tightStepSat(instance, Solver, time_limit)
                if d == -1:
                    continue
                e = get_randomStep(instance, Solver, time_limit)
                if e == -1:
                    continue
                f = get_restartTimes(instance, Solver, time_limit)
                if f == -1:
                    continue
                liftStep.append(a)
                breakStep.append(b)
                tightStepUnsat.append(c)
                tightStepSat.append(d)
                randomStep.append(e)
                restartTimes.append(f)
            # print(f"&{np.array(liftStep).mean():6.3f} &{np.array(breakStep).mean():6.3f} &{np.array(tightStepUnsat).mean():6.3f} &{np.array(tightStepSat).mean():6.3f} &{np.array(randomStep).mean():6.3f}", end='')
            print(f" &{np.array(restartTimes).mean():6.3f}", end='')
        print("\\\\")


def compSolver():

    Solver = "Local-ILP-cal"
    Benchmark = choose_samp()
    cal(Solver, Benchmark)


if __name__ == "__main__":
    compSolver()
