read_file = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\um\\all.dta', 'r')
index_file = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\um\\all.idx', 'r')
file1 = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\um\data1.txt', 'w')
file2 = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\um\data2.txt', 'w')
file3 = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\um\data3.txt', 'w')
file4 = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\um\data4.txt', 'w')
file5 = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\um\data5.txt', 'w')

indices = index_file.readlines()

for i, line in enumerate(read_file):
    if int(indices[i]) == 1:
        file1.write(line)
    elif int(indices[i]) == 2:
        file2.write(line)
    elif int(indices[i]) == 3:
        file3.write(line)
    elif int(indices[i]) == 4:
        file4.write(line)
    elif int(indices[i]) == 5:
        file5.write(line)
        
read_file.close()
index_file.close()
file1.close()
file2.close()
file3.close()
file4.close()
file5.close()