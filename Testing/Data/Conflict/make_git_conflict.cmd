rmdir /s/q gittest 2> NUL
mkdir gittest
pushd gittest
git init
echo aaaa > test.txt
git add test.txt
git commit -m "commit1"
git branch testbranch
git switch testbranch
echo bbbb >> test.txt
git add test.txt
git commit -m "commit2"
git switch master
echo cccc >> test.txt
git add test.txt
git commit -m "commit3"
git merge testbranch
popd

