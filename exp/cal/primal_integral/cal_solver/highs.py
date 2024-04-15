import os
import re

readfolder = "/pub/netdisk1/linpeng/Local-MIP/result-new/highs1.6/log"
result_file = "/pub/netdisk1/linpeng/Local-MIP/result-new/highs1.6/time"
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
                line1 = line.split()
                if len(line1) == 13  and line[0] == " " and line[1] != " " and line[2] == " " :
                    # print(line)
                    best_line = line1
                    best_obj = best_line[6]
                    time = best_line[-1][:-1]
                if best_line != "":
                    out_file.write(f"{best_obj} {time}\n")
                    print(f"{instance} {best_obj} {time}")
            out_file.close()


if __name__ == "__main__":
    record()
