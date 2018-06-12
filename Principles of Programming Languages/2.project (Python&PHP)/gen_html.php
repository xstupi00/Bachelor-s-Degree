<?php

function start_html() {
echo "
<!DOCCTYPE html>
<html>
<head>
<title>
Results of testing
</title>
<style>
table.greyGridTable {
  border: 2px solid #ffffff;
  width: 100%;
  text-align: center;
  border-collapse: collapse;
  font-family: courier new;
}
table.greyGridTable tbody td {
  font-size: 13px;
  border-bottom: 2px solid #333333;
  border-top: 2px solid #333333;
}
table.greyGridTable td, table.greyGridTable th {
  border: 1px solid #FFFFFF;
  padding: 3px 4px;
}
table.greyGridTable thead th {
  font-size: 15px;
  font-weight: bold;
  font-family: courier new;
  color: #333333;
  border-left: 2px solid #ffffff;
  border-bottom: 4px solid #ffffff;
  background: #ffcc00;
}

</style>
<h1 style=\"font-family:courier new;\">Results of testing</h1>
</head>\n";
}

function start_table() {

  echo "<table class=\"greyGridTable\">
<thead>
<tr>
<th style=\"width: 30%\">Test Case</th>
<th style=\"width: 10%\">parse.php</th>
<th style=\"width: 15%\">interpret.py</th>
<th style=\"width: 45%\">Note</th>
</tr>
</thead>
<tbody>\n";
}

function start_stats() {

echo "<table class=\"greyGridTable\">
<thead>
<tr>
<th style=\"width: 30%\">Test Directory</th>
<th style=\"width: 10%\">parse.php</th>
<th style=\"width: 15%\">interpret.py</th>
<th style=\"width: 45%\">Summary</th>
</tr>
</thead>
<tbody>
<br>
<p style=\"font-size: 20px; margin-bottom: 2px;\"><b>Overall Summary</b></p>\n";
}


function create_tag($name, $string) {

  echo "<".$name.">".$string."</".$name.">\n";
}

function special_tag($name, $type, $kind, $string) {

  echo "<$name style=\"".$type.": ".$kind."\">".$string."</".$name.">\n";
}

function start_tag($name) {
  
  echo "<".$name.">\n";
}

function end_tag($name) {


  echo "</".$name.">\n";
}

function create_row($parse_color, $int_color, $file, $parse_status, $int_status, $note) {

  start_tag('tr');
  create_tag('td', $file);
  special_tag('td', 'background-color', $parse_color, $parse_status);
  special_tag('td', 'background-color', $int_color, $int_status);
  create_tag('td', $note);
  end_tag('tr');
}


