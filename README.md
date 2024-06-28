# objchk
Check and ease human inspection of object files used to program microprocessor EPROMS.
An EPROM is an 
<a href="https://www.techtarget.com/whatis/definition/EPROM">
Erasable Programmable Read Only Memory chip</a>
and generally provides the executable binary to a microprocessor that is non-volatile
in that it survives the power off state.

This "objchk" code is anachronistic in that EPROMS and Microprocessors belong
to a bygone era. In those days Intel used "
<a href="https://en.wikipedia.org/wiki/Intel_HEX">hex</a>" formatted information and
Motorola used "
<a href="https://en.wikipedia.org/wiki/SREC_(file_format)">S19</a>"
formatted information to allow code to be transferred by media such as
<a href="https://en.wikipedia.org/wiki/Punched_tape">punched paper tape</a>.
Such a paper tape could be punched by a device like a Teletype ASR (auto-send/receive)
33 terminal from 110 Buad serial communication from a Main-frame or Mini computer.
The reverse operation of reading the tape with an ASR 33 could then be used with a
microprocessor development system or an EPROM programmer. Since both serial transmission
and the paper tape writing and reading process could result in incorrect information
transfer both Intel and Motorola information formats provided (limited) error checking
with a checksum incorporated into the format.

By default "objchk" outputs standard input or file data in a format of multiple
lines of three columns separated by a space character to standard output.
The first column is an address value taken from the start of the record in the
input/file which applies to the leftmost data byte in the second and third columns.
The second column is a hexadecimal representation of the value of each data byte in
the stream of input data and the third column shows any printable ASCII
characters in those same bytes of data.
An example of the default output of objchk is; -
```
% ./objchk test/wikipedia.hex 
0100 21 46 01 36 01 21 47 01 36 00 7e fe 09 d2 19 01 !F.6.!G.6.~.....
0110 21 46 01 7e 17 c2 00 01 ff 5f 16 00 21 48 01 19 !F.~....._..!H..
0120 19 4e 79 23 46 23 96 57 78 23 9e da 3f 01 b2 ca .Ny#F#.Wx#..?...
0130 3f 01 56 70 2b 5e 71 2b 72 2b 73 21 46 01 34 21 ?.Vp+^q+r+s!F.4!
%
```

## Help Information
The objchk command has a number of options which are outlined in the useage information.
Start the utility with the "-h" (help) option to see all the other options that are available; -
```
% ./objchk -h
Usage:
 objchk [-a] [-A] [-D] [-h] [-o TXT] [-v] [-w INT][ fileName1 .. [ fileNameX ]]
 -a ...... disable address output mode
 -A ...... disable ASCII output mode
 -D ...... enable Debug output mode
 -h ...... this help/usage information
 -o TXT .. output the dump to an output file named "TXT" 
 -v ...... enable more verbose information output
 -w INT .. output INT bytes per line - where 1 <= INT <= 32
%
```
The objchk utility exits with a return value of 0 on success and greater than zero if an error occurs.

## License and Disclaimer
objchk is released under the MIT license and must be used at your own risk.

## See Also
SRecord handles intel format records and it's documentation lists a large number of other formats that it handles.
It has a long standing history of development and appears to have enthusiastic reviews.
It has Microsoft Windows and as well as Linux versions available.
The SRecord website has the following URL; -
https://srecord.sourceforge.net
Source code may be found on github (https://github.com/sierrafoxtrot/srecord),
or on sourceforge (https://sourceforge.net/projects/srecord/)
