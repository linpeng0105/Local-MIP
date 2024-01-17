import re
import numpy as np
import os

result = "/pub/netdisk1/linpeng/Local-MIP/result"
samp_dir = "/pub/netdisk1/linpeng/Local-MIP/benchmark/list/open_hard.txt"
MAXV = 1e20


def get_opt(instance):
    fstr = open(f"data/instance_obj.csv", "r").read()
    line = re.findall(rf"{instance[:]}.*", fstr)[-1].split()
    return line[-1]


def get_obj_time(instance, solver, time_limit):
    obj_time = []
    for line in open(f"{result}/{solver}/time/{time_limit}/{instance}", "r"):
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
    # folder_path = "/pub/netdisk1/linpeng/Local-ILP/benchmark/list/class"
    # file_paths = [os.path.join(folder_path, filename)
    #               for filename in os.listdir(folder_path)]
    # lines_count = []
    # for path in file_paths:
    #     with open(path, 'r') as file:
    #         lines_count.append((path, sum(1 for line in file)))
    # sorted_file_paths = [path for path, _ in sorted(
    #     lines_count, key=lambda x: x[1])]
    # for f in sorted_file_paths:
    #     # print(f, f[:-4])
    #     Benchmark.append([f, f.split("/")[-1][:-4]])
    Benchmark.append([samp_dir, "Total"])
    return Benchmark


def cal(Solver, Benchmark):
    TimeLimit = [10, 60, 300]
    for benchmark, name in Benchmark:
        print(f"{name}")
        for solver in Solver:
            print(f'{solver:30s}=[', end='')
            for time_limit in TimeLimit:
                pTall = []
                for intance in open(benchmark):
                    intance = intance.strip()
                    opt = get_opt(intance)
                    if opt != "NA":
                        opt = float(opt)
                    else:
                        continue
                    # print(f"{intance} {opt}")
                    obj_time = get_obj_time(intance, solver, time_limit)
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
                print(f"({time_limit}, {avg:.3f}),", end='')
            print(']')
        print("--------------------------------------------------")


def compSolver():
    Solver = [
        # "cplex", "scip", "highs",
        #       "gurobi-c", "gurobi-h",
        "gurobi-c",
        "gurobi-h",
        "highs1.6",
        # "highs1.5",
        "scip",
        # "Local-MIP/v3/try/v3-4",
        # "Local-MIP/v3/try/v3-5",
        # "Local-MIP/v3/try/v3-stable",
        # "Local-MIP/v3/try/v3-tight",
        # "Local-MIP/v4/try/objtight",
        # "Local-MIP/v4/try/objtight-r",
        "Local-MIP/v4/try/6_2000_2000_40_60_30_50_3000000",
        "Local-MIP/v4/try/12_10000_4000_70_35_20_50_3000000",
    ]
    Benchmark = choose_samp()
    cal(Solver, Benchmark)


if __name__ == "__main__":
    compSolver()
