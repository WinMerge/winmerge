rmdir /s/q hgtest 2> NUL
mkdir hgtest
pushd hgtest
hg init
echo aaaa > test.txt
hg add test.txt
hg commit -m "commit1"
hg branch testbranch
echo bbbb >> test.txt
hg commit -m "commit2"
hg update default
echo cccc >> test.txt
hg commit -m "commit3"
hg merge testbranch
popd

