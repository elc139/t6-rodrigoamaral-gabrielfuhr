./a.out 1024 32 > results_1.csv
./a.out 1024 64 >> results_1.csv
./a.out 1024 400 >> results_1.csv

mpirun -n 2 fractalpar 1024 32 > results_2.csv
mpirun -n 2 fractalpar 1024 64 >> results_2.csv
mpirun -n 2 fractalpar 1024 400 >> results_2.csv
mpirun -n 3 fractalpar 1024 32 >> results_2.csv
mpirun -n 3 fractalpar 1024 64 >> results_2.csv
mpirun -n 3 fractalpar 1024 400 >> results_2.csv
mpirun -n 4 fractalpar 1024 32 >> results_2.csv
mpirun -n 4 fractalpar 1024 64 >> results_2.csv
mpirun --use-hwthread-cpus -n 8 fractalpar 1024 400 >> results_2.csv
mpirun --use-hwthread-cpus -n 8 fractalpar 1024 32 >> results_2.csv
mpirun --use-hwthread-cpus -n 8 fractalpar 1024 64 >> results_2.csv
mpirun --use-hwthread-cpus -n 8 fractalpar 1024 400 >> results_2.csv