import os
import re

readfolder = "/pub/netdisk1/linpeng/Local-MIP/result/Local-MIP/v4/try/12_3000_4000_70_35_20_50_3000000/log"
result_file = "/pub/netdisk1/linpeng/Local-MIP/result/Local-MIP/v4/try/12_3000_4000_70_35_20_50_3000000/time"
dataset = "/pub/netdisk1/linpeng/Local-MIP/benchmark/list/ALL.txt"


def record():
    for cutoff in ["10", "60", "300",]:
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
                if line[0] == "n" and len(line.split())==3:
                    best_line = line.split()
                    best_obj = best_line[1]
                    time = best_line[2]
                if best_obj != "":
                    out_file.write(f"{best_obj} {time}\n")
            out_file.close()


if __name__ == "__main__":
    record()
