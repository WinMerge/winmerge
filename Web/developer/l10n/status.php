<?php
  include('../../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Status of translations');
?>
<h2>Status of translations</h2>

<p>Our goal is to have stable releases fully localized. But as development is
pretty fast, there is no way to keep translations up-to-date in beta
releases.</p>

<p>The table below shows the releases where a particular language was updated.</p>

<h3>Maintained or unmaintained?</h3>
<p>Translations are divided to four categories:</p>
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
    <th>Language</th>
    <th>Status</th>
    <th>Author</th>
    <th colspan="3">Versions</th>
  </tr>
  <tr>
    <td>Brazilian Portuguese</td>
    <td>Unknown</td>
    <td><a href="mailto:wender at pta dot com dot br">Wender Firmino</a></td>
    <td>&nbsp;</td>
    <td>2.0</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>Bulgarian</td>
    <td>Maintained</td>
    <td><a href="mailto:sld at mail dot bg">sld</a></td>
    <td>2.1</td>
    <td>&nbsp;</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>Catalan</td>
    <td>Maintained</td>
    <td><a href="mailto:jordi_vilar at hotmail dot com">Jordi Vilar</a></td>
    <td>2.1</td>
    <td>2.0</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>Simplified Chinese</td>
    <td>Unknown</td>
    <td><a href="mailto:liaobin at jite dot net">Liaobin</a></td>
    <td>&nbsp;</td>
    <td>2.0</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>Traditional Chinese</td>
    <td>Unknown</td>
    <td><a href="mailto:kokokoko at giga dot net dot tw">Koko</a></td>
    <td>&nbsp;</td>
    <td>2.0</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>Czech</td>
    <td>Unmaintained</td>
    <td><a href="mailto:jt at taxoft dot cz">Jiri Tax</a></td>
    <td>&nbsp;</td>
    <td>&nbsp;</td>
    <td>1.7</td>
  </tr>
  <tr>
    <td>Danish</td>
    <td>Maintained</td>
    <td><a href="mailto:list1974 at hotmail dot com">Christian List</a></td>
    <td>&nbsp;</td>
    <td>2.0</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>Dutch</td>
    <td>Unmaintained</td>
    <td><a href="mailto:ronald at schaap dot demon dot nl">Ronald Schaap</a></td>
    <td>&nbsp;</td>
    <td>&nbsp;</td>
    <td>1.7</td>
  </tr>
  <tr>
    <td>Esperanto</td>
    <td>Unknown</td>
    <td>Tim Morley</td>
    <td>&nbsp;</td>
    <td>&nbsp;</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>French</td>
    <td>Actively maintained</td>
    <td><a href="mailto:gil_andre at compuserve dot com">Gil Andre</a> <br> <a href="mailto:ccganier at club-internet dot fr">Laurent Ganier</a></td>
    <td>&nbsp; <br> 2.1</td>
    <td>&nbsp; <br> 2.0</td>
    <td>1.7 <br> &nbsp;</td>
  </tr>
  <tr>
    <td>German</td>
    <td>Actively maintained</td>
    <td><a href="mailto:winfried dot peter at omicron dot at">Winfried Peter</a> <br> <a href="mailto:tonda at users dot sourceforge dot net">Joerg Schneider</a> <br> <a href="mailto:tim at gerundt dot de">Tim Gerundt</a></td>
    <td>&nbsp; <br> &nbsp; <br> 2.1</td>
    <td>&nbsp; <br> 2.0 <br> &nbsp;</td>
    <td>1.7 <br> &nbsp; <br> &nbsp;</td>
  </tr>
  <tr>
    <td>Italian</td>
    <td>Unmaintained</td>
    <td><a href="mailto:michele dot merega at ifminfomaster dot com">Michele Merega</a></td>
    <td>&nbsp;</td>
    <td>&nbsp;</td>
    <td>1.7</td>
  </tr>
  <tr>
    <td>Korean</td>
    <td>Unknown</td>
    <td><a href="mailto:sukjoon dot hwang at samsung dot com">Sukjoon</a></td>
    <td>&nbsp;</td>
    <td>2.0</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>Norwegian</td>
    <td>Unknown</td>
    <td><a href="mailto:hansfn at gexcon dot com">Hans Fredrik Nordhaug</a></td>
    <td>&nbsp;</td>
    <td>2.0</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>Polish</td>
    <td>Unknown</td>
    <td><a href="mailto:traduc at kki dot pl">Pawel Wawrzyszko</a></td>
    <td>&nbsp;</td>
    <td>2.0</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>Russian</td>
    <td>Actively maintained</td>
    <td><a href="mailto:wwarlock at mail dot ru">Dmitriy S. Aleshkowskiy</a></td>
    <td>2.1</td>
    <td>&nbsp;</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>Slovak</td>
    <td>Unknown</td>
    <td><a href="mailto:vancojur at decef dot elf dot stuba dot sk">majvan</a></td>
    <td>&nbsp;</td>
    <td>2.0</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>Spanish</td>
    <td>Unmaintained</td>
    <td><a href="mailto:grimmdp at yahoo dot com">Dean Grimm</a></td>
    <td>&nbsp;</td>
    <td>&nbsp;</td>
    <td>1.7</td>
  </tr>
  <tr>
    <td>Swedish</td>
    <td>Actively maintained</td>
    <td><a href="mailto:goran dot x dot hakansson at telia dot com">Göran Håkansson</a> <br> <a href="mailto:hans dot ericson at bredband dot net">Hans Eriksson</a></td>
    <td>&nbsp; <br> 2.1</td>
    <td>2.0 <br> &nbsp;</td>
    <td>&nbsp; <br> &nbsp;</td>
  </tr>

</table>
</div>

<p>Release 2.0 language versions also works with release 2.0.2.<br>
Release 1.4 language versions also works with release 1.7.</p>

<p>If you would like to update any of these translations or add another translation, then please follow <a href="instructions.php">these instructions</a>.</p>
<?php
  $page->printFoot();
?>