<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge translations');
?>

<h2>WinMerge translations</h2>

<p>We currently have WinMerge translated into the languages listed below.
To change languages, all you have to do is unzip the DLL in the same directory as WinMerge.exe, then select View / Language and choose your language.</p>

<h2>I want to translate into my own language</h2>
<p>If you are interested in translating WinMerge to your own native language, we'd be happy to include your language as well.</p>

<p>All the resources are moved into a DLL, so it should be fairly straightforward to support most languages.</p>

<h3>Instructions for those who know CVS and Visual C++</h3>
<p>Grab a copy of the source from CVS (<a href="http://winmerge.org/developer/wincvs.php" target="_top">WinCVS Hints</a>).</p>
<p>You will need to translate all strings in Merge.rc to your language. Make a MergeYourLanguage.rc and upload it to SF.net as a patch <a href="http://sourceforge.net/tracker/?group_id=13216&atid=313216" target="_top">here</a>.
<p>Then one of the developers will commit it to CVS and it will be released with the next release.</p>

<h3>Instructions for everybody else</h3>

<p>You must translate all strings inside a file called Merge.rc into your language.</p>
<p>The easiest way to get a copy of Merge.rc is to go to <a href="http://sourceforge.net/project/showfiles.php?group_id=13216" target="_top">our download site</a> and download the latest .zip with sources. If you can't find it, then please <a href="mailto:winmerge-support@lists.sourceforge.net">ask us</a> and we will send the file to you.</p>

<p>If you have Visual Studio, then we suggest you use that to do the translation. In case you do not have Visual Studio, then here is some instructions for you.</p>

<p>You only have to translate the strings between the quotes in Merge.rc. Here is an example. The english version looks like this:</P>

<pre>
    CONTROL         "Insert &Spaces",IDC_PROP_INSERT_SPACES,"Button",
                    BS_AUTORADIOBUTTON,77,72,59,10
</pre>
And here is the translation into danish:
<pre>
    CONTROL         "Indsæt &mellemrum",IDC_PROP_INSERT_SPACES,"Button",
                    BS_AUTORADIOBUTTON,77,72,59,10
</pre>

Here is a couple of rules to follow:
<ul>
<li>Do not translate The first 75 lines
<li>Do Not translate anything which is all uppercase letters
<li>The & sign is for accelerator buttons, try to make them meaningfull in your language.
<li>Don't translate the following strings, as they have special meaning in Visual C++: "Button", "MS Sans Serif" and "MS Shell Dlg"
</ul>

<p>Please find this line in Merge.rc</p>
<pre>
            VALUE "FileDescription", "WinMerge Application\0"
</pre>
<p>Just before that line, we recommend you put a line like this with your own name in it.</p>
<pre>
            VALUE "Comments", "Translated by Christian List\0"
</pre>
<p>This is your way to make sure you get credited for the translation, and we will of course also add your name in the table below.</p>

<p>Once you have translated all the strings, then you need to upload you translation to our patch section <a href="http://sourceforge.net/tracker/?group_id=13216&atid=313216" target="_top">here</a> (please leave some contact information in case there are any problems that needs to be resolved). In case you have problems with uploading it, you can also send it to us by e-mail 
<a href="mailto:winmerge-support@lists.sourceforge.net">here</a>.</p>

<p>Then one of the developers will compile and test your translation, if there are problems, we will contact you to get them resolved. When everything is resolved your translation will be included in the next release of WinMerge.</p>

<p>When the next major release is comming close we will notify you again, to give you a chance to update the translation with any new/changed strings</p>

<h2>Status of translations</h2>

<p>Our goal is to have stable releases fully localized. But as development is
pretty fast, there is no way to keep translations up-to-date in beta
releases.</p>

<p>The table below shows the releases where a particular language was updated.</p>

<h3>Maintained or unmaintained?</h3>
<p>Translations are divided to four categories:
<ul>
  <li class="toc"><b>Actively maintained</b> - translator(s) send frequent
  updates</li>
  <li class="toc"><b>Maintained</b> - translator(s) (we hope) send updates
  when we ask</li>
  <li class="toc"><b>Unmaintained</b> - we haven't heard anything about
  translator(s)</li>
  <li class="toc"><b>Unknown</b> - we get update to latest stable but don't
  know if translator is still interested</li>
</ul>

<div align="center">
<table border="1" cellpadding="3" cellspacing="0">
  <tr>
    <th>Status</th>
    <th>Language</th>
    <th>Author</th>
    <th colspan=3>Versions</th>
  </tr>
  <tr>
    <td>Unknown</td>
    <td>Brazilian Portuguese</td>
    <td>Wender Firmino</td>
    <td>&nbsp;</td>
    <td>2.0</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>Maintained</td>
    <td>Bulgarian</td>
    <td>sld</td>
    <td>2.1</td>
    <td>&nbsp;</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>Maintained</td>
    <td>Catalan</td>
    <td>Jordi Vilar</td>
    <td>2.1</td>
    <td>2.0</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>Unknown</td>
    <td>Simplified Chinese</td>
    <td>Liaobin</td>
    <td>&nbsp;</td>
    <td>2.0</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>Unknown</td>
    <td>Traditional Chinese</td>
    <td>Koko</td>
    <td>&nbsp;</td>
    <td>2.0</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>Unmaintained</td>
    <td>Czech</td>
    <td>Jiri Tax</td>
    <td>&nbsp;</td>
    <td>&nbsp;</td>
    <td>1.7</td>
  </tr>
  <tr>
    <td>Maintained</td>
    <td>Danish</td>
    <td>Christian List</td>
    <td>&nbsp;</td>
    <td>2.0</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>Unmaintained</td>
    <td>Dutch</td>
    <td>Ronald Schaap</td>
    <td>&nbsp;</td>
    <td>&nbsp;</td>
    <td>1.7</td>
  </tr>
  <tr>
    <td>Actively maintained</td>
    <td>French</td>
    <td>Gil Andre <br> Laurent Ganier</td>
    <td>&nbsp; <br> 2.1</td>
    <td>&nbsp; <br> 2.0</td>
    <td>1.7 <br> &nbsp;</td>
  </tr>
  <tr>
    <td>Actively maintained</td>
    <td>German</td>
    <td>Winfried Peter <br> Joerg Schneider and Tim Gerundt</td>
    <td>&nbsp; <br> 2.1</td>
    <td>&nbsp; <br> 2.0</td>
    <td>1.7 <br> &nbsp;</td>
  </tr>
  <tr>
    <td>Unmaintained</td>
    <td>Italian</td>
    <td>Michele Merega</td>
    <td>&nbsp;</td>
    <td>&nbsp;</td>
    <td>1.7</td>
  </tr>
  <tr>
    <td>Unknown</td>
    <td>Korean</td>
    <td>Sukjoon</td>
    <td>&nbsp;</td>
    <td>2.0</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>Unknown</td>
    <td>Norwegian</td>
    <td>Hans Fredrik Nordhaug</td>
    <td>&nbsp;</td>
    <td>2.0</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>Unknown</td>
    <td>Polish</td>
    <td>Pawe Wawrzyszko</td>
    <td>&nbsp;</td>
    <td>2.0</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>Actively maintained</td>
    <td>Russian</td>
    <td>Dmitriy S. Aleshkowskiy</td>
    <td>2.1</td>
    <td>&nbsp;</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>Unknown</td>
    <td>Slovak</td>
    <td>majvan</td>
    <td>&nbsp;</td>
    <td>2.0</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>Unmaintained</td>
    <td>Spanish</td>
    <td>Dean Grimm</td>
    <td>&nbsp;</td>
    <td>&nbsp;</td>
    <td>1.7</td>
  </tr>
  <tr>
    <td>Unmaintained</td>
    <td>Swedish</td>
    <td>Göran Håkansson</td>
    <td>&nbsp;</td>
    <td>2.0</td>
    <td>&nbsp;</td>
  </tr>

</table>
</div>

<br>
Release 2.0 language versions also works with release 2.0.2.<br>
Release 1.4 language versions also works with release 1.7.

<p>If you would like to update any of these translations, then please follow the instructions above.</p>

<?php
  $page->printFoot();
?>