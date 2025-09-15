rmdir /s/q hgtest 2> NUL
mkdir hgtest
pushd hgtest
hg init
echo //aaaa > test.cpp
hg add test.cpp
hg commit -m "commit1"
hg branch testbranch
echo //bbbb >> test.cpp
hg commit -m "commit2"
hg update default
echo //cccc >> test.cpp
hg commit -m "commit3"
hg merge testbranch
popd

