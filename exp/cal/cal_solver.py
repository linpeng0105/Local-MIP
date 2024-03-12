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
                        best_objstr = re.findall(r"B 1.*", fstr)[-1]
                    else:
                        best_objstr = re.findall(r"F.*", fstr)[-1]

                    best_obj = float(best_objstr.split()[2])
                    self.datas[ins_name].best_obj = best_obj
            except:
                pass
                print(f"real_file_path: {real_file_path}\t ins_name: {ins_name}\t solver: {self.print_name}")
        return super().cal_soln(ins_name)

    def to_string(self, state):
        return super().to_string(state)


SOLVER_LEN = 50
SAMPLE_LEN = 15
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
        # return self.solvers[-1].win_num
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
        for sample in self.sample_dirs:
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
                # if True:
                # # if False:
                #    print(ins_detail)
                #    print()
            self.sample_ins_ct = sample_ins_ct
            return self.__show_in_mark_down(samp_name, show)


result = "/pub/netdisk1/linpeng/Local-MIP/result-new"
data = "/pub/netdisk1/linpeng/paralle-local-ILP/miplib_open_hard_17_10_03/"


def turn(path):
    max_all_gap = -1000
    best_temp = ""
    times = ['60']
    best_list = []
    dirs = os.listdir(path)
    dirs.sort()
    for dir in dirs:
        all_gap = 0
        for time in times:
            solvers = []
            result = "/pub/netdisk1/linpeng/Local-MIP/result-new"
            for so in ["gurobi-h", "gurobi-c", "scip", "highs1.6", "cplex"]:
                # for so in ["scip", "highs1.6"]:
                # for so in ["gurobi-h", "gurobi-c"]:
                solvers.append(solver(f"{result}/{so}/result/{time}", f"{so}"))
            solvers.append(solver(f"{path}/{dir}/", dir))
            print(path+dir)
            samples = []
            samples.append(
                ["/pub/netdisk1/linpeng/Local-MIP/benchmark/list/ALL.txt", "ALL"])
            clt = calculater(solvers, samples)
            gap = clt.cal_and_show()
            all_gap += gap
        if all_gap >= max_all_gap:
            max_all_gap = all_gap
            best_temp = dir
        if all_gap >= -25:
            best_list.append([dir, all_gap])
    print()
    best_list_0 = sorted(best_list, key=lambda item: item[1])
    for item in best_list_0:
        print(f"{item[0]:40s}       {item[1]}")
    print(f"max_all_gap: {max_all_gap}")
    print(f"best_temp: {best_temp}")


def compGurobi(time):
    solvers = []
    result = "/pub/netdisk1/linpeng/Local-MIP/result-new"
    for so in ["gurobi-h", "gurobi-c", "scip", "highs1.6", "cplex"]:
    # for so in ["gurobi-h", "gurobi-c"]:
        solvers.append(solver(f"{result}/{so}/result/{time}", f"{so}"))
    solvers.append(solver(
        f"{result}/{LocalMIP}/{time}", "Local-MIP"))
    samples = []
    D = "/pub/netdisk1/linpeng/Local-MIP/benchmark/list"
    for data in ["BP", "IP", "MBP", "MIP", "jsp", "openshop"]:
    # for data in ["MBP"]:
        samples.append(
            [f"{D}/{data}.txt", f"{data}"])
    samples.append(
        ["/pub/netdisk1/linpeng/Local-MIP/benchmark/list/ALL.txt", "ALL"])
    clt = calculater(solvers, samples)
    all_gap = clt.cal_and_show()


def compSCIP(time):
    solvers = []
    result = "/pub/netdisk1/linpeng/Local-MIP/result-new"
    for so in ["scip", "highs1.6"]:
        solvers.append(solver(f"{result}/{so}/result/{time}", f"{so}"))
    solvers.append(solver(
        f"{result}/{LocalMIP}/{time}", "Local-MIP"))
    samples = []
    D = "/pub/netdisk1/linpeng/Local-MIP/benchmark/list"
    for data in ["BP", "IP", "MBP", "MIP", "jsp", "fjsp", "openshop"]:
        samples.append(
            [f"{D}/{data}.txt", f"{data}"])
    samples.append(
        ["/pub/netdisk1/linpeng/Local-MIP/benchmark/list/ALL.txt", "ALL"])
    clt = calculater(solvers, samples)
    all_gap = clt.cal_and_show()

def compFJ(time):
    solvers = []
    result = "/pub/netdisk1/linpeng/Local-MIP/result-new"
    for so in ["FJ-16"]:
        solvers.append(solver(f"{result}/{so}/result/{time}", f"{so}"))
    solvers.append(solver(
        f"{result}/{LocalMIP}/{time}", "Local-MIP"))
    samples = []
    D = "/pub/netdisk1/linpeng/Local-MIP/benchmark/list"
    for data in ["BP", "IP", "MBP", "MIP", "jsp", "openshop"]:
        samples.append(
            [f"{D}/{data}.txt", f"{data}"])
    samples.append(
        ["/pub/netdisk1/linpeng/Local-MIP/benchmark/list/ALL.txt", "ALL"])
    clt = calculater(solvers, samples)
    all_gap = clt.cal_and_show()


# LocalMIP = "Local-MIP/v3/turn/4/log/"
# LocalMIP = "Local-MIP/v4/try/6_2000_2000_40_60_30_50_3000000/log/"
# LocalMIP = "Local-MIP/v5/turn/3/log/_12_2000_3000_20_190_20_150_3"
# LocalMIP = "Local-MIP/v4/try/_12_900_100_10_20_20_200_2832/"
# LocalMIP = "Local-MIP/v5/try/_12_2000_3000_20_190_20_150_2832/"
# LocalMIP = "Local-MIP/v6/try/_12_2000_3000_20_190_20_150_2832/"
# LocalMIP = "Local-MIP/v6/try/_12_3000_3000_20_200_20_150_2832/"
LocalMIP = "Local-MIP/v6/try/std/"
if __name__ == "__main__":
    compGurobi("10")
    compGurobi("60")
    # compGurobi("300")
    # compSCIP("10")
    # compSCIP("60")
    # compSCIP("300")
    # compFJ("10")
    # compFJ("60")
    # compFJ("300")
    # turn(f"{result}/Local-MIP/v6/turn/1/log/")
    turn(f"{result}/Local-MIP/v6/turning/17-new/")