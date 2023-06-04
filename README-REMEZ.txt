This is a fairly straightforward binding to Jake Janovetz remez exchange (or Parks - McClellan algorithm) for
symetrical finite impule response discrete time filter design in EE.

Jake Janovetz's code seems the sole C implementation freely available.
Although it seems to works fine for standard filter, I had bad experience trying to design an hilbert transformer
with it, the code seems bugged as of 24th of Jan 2012.

I've split the bands based specification part from the dense grid optimization algorithm, so as to allow
the use of the code on arbitrary, user specified desired frequency response and error weighting.

The remez exchange algorithm for filter design is described in a IEEE paper entitled:


"FIR Digital Filter Design Techniques Using Weighted Chebyshev Approximation", published in Proceedings of the IEEE, Vol. 63, No. 4, April 1975, Rabiner, Parks, McClellan


--
Philippe Strauss
<philippe at strauss-acoustics.ch>
