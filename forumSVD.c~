  // Keep looping until you have stopped making significant progress
  while (probe_rmse < probe_rmse_last - MIN_IMPROVEMENT) {

    for (custId: all customers) {

      // *** Calc all movie weights sum for this custId, for each factor
      // *** separetly and store in array:  sum_mw[custId][f]
      calc_user_moviebag (custId);


      // *** Keep a separe sum of errors for every factor
      for (f=0; f<TOTAL_FEATURES; f++) {      
        tmp_sum[f] = 0.0;
      }

      for (movieId: all movies custId has RATED) {
          
        pred = predict_rating (movieId, custId);
        err = real_rating - pred;

        //*** train biases                    
        c_bias[custId] += (LRATE_cb * (err - LAMDA_cb * c_bias[custId]));
        m_bias[movieId] += (LRATE_mb * (err - LAMDA_mb * m_bias[movieId]));          


        for (f=1; f<=NUM_OF_FACTORS; f++) {      
                
          // Cache off old values
          tmp_cf = c_factor[custId][f];
          tmp_mf = m_factor[movieId][f];
            
          // *** User vector
          c_factor[custId][f] += (LRATE_c * (err * tmp_mf - LAMDA_c * tmp_cf));

          // *** Movie vector (NORM is the sqrt(user_support) that normalizes the sum(movie_weights))
          m_factor[movieId][f] += (LRATE_m * (err * (tmp_cf + NORM[custId] * sum_mw[custId][f]) - LAMDA_m * tmp_mf));

          // *** movie_weights (just sum the gradients... no train yet)
          tmp_sum[f] += (err * NORM[custId] * tmp_mf);
         
        }
      }         

      // *** Train movie_weights
      for (movieId: all movies this custId has SEEN) {
   
        for (f=1; f<=TOTAL_FACTORS; f++) {      

          // Cache off old feature values          
          tmp_mw = movie_weight[movieId][f];
  
          // *** movie weights
          movie_weight[movieId][f] += (LRATE_mw * (tmp_sum - LAMDA_mw * tmp_mw)); 

          // *** Update Sum of the weights for this user (trick for speed)
          sum_mw[custId][f] += (movie_weight[movieId][f] - tmp_mw);

        }
        
      }

    } // *** end For Customers Loop
    
    // *** Calc all movie weights sums for ALL custIds and all factors
    // *** separetly and store in array:  sum_mw[custId][f]
    calc_users_moviebag ();

    calc_rmse_for_probe();
  } // *** end WHILE