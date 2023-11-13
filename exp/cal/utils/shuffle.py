import random

# 原始文件名
filename = "/pub/netdisk1/linpeng/Local-MIP/benchmark/list/open_hard.txt"

# 新文件名
new_filename = "/pub/netdisk1/linpeng/Local-MIP/benchmark/list/open_hard-shuffle.txt"

# 打开原始文件，并将每行内容读入列表
with open(filename, "r") as f:
    lines = f.readlines()
print(len(lines))
# 随机打乱列表中元素的顺序
random.shuffle(lines)

print(len(lines))
# 将打乱后的列表中的每一个元素写入新文件中
with open(new_filename, "w") as f:
    for line in lines:
      line = line.strip()
      f.write(line+"\n")
      print(line)
