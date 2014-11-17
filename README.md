<h2><u>bencode-pretty</u></h2>

<p>This code is for three programs to create a "pretty" version of a bencoded file
that can easily be examined and edited. And a fourth program to "unpretty" it again
after editing.

<p>For an explanation of bencoding see for example:
<ul>
<li>
"Bencoding" in <a target=_blank href="https://wiki.theory.org/BitTorrentSpecification">https://wiki.theory.org/BitTorrentSpecification</a>
<li>
"bencoding" in <a target=_blank href="http://bittorrent.org/beps/bep_0003.html">http://bittorrent.org/beps/bep_0003.html</a>
</ul>

<p>To download this repository and build, in the user's home directory:
<pre>
pushd ~
git clone https://github.com/tool-maker/bencode-pretty
pushd bencode-pretty
make
make clean
popd

</pre>

<p>I have tested with
<a target=_blank href="https://www.debian.org/">Linux</a>,
<a target=_blank href="https://www.cygwin.com/">Cygwin</a> and
<a target=_blank href="http://www.mingw.org/wiki/MSYS">MSYS/MinGW</a>.

<p>Building produces <b><u>three executables</u></b>: "bencode_pretty", "bencode_prettier" and "bencode_unpretty".
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
inserted. So on Windows use an editor such as
<a target=_blank href="http://notepad-plus-plus.org/">Notepad++</a>.
<p>
<dt><b>bencode_prettiest</b>
<dd>
The same as "bencode_prettier" except that indents are also inserted so that
the logical structure of the data is more apparent.
<p>
<dt><b>bencode_unpretty</b>
<dd>
Takes "pretty", "prettier" or "prettiest" files and recreates bencode. The idea is that
you will have changed some string(s) for tracker URL-s or file paths.
</dl>
<p>
<p>There is also a <b><u>bash script</u></b>: "bencode_many_prettiest.sh". This should
called like this (for example):
<pre>
pushd ~/bencode-pretty
bash bencode_many_prettiest.sh ~/private/rtorrent/work/*.rtorrent > many_rtorrents.txt
popd

</pre>
The file that is produced ("many_rtorrents.txt" in the example) is itself a script.
It will contain a "prettiest" version of each of the bencoded files passed as arguments,
along with enough bash script wrapping so that when it is run, it will recreate
the bencoded version of the files in the current working directory. The idea is to
allow a mass edit of many bencoded files using the "replace all" operation
of a text editor. After it has been edited, it should be run in an empty
directory (it checks to be sure the current directory is empty), in which the
individual files will be recreated.. Either this empty directory
should be within the directory where
"bencode_unpretty" et al were built, or "bencode_unpretty" must be in the
program lookup path ("PATH" environment variable) when the script is run 
("which bencode_unpretty"). So it could be called like this (for example):
<pre>
pushd ~/bencode-pretty
mkdir work_new
pushd work_new
bash ../many_rtorrents.txt
popd
popd

</pre>
<p>
<p>The idea in this example is that an rtorrent installation is being copied to another
server or under a different user (a different "slot" on a seedbox for example) where
the absolute path for the user's home directory will have
changed. One would:

<ol>
<li>Build bencode_pretty on each "slot".
<li>Generate the script on the slot from which the rtorrent installation is being copied.
<li>Edit the home directory appearing in the generated script using a "replace all"
operation in a text editor.
<li>Place the edited script onto the new slot.
<li>Run the edited script on the new slot.
</ol>
The "*.rtorrent" files that get generated then replace the previous ones on the new slot.
