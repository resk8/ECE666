#!/bin/bash

#BENCHMARK="x264 freqmine streamcluster vips ferret fluidanimate dedup canneal bodytrack swaptions"
#BENCHMARK="blackscholes"
#BENCHMARK="canneal"
#BENCHMARK="blackscholes"
#BENCHMARK="bodytrack"
BENCHMARK="ferret"
for benchmark in $BENCHMARK; do
  rcs="_16c_simsmall.rcS"
  #rcs="_16c_test.rcS"
  runscript="$benchmark$rcs"
  ptracefile="_ptrace.log.gz"
  benchmark_ptrace_file="$benchmark$ptracefile"
  outfile_suffix=".out"
  errfile_suffix=".err"
  outfile="$benchmark$outfile_suffix"
  errfile="$benchmark$errfile_suffix"

  #gnome-terminal --title="pf_$benchmark" -- bash -c "sudo ./build/X86/gem5.opt --outdir=/home/resk8/ECE666/Parsec/pf_on_$benchmark configs/example/fs.py --num-cpus=16 --num-dirs=16 --network=garnet --topology=Mesh_XY --mesh-rows=4 --l1i_size=32768 --l1d_size=32768 --l1i_assoc=4 --l1d_assoc=4 --l2_size=262144 --l2_assoc=4 --ruby --pf-on --dir-on --kernel=/home/resk8/dev/x86_64-vmlinux-2.6.28.4-smp --disk-image=/home/resk8/dev/x86root-parsec.img --cpu-type=TimingSimpleCPU --script=/home/resk8/dev/parsec/$runscript && exit; exec bash"
  gnome-terminal --title="nopf_$benchmark" -- bash -c "sudo ./build/X86/gem5.opt --outdir=/home/resk8/ECE666/Parsec/pf_off_$benchmark configs/example/fs.py --num-cpus=16 --num-dirs=16 --network=garnet --topology=Mesh_XY --mesh-rows=4 --l1i_size=32768 --l1d_size=32768 --l1i_assoc=4 --l1d_assoc=4 --l2_size=262144 --l2_assoc=4 --ruby --kernel=/home/resk8/dev/x86_64-vmlinux-2.6.28.4-smp --disk-image=/home/resk8/dev/x86root-parsec.img --cpu-type=TimingSimpleCPU --script=/home/resk8/dev/parsec/$runscript && exit; exec bash"
  #sleep 10
  #gnome-terminal -- bash -c "sudo ./util/term/m5term localhost 3456; exec bash"
done
exit 0
