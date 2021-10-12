import numpy as np
import pandas as pd
import sys
import csv

HEADER = ['P/V', 'data structure', 'thread number', 'value size', 
        'latency', 'throughput', 'log extra time', 'log extra ratio']

if __name__ == "__main__":
    if len(sys.argv) > 2:
        print ('arguments error')
        exit(1)
    
    logFileName = 'log.txt'
    if (len(sys.argv) == 2):
        logFileName = sys.argv[1]

    csvFile = open("./log.csv", 'w')
    csvFileWriter = csv.writer(csvFile)
    logFile = open(logFileName, 'r')
    csvFileWriter.writerow(HEADER)
    for line in logFile:
        csvRow = line[:-1].split(',')
        csvFileWriter.writerow(csvRow)
    
    csvFile.close()
    logFile.close()
    
