import subprocess
import time
#import os.system
#import os.spawn*
#import os.popen*
#import popen2.*
#import commands.*

def main():

    o = subprocess.call(["qsub", "qtest3.sub"]);
    p = subprocess.Popen(["qstat"], stdout=subprocess.PIPE);
    stdout_data = p.communicate()[0]
    while (stdout_data != ""):
        print(stdout_data)
        p = subprocess.Popen(["qstat"], stdout=subprocess.PIPE);
        stdout_data = p.communicate()[0]
        time.sleep(3)
        #print(" ")
    print("FINISHED")
                    
                  
                        


main()
