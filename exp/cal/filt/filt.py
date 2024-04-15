import os
import re

dataset = "/pub/netdisk1/linpeng/Local-MIP/benchmark/list/BPP.txt"


def record():
    for solver in ["gurobi-c", "gurobi-h", "highs1.6", "scip"]:
    # for solver in ["FJ-16", "FJ-192"]:
        readfolder = f"/pub/netdisk1/linpeng/Local-MIP/result-new/{solver}/time"
        result_file = f"/pub/netdisk1/linpeng/Local-MIP/result-new/{solver}/result"
        for cutoff in [10, 60, 300]:
            for instance in open(dataset):
                instance = instance.strip()
                try:
                    out_file = open(f"{result_file}/{cutoff}/{instance}", "w")
                    instance_path = f"{readfolder}/{cutoff}/{instance}"
                except:
                    print(f"{instance}")
                    continue
                bestVal = 1e20
                for line in open(instance_path, "r"):
                    time = float(line.split()[1])
                    if time < cutoff:
                        bestVal = line.split()[0]
                if bestVal==1e20:
                  out_file.write("o no feasible solution found")
                else:
                  out_file.write(f"B 1 {bestVal}")
                out_file.close()


if __name__ == "__main__":
    record()
