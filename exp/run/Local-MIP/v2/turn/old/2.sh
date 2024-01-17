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

res_no="/pub/netdisk1/linpeng/Local-MIP/result/unused"
instance="/pub/netdisk1/linpeng/Local-MIP/benchmark/collection"
benchmark_list="/pub/netdisk1/linpeng/Local-MIP/benchmark/list/open_hard-shuffle.txt"
result="/pub/netdisk1/linpeng/Local-MIP/result/Local-MIP/v2/turn/2/log/"
ulimit -t 400
sampleUnsat="1 2 3 4 5 6 7 8 9 10"
bmsUnsat="2000"
sampleSat="30"
bmsSat="50"
bmsRandom="150"
restartStep="1500000"
rvd="0.5"
wf="10"

all_datas=($instance)
for a in $sampleUnsat
do
for b in $bmsUnsat
do
for c in $sampleSat
do
for d in $bmsSat
do
for e in $bmsRandom
do
for f in $restartStep
do
for g in $rvd
do
for h in $wf
do
  for((i=0;i<${#all_datas[*]};i++))
  do
    instance=${all_datas[$i]}
    res_solver_ins=$result/${a}_${b}_${c}_${d}_${e}_${f}_${g}_${h}
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
        cd /pub/netdisk1/linpeng/Local-MIP/code/bin/Local-MIP/
        time ./Local-MIP -i $instance/$file --cutoff=300 --sampleUnsat=${a} --bmsUnsat=${b} --sampleSat=${c} --bmsSat=${d} --bmsRandom=${e} --restartStep=${f} --rvd=${g} --wf=${h} 
        echo >&6
      } >$res_solver_ins/$file   2>>$res_solver_ins/$file &
    done
  done
done
done
done
done
done
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
