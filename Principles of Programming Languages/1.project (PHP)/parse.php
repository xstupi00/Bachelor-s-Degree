<?php

/**************************************************************
 * File:        parse.php
 * Author:      Šimon Stupinský
 * University:  Brno University of Technology
 * Faculty:     Faculty of Information Technology
 * Course:      Principles of Programming Languages
 * Date:        24.02.2018
 * Last change: 14.03.2018
 *
 * Subscribe:   Script for control lexical and syntactic analysis of code IPPcode18
 *
 **************************************************************/

/**
 * @file  parse.php
 * @brief Script for control lexical and syntactic analysis of code IPPcode18
 */


// sets encoding of script
mb_internal_encoding("UTF-8");

// declaration of variables
$access = false;
$cnt_comments = $cnt_loc = 0;
$file_path;

// Shorthended options
$shortopts = '';     // running without parameters (only php4.6 parse.php)
$shortopts .= 'h';   // view of help (php 5.6 parse.php -h)
$shortopts .= 's::'; // parameter of extension STATP
$shortopts .= 'l';   // count of lines with instructions
$shortopts .= 'c';   // count of lines with comments

// Array of long (normal) options
$longopts = array(
  'help',	     // --help
  'stats::',         // --stats=file
  'loc',             // --stats=file --loc
  'comments',        // --stats=file --comments
);

/**
 * @brief   Transforming shorthanded options to its relevant long version.
 *          For unambiguous work with all type of options.
 * 
 * @param   shortopts   Options which will be transforming
 *
 * @return           	Returns tranformed options in long (normal) version
 */
function transform_opts($shortopts) {

  if (preg_match('/^-(h|s|l|c)$/', $shortopts) == true) {           // cotrol valid arguments for transform
    $shortopts = preg_replace('/^-h$/u', '--help', $shortopts);     // -h -> --help
    $shortopts = preg_replace('/^-s$/u', '--stats', $shortopts);    // -s -> --stats
    $shortopts = preg_replace('/^-l$/u', '--loc', $shortopts);      // -l -> --loc
    $shortopts = preg_replace('/^-c$/u', '--comments', $shortopts); // -c -> --comments
  }
  return $shortopts;	// return long variants of arguments
}


/**
 * @brief   Function to print the help on the STDOUT and print error message
 *	    on the STDERR when error has been occured.
 *	    After the writing program exit with relevant exit code.
 * 
 * @param   err_code	Code with which the program will be exit after the writing
 *			Used for recognition of type of the error message
 */
function print_err($err_code) {
  
  switch ($err_code) {
    case 0:
      fwrite(STDOUT, "\nAnalyzer of code in IPPcode18 (parse.php)\n");
      fwrite(STDOUT, "Usage: php5.6 parse.php [OPTIONS]\n");
      fwrite(STDOUT, "Options:\n");
      fwrite(STDOUT, "--help 		  Show this message and exit.\n");
      fwrite(STDOUT, "--stats=file      Output file for writing statistics during the analysis.\n");
      fwrite(STDOUT, "--loc             The number of rows with instructions.\n");
      fwrite(STDOUT, "--comments        The number of rows with comments.\n\n");
      break;
    case 10:
      fwrite(STDERR, "Missing script parameter or usage incorrect combination of paramaters.\n");
      break;
    case 12:
      fwrite(STDERR, "Error opening output file for writing.\n");
      break;
    case 21:
      fwrite(STDERR, "Lexical or syntactic error of source code writing in IPPcode18.\n");

      break;
  }
  exit($err_code);
}

// obtains options from command line
$options = getopt($shortopts, $longopts);

// resets the options of stats for obtaining its options later
if (array_key_exists('s', $options))
  $options['stats'] = $options['s'];

// skip first option (name of script)
unset($argv[0]);

/* 
 * array of permissible options;
 * in the case, that options will be used, then the 
   relevant item of array will be set to true value 
 */
$used_params = array (
  '--help' => false,
  '--stats' => false,
  '--loc' => false,
  '--comments' => false,
  'first_loc' => false,
);

/* 
 * Checking of entered options in the command line
 * Checks are performed for accetable count of entered options,
   accetable combinations of entered options, duplicity of options and
   accepting only knowed options.
 * In the case of options "--stats" is checking required options. ("--stats=file")
 */ 
foreach ($argv as $arg) {
  
  // splitting in the case of required options at the individual arguments
  $arg_opts = preg_split('/[=]/', $arg); 
  // removing white spaces from the beginning and ending of the all options
  foreach ($arg_opts as $item)
    $item = trim($item);
  // transforming options to unity form of all options, respectively to form of long options
  $longopts = transform_opts($arg_opts[0]);

  // setting up the correct order of statistics
  if (preg_match('/^--loc$/', $longopts) && !($used_params['--comments']) )
    $used_params['first_loc'] = true;

  // checking knowed options, duplitcity of options and accetable combinatios of options
  if ($used_params[$longopts] || !array_key_exists($longopts, $used_params) 
   || (preg_match('/^--(stats|loc|comments)$/', $longopts) && $used_params['--help'])
   || (preg_match('/^--help$/', $longopts) && in_array(true, $used_params)) 
     ) {
    print_err(10);
  }

  // checking required options in the case of argument "--stats"
  if (preg_match('/^--stats$/', $longopts)) {
    if (!preg_match("/\S/", $arg_opts[1])) 
      print_err(10);
  } // checking forbidden option in the case of other arguments
  else if (preg_match('/\S/', $arg_opts[1]))
    print_err(10);

  // sets item for relevant type of argument
  $used_params[$longopts] = true;  
}

// invalid combination of entered optiosn
if (($used_params['--loc'] || $used_params['--comments']) && !$used_params['--stats'])
  print_err(10); 	 

$opt_keys = array_keys($options);

// obtaining required entered options
foreach ($opt_keys as $key) {

  // executing of requested operations before the obtaining
  $key = "-".$key;
  $key = transform_opts($key);
  $key = preg_replace('/^(--|-)(.*)$/u', '\2', $key);
 
  switch (trim($key)) {
    case 'help':
      print_err(0);
    case 'stats':
      $file_path = $options[$key];
      break;
  }
}

/**
 * @brief   Function check occurences '\' in the given line and
 *          in the case, that after them not follows three digit
 *	    error is announced
 * 
 * @param   $line 	Line for control
 */
function check_char($line) {

  // position of the beginning in the given line (string@...)
  for ($i = 7; $i <= strlen($line)-1; $i++) {
    // finding the desired character
    if ($line[$i] == '\\')
      // if next three characters exist, all must be the numbers
      if ( $i+3 <= strlen($line)-1) {
        if (!ctype_digit($line[$i+1]) || !ctype_digit($line[$i+2]) || !ctype_digit($line[$i+3]))
          print_err(21);
        }
      else
        print_err(21);
  }
}

/**
 * @brief   Function checks permitted constructions of LABEL type
 *          Check is executed with regular expression
 * 
 * @param   $opts 	Options for control
 */
function match_label($opts) {

  if (!preg_match('/^([a-zA-Z]|_|-|\$|&|%|\*)([a-zA-Z0-9]|_|-|\$|&|%|\*)*$/', $opts))
    print_err(21);
}


/**
 * @brief   Function checks permitted constructions of VAR type
 *          Check is executed with regular expression
 * 
 * @param   $opts       Options for control
 */
function match_var($opts) {

  if (!preg_match('/^(LF|TF|GF)@([a-zA-Z]|_|-|\$|&|%|\*)([a-zA-Z0-9]|_|-|\$|&|%|\*)*$/', $opts))
    print_err(21);
}


/**
 * @brief   Function checks permitted constructions of SYMB type
 *          Check is executed with regular expression
 * 
 * @param   $opts       Options for control
 *
 * @return	Function returns relevant type of given option
 */
function match_symb($opts) {

  // type of constant -> INT
  if (preg_match('/^int@(-|\+)?[0-9]+$/', $opts))
    $type = 'int';   
  // type of constant -> BOOL
  else if (preg_match('/^bool@(false|true)$/', $opts))
    $type = 'bool';
  // type of constant -> STRING
  else if (preg_match('/^string@(([^\x00-\x1F\x7F\xA0])+|(\x5C[0-9]{3})+|($))$/', $opts)) {
    $type = 'string';
    check_char($opts); // check accetable occurences of char '\'
  }
  else { // is the not constant, check whether is the not variable
    match_var($opts, $end);
    $type = 'var';
  }
  return $type;
}

/**
 * @brief   Function creates head of XML OUTPUT for the all instruction.
 * 
 * @param   opts        Actually proccesed instructuon with its all items
 * @param   type        Array with type of instructions arguments
 * @param   count	Count of arguments of instruction
 */
function xml_create($opts, $type, $count) {
 
  global $xw, $cnt_loc;

  xmlwriter_start_element($xw, 'instruction');
  xmlwriter_start_attribute($xw, 'order');
  xmlwriter_text($xw, $cnt_loc);
  xmlwriter_start_attribute($xw, 'opcode');
  xmlwriter_text($xw, mb_strtoupper($opts[0]));
  xml_arg($opts, $type, $count);
  xmlwriter_end_element($xw);
}


/**
 * @brief   Function creates elemdnt of XML OUTPUT for the all instructions arguments.
 * 
 * @param   opts        Actually proccesed instruction with its all items
 * @param   type        Array with type of instructions arguments
 * @param   count       Count of arguments of instruction
 */
function xml_arg($opts, $type, $count) {

   global $xw;
 
   for ($i =  1; $i <= $count; $i++) {
     $arg = 'arg'.$i;
     xmlwriter_start_element($xw, $arg);
     xmlwriter_start_attribute($xw, 'type');
     xmlwriter_text($xw, $type[$i]);
     xmlwriter_end_attribute($xw);
     xmlwriter_text($xw, $opts[$i]);
     xmlwriter_end_element($xw);
   }
}

/**
 * @brief   Function creates head of XML OUTPUT for the all instruction.
 * 
 * @param   $type       Type of the proccesed costant
 * @param   $i		The index in the array, which contains actual proccesing line
 */
function revision_const($type, $i) {

  // actually proccesed line 
  global $parse_line;

  // match with the type of constant
  if (preg_match('/^(int|string|bool)/', $type)) {
    // dividing into two parts, before and after the @
    $tmp = explode('@', $parse_line[$i], 2);
    // obtaining the part after the @
    $parse_line[$i] = $tmp[1];
    // costant type is BOOL
    if (preg_match('/^bool/', $tmp[0]))
      // convert bool value to strlower
      $parse_line[$i] = strtolower($parse_line[$i]);
  }
}

/**
 * @brief   Function to writing the statistics about
 *          the count of lines with code and line with
 *          comments to the given file
 */
function write_stats() {

  global $file_path, $cnt_loc, $cnt_comments, $used_params;

  // opened the output file (check success)
  $output = fopen($file_path, 'w') or die(print_err(12));
  // was entered both statistics options
  if ($used_params['--loc'] == true and $used_params['--comments'] == true) {
    // checks order of statistics options
    if ($used_params['first_loc'] == true)
      fwrite($output, $cnt_loc."\n".$cnt_comments."\n");
    else
      fwrite($output, $cnt_comments."\n".$cnt_loc."\n");
  } // was entered only one statistics option
  else if ($used_params['--loc'] == true)
    fwrite($output, $cnt_loc."\n");
  else if ($used_params['--comments'] == true)
    fwrite($output, $cnt_comments."\n");

  // closed the output file
  fclose($output);
}

// obtaining XML handle and parameter setting
$xw = xmlwriter_open_memory();
xmlwriter_set_indent($xw, 1);
// set XMl Header
xmlwriter_start_document($xw, '1.0', 'UTF-8');

// set element with program language
xmlwriter_start_element($xw, 'program');
xmlwriter_start_attribute($xw, 'language');
xmlwriter_text($xw, 'IPPcode18');
xmlwriter_end_attribute($xw);

// open STDIN
$handle = fopen('php://stdin', 'r');
while (!feof($handle)) {
  $line = fgets($handle); // obtains line
  $line = trim($line);    // strip whitespace (or other characters) from the beginning and end of a string
  
  // waiting to head of code (.IPPcode18)
  if ($access == false) {
    if (preg_match('/^(#.*)/', $line) && !preg_match('/^(\.ippcode18)(\s*#.*|$)$/i', $line))
      $cnt_comments++;	 // comment or empty line before the head of code
    else if (preg_match('/^(\.ippcode18)(\s*#.*|$)$/i', $line)) { // line with head of code
      $part_of_line = explode("#", $line, 2);   // check comment on the line after the head
      if (preg_match("/\S/", $part_of_line[1]))
        $cnt_comments++;
      $access = true;
    }
    else if (preg_match('/^\s*$/', $line))
      continue;
    else // something else before the head of code
      print_err(21);
  }
  else if ($access == true) {
    $part_of_line = explode("#", $line, 2);   // remove comments part of line 
    if (preg_match('/\S/', $part_of_line[1])) // check presence of comment on actual line
      $cnt_comments++;
  
    if (preg_match('/\S/', $part_of_line[0])) {
      $parse_line = preg_split('/\s+/', trim($part_of_line[0])); // obtain all parts of instruction
      if (preg_match('/^(createframe|pushframe|return|break|popframe)$/i', $parse_line[0])) {
          if (count($parse_line) > 1)
            print_err(21);
          $cnt_loc++;
          xml_create($parse_line, NULL, 0);
      }
      // proccesing the group of instructions with arguments: <LABEL>
      else if (preg_match('/^(call|label|jump)$/i', $parse_line[0])) {
        if (count($parse_line) > 2)
          print_err(21);
        match_label($parse_line[1]);
        $type[1] = 'label';
        $cnt_loc++;
        xml_create($parse_line, $type, 1);
      }
      // proccesing the group of instructions with arguments: <VAR>
      else if (preg_match('/^(pops|defvar)$/i', $parse_line[0])) {
        if (count($parse_line) > 2)
          print_err(21);
        match_var($parse_line[1]);
        $type[1] = 'var';
        $cnt_loc++; 
        xml_create($parse_line, $type, 1);
      }
      // proccesing the group of instructions with arguments: <SYMB>
      else if (preg_match('/^(pushs|dprint|write)$/i', $parse_line[0])) {
        if (count($parse_line) > 2)
          print_err(21);
        $type[1] = match_symb($parse_line[1]);
        revision_const($type[1], 1);
        $cnt_loc++;
        xml_create($parse_line, $type, 1);
      }
      // proccesing the group of instructions with arguments: <TYPE>
      else if (preg_match('/^read$/i', $parse_line[0])) {
        if (count($parse_line) > 3)
          print_err(21);
        match_var($parse_line[1]);
        $type[1] = 'var';
        if (preg_match('/^\s*(int|bool|string)(\s*#.*|$)$/', $parse_line[2]))
          $type[2] = 'type';
        else
          print_err(21);
        $cnt_loc++;
        xml_create($parse_line, $type, 2);
      }
      // proccesing the group of instructions with arguments: <VAR> <SYMB>
      else if (preg_match('/^(type|strlen|int2char|move|not)$/i', $parse_line[0])) {
        if (count($parse_line) > 3)
          print_err(21);
        match_var($parse_line[1]);
        $type[1] = 'var';
        $type[2] = match_symb($parse_line[2]);
        revision_const($type[2], 2);
        $cnt_loc++;
        xml_create($parse_line, $type, 2);          
      }
      // proccesing the group of instructions with arguments: <LABEL> <SYMB> <SYMB>
      else if (preg_match('/^(jumpifeq|jumpifneq)$/i', $parse_line[0])) {
        if (count($parse_line) > 4)
            print_err(21);
        match_label($parse_line[1]);
        $type[1] = 'label';
        $type[2] = match_symb($parse_line[2]);
        $type[3] = match_symb($parse_line[3]);
        revision_const($type[2], 2);
        revision_const($type[3], 3);
        $cnt_loc++;
        xml_create($parse_line, $type, 3);
      }
      // proccesing the group of instructions with arguments: <VAR> <SYMB> <SYMB>
      else if (preg_match('/^(add|sub|mul|idiv|lt|gt|eq|and|or|stri2int|concat|getchar|setchar)$/i', $parse_line[0])) {
        if (count($parse_line) > 4)
          print_err(21);
        match_var($parse_line[1]);
        $type[1] = 'var';
        $type[2] = match_symb($parse_line[2]);
        $type[3] = match_symb($parse_line[3]);
        revision_const($type[2], 2);
        revision_const($type[3], 3);
        $cnt_loc++;
        xml_create($parse_line, $type, 3);              
      }
      // forbidden states
      else 
        print_err(21);
    }   
  }
} 

// HEAD of input file was not found
if ($access == false) print_err(21);

// closed XML Hadle
xmlwriter_end_element($xw);
xmlwriter_end_document($xw);
// flush XML to STDOUT
echo xmlwriter_output_memory($xw);
// writing statistics if this options was entered by users
if ($used_params['--stats'])
  write_stats();

fclose($handle);
?>
