#!/bin/sh

html_stylesheet=../Manual-html.dsl
html_inputfile=../WinMerge_help.xml
html_outputdir=html

pdf_stylesheet=../Manual-html.dsl
pdf_inputfile=../WinMerge_help.xml
pdf_outputdir=pdf

txt_stylesheet=../Manual-html.dsl
txt_inputfile=../WinMerge_help.xml
txt_outputdir=txt

function patch_inputfile {
  sed -e 's/http:\/\/www\.oasis-open\.org\/docbook\/xml\/4\.2\/docbookx\.dtd/\/usr\/share\/sgml\/docbook\/sgml-dtd-4\.2-1\.0-24\/docbook\.dtd/g' "$1" > "$1.tmp"
  mv "$1.tmp" "$1"
}

function create_dir {
  if ! [ -d "$1" ]; then
    mkdir "$1"
  fi
}

if [ "$1" = "html" ] || [ "$1" = "" ]; then #HTML files...
  create_dir $html_outputdir
  
  echo "Patch inputfile..."
  patch_inputfile $html_inputfile
  
  echo "Copy images..."
  create_dir $html_outputdir/images
  cp ../images/*.gif $html_outputdir/images/.
  
  echo "Copy screenshots..."
  create_dir $html_outputdir/screenshots
  cp ../screenshots/*.* $html_outputdir/screenshots/.
  
  echo "Create HTML files..."
  docbook2html -d "$html_stylesheet" -o "$html_outputdir" "$html_inputfile"
  
  echo "Finished!"
  
elif [ "$1" = "pdf" ]; then #PDF file...
  create_dir $pdf_outputdir
  
  echo "Patch inputfile..."
  patch_inputfile $pdf_inputfile
  
  echo "Create PDF file..."
  docbook2pdf -d "$pdf_stylesheet" -o "$pdf_outputdir" "$pdf_inputfile"
  
  echo "Finished!"
  
elif [ "$1" = "txt" ]; then #TXT file...
  create_dir $txt_outputdir
  
  echo "Patch inputfile..."
  patch_inputfile $txt_inputfile
  
  echo "Create TXT file..."
  docbook2txt -d "$txt_stylesheet" -o "$txt_outputdir" "$txt_inputfile"

  echo "Finished!"
  
else
  echo "Syntax: build [html|pdf|txt]"
fi