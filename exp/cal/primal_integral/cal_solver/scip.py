import os
import re

readfolder = "/pub/netdisk1/linpeng/Local-MIP/result-new/scip/log"
result_file = "/pub/netdisk1/linpeng/Local-MIP/result-new/scip/time"
dataset = "/pub/netdisk1/linpeng/Local-MIP/benchmark/list/BPP.txt"


def record():
    for cutoff in ["10", "60", "300"]:
        for instance in open(dataset):
            instance = instance.strip()
            out_file = open(f"{result_file}/{cutoff}/{instance}", "w")
            instance_path = f"{readfolder}/{cutoff}/{instance}"
            fstr = open(instance_path, "r").read()
            res = ""
            for line in open(instance_path, "r"):
                best_obj = ""
                best_line = ""
                time = ""
                if not len(re.findall(r"feasible solution found by.*", line)) == 0:
                    best_line = line.split()
                    best_obj = best_line[-1]
                    time = best_line[7]
                else:
                    line1 = line.split('|')
                    if len(line1) == 18 and line1[-3] != " primalbound  ":
                        if len(line1[0].split())==2:
                            best_line = line1
                            best_obj = best_line[-3]
                            time=line1[0].split()[1][:-1]
                        elif line[0][0].isalpha():
                            best_line = line1
                            best_obj = best_line[-3]
                            time=line1[0][1:-1]
                if best_obj != "      --      " and best_line != "":
                    out_file.write(f"{best_obj} {time}\n")
                    print(f"{instance} {best_obj} {time}")
            out_file.close()


if __name__ == "__main__":
    record()
