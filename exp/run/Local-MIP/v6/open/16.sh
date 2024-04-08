#!/bin/bash

SEND_THREAD_NUM=227
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
benchmark_list="/pub/netdisk1/linpeng/Local-MIP/benchmark/list/open.txt"
result="/pub/netdisk1/linpeng/Local-MIP/result-new/Local-MIP/v6/open/"

sampleUnsat="12000"
bmsUnsatInfeas="200000"
bmsUnsatFeas="300000"
sampleSat="20000"
bmsSat="190000"
bmsFlip="20000"
bmsRandom="150000"
sp="3"

cutoff="60"
all_datas=($instance)
for a in $sampleUnsat
do
for b in $bmsUnsatInfeas
do
for c in $bmsUnsatFeas
do
for d in $sampleSat
do
for e in $bmsSat
do
for f in $bmsFlip
do
for g in $bmsRandom
do
for h in $sp
do
for co in $cutoff
do
  for((i=0;i<${#all_datas[*]};i++))
  do
    instance=${all_datas[$i]}
    res_solver_ins=$result/_${a}_${b}_${c}_${d}_${e}_${f}_${g}_${h}/${co}
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
        cd /pub/netdisk1/linpeng/Local-MIP/code/bin/Local-MIP/std
        time ./Local-MIP -i $instance/$file --cutoff=1204800 --sampleUnsat=$a --bmsUnsatInfeas=$b --bmsUnsatFeas=$c --sampleSat=$d --bmsSat=$e --bmsFlip=$f --bmsRandom=$g --sp=$h --PrintSol=1
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
done