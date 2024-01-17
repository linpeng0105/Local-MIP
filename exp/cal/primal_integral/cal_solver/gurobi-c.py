import os
import re

readfolder = "/pub/netdisk1/linpeng/Local-MIP/result/gurobi-c/log"
result_file = "/pub/netdisk1/linpeng/Local-MIP/result/gurobi-c/time"
dataset = "/pub/netdisk1/linpeng/Local-MIP/benchmark/list/open_hard.txt"


def record():
    for cutoff in ["10", "60", "300", "3600"]:
        for instance in open(dataset):
            instance = instance.strip()
            try:
                out_file = open(f"{result_file}/{cutoff}/{instance}", "w")
                instance_path = f"{readfolder}/{cutoff}/{instance}"
                fstr = open(instance_path, "r").read()
            except:
                print(f"{instance}")
                continue
            pre_time = ""
            pre_solution = ""
            for line in open(instance_path, "r"):
                if not len(re.findall(r"Found heuristic solution: objective.*", line)) == 0:
                    best_line = line.split()
                    pre_solution = best_line[-1]
                if not len(re.findall(r"Presolve time: .*", fstr)) == 0:
                    pre_time = re.findall(
                        r"Presolve time: .*", fstr)[-1].split()
                    pre_time = pre_time[-1][:-1]
            if pre_solution != "":
                out_file.write(f"{pre_solution} {pre_time}\n")
            for line in open(instance_path, "r"):
                best_obj = ""
                best_line = ""
                time = ""
                if line[0] == "H" or line[0] == "*":
                    best_line = line.split()
                    best_obj = best_line[-5]
                    time = best_line[-1][:-1]
                if best_obj != "":
                    out_file.write(f"{best_obj} {time}\n")
            out_file.close()


if __name__ == "__main__":
    record()
