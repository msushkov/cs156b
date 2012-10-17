read_file = open('C:\Users\Mike\Documents\Mike\Caltech\Junior Fall\CS 156b\mu\qual.dta', 'r')
write_file = open('C:\Users\Mike\Documents\Mike\Caltech\Junior Fall\CS 156b\mu\sub2.txt', 'w')


for line in read_file:
    write_file.write('3.84358\n')
    
read_file.close()
write_file.close()