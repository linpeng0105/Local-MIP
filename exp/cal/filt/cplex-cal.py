import re
import os


def cal():
    for TT in [10,60,300]:
        ori_path = f"/pub/netdisk1/linpeng/Local-MIP/result-new/cplex/log/{TT}/"
        out_path = f"/pub/netdisk1/linpeng/Local-MIP/result-new/cplex/result/{TT}/"
        file_list = os.listdir(ori_path)
        # print(file_list)
        for instance in file_list:
            instance_path = ori_path+instance

            fstr = open(instance_path, "r").read()

            res = ""

            if (not len(re.findall(r"MIP - Time limit exceeded, no integer solution.", fstr)) == 0):
                res = "U	 no feasible solution"
            elif (not len(re.findall(r"MIP - Time limit exceeded, integer feasible:  Objective =.*", fstr)) == 0):
                best_objstr = re.findall(r"MIP - Time limit exceeded, integer feasible:  Objective =.*", fstr)[-1]
                best_obj = float(best_objstr.split()[-1])
                res="B 1	 "+str(best_obj)
            else:
                res = "U	 no feasible solution"
            
            out_file = open(out_path+instance, "w")
            out_file.write(res)
            out_file.close()

def opt():
    # if (strcmp(name, "1.wecnf" ) == 0) return 10;
    ori_path = "/pub/netdisk1/linpeng/paralle-local-ILP/result/cplex/log/10/"
    file_list = os.listdir(ori_path)
    out_file = open("opt-3600.txt", "w")
    for instance in file_list:
        instance_path = ori_path+instance

        fstr = open(instance_path, "r").read()

        res = ""

        if (not len(re.findall(r"Optimal solution found", fstr)) == 0):
            best_objstr = re.findall(r"Solution count.*", fstr)[-1]
            best_obj = float(best_objstr.split()[3])
            ins =instance[:-4]+".wecnf"
            # print(best_obj)
            res="if (strcmp(name, \""+ins +"\") == 0) return (ll) "+ str(best_obj)+";\n"
            out_file.write(res)
    out_file.close()
        
if __name__ == "__main__":
    cal()
    # opt()

