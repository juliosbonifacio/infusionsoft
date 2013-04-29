<?php

/**
 * @file
 * Drush commands for infusionsoft.
 *
 */

/**
 * Implements hook_drush_help().
 */
function infusionsoft_drush_help($command) {
  switch ($command) {
    case 'drush:inf-test':
      return dt('Runs a test to see if a connection to infusionsoft works');
  }
}

/**
 * Implements hook_drush_command().
 */
function infusionsoft_drush_command() {
  $items = array();
  $items['inf-test'] = array(
    'description' => dt('Test infusionsoft.'),
    'examples' => array(
      'Standard example' => 'drush inf-test',
    ),
    'aliases' => array('inftst'),
  );
  return $items;
}

/**
 * Callback function for drush my-command.
 * Callback is called by using drush_hook_command() where
 * hook is the name of the module (MYMODULE) and command is the name of
 * the Drush command with all "-" characters converted to "_" characters (my_command)
 *
 */
function drush_infusionsoft_inf_test() {
  drush_print(_infusionsoft_test_connection());

  //log to the command line with an OK status
  drush_log('Running infusionsoft test', 'ok');
}
