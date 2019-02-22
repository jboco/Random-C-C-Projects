sumo_with_semaphore.c'  provides an example of programs which communicate through shared memory and use semaphores to prevent critical region violations. 
In the sumo program each wrestler ('rikishi') is prevented from entering the critical region if the other wrestler occupies it.
The code implements a contest between four wrestlers (North, South, East, West).  The four wrestlers are to push in their individual directions
(the four points of the compass) and the first wrestler whose radial distance from the center of the ring is greater than DOHYO_SAIZU in
that wrestler's quadrant (e.g. NW to NE is North's quadrant) wins the contest. 
