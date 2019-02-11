import subprocess
import time
#import os.system
#import os.spawn*
#import os.popen*
#import popen2.*
#import commands.*

def main():
    with open("output.txt", "w") as ot:
        ot.write("DataPoints;Nodes;readTime;1qsort;SortBetweeNodes;TimedataExchange;2qsort;TimeTotal\n")
    first = 16
    ranks = []
    for i in range(2):
        ranks.append(first)
        first = first*16
        #ranks = [i for i in range(8] #,16]
    datapoints = [100000000] #i for i in range(10000000,20000001,10000000)] #[40] #, 80, 160]
    
    for rank in ranks:
        for datapoint in datapoints:
            with open("qtest3.sub", "w") as qt:
                qt.write("!#/bin/bash\n"


                   "#$ -N main\n"
                   "#$ -pe mpi " + str(rank) + "\n"
                   "#$ -q amd.q\n"


                   "mpirun -np $NSLOTS $HOME/COMS7900/main " + str(datapoint) + " >> $HOME/COMS7900/output.txt\n")
   
            o = subprocess.call(["qsub", "qtest3.sub"]);
            p = subprocess.Popen(["qstat"], stdout=subprocess.PIPE);
            stdout_data = p.communicate()[0]
            while (stdout_data != ""):
                #print(stdout_data)
                p = subprocess.Popen(["qstat"], stdout=subprocess.PIPE);
                stdout_data = p.communicate()[0]
                time.sleep(3)
                #print(" ")
    


main()
