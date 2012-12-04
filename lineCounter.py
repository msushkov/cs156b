MU = False

if MU:
    file123 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data123.txt', 'r')
    file123_86of4 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data123_86%of4.txt', 'r')
    file_7of4_1 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data_7%of4(valid).txt', 'r')
    file_7of4_2 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data_7%of4(aggr).txt', 'r')
else:
    file123 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data123.txt', 'r')
    file1234 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data1234.txt', 'r')
    file123_86of4 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data123_86%of4.txt', 'r')
    file_7of4_1 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data_7%of4(valid).txt', 'r')
    file_7of4_2 = open('/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data_7%of4(aggr).txt', 'r')

count = 0

for line in file123:
    count += 1
print "data123: " + str(count)

count = 0
for line in file123_86of4:
    count += 1
print "data123_86of4: " + str(count)
        

file123.close()
file1234.close()
file123_86of4.close()
file_7of4_1.close()
file_7of4_2.close()
