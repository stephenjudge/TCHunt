* Quickly find encrypted TrueCrypt files and other files made-up
  of random data that have the four (4) TCHunt attributes.

* The four (4) TCHunt attributes:

   1. The suspect file size modulo 512 must equal zero.
   2. The suspect file size is at least 19 KB in size (although in practice this is set to 5 MB).
   3. The suspect file contents pass a chi-square distribution test.
   4. The suspect file must not contain a common file header.

* TCHunt 1.5a has been released under the GPLv3 software license.
  Older versions of TCHunt were released under a non-free license.

* To compile TCHunt, you need the boost and FLTK libraries. I link statically
  and you should consider doing that as well. Read the build scripts to see how
  I compile TCHunt.

* Visit the TCHunt homepage for more details:

   http://16s.us/TCHunt/

END

