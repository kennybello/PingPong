# PingPong

Authors: Divino Kenneth Bello and Kohei Hisakuni
----------------------------------------------------------------------------------------------------------------------------
![alt tag](https://github.com/kennybello/PingPong/blob/master/Capture.png)

We called our Ping Pong game “Lonely Ping Pong.” In order to win the game the player must hit the opposing paddle three times and they can not lose. There is a dialog box above the table that displays a text when the user has received a point, fouled, or if the ball has gone out of bounds. Now, in order to try and simulate a real ping pong physics, we used kinematic equations and a vector reflection equation. To get our ball velocity, we subtracted gravity multiplied by time from the balls current y velocity and kept z constant, except for when the ball bounces, hits a net, or hits a paddle. We also used the formula d = vi*t+0.5*a*t2 in order to get the balls displacement or simply put position. To make the ball bounce and reflect, we used the formula Vnew = b * ( -2*(V dot N)*N + V ), where b takes away some of the speed from the ball once it bounces and the rest allowing our ball to bounce. The bounce formula is also used on the paddle, but some of the paddle’s velocity is added onto the ball’s velocity after bouncing in order to simulate the ball being hit. This allows us to hit the ball forward, backwards, and even sideways. We also use the paddles previous position and its currents position in order to check if the ball would have been hit during the previous timestep or the current timestep. Lastly, for aesthetic purposes we added the ball and paddle shadows using cylinders and boxes that change position based on the positions of the ball and paddle, and we also added an opposing paddle that moves in the x and y axis based on the ball’s x and y positions. 

![alt tag](https://github.com/kennybello/PingPong/blob/master/Capture1.png)
![alt tag](https://github.com/kennybello/PingPong/blob/master/Capture2.png)

