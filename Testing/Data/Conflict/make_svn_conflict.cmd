rmdir /s/q svnrepo 2> NUL
rmdir /s/q svntest 2> NUL
mkdir svnrepo
pushd svnrepo
svnadmin create svntest
popd
svn checkout file:///%~dp0svnrepo/svntest
pushd svntest
svn mkdir trunk
svn mkdir branches
svn mkdir tags
svn commit -m "commit0"
echo aaaa > trunk/test.txt
svn add trunk/test.txt
svn commit -m "commit1" 
svn copy trunk branches/testbranch
svn commit -m "commit2"
echo bbbb >> branches/testbranch/test.txt
svn commit -m "commit3" 
echo cccc >> trunk/test.txt
svn commit -m "commit4"
svn update 
pushd trunk
svn merge ../branches/testbranch
popd
popd

