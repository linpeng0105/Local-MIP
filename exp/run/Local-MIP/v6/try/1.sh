#!/bin/bash

SEND_THREAD_NUM=16
tmp_fifofile="/tmp/$$.fifo" # 脚本运行的当前进程ID号作为文件名
mkfifo "$tmp_fifofile" # 新建一个随机fifo管道文件
exec 6<>"$tmp_fifofile" # 定义文件描述符6指向这个fifo管道文件
rm $tmp_fifofile
for i in $(seq 1 $SEND_THREAD_NUM)
do
  echo # for循环 往 fifo管道文件中写入 $SEND_THREAD_NUM 个空行
done >&6

res_no="/pub/netdisk1/linpeng/Local-MIP/result-new/unused"
instance="/pub/netdisk1/linpeng/Local-MIP/benchmark/ALL"
benchmark_list="/pub/netdisk1/linpeng/Local-MIP/benchmark/list/ALL-1.txt"
result="/pub/netdisk1/linpeng/Local-MIP/result-new/Local-MIP/v6/try/"

sampleUnsat="12"
bmsUnsatInfeas="3000"
bmsUnsatFeas="3000"
sampleSat="20"
bmsSat="200"
bmsFlip="20"
bmsRandom="150"
Seed="2832"

cutoff="10 60"
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
for h in $Seed
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
        cd /pub/netdisk1/linpeng/Local-MIP/code/bin/Local-MIP/v6
        time ./Local-MIP -i $instance/$file --cutoff=$co --sampleUnsat=$a --bmsUnsatInfeas=$b --bmsUnsatFeas=$c --sampleSat=$d --bmsSat=$e --bmsFlip=$f --bmsRandom=$g --Seed=$h
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
exit 0
