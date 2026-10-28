pushd "%~dp0"
call build_html_en.bat %*
call build_html_es.bat %*
call build_html_fr.bat %*
call build_html_he.bat %*
call build_html_it.bat %*
call build_html_jp.bat %*
call build_html_pl.bat %*
popd

