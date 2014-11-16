<h2><u>bencode-pretty</u></h2>

<p>This code is for two programs to create a "pretty" version of a bencoded file
that can easily be examined and edited. And a third program to "unpretty" it again
after editing.

<p>For an explanation of bencoding see for example:
<ul>
<li>
"Bencoding" in <a target=_blank href="https://wiki.theory.org/BitTorrentSpecification">https://wiki.theory.org/BitTorrentSpecification</a>
<li>
"bencoding" in <a target=_blank href="http://bittorrent.org/beps/bep_0003.html">http://bittorrent.org/beps/bep_0003.html</a>
</ul>

<p>To download this repository and build, in an appropriate location:
<pre>
git clone https://github.com/tool-maker/bencode-pretty
pushd bencode-pretty
make
make clean
</pre>

<p>I have tested with Linux, Cygwin and MSYS/MinGW.

<p>Building produces three executables: "bencode_pretty", "bencode_prettier" and "bencode_unpretty".
Each is a filter, meant to be run like this:
<pre>
cat some_file.torrent | ./bencode_pretty > some_file.torrent.txt
</pre>

<dl>
<dt><b>bencode_pretty</b>
<dd>
Modifies the encoding of strings (see the references above) so that there is
no length field preceding it, enclosing in quotes (") instead. And within the
strings non-printable characters are encoded using "," as an escape character
(because "," seems unlikely to appear in either tracker URL-s or file paths).
The resulting file will have a single line of text.
<p>
<dt><b>bencode_prettier</b>
<dd>
The same as "bencode_pretty" except that newline characters are inserted
so that each bencode element occupies its own line. No carriage returns are
inserted. So on Windows use an editor such as <a target=_blank href="http://notepad-plus-plus.org/">Notepad++</a>.
<p>
<dt><b>bencode_unpretty</b>
<dd>
Takes "pretty" or "prettier" files and recreates bencode. The idea is that
you will have changed some string(s) for tracker URL-s or file paths.
<p>
