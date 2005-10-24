<?php
  include('../../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Translations instructions', WINMERGE_DEV);
?>
<h2>Translations instructions</h2>
<ul>
  <li><a href="#translate">I want to translate into my own language</a></li>
  <li><a href="#developer">Instructions for those who know CVS and Visual C++</a></li>
  <li><a href="#everybody">Instructions for everybody else</a></li>
  <li><a href="#shellextension">ShellExtension</a></li>
  <li><a href="#installer">Translating Installer</a></li>
  <li><a href="#readme">Translating ReadMe.txt</a></li>
</ul>

<h3><a name="translate">I want to translate into my own language</a></h3>
<p>If you are interested in translating WinMerge to your own language, we'd be happy to include your language as well.</p>

<p>All the resources are located in a DLL, so it should be fairly straightforward to support most languages.</p>

<p>We have a list with the <a href="status.php">status of current translations</a>, so you can check if your language is already available and/or it needs to be updated.</p>

<h3><a name="developer">Instructions for those who know CVS and Visual C++</a></h3>
<p>Grab a copy of the source from CVS (<a href="../wincvs.php">WinCVS Hints</a>).</p>
<p>You will need to translate all the strings in <code>Merge.rc</code> to your language. Make a <code>MergeYourLanguage.rc</code> and upload it to SF.net as a patch <a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=313216">here</a>.
<p>Then one of the developers will commit it to CVS and it will be released with the next release.</p>
<p class="note">Please try to avoid changing dialog layout when translating.
The layout should be shared by all translations as much as possible, to make merging (and future translation changes) easier.
You can of course make controls wider to fit longer texts, but you should not move buttons and other controls if not really necessary.<br /><br />
If you want to improve dialog layout (we welcome UI improvements) you should submit it as src/Merge.rc patch so it will be merged to all translations.</p>

<h3><a name="everybody">Instructions for everybody else</a></h3>

<p>You must translate all the strings inside a file called <code>Merge.rc</code> into your language.</p>
<p>The easiest way to get a copy of <code>Merge.rc</code> is to go to <a href="http://sourceforge.net/project/showfiles.php?group_id=13216">our download site</a> and download the latest .zip with sources. If you can't find it, then please <a href="mailto:winmerge-support@lists.sourceforge.net">ask us</a> and we will send the file to you.</p>

<p>If you have Visual Studio, then we suggest you use that to do the translation. If you do not have Visual Studio, then here are some instructions for you.</p>

<p>You only have to translate the strings between the quotes in <code>Merge.rc</code>. Here is an example. The English version looks like this:</p>

<pre class="code">
    CONTROL         "Insert &amp;Spaces",IDC_PROP_INSERT_SPACES,"Button",
                    BS_AUTORADIOBUTTON,77,72,59,10
</pre>
<p>And here is the translation into Danish:</p>
<pre class="code">
    CONTROL         "Indsæt &amp;mellemrum",IDC_PROP_INSERT_SPACES,"Button",
                    BS_AUTORADIOBUTTON,77,72,59,10
</pre>

<p>Here are a couple of rules to follow:</p>
<ul>
  <li>Do not translate the first 75 lines.</li>
  <li>Do not translate anything that is all uppercase letters.</li>
  <li>The <code>&amp;</code> sign is for accelerator buttons, try to make them meaningful in your language.</li>
  <li>Don't translate the following strings, as they have special meaning in Visual C++: <code>"Button"</code>, <code>"MS Sans Serif"</code> and <code>"MS Shell Dlg"</code>.</li>
</ul>

<p>Please find this line in <code>Merge.rc</code></p>
<pre class="code">
            VALUE "FileDescription", "WinMerge Application\0"
</pre>
<p>Just before that line, we recommend you put a line like this with your own name in it.</p>
<pre class="code">
            VALUE "Comments", "Translated by Christian List\0"
</pre>
<p>This ensures that you get credit for the translation. We will of course also add your name to the <a href="status.php">status table</a>.</p>

<p>Once you have translated all the strings, then you need to upload your translation to our patch section <a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=313216">here</a> (please leave your contact information in case there are any problems that need to be resolved). In case you have problems uploading the patch, you can also send it to us by e-mail 
<a href="mailto:winmerge-support@lists.sourceforge.net">here</a>.</p>

<p>Then one of the developers will compile and test your translation; if there are problems, we will contact you to get them resolved. When everything is working your translation will be included in the next release of WinMerge.</p>

<p>When the next major release is emminent, we will notify you again to give you a chance to update the translation with any new/changed strings.</p>

<h3><a name="shellextension">ShellExtension</a></h3>
<p>ShellExtension can be translated by translating <code>.rc</code> resource file similarly to WinMerge executable.
ShellExtension resource files are in <code><a href="http://cvs.sourceforge.net/viewcvs.py/winmerge/WinMerge/ShellExtension/Languages/">WinMerge\ShellExtension\Languages\</a></code> folder.
There are only few strings to translate so it should be fast to do.</p>

<h3><a name="installer">Translating Installer</a></h3>
<p>Installer translation files are in <code><a href="http://cvs.sourceforge.net/viewcvs.py/winmerge/WinMerge/InnoSetup/Languages/">WinMerge\InnoSetup\Languages\</a></code> folder.
Look for a file named by your language. If one does not yet exist, you should look for InnoSetup translations in <a href="http://www.jrsoftware.org/files/istrans/">http://www.jrsoftware.org/files/istrans/</a>.</p>

<p>After you open the file, scroll to end of it, there are few custom strings you
need to translate. Look for a header <code><strong>[CustomMessages]</strong></code>
and strings after it.</p>

<p>If you want to change other strings in file, please consider submitting those
changes upstream to InnoSetup translators so all InnoSetup users may benefit
from your improvements. We'll pick those updates then also.</p>

<h3><a name="readme">Translating ReadMe.txt</a></h3>
<p>Please consider translating also <code>ReadMe.txt</code> file - it is shown in end of installer
and is added to Start-menu. Translated versions of <code>ReadMe.txt</code> are
in <code><a href="http://cvs.sourceforge.net/viewcvs.py/winmerge/WinMerge/Docs/Users/Languages/">WinMerge\Docs\Users\Languages\</a></code> folder. And master <code>ReadMe.txt</code> is
in <code><a href="http://cvs.sourceforge.net/viewcvs.py/winmerge/WinMerge/Docs/Users/ReadMe.txt">WinMerge\Docs\Users\ReadMe.txt</a></code>.</p>

<?php
  $page->printFoot();
?>