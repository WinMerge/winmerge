<?php
  include('../../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Translations instructions');
?>
<h2>I want to translate into my own language</h2>
<p>If you are interested in translating WinMerge to your own language, we'd be happy to include your language as well.</p>

<p>All the resources are located in a DLL, so it should be fairly straightforward to support most languages.</p>

<h3>Instructions for those who know CVS and Visual C++</h3>
<p>Grab a copy of the source from CVS (<a href="../wincvs.php">WinCVS Hints</a>).</p>
<p>You will need to translate all the strings in <code>Merge.rc</code> to your language. Make a <code>MergeYourLanguage.rc</code> and upload it to SF.net as a patch <a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=313216" target="_top">here</a>.
<p>Then one of the developers will commit it to CVS and it will be released with the next release.</p>

<h3>Instructions for everybody else</h3>

<p>You must translate all the strings inside a file called <code>Merge.rc</code> into your language.</p>
<p>The easiest way to get a copy of <code>Merge.rc</code> is to go to <a href="http://sourceforge.net/project/showfiles.php?group_id=13216" target="_top">our download site</a> and download the latest .zip with sources. If you can't find it, then please <a href="mailto:winmerge-support@lists.sourceforge.net">ask us</a> and we will send the file to you.</p>

<p>If you have Visual Studio, then we suggest you use that to do the translation. In case you do not have Visual Studio, then here is some instructions for you.</p>

<p>You only have to translate the strings between the quotes in <code>Merge.rc</code>. Here is an example. The English version looks like this:</p>

<pre>
    CONTROL         "Insert &amp;Spaces",IDC_PROP_INSERT_SPACES,"Button",
                    BS_AUTORADIOBUTTON,77,72,59,10
</pre>
<p>And here is the translation into Danish:</p>
<pre>
    CONTROL         "Indsæt &amp;mellemrum",IDC_PROP_INSERT_SPACES,"Button",
                    BS_AUTORADIOBUTTON,77,72,59,10
</pre>

<p>Here are a couple of rules to follow:</p>
<ul>
  <li>Do not translate the first 75 lines.</li>
  <li>Do not translate anything which is all uppercase letters.</li>
  <li>The <code>&amp;</code> sign is for accelerator buttons, try to make them meaningful in your language.</li>
  <li>Don't translate the following strings, as they have special meaning in Visual C++: <code>"Button"</code>, <code>"MS Sans Serif"</code> and <code>"MS Shell Dlg"</code>.</li>
</ul>

<p>Please find this line in <code>Merge.rc</code></p>
<pre>
            VALUE "FileDescription", "WinMerge Application\0"
</pre>
<p>Just before that line, we recommend you put a line like this with your own name in it.</p>
<pre>
            VALUE "Comments", "Translated by Christian List\0"
</pre>
<p>This is your way to make sure you get credited for the translation, and we will of course also add your name in the table below.</p>

<p>Once you have translated all the strings, then you need to upload you translation to our patch section <a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=313216" target="_top">here</a> (please leave some contact information in case there are any problems that needs to be resolved). In case you have problems with uploading it, you can also send it to us by e-mail 
<a href="mailto:winmerge-support@lists.sourceforge.net">here</a>.</p>

<p>Then one of the developers will compile and test your translation; if there are problems, we will contact you to get them resolved. When everything is resolved your translation will be included in the next release of WinMerge.</p>

<p>When the next major release is comming close we will notify you again, to give you a chance to update the translation with any new/changed strings.</p>
<?php
  $page->printFoot();
?>