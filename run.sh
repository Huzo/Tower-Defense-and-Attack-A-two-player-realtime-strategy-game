./game_admin &
sleep 1     #take some time for game admin to start

./display_admin &
sleep 1     #take some time for display admin to start

./input_admin &
sleep 1		#take some time for input admin to start

./courier 2 &   #start couriers simultaniously 
sleep 1
./courier 0 &
sleep 1
./courier 1 &

./timer 0 &    #start timers simultaniously 
sleep 1
./timer 1 &
sleep 1
./timer 2 &

./painter &    #run painter in background

./keyboard     #run keyboard in foreground