<?php
/**
 * @file
 * Additional Infusionsoft API functions.
 *
 * More documentation can be found in infusionsoft.module file.
 */

/**
 * Defines infusionsoft contact mappings for a module.
 *
 * To define a custom mapping it must be in the format of $map['map_name']
 * where the map name is the internal name for which the map will be called.
 *
 * Each field mapping must contain the following:
 *   'title' - What the map will be called.
 *   'description' - a short description of the map
 *   'callback' - a callback which will provide the map.
 *   'options' -  array of options (if any) this map can handle. (optional)
 *
 * So to add a new map it would look like:
 *   $maps['custom_map'] = array(
 *     'title' => 'My Custom Mapping',
 *     'description' => 'Send user name and email address.',
 *     'callback' => 'my_custom_function_name',
 *   );
 *
 * All the mapping logic will be contained in your mapping function.
 * The mapping function gets passed 2 parameters. first: $user, the user
 * object, and second an array of $options, which custom modules can pass
 * through. It must return mapped contact data.
 *
 * This function will look like:
 *   function my_custom_function_name($user, $options) {
 *     return array(
 *       'Username' => $user->name,
 *       'EmailAddress' => $user->mail,
 *     )
 *   }
 *
 * The key must be a field defined on the infusionsoft user table, as
 * documented here:
 * @link http://help.infusionsoft.com/developers/tables/user
 *
 */
function hook_infusionsoft_contact_map_info() {
  // Define your custom maps here.
  $maps['custom_map'] = array(
    'title' => 'My Custom Mapping',
    'description' => 'Send user name and email address.',
    'callback' => 'my_custom_function_name',
  );
  $maps['another_custom_map'] = array(
    'title' => 'My Other Custom Mapping',
    'description' => 'Send only the user name.',
    'callback' => 'my_custom_function_name_two',
    'options' => array('email_override'),
  );

  return $maps;
}

/**
 * Alter an existing infusionsoft mapping before the mapping is used.
 */
function hook_infusionsoft_contact_map_info_alter(&$maps) {
  // Alter the maps array here.
}

/**
 * Example map callback.
 */
function my_custom_function_name_two($user, $options) {
  // Load a user profile, to get their first name custom profile field.
  profile_load_profile($user);

  // Allow an email to be overriden.
  // This map can be used with one way sync because it provides an option
  // for email override and it handles that option here in the callback
  // function.
  if (empty($options['email_override'])) {
    $email = $user->mail;
  }
  else {
    $email = $options['email_override'];
  }

  // Return contact data.
  return array(
    'Email' => $email,
    'Username' => $user->name,
    'FirstName' => $user->profile_name,
  );
}
