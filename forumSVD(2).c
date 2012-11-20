
cNorm = |N(u)|^-1/2
for each movie m seen by customer c
{
   for int f = 0 to totalFactors
       sum_y_factors += y_factors[m][f];
}

for int f = 0 to totalFactors
{
   sum_y_factors[f] = cNorm * sum_y_factors[f];
   tmp_sum_y_factors[f] = sum_y_factors[f];
}



////in prediction method
p = glob_avg + cBias + mBias

for each factor f
   p += movie_factors[m][f] * (customer_factors[c][f] + tmp_sum_y_factors[f]);


////in update method
movie_factors[m][f] += lrate * 
	(err * (customer_factors[c][f] + tmp_sum_y_factors[f]) - reg_2 * movie_factors[m][f]);

tmp_sum_y_factors[f] += lrate * (err * movie_factors[m][f] - reg * tmp_sum_y_factors[f]);




for each movie m seen by customer c
{
   for int f = 0 to totalFactors
       y_factors[m][f] +=   cNorm * (tmp_sum_y_factors - sum_y_factors);
}