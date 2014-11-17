echo "if [ -f ../bencode_unpretty ]; then PATH=..:\$PATH; fi"
echo "if ! which bencode_unpretty>/dev/null; then echo bencode_unpretty not found; exit; fi"
echo "for f in * .*; do if [ \$f != \"*\" ] && [ \$f != \".\" ] && [ \$f != \"..\" ]; then echo current directory is not empty; exit; fi; done"
for f in $*; do
  echo $f >&2
  b=`basename $f`
  echo "echo $b; cat <<\"EOF\" | bencode_unpretty > $b"
  cat $f | ./bencode_prettiest
  echo
  echo EOF
done
