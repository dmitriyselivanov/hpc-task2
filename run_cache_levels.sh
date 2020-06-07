rm -f *.txt *.png *~
g++ -pthread -O0 -o cache_levels.out cache_levels.cc
./cache_levels.out
python3 plot.py
python3 plot_threads.py
