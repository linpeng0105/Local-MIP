import os
import re
import numpy as np
turnTime = 11
readFolder = f"/pub/netdisk1/linpeng/Local-MIP/result/Local-MIP/v2/turn/{turnTime}/log"
resultFolder = f"/pub/netdisk1/linpeng/Local-MIP/result/Local-MIP/v2/turn/{turnTime}/time"
dataset = "/pub/netdisk1/linpeng/Local-MIP/benchmark/list/open_hard.txt"
MAXV = 1e20


def record():
    paras = os.listdir(readFolder)
    for para in paras:
        for instance in open(dataset):
            instance = instance.strip()
            instance_path = f"{readFolder}/{para}/{instance}"
            timePath = f"{resultFolder}/{para}"
            if not os.path.exists(timePath):
                os.makedirs(timePath)
            out_file = open(f"{timePath}/{instance}", "w")
            try:
                fstr = open(instance_path, "r").read()
            except:
                continue
            for line in open(instance_path, "r"):
                best_obj = ""
                best_line = ""
                time = ""
                if line[0] == "n" and len(line.split()) == 3:
                    best_line = line.split()
                    best_obj = best_line[1]
                    time = best_line[2]
                if best_obj != "":
                    out_file.write(f"{best_obj} {time}\n")
            out_file.close()


def get_opt(instance):
    fstr = open(f"../data/instance_obj.csv", "r").read()
    line = re.findall(rf"{instance[:]}.*", fstr)[-1].split()
    return line[-1]


def get_obj_time(instance, solver, time_limit):
    obj_time = []
    for line in open(f"{resultFolder}/{solver}/{instance}", "r"):
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
    Benchmark.append([dataset, "Total"])
    return Benchmark


def cal(Solver, Benchmark):
    TimeLimit = [300]
    BestPT = 100
    BestPa = "NA"
    best_list = []
    for benchmark, name in Benchmark:
        print(f"{name}")
        for solver in Solver:
            print(f'{solver:40s}', end='')
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
                avg = np.array(pTall).mean()*100
                if avg < BestPT:
                    BestPT = avg
                    BestPa = solver
                best_list.append([avg, solver])
                print(f"{avg:.4f}%", end='\n')
        print("--------------------------------------------------")
    best_list_0 = sorted(best_list, key=lambda item: item[0])
    for item in best_list_0:
        print(f"{item[1]:40s}       {item[0]:.2f}%")
    print("--------------------------------------------------")
    print(f"{BestPa}        {BestPT:.4f}%")


def compSolver():
    Solver = os.listdir(resultFolder)
    Benchmark = choose_samp()
    cal(Solver, Benchmark)


if __name__ == "__main__":
    record()
    compSolver()