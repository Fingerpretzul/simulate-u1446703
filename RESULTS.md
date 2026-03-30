# Week 8

At the moment, there doesn't appear to be any physics bugs with my actual simulator. The only issue at the moment is that it runs pretty slow, however the assignment prompt doesn't describe a speed minium. I would imagine the model may have optimized for particular setup however, and would not be surprised if being able to describe a new setup introduces a whole host of new physics bugs

I surprisingly had to make no changes to my prompt. I took my time and was incredibly careful when setting up my first prompt, and it appears to have paid off. I let the model run a few iterations on it's own for a while and came back to a functioning (albeit slow) simulator. Frankly I didn't even let it run that long, and I would imagine even better results if I did (I misunderstood the Github page and thought the assignment was due next Saturday 3/27, so I was unfortunately a little short on time when I realized my mistake)

One change I did make to the prompt was to specify that the simulator should be able to comfortably simulate 1000 balls at 30 FPS.

# Week 9

I made very few changes to my prompt, the biggest one was some lines trying to emphasize the importance of tests. I also shifted to instruct it to write tests before beginning a new feature, in order to to better define the parameters of what it was working towards

As for bugs, I thought until I came to write this that it was mostly bug free. However I then realized i had been building the project incorrectly, which was not updating it with the newest changes. As I finally rebuilt, I noticed a glaring bug where some balls seem to just begin completely static. It seems to be random, as each time I restart it affects different balls.

When I noticed htis