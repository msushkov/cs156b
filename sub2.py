read_file = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\mu\\all.dta', 'r')
index_file = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\mu\\all.idx', 'r')
write_file = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\mu\movie_average.txt', 'w')

previous_movie = '1'
index = 0
rating = 0.0
movie_ratings = {}

for line in read_file:
    vals = line.split()
    current_movie = vals[1]
    current_rating = int(vals[3])
    
    if previous_movie != current_movie:
        average_rating = rating / index
        movie_ratings[previous_movie] = average_rating
        index = 1
        rating = current_rating
        
    else:
        if current_rating != 0:
            index += 1
        rating += current_rating
        
    previous_movie = current_movie
    
# deals with the last line of the file
average_rating = rating / index
movie_ratings[previous_movie] = average_rating



    
read_file.close()
index_file.close()
write_file.close()