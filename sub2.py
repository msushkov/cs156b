read_movie_file = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\mu\data2.txt', 'r')
read_user_file = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\um\data2.txt', 'r')
qual = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\mu\qual.dta', 'r')
write_file = open('C:\Users\Mike\Documents\Mike\Caltech\\aJunior Fall\CS 156b\sub2\average_of_averages.txt', 'w')


previous_movie = '1'
index = 0
rating = 0.0
movie_ratings = {}

for line in read_movie_file:
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

read_movie_file.close()


# now for the user
previous_user = '1'
index = 0
rating = 0.0
user_ratings = {}

for line in read_user_file:
    vals = line.split()
    current_user = vals[0]
    current_rating = int(vals[3])
    
    if previous_user != current_user:
        average_rating = rating / index
        user_ratings[previous_user] = average_rating
        index = 1
        rating = current_rating
        
    else:
        if current_rating != 0:
            index += 1
        rating += current_rating
        
    previous_user = current_user
    
# deals with the last line of the file
average_rating = rating / index
user_ratings[previous_user] = average_rating

read_user_file.close()


# creating the output


for line in qual:
    vals = line.split()
    current_user_rating = user_ratings[vals[0]]
    current_movie_rating = movie_ratings[vals[1]]
    predicted_score = (float(current_user_rating) + float(current_movie_rating)) / 2.
    write_file.write(str(predicted_score) + '\n')

write_file.close()
qual.close()