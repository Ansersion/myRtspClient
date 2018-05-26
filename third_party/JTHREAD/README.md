JThread
=======

Library location and contact
----------------------------

Normally, you should be able to download the latest version of the library
from this url: 
[http://research.edm.uhasselt.be/jori/jthread](http://research.edm.uhasselt.be/jori/jthread)

The documentation is in the `doc/manual.tex` LaTeX file, and the resulting
PDF can be found on-line at [http://jthread.readthedocs.io](http://jthread.readthedocs.io).

If you have questions about the library, you can mail me at:
[jori.liesenborgs@gmail.com](mailto:jori.liesenborgs@gmail.com)


Disclaimer & copyright
----------------------

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.


Installation notes
------------------

The library can be compiled using the [CMake](https://cmake.org/) build system. 
In case extra include directories or libraries are needed, you can use the 
`ADDITIONAL_` CMake variables to specify these. They will be stored in both 
the resulting JThread CMake configuration file and the pkg-config file.

