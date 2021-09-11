# AP Demodulation
**AP Demodulation** is a mini-library of routines implementing the new approach to amplitude demodulation of arbitrary-bandwidth signals introduced in

> [M.&nbsp;Gabrielaitis. "Fast and Accurate Amplitude Demodulation of Wideband signals," IEEE Transactions on Signal Processing, vol.&nbsp;69, pp.&nbsp;4039-4054, 2021. DOI:&nbsp;10.1109/TSP.2021.3087899.](https://ieeexplore.ieee.org/document/9449954)<sup>[1](#footnote1)</sup>

In this approach, demodulation is posed as a recovery problem of a signal from an unlabelled mix of its true and corrupted sample points. The recovery is achieved via tailor-made alternating projection algorithms. In the context of narrowband signals, the new method outperforms classical demodulation algorithms in terms of robustness to data distortions and compatibility with nonuniform sampling. When considering demodulation of wideband signals, it surpasses the current state-of-the-art techniques in terms of computational efficiency by up to many orders of magnitude. Such performance enables practical applications of amplitude demodulation in previously inaccessible settings. Specifically, online and large-scale offline demodulation of wideband signals, signals in higher dimensions, and poorly-sampled signals become practically feasible.

#### Organization and usage of the library
Currently, the library includes C and MATLAB implementations, organized into separate folders in the repository root. The MATLAB implementation is divided into m-file and MEX function sections.<sup>[2](#footnote2)</sup> Folders of each programming language version contain comprehensive README.txt files that describe the contents of these folders and the usage and compilation guidelines of the library. In all cases, code with different signal demodulation examples is included to illustrate application of the library in practice. Note that the functionality of each library interface is accessible by a user via only a single function.
___
<a name="footnote1">1</a>: A freely available preprint of this work is on [arXiv](https://arxiv.org/abs/2102.04832).

<a name="footnote2">2</a>: The MEX implementation features shorter computing times than its m-file counterpart. The difference between the two versions is most noticeable for short 1D signals, when it may reach ~100 times. For very long signals, the MEX version advantage saturates at ~4 times. For signals defined in higher dimensions, the difference between the two implementations is small.
