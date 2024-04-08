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
                # print(f"real_file_path: {real_file_path}\t ins_name: {ins_name}\t solver: {self.print_name}")
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

    def cal_and_show(self, show=True):
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
            best_feas = 0
            for slv in self.solvers:
                best_feas = max(best_feas, slv.feasible_num)
            best_win = 0
            for slv in self.solvers:
                best_win = max(best_win, slv.win_num)
            for slv in self.solvers:
                if slv.feasible_num == best_feas and best_feas > 0:
                    wins[idx] = f"{wins[idx]} &\\textbf{{{slv.feasible_num}}}"
                else:
                    wins[idx] = f"{wins[idx]} &{slv.feasible_num}"
                if slv.win_num == best_win and best_win > 0:
                    wins[idx] = f"{wins[idx]} &\\textbf{{{slv.win_num}}}"
                else:
                    wins[idx] = f"{wins[idx]} &{slv.win_num}"
            self.sample_ins_ct = sample_ins_ct
            self.__show_in_mark_down(samp_name, show)


res = "/pub/netdisk1/linpeng/paralle-local-ILP/result/"
data = "/pub/netdisk1/linpeng/paralle-local-ILP/miplib_open_hard_17_10_03/"
wins = []


def choose_samp():
    Benchmark = []
    D = "/pub/netdisk1/linpeng/Local-MIP/benchmark/list"
    for data in ["MIPLIB-BP", "MIPLIB-IP", "MIPLIB-MBP", "MIPLIB-MIP", "JSP", "OSP"]:
        Benchmark.append(
            [f"{D}/{data}.txt", f"{data}"])
    Benchmark.append(
        ["/pub/netdisk1/linpeng/Local-MIP/benchmark/list/ALL.txt", "Total"])
    return Benchmark


def compSolver():
    samples = choose_samp()
    LocalMIP = "Local-MIP/v6/try/std/"
    for s in samples:
        ins = 0
        for ins_name in open(s[0]):
            ins += 1
        wins.append(f"{s[1]} &{ins}")
    for time in ["300",]:
        solvers = []
        result = "/pub/netdisk1/linpeng/Local-MIP/result-new"
        for so in ["highs1.6", "scip", "cplex", "gurobi-c", "gurobi-h"]:
        # for so in ["FJ-16"]:
            solvers.append(solver(f"{result}/{so}/result/{time}", f"{so}"))
        solvers.append(solver(
            f"{result}/{LocalMIP}/{time}", "Local-MIP"))
        clt = calculater(solvers, samples)
        clt.cal_and_show()
    for idx in range(0, len(samples)):
        print(f"{wins[idx]} \\\\")


if __name__ == "__main__":
    compSolver()
