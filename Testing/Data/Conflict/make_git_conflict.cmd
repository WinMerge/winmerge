rmdir /s/q gittest 2> NUL
mkdir gittest
pushd gittest
git init
echo //aaaa > test.cpp
git add test.cpp
git commit -m "commit1"
git branch testbranch
git switch testbranch
echo //bbbb >> test.cpp
git add test.cpp
git commit -m "commit2"
git switch master
echo //cccc >> test.cpp
git add test.cpp
git commit -m "commit3"
git merge testbranch
popd

