# RouletteStrategy
This program can be used to produce and test progressive roulette betting strategies on any spot on any table. The program contains three major features.

** If you have any questions about the program feel free to email me at _brettcs@umich.edu_ and I will respond to your request as soon as I can. **

## Compute An Optimal Strategy
This part of the program can be utilized to produce and new progressive betting strategy using one of three different computation protocols.

1. Maximize Win EV Sum
  - This mode will print to the screen a sequence of bets in a table that maximizes the sum of the column for expected value on wins.
2. Descending Win EV
  - This mode will print to the screen a sequence of bets in a table. The expected value on wins for each roll must be lower than the expected value on wins for the roll before it. The expected value on wins for each roll is then maximized while adhering to this constraint.
3. Simulator Optimal ROI
  - This mode will find the absolute most optimal progressive betting strategy for a table by running every single permuation of possible bets through the simulator and saving the result with the greatest ROI. As you can imagine, running every single permutation of bets through the simulator takes a lot of time to compute. The application attempts to reduce the time by utlizing a multithreaded implementation, but it can still take a long time to produce quality output. The benefit to this however is the output is absolutely certain to be the best possible solution.
  
**  All outputs from the optimal strategy solution finder can be immediately tested in the accompanying simulator to determine expected ROI in the long term by running a large number of simulations ( > 1,000,000). **

## Run Simulations On An Existing Strategy
This part of the program allows a user to test a custom progressive betting strategy on any table for a defined number of simulations.

## Run simulations on an existing strategy with playthrough requirements for bonus.
This part of the program allows a user to test a custom progressive betting strategy. The user is asked for an initial bankroll and bonus offerings from the casino that is to be tested. The program will then output how many times the bonus money was played through enabling the user to withdraw from the casino with profit and how many times the bankroll went bust or was played through, but didn't enable the user to withdraw with profit.

If you are unaware of how online casino bonuses work, read up on the terms and conditions for bonuses at a popular site like _bovada.lv_. Many online casinos offer deposit bonuses of up to 300%, but that money has to be "played through" to allow a user to withdraw funds. On a game with a relative low house edge like roulette, this playthrough is often prorated down to 5%, making a simulator such as this very handy to determine long-term expectation.
