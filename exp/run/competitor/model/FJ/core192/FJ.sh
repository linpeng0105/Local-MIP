#!/bin/bash

INSTANCE_NUM=16
tmp_fifofile="/tmp/$$.fifo" # 脚本运行的当前进程ID号作为文件名
mkfifo "$tmp_fifofile" # 新建一个随机fifo管道文件
exec 6<>"$tmp_fifofile" # 定义文件描述符6指向这个fifo管道文件
rm $tmp_fifofile
for i in $(seq 1 $INSTANCE_NUM)
do
    echo # for循环 往 fifo管道文件中写入 $INSTANCE_NUM 个空行
done >&6

res_no="/pub/netdisk1/linpeng/Local-MIP/result-new/unused"
instance="/pub/netdisk1/linpeng/Local-MIP/benchmark/ALL"
benchmark_list="/pub/netdisk1/linpeng/Local-MIP/benchmark/list/BPP.txt"
result="/pub/netdisk1/linpeng/Local-MIP/result-new/FJ-16/log"
cutoff="10 60 300"
all_datas=($instance)
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
                cd /pub/netdisk1/linpeng/Local-MIP/code/exp/run/competitor/model/FJ/FJ-h
                time ./FJ -i $instance/$file -t $cut
                echo >&6
            } >$res_solver_ins/$file   2>>$res_solver_ins/$file &
        done
    done
done


ulimit -t 500
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
