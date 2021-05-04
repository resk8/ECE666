#!/usr/bin/python3

import sys
import os
import re
import argparse
import traceback
import re
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

parser = argparse.ArgumentParser(description='extract data from parsec benchmark runs and generate graphs')
parser.add_argument('benchmark_dir')
args = parser.parse_args()

def main():
    """
    TODO
    """  
    field_list=['simSeconds', 
                'simInsts',
                'system.ruby.network.msg_count.Request_Control',
                'system.ruby.network.msg_count.Response_Data',
                'system.ruby.network.msg_count.Response_Control',
                'system.ruby.network.msg_count.Writeback_Data',
                'system.ruby.network.msg_count.Writeback_Control',
                'system.ruby.network.msg_count.Broadcast_Control',
                'system.ruby.network.msg_count.Multicast_Control',
                'system.ruby.network.msg_count.Unblock_Control']

    try:
        if not os.path.isdir(args.benchmark_dir):
            raise Exception("%s is not a valid path" % args.benchmark_dir)
        
        stats_file_list = []
        for root, dirs, files in os.walk(os.path.join(os.getcwd(),args.benchmark_dir)):
            for file in files:
                if file == "stats.txt":
                    stats_file_list.append(os.path.join(root, file))

        data = {}
        for stat_file in sorted(stats_file_list):
            benchmark_mode = re.findall("(?=pf_)(.*)(?=\/)", stat_file)[0]            
            (dir_index,benchmark) = benchmark_mode.rsplit('_',1)          
            data.setdefault(benchmark,{}).setdefault(dir_index,{})
            with open(stat_file, 'r') as f:
                lines = f.readlines()
                for line in lines:
                    value = re.findall("(?<=\s)(-?[0-9.]+)(?=\s)", line)
                    field = re.findall("^([^\s]+)",line)                    
                    if value and field:
                        if field[0] in field_list:
                            data[benchmark][dir_index].setdefault(field[0], float(value[0]))                            
             
        for field in field_list:
            average_pf = 0 
            average_no_pf = 0
            difference = 0   

            pf_field_list=[]
            no_pf_field_list=[]
            for BenMrk in data.keys():
                pf_data = 0
                no_pf_data = 0
                try: #try block to check if field key exist in the dict                          
                    pf_data = data[BenMrk]['pf_on'][field] 
                except:
                    pass

                try: #try block to check if field key exist in the dict                          
                    no_pf_data = data[BenMrk]['pf_off'][field]
                except:
                    pass
                
                pf_field_list.append(pf_data)
                no_pf_field_list.append(no_pf_data)
                diff = abs(pf_data - no_pf_data)
                median = (pf_data + no_pf_data)/2         
                celling = median+3*diff
                floor = median-3*diff if median-3*diff > 0 else 0
                if field in ['simSeconds','simInsts']:
                    title = field
                    msg = 'seconds'                    
                else:
                    title = field.rsplit('.',1)[1]
                    msg = 'count'

                average_pf +=  pf_data
                average_no_pf += no_pf_data

            width = 0.2
            plt.figure(figsize=(15,3))
            plt.bar(data.keys(), no_pf_field_list, width=width, align='edge', label='no probe filtering')
            plt.bar(data.keys(), pf_field_list, width=-width, align='edge', label='probe filtering')    
            plt.xticks([r for r in range(len(list(data.keys())))],list(data.keys()))
            plt.ylabel(msg)
            plt.ylim(floor,celling)
            plt.title(title)        

            plt.legend()
            plt.savefig(field + '.png')
            plt.show()

            average_pf = average_pf/len(data.keys())    
            average_no_pf = average_no_pf/len(data.keys())
            try:
                if average_pf > average_no_pf:
                    difference = (abs(average_pf - average_no_pf)/average_pf)*100
                else:
                    difference = (abs(average_pf - average_no_pf)/average_no_pf)*100
            except:
                difference = 0                
            print("{} : average_pf {}, average_no_pf {}, difference percent {}".format(field, average_pf, average_no_pf, difference))

    except Exception as e:
        traceback.print_exc()
        sys.exit(-1)


if __name__ == '__main__':
    main()
    sys.exit(0)
