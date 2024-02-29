#!/bin/bash

SEND_THREAD_NUM=192
tmp_fifofile="/tmp/$$.fifo" # 脚本运行的当前进程ID号作为文件名
mkfifo "$tmp_fifofile" # 新建一个随机fifo管道文件
exec 6<>"$tmp_fifofile" # 定义文件描述符6指向这个fifo管道文件
rm $tmp_fifofile
for i in $(seq 1 $SEND_THREAD_NUM)
do
  echo # for循环 往 fifo管道文件中写入 $SEND_THREAD_NUM 个空行
done >&6

res_no="/pub/netdisk1/linpeng/Local-MIP/result-new/unused"
Instance="/pub/netdisk1/linpeng/Local-MIP/benchmark/ALL"
benchmark_list="/pub/netdisk1/linpeng/Local-MIP/benchmark/list/ALL.txt"
cutoff="10 60 300"
ulimit -t 5000
all_datas=($Instance)

result="/pub/netdisk1/linpeng/Local-MIP/result-new/scip/log"
for cut in $cutoff
do
  for((i=0;i<${#all_datas[*]};i++))
  do
    instance=${all_datas[$i]}
    res_solver_ins=$result/${cut}
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
        time scip -s $cut.set  -f $instance/$file
        echo >&6
      } >$res_solver_ins/$file   2>>$res_solver_ins/$file &
    done
  done
done

result="/pub/netdisk1/linpeng/Local-MIP/result-new/highs1.6/log"
for cut in $cutoff
do
  for((i=0;i<${#all_datas[*]};i++))
  do
    instance=${all_datas[$i]}
    res_solver_ins=$result/${cut}
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
        export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/pub/netdisk1/linpeng/paralle-local-ILP/highs/HiGHS-1.6/build/lib/
        cd /pub/netdisk1/linpeng/paralle-local-ILP/highs/HiGHS-1.6/build/bin
        time ./highs --options_file 1.opt --time_limit $cut $instance/$file
        echo >&6
      } >$res_solver_ins/$file   2>>$res_solver_ins/$file &
    done
  done
done

result="/pub/netdisk1/linpeng/Local-MIP/result-new/gurobi-h/log"
for cut in $cutoff
do
  for((i=0;i<${#all_datas[*]};i++))
  do
    instance=${all_datas[$i]}
    res_solver_ins=$result/${cut}
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
        time gurobi_cl Threads=1 Heuristics=1 timeLimit=$cut $instance/$file
        echo >&6
      } >$res_solver_ins/$file   2>>$res_solver_ins/$file &
    done
  done
done

result="/pub/netdisk1/linpeng/Local-MIP/result-new/gurobi-c/log"
for cut in $cutoff
do
  for((i=0;i<${#all_datas[*]};i++))
  do
    instance=${all_datas[$i]}
    res_solver_ins=$result/${cut}
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
        time gurobi_cl Threads=1 timeLimit=$cut $instance/$file
        echo >&6
      } >$res_solver_ins/$file   2>>$res_solver_ins/$file &
    done
  done
done

res_solver_ins=$res_no
if [ ! -d "$res_solver_ins" ]; then
  mkdir -p $res_solver_ins
fi
for((i=0;i<192;i++))
do
  read -u 6
  {
    cd /home/chenzh/solvers/sota/kissat-MAB/build
    ./kissat /home/chenzh/data/hard_cnfs/49.cnf
    echo >&6
  } >$res_solver_ins/$i &
done
exit 0
