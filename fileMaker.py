from random import random

sushkov = True
MU = True

if MU:
    if sushkov:
        read_file = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/all.dta', 'r')
        index_file = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/all.idx', 'r')
        '''file1 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data1.txt', 'w')
        file2 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data2.txt', 'w')
        file3 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data3.txt', 'w')'''
        file4 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data4.txt', 'r')
        #file5 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data5.txt', 'w')
        file123 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data123.txt', 'r')
        #file1234 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data1234.txt', 'w')'''
        file123_86of4 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data123_86%of4.txt', 'w')
        file_7of4_1 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data_7%of4(valid).txt', 'w')
        file_7of4_2 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data_7%of4(aggr).txt', 'w')
    else:
        read_file = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\mu\\all.dta', 'r')
        index_file = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\mu\\all.idx', 'r')
        file1 = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\mu\data1.txt', 'w')
        file2 = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\mu\data2.txt', 'w')
        file3 = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\mu\data3.txt', 'w')
        file4 = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\mu\data4.txt', 'w')
        file5 = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\mu\data5.txt', 'w')
else:
    if sushkov:
        read_file = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/all.dta', 'r')
        index_file = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/all.idx', 'r')
        '''file1 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data1.txt', 'w')
        file2 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data2.txt', 'w')
        file3 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data3.txt', 'w')'''
        file4 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data4.txt', 'r')
        #file5 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data5.txt', 'w')'''
        file123 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data123.txt', 'r')
        #file1234 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data1234.txt', 'w')
        file123_86of4 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data123_86%of4.txt', 'w')
        file_7of4_1 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data_7%of4(valid).txt', 'w')
        file_7of4_2 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data_7%of4(aggr).txt', 'w')
    else:
        read_file = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\um\\all.dta', 'r')
        index_file = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\um\\all.idx', 'r')
        file1 = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\um\data1.txt', 'w')
        file2 = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\um\data2.txt', 'w')
        file3 = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\um\data3.txt', 'w')
        file4 = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\um\data4.txt', 'w')
        file5 = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\um\data5.txt', 'w')

#indices = index_file.readlines()

count1 = 0
count2 = 0

for line in file123:
    file123_86of4.write(line)

# for i, line in enumerate(read_file):
#     if int(indices[i]) == 1:
#         pass
#         #file1.write(line)
#         #file123.write(line)
#         #file1234.write(line)
#         #file123_86of4.write(line)
#     elif int(indices[i]) == 2:
#         pass
#         #file2.write(line)
#         #file123.write(line)
#         #file1234.write(line)
#         #file123_86of4.write(line)
#     elif int(indices[i]) == 3:   
#         pass     
#         #file3.write(line)
#         #file123.write(line)
#         #file1234.write(line)
#         #file123_86of4.write(line)
#     if int(indices[i]) == 4:
#         file4.write(line)
#         #file1234.write(line)
        
#         # random value between 0 and 1
#         # randVal = random.random()

#         # # write 86% of data4 into this file, as well as 123
#         # if randVal <= 0.86 and count1 < 1182275:
#         #     file123_86of4.write(line)
#         #     count1 += 1
#         # elif randVal > 0.86 and randVal <= 0.93 and count2 < 96231:
#         #     file_7of4_1.write(line)
#         #     count2 += 1
#         # else:
#         #     file_7of4_2.write(line)
#     #elif int(indices[i]) == 5:
#         #file5.write(line)

for line in file4:
    # random value between 0 and 1
    randVal = random()

    # write 86% of data4 into this file
    if randVal <= 0.86 and count1 < 1182275:
        file123_86of4.write(line)
        count1 += 1
    elif randVal > 0.86 and randVal <= 0.93 and count2 < 96231:
        file_7of4_1.write(line)
        count2 += 1
    else:
        file_7of4_2.write(line)
        
read_file.close()
index_file.close()
'''file1.close()
file2.close()
file3.close()'''
file4.close()
#file5.close()
file123.close()
#file1234.close()
file123_86of4.close()
file_7of4_1.close()
file_7of4_2.close()
