import os
import re

readfolder = "/pub/netdisk1/linpeng/Local-ILP/result/variation"
result_file = "/pub/netdisk1/linpeng/Local-ILP/result/variation"
dataset = "/pub/netdisk1/linpeng/Local-ILP/benchmark/list/open-hard-union.txt"


def record():
    for solver in ["v-fix-1", "v-fix-5", "v-no-improve", "v-no-restore", "v-per-bound", "v-per-random"]:
        for cutoff in ["10", "60", "300"]:
            for instance in open(dataset):
                instance = instance.strip()
                out_file = open(f"{result_file}/{solver}/time/{cutoff}/{instance}", "w")
                instance_path = f"{readfolder}/{solver}/log/{cutoff}/{instance}"
                fstr = open(instance_path, "r").read()
                res = ""
                for line in open(instance_path, "r"):
                    best_obj = ""
                    best_line = ""
                    time = ""
                    if line[0] == "n":
                        best_line = line.split()
                        best_obj = best_line[1]
                        time = best_line[2]
                    if best_obj != "":
                        out_file.write(f"{best_obj} {time}\n")
                out_file.close()


if __name__ == "__main__":
    record()
