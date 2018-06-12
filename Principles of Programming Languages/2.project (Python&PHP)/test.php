<?php

/**************************************************************
 * File:        test.php
 * Author:      Šimon Stupinský
 * University:  Brno University of Technology
 * Faculty:     Faculty of Information Technology
 * Course:      Principles of Programming Languages
 * Date:        4.02.2018
 * Last change: 14.03.2018
 *
 * Subscribe:   Script for running test of parse.php and interpet.py
 *
 **************************************************************/

/**
 * @file  test.php
 * @brief Script for running test of parse.php and interpet.py
 */


include 'gen_html.php';

mb_internal_encoding("UTF-8");

$directory = $parse = $interpret = false;
$parse_ok = $parse_err = $int_ok = $int_err = $without_run = $row_type = 0;
$RECURSIVE = 'OFF';
$APPROX_ZERO = 0.000001;

// Shorthanded options 
$shortopts = '';
$shortopts .= 'h';
$shortopts .= 'd::';
$shortopts .= 'r';
$shortopts .= 'p::';
$shortopts .= 'i::';

// Long (normal) options 
$longopts = array (
  'help',
  'directory::',
  'recursive',
  'parse-script::',
  'int-script::',
);

function transform_opts($shortopts) {

  if (preg_match('/^-(h|d|r|p|i)$/', $shortopts)) {
    $shortopts = preg_replace('/^-h$/u', '--help', $shortopts);
    $shortopts = preg_replace('/^-d$/u', '--directory', $shortopts);
    $shortopts = preg_replace('/^-r$/u', '--recursive', $shortopts);
    $shortopts = preg_replace('/^-p$/u', '--parse-script', $shortopts);
    $shortopts = preg_replace('/^-i$/u', '--int-script', $shortopts);
  }
  return $shortopts;
}

function print_err($err_code) {

  switch ($err_code) {
    case 0:
      fwrite(STDOUT, "\nScript for automatic testing gradually aplication parse.php and interpret.py (test.php)\n");
      fwrite(STDOUT, "Usage php5.6 test.php [OPTIONS]\n");
      fwrite(STDOUT, "Options:\n");
      fwrite(STDOUT, "--help 		       Show this message and exit.\n");
      fwrite(STDOUT, "--directory=path       Tests will be searched in the specified directory (if this parameter is missing, then the script browse the current directory.\n");
      fwrite(STDOUT, "--recursive            Tests will be searched not only in the specified directory but also recursively in all its subdirectories.\n");
      fwrite(STDOUT, "--parse-script=file    File with script in the PHP5.6 to analysis of source code in the IPPcode18 (if this parameter is missing, then implicite value is parse.php saved in the actual directory.\n");
      fwrite(STDOUT, "--int-script=file      File with script in the Python3.6 for interpret XML representation of code IPPcode18 (if this parameter is missing, then implicite value is interpret.py saved in the actual directories.\n");
      break;
    case 10: 
      fwrite(STDOUT, "Missing script parameter or usage incorrect combination of paramaters.\n");
      break;
    case 11:
      fwrite(STDOUT, "Error opening input file (eg. non-existence, insufficient authorization).\n");
      break;
    case 12:
      fwrite(STDERR, "Error opening output file for writing.\n");
      break; 
  }
  exit($err_code);
}

$options = getopt($shortopts, $longopts);

if (array_key_exists('d', $options))
  $options['directory'] = $options['d'];
if (array_key_exists('p', $options))
  $options['parse-script'] = $options['p'];
if (array_key_exists('i', $options))
  $options['int-script'] = $options['i'];

unset($argv[0]);

$used_params = array (
  '--help' => false,
  '--directory' => false,
  '--recursive' => false,
  '--parse-script' => false,
  '--int-script' => false,
);

foreach ($argv as $arg) {

  $arg_opts = preg_split('/=/', $arg);
  foreach ($arg_opts as $item)
    $item = trim($item);
  $longopts = transform_opts($arg_opts[0]);

  if ($used_params[$longopts] || !array_key_exists($longopts, $used_params) 
   || (preg_match('/^--(directory|recursive|parse-script|int-script)$/', $longopts) && $used_params['--help']) 
   || (preg_match('/^--help$/', $longopts) && in_array(true, $used_params)) ) 
      print_err(10);

  if (preg_match('/^--(directory|parse-script|int-script)$/', $longopts)) {
    if (!preg_match('/\S/', $arg_opts[1]))
      print_err(10);
  }
  else if (preg_match('/\S/', $arg_opts[2]))
    print_err(10);

  
  $used_params[$longopts] = true;   
}

function insert_row($action, $test_name, $var1, $var2) {

  global $parse, $interpret;

  switch($action) {
    case 1:
      create_row('#32CD32', '#ADD8E6', $test_name, 'OK', 'WITHOUT RUNNING', '<b>'.$parse.'</b> exit with code <b>'.$var1.'</b>');
      break;
    case 2:
      create_row('#DC143C', '#ADD8E6', $test_name, 'FAILURE', 'WITHOUT RUNNING', '<b>'.$parse.'</b> exit with code <b> '.$var1.'</b>, expected code <b>'.$var2.'</b>');
      break;
    case 3:
      create_row('#DC143C', '#ADD8E6', $test_name, 'FAILURE', 'WITHOUT RUNNING', '<b>'.$parse.'</b> exit with code <b>'.$var1.'</b>, expected code <b>'.$var2.'</b>');
      break;
    case 4:
      create_row('#32CD32', '#32CD32', $test_name, 'OK', 'OK', '<b>'.$interpret.'</b> exit with code <b>'.$var1.'</b>, output is identical with <i>'.$var2.'</i>');
      break;
    case 5:
      create_row('#32CD32', '#DC143C', $test_name, 'OK', 'FAILURE', '<b>'.$interpret.'</b> exit with code <b>'.$var1.'</b> but output is different from <i>'.$var2.'</i>');
      break;
    case 6:
      create_row('#32CD32', '#DC143C', $test_name, 'OK', 'FAILURE', '<b>'.$interpret.'</b> exit with code <b>'.$var1.'</b>, expected code is <b>'.$var2.'</b>');
      break;
    case 7:
      create_row('#32CD32', '#32CD32', $test_name, 'OK', 'OK', '<b>'.$interpret.'</b> exit with code <b>'.$var1.'</b>');
      break;
  }

}

$opt_keys = array_keys($options);

foreach ($opt_keys as $key) {

  $key = '-'.$key;
  $key = transform_opts($key);
  $key = preg_replace('/^(--|-)(.*)$/u', '\2', $key);

  switch (trim($key)) {
    case 'help':
      print_err(0);
      break;
    case 'directory':
      $directory = $options[$key];
      break;      
    case 'parse-script':
      $parse = $options[$key];
      break;
    case 'int-script':
      $interpret = $options[$key];
      break;
  }
}

if (!$used_params['--directory'])
  $directory = getcwd();
if (!$used_params['--parse-script'])
  $parse = 'parse.php';
if (!$used_params['--int-script'])
  $interpret = 'interpret.py';
if ($used_params['--recursive'])
  $RECURSIVE = 'ON';

//if (!is_dir($directory))
//  print_err(11);
if (!file_exists($parse) || !preg_match('/.php$/', $parse) )
  print_err(11);
if (!file_exists($interpret) || !preg_match('/.py$/', $interpret) )
  print_err(11);

if ($used_params['--recursive']) {
  $src_files = shell_exec('find '.escapeshellarg($directory).' -name "*.src"');
  $in_files = shell_exec('find '.escapeshellarg($directory).' -name "*.in"');
  $out_files = shell_exec('find '.escapeshellarg($directory).' -name "*.out"');
  $rc_files = shell_exec('find '.escapeshellarg($directory).' -name "*.rc"');
}
else {
  $src_files = shell_exec('find '.escapeshellarg($directory).' -maxdepth 1 -name "*.src"');
  $in_files = shell_exec('find '.escapeshellarg($directory).' -maxdepth 1 -name "*.in"');
  $out_files = shell_exec('find '.escapeshellarg($directory).' -maxdepth 1 -name "*.out"');
  $rc_files = shell_exec('find '.escapeshellarg($directory).' -maxdepth 1 -name "*.rc"');
}

$src_files = preg_split('/\s/', $src_files);
$in_files = preg_split('/\s/', $in_files);
$out_files = preg_split('/\s/', $out_files);
$rc_files = preg_split('/\s/', $rc_files);

array_pop($src_files);
array_pop($in_files);
array_pop($out_files);
array_pop($rc_files);

start_html();
special_tag('p', 'font-family', 'courier new; font-size: 13pt;', '<b> pwd: </b>'.getcwd().'<br> <b> Tested Directory: </b>'.$directory.'<br> <b> Parse: </b>'.$parse.'<br> <b> Interpret: </b>'.$interpret.'<br> <b> RECURSIVE : </b>'.$RECURSIVE);

$dir_arr = array();

foreach ($src_files as $src_file) {

  $test_name_ = basename($src_file, ".src");

  $search_in = preg_replace('/.src$/', '.in', $src_file);
  if (!in_array($search_in, $in_files))
    touch($search_in);

  $search_out = preg_replace('/.src$/', '.out', $src_file);
  if (!in_array($search_out, $out_files))
    touch($search_out);

  $search_rc = preg_replace('/.src$/', '.rc', $src_file);
  if (!in_array($search_rc, $rc_files)) {
    $handle = fopen($search_rc, 'w');
    fwrite($handle, "0");
    $expected_code = 0;
    fclose($handle);
  }
  else {
    $handle = fopen($search_rc, 'r');
    $expected_code = fgets($handle);
    fclose($handle);
  }

  $tmp_parse_out = tmpfile();
  $tmp_parse_name = stream_get_meta_data($tmp_parse_out)['uri'];
  $tmp_int_out = tmpfile();
  $tmp_int_name = stream_get_meta_data($tmp_int_out)['uri'];

  $exit_code = shell_exec('php5.6 '.escapeshellarg($parse). ' <'.escapeshellarg($src_file).' >'.escapeshellarg($tmp_parse_name).' ; echo $?');
  $exit_code = intval($exit_code);
  $actual_dir = shell_exec('echo '.escapeshellarg($src_file).' | rev | cut -d"/" -f2- | rev');
  $actual_dir = preg_replace('/\s/', '', $actual_dir);

  $test_name = explode('/', $src_file);
  if ($exit_code) {
    if ($exit_code == $expected_code) {
      $row_type = 1; $var1 = $exit_code; $var2 = 0;
      $parse_ok++; $without_run++;
    }
    else { 
      $row_type = 2; $var1 = $exit_code; $var2 = $expected_code;
      $parse_err++;
    }
  }
  else if ($expected_code == 21) {
      $row_type = 3; $var1 = $exit_code; $var2 = $expected_code;
      $parse_err++;
  }
  else {
    $exit_code = shell_exec('python3.6 '.escapeshellarg($interpret).' --source='.escapeshellarg($tmp_parse_name).' <'.escapeshellarg($search_in).' >'.escapeshellarg($tmp_int_name).' ; echo $?');
    $exit_code = intval($exit_code);
    if ($exit_code == $expected_code) {
      if (!$exit_code) {
        $match = shell_exec('diff '.escapeshellarg($tmp_int_name).' '.escapeshellarg($search_out).' ; echo $?');
	$match = intval($match);
        if (!$match) {
          $row_type = 4; $var1 = $exit_code; $var2 = $search_out;
          $parse_ok++; $int_ok++;
        }
        else {
          $row_type = 5; $var1 = $exit_code; $var2 = $search_out;
          $parse_ok++; $int_err++;
        }
       } else {
	  $row_type = 7; $var1 = $exit_code; $var2 = $search_out;
          $parse_ok++; $int_ok++;
       }    
    }
    else {
      $row_type = 6; $var1 = $exit_code; $var2 = $expected_code; 
      $parse_ok++; $int_err++;
    }
  }
  if (!array_key_exists($actual_dir, $dir_arr)) {
    $dir_arr[$actual_dir] = array();
  }
  array_push($dir_arr[$actual_dir], $test_name_, $parse_ok, $parse_err, $int_ok, $int_err, $without_run, $var1, $var2, $row_type);
  $parse_ok = $parse_err = $int_ok = $int_err = $without_run = 0;

  fclose($tmp_parse_out);
  fclose($tmp_int_out);
}


$counter = 1;
foreach ($dir_arr as $key => $value) {
   start_table();
   special_tag('p', 'font-family', 'courier new; margin-bottom: 1mm;', '<i>'.$key.'</i>');
   foreach ($value as $subvalue) {
     if ($counter == 1)
       $test_name = $subvalue;
     if ($counter == 7)
       $var1 = $subvalue;
     if ($counter == 8)
       $var2 = $subvalue;
     if ($counter == 9) {
       insert_row($subvalue, $test_name, $var1, $var2);
       $counter = 0;
     }
     $counter++;
   }
   end_tag('tbody');
   end_tag('table');
}

start_stats();

$counter = 0;
$succ_parse = $all_parse = $succ_int = $all_int = $out_run = 0;

foreach($dir_arr as $key => $stats_arr) {
  while($counter < count($stats_arr)) {
    $counter++;
    $succ_parse += $stats_arr[$counter];
    $err_parse += $stats_arr[$counter+1];
    $succ_int += $stats_arr[$counter+2];
    $err_int += $stats_arr[$counter+3];
    $out_run += $stats_arr[$counter+4];
    $counter += 8;
  }
  if ($succ_parse || $err_parse) {
      $per_parse = $succ_parse / ($succ_parse + $err_parse) * 100;
      $per_sum = ($succ_int + $out_run) / ($succ_parse + $err_parse) * 100;
    }
  else {
      $per_parse = 0;
      $per_sum = 0;
    }

    if ($succ_int && $err_int)
      $per_int = $succ_int / ($succ_int + $err_int) * 100;
    else
      $per_int = 0;

  $per_parse = number_format($per_parse, 2, '.', '');
  $per_int = number_format($per_int, 2, '.', '');
  $per_sum = number_format($per_sum, 2, '.', '');

  create_row('#FFFFFF', '#FFFFFF', $key, $succ_parse.'/'.intval($succ_parse + $err_parse).' ('.$per_parse.'%)', $succ_int.'/'.intval($succ_int+ $err_int).' ('.$per_int.'%)',($succ_int + $out_run).'/'.intval($succ_parse + $err_parse).' ('.$per_sum.'%)
');

  $all_succ_parse += $succ_parse;
  $all_succ_int += $succ_int;
  $all_out_run += $out_run;
  $all_parse += $succ_parse + $err_parse;
  $all_int += $succ_int + $err_int;
  $counter = $succ_parse = $succ_int = $out_run = $err_parse = $err_int = 0;
}


if ($all_parse) {
  $per_parse = $all_succ_parse / $all_parse * 100;
  $per_sum = ($all_succ_int + $all_out_run) / $all_parse * 100;
}
else {
  $per_parse = 0;
  $per_sum = 0;
}

if ($all_int)
  $per_int = $all_succ_int / $all_int * 100;
else
  $per_int = 0;

$per_parse = number_format($per_parse, 2, '.', '');
$per_int = number_format($per_int, 2, '.', '');
$per_sum = number_format($per_sum, 2, '.', '');


create_row('#FFFFFF', '#FFFFFF', '<b>'.$directory.'</b>', '<b>'.$all_succ_parse.'/'.intval($all_parse).' ('.$per_parse.'%)</b>', '<b>'.intval($all_succ_int).'/'.intval($all_int).' ('.$per_int.'%)</b>', '<b>'.intval($all_succ_int + $all_out_run).'/'.intval($all_parse).' ('.$per_sum.'%)</b>');

end_tag('tbody');
end_tag('table');
end_tag('html');
?>
