import re
import numpy as np
import os

result = "/pub/netdisk1/linpeng/Local-ILP/result"
samp_dir = "/pub/netdisk1/linpeng/Local-ILP/benchmark/list/open-hard-union.txt"
MAXV = 1e20


def get_opt(instance):
    fstr = open(
        f"/pub/netdisk1/linpeng/Local-ILP/code/exp/cal/primal_integral/data/instance_obj.csv", "r").read()
    line = re.findall(rf"{instance[:]}.*", fstr)[-1].split()
    return line[-1]


def get_obj_time(instance, solver):
    obj_time = []
    for line in open(f"{result}/{solver}/time/300/{instance}", "r"):
        line = line.split()
        obj_time.append([float(line[0]), float(line[1])])
    return obj_time


def p(incumbent, opt):
    try:
        if incumbent == MAXV:
            return 1
        if (incumbent == opt and opt == 0):
            return 0
        elif incumbent * opt < 0:
            return 1
        else:
            return abs(incumbent-opt)/max(abs(incumbent), abs(opt))
    except:
        print(f"{incumbent},{opt}")


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
        # print(f, f[:-4])
        Benchmark.append([f, f.split("/")[-1][:-4]])
    Benchmark.append([samp_dir, "Total"])
    return Benchmark


def cal(Solver, Benchmark):
    TimeLimit = [10, 60, 300]
    for benchmark, name in Benchmark:
        ins = 0
        for intance in open(benchmark):
            ins += 1
        print(f"{name} &{ins} ", end='')
        # print(f'{solver:30s}=[', end='')
        for time_limit in TimeLimit:
            avgs = []
            for solver in Solver:
                pTall = []
                for intance in open(benchmark):
                    intance = intance.strip()
                    opt = get_opt(intance)
                    if opt != "NA":
                        opt = float(opt)
                    else:
                        continue
                    # print(f"{intance} {opt}")
                    obj_time = get_obj_time(intance, solver)
                    # print(obj_time)
                    pT = 0
                    if (len(obj_time) > 0 and obj_time[0][1] < time_limit):
                        pT += p(MAXV, opt)*obj_time[0][1]
                    else:
                        pT += p(MAXV, opt)*time_limit
                    for i in range(0, len(obj_time)):
                        t_im1 = obj_time[i][1]
                        if t_im1 > time_limit:
                            break
                        t_i = -1
                        if i == (len(obj_time))-1 or obj_time[i+1][1] > time_limit:
                            t_i = time_limit
                        else:
                            t_i = obj_time[i+1][1]
                        pT += p(obj_time[i][0], opt)*(t_i-t_im1)
                    if time_limit == 0:
                        pT = 1
                    else:
                        pT = pT/time_limit
                    # print(f"{intance} {pT}")
                    pTall.append(pT)
                avg = np.array(pTall).mean()
                avgs.append(avg)
            best_avg = 1
            for avg in avgs:
                best_avg = min(avg, best_avg)
            for avg in avgs:
                if avg == best_avg and best_avg != 1:
                    print(f"&\\textbf{{{avg:.3f}}} ", end='')
                else:
                    print(f"&{avg:.3f} ", end='')
        print("\\\\")
        # print("--------------------------------------------------")


def compSolver():
    Solver = ["Local-ILP", "FJ-core0", "scip-core0",
              "gurobi-core0-c", "gurobi-core0-h"]
    Benchmark = choose_samp()
    cal(Solver, Benchmark)


def core8():
    Solver = ["local-ilp-diversity-core8-v2", "FJ-core0", "scip-core0", "gurobi-core8-c",
              "gurobi-core8-h"]
    Benchmark = choose_samp()
    cal(Solver, Benchmark)


if __name__ == "__main__":
    # compSolver()
    core8()
