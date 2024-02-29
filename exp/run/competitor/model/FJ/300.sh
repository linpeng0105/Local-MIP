#!/bin/bash

INSTANCE_NUM=121
tmp_fifofile="/tmp/$$.fifo" # 脚本运行的当前进程ID号作为文件名
mkfifo "$tmp_fifofile" # 新建一个随机fifo管道文件
exec 6<>"$tmp_fifofile" # 定义文件描述符6指向这个fifo管道文件
rm $tmp_fifofile
for i in $(seq 1 $INSTANCE_NUM)
do
  echo # for循环 往 fifo管道文件中写入 $INSTANCE_NUM 个空行
done >&6

instance="/pub/netdisk1/linpeng/Local-ILP/benchmark/collection"
benchmark_list="/pub/netdisk1/linpeng/Local-ILP/benchmark/list/open-hard-union-shuffle.txt"
result="/pub/netdisk1/linpeng/Local-ILP/result/FJ/seed"
cutoff="300"
randomSeed="0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20"
all_datas=($instance)
for seed in $randomSeed
do
  for cut in $cutoff
  do
    for((i=0;i<${#all_datas[*]};i++))
    do
      instance=${all_datas[$i]}
      res_solver_ins=$result/${seed}/${cut}
      if [ ! -d "$res_solver_ins" ]; then
        mkdir -p $res_solver_ins
      fi
      for dir_file in `cat $benchmark_list`
      do
        file=$dir_file
        echo $file
        touch $res_solver_ins/$file
        read -u 6
        {
          cd /pub/netdisk1/linpeng/Local-ILP/code/exp/run/competitor/FJ/FJ-h
          ./FJ -i $instance/$file -t $cut --seed $seed
          echo >&6
        } >$res_solver_ins/$file   2>>$res_solver_ins/$file &
      done
    done
  done
done

