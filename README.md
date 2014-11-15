bencode-pretty
==============

This code is for two programs to create a "pretty" version of a bencoded file
that can be easily examined and edited, and another to "unpretty" it again.

For an explanation of bencooding see:

- "Bencoding" in https://wiki.theory.org/BitTorrentSpecification

or

- "bencoding" in http://bittorrent.org/beps/bep_0003.html

To download this repository and build, in an appropriate location:

git clone https://github.com/tool-maker/bencode-pretty
pushd bencode-pretty
make

I have tested with Linux, Cygwin and MSYS/MinGW.

There are three executables: bencode_pretty, bencode_prettier and bencode_unpretty.
Each is a filter, meant to be run like this:

cat some_file.torrent | ./bencode_pretty > some_file.torrent.txt

The executables created:

"bencode_pretty"

Modifies the encoding of strings (see the references above) so that there is
no length field preceding it, enclosing in quotes (") instead. And within the
strings non-printable characters are encoded using "," as an escape character
(because "," seems unlikely to appear in either tracker URL-s or file paths.
The resulting file will have a single line of text.

"bencode_prettier"

The same as "bencode_pretty" except that newline characters are inserted
so that each bencode element occupies its own line. No carriage returns are
inserted, so use something like Notepad++ on Windows.

"bencode_unpretty"

Takes "pretty" or "prettier" files and recreates bencode. The idea is that
you will have changed some string(s) for tracker URL-s or file paths.
