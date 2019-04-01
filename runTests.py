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
    numtest = 20
    first = 2
    ranks = []
    for i in range(7):
        ranks.append(first)
        first = first*2
        #ranks = [i for i in range(8] #,16]
    datapoints = [i for i in range(20000000,100000001,20000000)] #[40] #, 80, 160]
    
    for datapoint in datapoints:
        for rank in ranks:
            testArray = [0.0 for i in range(6)] 
            my_file = open("test.txt", "w").write("")
            for k in range(numtest):
                print("datapointe:  ", datapoint, "num_ranks:  ", rank )
                with open("qtest3.sub", "w") as qt:
                    qt.write("!#/bin/bash\n"


                             "#$ -N main\n"
                             "#$ -pe mpi " + str(rank) + "\n"
                             "#$ -q amd8.q\n"
                             
                             
                             "mpirun -np $NSLOTS $HOME/COMS7900/clone/main " + str(datapoint) + " >> $HOME/COMS7900/clone/test.txt\n")
   

                o = subprocess.call(["qsub", "qtest3.sub"]);
                p = subprocess.Popen(["qstat"], stdout=subprocess.PIPE);
                stdout_data = p.communicate()[0]
                while (stdout_data != ""):
                    #print(stdout_data)
                    p = subprocess.Popen(["qstat"], stdout=subprocess.PIPE);
                    stdout_data = p.communicate()[0]
                    time.sleep(3)
                    #print(" ")
    

            with open("test.txt", "r") as file2:
                #file2.readline()
                allLines = file2.readlines() #.split(",")
            for line in allLines:
                temp = line.split(";")
                for j in range(2,8):
                  testArray[j-2] += float(temp[j])
            with open("output.txt", "a") as file2:
                file2.write(str(datapoint) + ";" + str(rank))
                for el in testArray:
                  file2.write(";" + str(el/numtest))
                file2.write("\n")
                
                  
                        


main()
