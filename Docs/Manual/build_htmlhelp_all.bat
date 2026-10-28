pushd "%~dp0"
call build_htmlhelp_en.bat %*
call build_htmlhelp_es.bat %*
call build_htmlhelp_fr.bat %*
call build_htmlhelp_he.bat %*
call build_htmlhelp_it.bat %*
call build_htmlhelp_jp.bat %*
call build_htmlhelp_pl.bat %*
popd

