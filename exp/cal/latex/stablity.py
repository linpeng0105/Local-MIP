#!/usr/bin/python
# -*- coding: UTF-8 -*-

from multiprocessing import set_forkserver_preload
import os
import os.path
from posixpath import split
from random import sample
import re
import shutil
from time import monotonic, sleep
from tokenize import Number
import numpy as np

# global limit
CUTOFF = 300
PUNISH = 2  # PAR2
MEMS_MAX = 61440  # 60G
OBJ_MAX = 1e20
# Check = 0


class states(object):
    res = "unknown"
    feasible_time = CUTOFF*PUNISH
    best_time = CUTOFF*PUNISH
    mems = MEMS_MAX
    win = False
    best_obj = OBJ_MAX


class solver(object):
    def __init__(self, res_dir, name):
        self.res_dir = res_dir  # save the results files
        self.print_name = name     # names want to show
        self.datas = dict()   # datas[ins] save the instances

    def reset(self):
        # SAT-ins UNSAT-ins solved-ins all-ins
        self.feasible_num = self.unknown_num = self.all_num = 0
        self.win_num = 0

    def cal_soln(self, ins_name):
        self.all_num += 1
        state = self.datas[ins_name]
        if (state.res == "feasible"):
            self.feasible_num += 1
        elif (state.res == "unknown"):
            self.unknown_num += 1
        else:
            print("wrong...")

    def to_string(self, state):
        line = ""
        line += self.print_name.ljust(40)
        # line += str(state.res).ljust(18)
        line += str(round(state.best_obj, 2)).ljust(18)
        if state.win:
            line += "[W]".ljust(18)
        line += str()
        return line.ljust(18)


def p(incumbent, opt):
    try:
        if incumbent == OBJ_MAX:
            return 1
        if (incumbent == opt and opt == 0):
            return 0
        elif incumbent * opt < 0:
            return 1
        else:
            return abs(incumbent-opt)/max(abs(incumbent), abs(opt))
    except:
        print(f"{incumbent},{opt}")


class solver(solver):
    def cal_soln(self, ins_name):
        if (not ins_name in self.datas):
            self.datas[ins_name] = states()
            real_file_path = self.res_dir + "/" + ins_name
            try:
                fstr = open(real_file_path, "r").read()
                if (not len(re.findall(r"o no feasible solution found", fstr)) == 0 or
                        not len(re.findall(r"U	 no feasible solution", fstr)) == 0):
                    self.datas[ins_name].res = "unknown"
                elif (not len(re.findall(r"F.*", fstr)) == 0):
                    self.datas[ins_name].res = "feasible"
                elif (not len(re.findall(r"B\s+[0-9]", fstr)) == 0):
                    self.datas[ins_name].res = "feasible"

                if (self.datas[ins_name].res == "feasible"):

                    if (not len(re.findall(r"B\s+[0-9]", fstr)) == 0):
                        best_objstr = re.findall(r"B.*", fstr)[-1]
                    else:
                        best_objstr = re.findall(r"F.*", fstr)[-1]

                    best_obj = float(best_objstr.split()[2])
                    self.datas[ins_name].best_obj = best_obj
            except:
                pass
                print(
                    f"real_file_path: {real_file_path}\t ins_name: {ins_name}\t solver: {self.print_name}")
        return super().cal_soln(ins_name)

    def to_string(self, state):
        return super().to_string(state)


SOLVER_LEN = 20
SAMPLE_LEN = 30
NUMBER_LEN = 10
print_title = True


class calculater(object):
    solvers = []
    sample_dirs = []    # sample dirs, [sample_dir, sample_name]s

    def __init__(self, solvers, sample_dirs):
        self.solvers = solvers
        self.sample_dirs = sample_dirs

    def __show_in_mark_down(self, samp_name, show):
        global print_title
        if show:
            if (print_title):
                print_title = False
                title = "| benchmark".ljust(SAMPLE_LEN+2)
                title += " | #ins".ljust(NUMBER_LEN+3)
                title += " | solver".ljust(SOLVER_LEN+3)
                title += " | #feas".ljust(NUMBER_LEN+3)
                title += " | #win".ljust(NUMBER_LEN+3)
                title += " |"
                print(title)

                print(self.split_line)

            for slv in self.solvers:
                line = "| " + \
                    (samp_name + "("+str(self.sample_ins_ct) + ")").ljust(SAMPLE_LEN)
                line += " | " + str(round(slv.all_num, 2)).ljust(NUMBER_LEN)
                line += " | " + slv.print_name.ljust(SOLVER_LEN)
                line += " | " + str(slv.feasible_num).ljust(NUMBER_LEN)
                # line += " | " + \
                #     str(round(slv.avg_feasible_time, 2)).ljust(NUMBER_LEN)
                # line += " | " + str(slv.unknown_num).ljust(NUMBER_LEN)
                line += " | " + str(slv.win_num).ljust(NUMBER_LEN)
                # line += " | " + str(round(slv.avg_best_time, 2)).ljust(NUMBER_LEN)
                line += " |"
                print(line)
            print(self.split_line)
        maxWin = 0
        for i in range(0, len(self.solvers)-1):
            maxWin = max(self.solvers[i].win_num, maxWin)
        gap = self.solvers[-1].win_num-maxWin
        print(f"{self.split_line}gap: {gap}")
        return gap

    def cal_and_show(self, show=False):
        split = ""
        split = "| " + '-'*(SAMPLE_LEN)
        split += " | " + '-'*(NUMBER_LEN)
        split += " | " + '-'*(SOLVER_LEN)
        split += " | " + '-'*(NUMBER_LEN)
        split += " | " + '-'*(NUMBER_LEN)
        split += " |"
        self.split_line = split
        if show:
            print(self.split_line)
        for idx in range(0, len(self.sample_dirs)):
            sample = self.sample_dirs[idx]
            samp_dir = sample[0]
            samp_name = sample[1]
            sample_ins_ct = 0
            for slv in self.solvers:
                slv.reset()
            S_avg_sol = 0
            S_std = 0
            S_ins_count = 0
            A = 0
            B = 0
            C = 0
            D = 0
            for ins_name in open(samp_dir):
                ins_detail = ""
                ins_detail += ins_name
                sample_ins_ct += 1
                ins_name = ins_name.strip()
                best_value = OBJ_MAX
                for slv in self.solvers:
                    slv.cal_soln(ins_name)
                    best_value = min(slv.datas[ins_name].best_obj, best_value)

                if (not best_value == OBJ_MAX):
                    for slv in self.solvers:
                        if (slv.datas[ins_name].best_obj == best_value):
                            slv.win_num += 1
                            slv.datas[ins_name].win = True
                for slv in self.solvers:
                    ins_detail += slv.to_string(slv.datas[ins_name]) + "\n"
                sol = []
                for slv in self.solvers:
                    sol_i = slv.datas[ins_name].best_obj
                    if sol_i != OBJ_MAX:
                        sol.append(abs(sol_i))
                if len(sol) > 0:
                    np_sol = np.array(sol)
                    S_avg_sol += np_sol.mean()
                    S_std += np_sol.std()
                    S_ins_count += 1
                    now = np_sol.std()/np_sol.mean()
                    if now < 0.01:
                        A += 1
                    elif now < 0.1:
                        B += 1
                    elif now < 0.5:
                        C += 1
                    else:
                        D += 1
                else:
                    A += 1
            S_avg_sol /= S_ins_count
            S_std /= S_ins_count
            print(f"&{A} &{B} &{C} &{D}\\\\")
            print(A+B+C+D)
            wins[idx] = f"{wins[idx]} &{S_avg_sol:.3f} &{S_std:.3f}  &{S_std/S_avg_sol:.3f}"
            self.sample_ins_ct = sample_ins_ct
            self.__show_in_mark_down(samp_name, show)


res = "/pub/netdisk1/linpeng/paralle-local-ILP/result/"
data = "/pub/netdisk1/linpeng/paralle-local-ILP/miplib_open_hard_17_10_03/"
wins = []


def choose_samp():
    Benchmark = []
    D = "/pub/netdisk1/linpeng/Local-MIP/benchmark/list"
    # for data in ["MIPLIB-BP", "MIPLIB-IP", "MIPLIB-MBP", "MIPLIB-MIP", "BPP", "JSP", "OSP"]:
    #     Benchmark.append(
    #         [f"{D}/{data}.txt", f"{data}"])
    Benchmark.append(
        ["/pub/netdisk1/linpeng/Local-MIP/benchmark/list/ALL-20240408.txt", "Total"])
    return Benchmark


def compSolver():
    samples = choose_samp()
    LocalMIP = "Local-MIP/v6/try/seed"
    for s in samples:
        ins = 0
        for ins_name in open(s[0]):
            ins += 1
        wins.append(f"{s[1]} &{ins}\t")
    for time in ["10", "60", "300",]:
        solvers = []
        result = "/pub/netdisk1/linpeng/Local-MIP/result-new"
        # for so in ["highs1.6", "scip", "cplex", "gurobi-c", "gurobi-h"]:
        # for so in ["FJ-16"]:
        #     solvers.append(solver(f"{result}/{so}/result/{time}", f"{so}"))
        for so in range(1, 11):
            solvers.append(solver(
                f"{result}/{LocalMIP}/{so}/{time}", "Local-MIP"))
        clt = calculater(solvers, samples)
        clt.cal_and_show()
    # for time in ["3600",]:
    #     solvers = []
    #     result = "/pub/netdisk1/linpeng/Local-MIP/result-new"
    #     # for so in ["highs1.6", "scip", "cplex", "gurobi-c", "gurobi-h"]:
    #     # for so in ["FJ-16"]:
    #     #     solvers.append(solver(f"{result}/{so}/result/{time}", f"{so}"))
    #     for so in range(1, 10):
    #         solvers.append(solver(
    #             f"{result}/{LocalMIP}/{so}/{time}/", "Local-MIP"))
    #     clt = calculater(solvers, samples)
    #     clt.cal_and_show()
    print('\hline')
    # for idx in range(0, len(samples)):
    #     if idx == len(samples)-1:
    #         print("\hline")
    #     print(f"{wins[idx]} \\\\")


if __name__ == "__main__":
    compSolver()
